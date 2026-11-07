#include "spatial_tracker.h"
#include <string.h>
#include <math.h>

#if defined(PICO_RP2040) || defined(PICO_BUILD)
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#define SPATIAL_HW_I2C 1
#else
#define SPATIAL_HW_I2C 0
#endif

static spatial_tracker_t g_spatial_tracker_instance;

static const float SENSOR_OFFSET_X_M = 0.000f;
static const float SENSOR_OFFSET_Y_M = 0.000f;
static const float SENSOR_MOUNT_Z_M  = 0.023f;

static const float KALMAN_Q_POS = 0.00008f;
static const float KALMAN_Q_VEL = 0.00400f;
static const float KALMAN_R_BASE = 0.00015f;

static const float SPRING_CONSTANT_K0_NPM = 180.0f;
static const float SPRING_NONLINEAR_GAMMA = 4.2f;
static const float DAMPING_COEFFICIENT_B  = 1.45f;
static const float MAX_TACTILE_AM_DEPTH   = 255.0f;

static void kalman_6d_reset(tracker_kalman_6d_t *kf, const vec3f_t *init_pos) {
    memset(kf, 0, sizeof(*kf));
    if (init_pos) {
        kf->pos = *init_pos;
    }
    for (int i = 0; i < 3; ++i) {
        kf->P[i][i]     = 0.010f;
        kf->P[i+3][i+3] = 0.500f;
    }
    kf->confidence   = 0.0f;
    kf->active_ticks = 0u;
    kf->missed_ticks = 0u;
    kf->is_locked    = false;
}

#if SPATIAL_HW_I2C
static bool i2c_write_register_16(uint16_t reg, const uint8_t *data, size_t len) {
    uint8_t buffer[len + 2];
    buffer[0] = (uint8_t)((reg >> 8) & 0xFF);
    buffer[1] = (uint8_t)(reg & 0xFF);
    if (data && len > 0) {
        memcpy(&buffer[2], data, len);
    }
    int res = i2c_write_blocking(i2c0, VL53L5CX_DEFAULT_I2C_ADDR_7BIT, buffer, len + 2, false);
    return (res == (int)(len + 2));
}

static bool i2c_read_register_16(uint16_t reg, uint8_t *data, size_t len) {
    uint8_t reg_addr[2];
    reg_addr[0] = (uint8_t)((reg >> 8) & 0xFF);
    reg_addr[1] = (uint8_t)(reg & 0xFF);
    int res = i2c_write_blocking(i2c0, VL53L5CX_DEFAULT_I2C_ADDR_7BIT, reg_addr, 2, true);
    if (res != 2) {
        return false;
    }
    res = i2c_read_blocking(i2c0, VL53L5CX_DEFAULT_I2C_ADDR_7BIT, data, len, false);
    return (res == (int)len);
}
#endif

void tracker_init(spatial_tracker_t *trk) {
    if (!trk) {
        trk = &g_spatial_tracker_instance;
    }
    memset(trk, 0, sizeof(*trk));
    trk->state            = TRACKER_STATE_BOOT;
    trk->timestamp_ms     = 0u;
    trk->sensor_fps       = 60.0f;
    trk->sensor_temp_degc = 28.0f;

    vec3f_t zero_pos = {0.0f, 0.0f, WORKSPACE_DEFAULT_Z_M};
    kalman_6d_reset(&trk->hand_filter, &zero_pos);
    kalman_6d_reset(&trk->finger_filter, &zero_pos);

    trk->acoustic_target.x = 0.0f;
    trk->acoustic_target.y = 0.0f;
    trk->acoustic_target.z = WORKSPACE_DEFAULT_Z_M;

    trk->haptic_intercept_pos = trk->acoustic_target;

#if SPATIAL_HW_I2C
    i2c_init(i2c0, 400000);
    gpio_set_function(PIN_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_I2C_SDA);
    gpio_pull_up(PIN_I2C_SCL);
#endif

    tracker_sensor_hardware_handshake(trk);
}

bool tracker_sensor_hardware_handshake(spatial_tracker_t *trk) {
    trk->state = TRACKER_STATE_CALIBRATING;
#if SPATIAL_HW_I2C
    uint8_t dev_id_bytes[2] = {0};
    if (!i2c_read_register_16(VL53L5CX_REG_DEVICE_ID, dev_id_bytes, 2)) {
        trk->total_i2c_bus_errors++;
        trk->state = TRACKER_STATE_SEARCHING;
        return false;
    }
    uint16_t id = ((uint16_t)dev_id_bytes[0] << 8) | dev_id_bytes[1];
    if (id != VL53L5CX_EXPECTED_DEVICE_ID) {
        trk->state = TRACKER_STATE_SEARCHING;
        return false;
    }
#endif
    tracker_configure_high_speed_ranging(trk, 60u);
    trk->state = TRACKER_STATE_SEARCHING;
    return true;
}

void tracker_configure_high_speed_ranging(spatial_tracker_t *trk, uint8_t frequency_hz) {
    (void)trk;
#if SPATIAL_HW_I2C
    uint8_t resol = VL53L5CX_RESOL_8X8;
    i2c_write_register_16(VL53L5CX_REG_ZONE_RESOLUTION, &resol, 1);

    uint8_t freq = (frequency_hz > 60u) ? 60u : frequency_hz;
    i2c_write_register_16(VL53L5CX_REG_RANGING_FREQ_HZ, &freq, 1);

    uint8_t sharpener = 25u;
    i2c_write_register_16(VL53L5CX_REG_SHARPENER_PERCENT, &sharpener, 1);

    uint8_t cmd_start[2] = {0x00, VL53L5CX_CMD_START_STREAM};
    i2c_write_register_16(VL53L5CX_REG_RANGING_COMMAND, cmd_start, 2);
#else
    (void)frequency_hz;
#endif
}

bool tracker_poll_and_read_depth_map(spatial_tracker_t *trk, uint32_t now_ms) {
#if SPATIAL_HW_I2C
    uint8_t ready_byte = 0;
    if (!i2c_read_register_16(VL53L5CX_REG_DATA_READY_STATUS, &ready_byte, 1)) {
        trk->total_i2c_bus_errors++;
        return false;
    }
    if (ready_byte == 0u) {
        return false;
    }

    vl53l5cx_raw_data_buffer_t raw_buf;
    if (!i2c_read_register_16(VL53L5CX_REG_DATA_STREAM_OUTPUT, (uint8_t *)&raw_buf, sizeof(raw_buf))) {
        trk->total_i2c_bus_errors++;
        return false;
    }
    tracker_ingest_depth_buffer(trk, &raw_buf, now_ms);
    return true;
#else
    (void)trk;
    (void)now_ms;
    return false;
#endif
}

void tracker_ingest_depth_buffer(spatial_tracker_t *trk, const vl53l5cx_raw_data_buffer_t *raw, uint32_t now_ms) {
    if (!trk || !raw) {
        return;
    }
    trk->timestamp_ms = now_ms;
    trk->frame_index++;
    trk->total_frames_processed++;
    trk->sensor_temp_degc = (float)raw->silicon_temp_degc;
    trk->valid_pixels_count = 0u;

    for (uint8_t r = 0; r < TRACKER_GRID_DIM; ++r) {
        for (uint8_t c = 0; c < TRACKER_GRID_DIM; ++c) {
            uint8_t zone_idx = r * TRACKER_GRID_DIM + c;
            const vl53l5cx_raw_target_t *tgt = &raw->targets[zone_idx];
            tracker_depth_pixel_t *pix = &trk->depth_map[r][c];

            pix->status       = tgt->target_status;
            pix->sigma_mm     = tgt->sigma_mm;
            pix->signal_kcps  = (float)tgt->signal_rate_kcps;
            float raw_dist_m  = (float)tgt->distance_mm * 0.001f;
            pix->distance_m   = raw_dist_m;

            if (tgt->target_status == VL53L5CX_STATUS_VALID_TARGET &&
                raw_dist_m >= TRACKER_RANGE_MIN_M &&
                raw_dist_m <= TRACKER_RANGE_MAX_M) {
                pix->valid = true;
                trk->valid_pixels_count++;

                float norm_x = ((float)c - 3.5f) / 3.5f;
                float norm_y = (3.5f - (float)r) / 3.5f;

                pix->x = SENSOR_OFFSET_X_M + norm_x * raw_dist_m * VL53L5CX_TAN_HALF_FOV;
                pix->y = SENSOR_OFFSET_Y_M + norm_y * raw_dist_m * VL53L5CX_TAN_HALF_FOV;
                pix->z = raw_dist_m - SENSOR_MOUNT_Z_M;
            } else {
                pix->valid = false;
                pix->x = 0.0f;
                pix->y = 0.0f;
                pix->z = 999.0f;
            }
        }
    }

    tracker_execute_spatial_clustering(trk);
    tracker_extract_apex_fingertip(trk);
}

void tracker_execute_spatial_clustering(spatial_tracker_t *trk) {
    trk->cluster_count = 0u;
    if (trk->valid_pixels_count == 0u) {
        return;
    }

    bool visited[TRACKER_TOTAL_ZONES];
    memset(visited, 0, sizeof(visited));

    for (uint8_t i = 0; i < TRACKER_TOTAL_ZONES; ++i) {
        uint8_t r = i / TRACKER_GRID_DIM;
        uint8_t c = i % TRACKER_GRID_DIM;

        if (visited[i] || !trk->depth_map[r][c].valid) {
            continue;
        }

        if (trk->cluster_count >= TRACKER_MAX_CLUSTERS) {
            break;
        }

        tracker_point_cluster_t *cl = &trk->clusters[trk->cluster_count];
        memset(cl, 0, sizeof(*cl));
        cl->min_bounds = (vec3f_t){ 999.0f,  999.0f,  999.0f};
        cl->max_bounds = (vec3f_t){-999.0f, -999.0f, -999.0f};

        uint8_t queue[TRACKER_TOTAL_ZONES];
        uint8_t q_head = 0u;
        uint8_t q_tail = 0u;

        queue[q_tail++] = i;
        visited[i] = true;

        while (q_head < q_tail && cl->pixel_count < TRACKER_TOTAL_ZONES) {
            uint8_t curr = queue[q_head++];
            cl->pixel_indices[cl->pixel_count++] = curr;

            uint8_t cr = curr / TRACKER_GRID_DIM;
            uint8_t cc = curr % TRACKER_GRID_DIM;
            const tracker_depth_pixel_t *cp = &trk->depth_map[cr][cc];

            cl->centroid.x += cp->x;
            cl->centroid.y += cp->y;
            cl->centroid.z += cp->z;
            cl->total_mass += 1.0f;

            if (cp->x < cl->min_bounds.x) cl->min_bounds.x = cp->x;
            if (cp->y < cl->min_bounds.y) cl->min_bounds.y = cp->y;
            if (cp->z < cl->min_bounds.z) cl->min_bounds.z = cp->z;
            if (cp->x > cl->max_bounds.x) cl->max_bounds.x = cp->x;
            if (cp->y > cl->max_bounds.y) cl->max_bounds.y = cp->y;
            if (cp->z > cl->max_bounds.z) cl->max_bounds.z = cp->z;

            const int8_t dr[4] = {-1, 1,  0, 0};
            const int8_t dc[4] = { 0, 0, -1, 1};

            for (uint8_t n = 0; n < 4; ++n) {
                int nr = (int)cr + dr[n];
                int nc = (int)cc + dc[n];
                if (nr >= 0 && nr < (int)TRACKER_GRID_DIM && nc >= 0 && nc < (int)TRACKER_GRID_DIM) {
                    uint8_t ni = (uint8_t)(nr * TRACKER_GRID_DIM + nc);
                    if (!visited[ni] && trk->depth_map[nr][nc].valid) {
                        float dz = fabsf(trk->depth_map[nr][nc].z - cp->z);
                        if (dz <= TRACKER_OUTLIER_SIGMA_M) {
                            visited[ni] = true;
                            queue[q_tail++] = ni;
                        }
                    }
                }
            }
        }

        if (cl->pixel_count >= 2u) {
            cl->centroid.x /= (float)cl->pixel_count;
            cl->centroid.y /= (float)cl->pixel_count;
            cl->centroid.z /= (float)cl->pixel_count;
            trk->cluster_count++;
        }
    }
}

void tracker_extract_apex_fingertip(spatial_tracker_t *trk) {
    if (trk->cluster_count == 0u) {
        trk->hand_filter.missed_ticks++;
        trk->finger_filter.missed_ticks++;
        if (trk->finger_filter.missed_ticks > 8u) {
            trk->state = TRACKER_STATE_SEARCHING;
            trk->hand_filter.is_locked = false;
            trk->finger_filter.is_locked = false;
            trk->forcefield.active = false;
        }
        return;
    }

    uint8_t primary_cluster_idx = 0u;
    uint8_t max_mass = 0u;
    for (uint8_t k = 0; k < trk->cluster_count; ++k) {
        if (trk->clusters[k].pixel_count > max_mass) {
            max_mass = trk->clusters[k].pixel_count;
            primary_cluster_idx = k;
        }
    }

    const tracker_point_cluster_t *main_cl = &trk->clusters[primary_cluster_idx];
    vec3f_t hand_meas = main_cl->centroid;

    vec3f_t tip_meas = {0.0f, 0.0f, 999.0f};
    float min_z = 999.0f;
    uint8_t apex_idx = 0u;

    for (uint8_t p = 0; p < main_cl->pixel_count; ++p) {
        uint8_t idx = main_cl->pixel_indices[p];
        uint8_t r = idx / TRACKER_GRID_DIM;
        uint8_t c = idx % TRACKER_GRID_DIM;
        const tracker_depth_pixel_t *pix = &trk->depth_map[r][c];

        if (pix->z < min_z) {
            min_z = pix->z;
            tip_meas = (vec3f_t){pix->x, pix->y, pix->z};
            apex_idx = idx;
        }
    }

    (void)apex_idx;
    float dt = (trk->sensor_fps > 0.0f) ? (1.0f / trk->sensor_fps) : 0.01667f;
    tracker_update_kalman_filter(&trk->hand_filter, &hand_meas, dt, 1.2f);
    tracker_update_kalman_filter(&trk->finger_filter, &tip_meas, dt, 0.8f);

    if (trk->finger_filter.is_locked) {
        trk->state = TRACKER_STATE_FINGER_LOCKED;
    } else {
        trk->state = TRACKER_STATE_HAND_DETECTED;
    }
}

void tracker_update_kalman_filter(tracker_kalman_6d_t *kf, const vec3f_t *measured, float dt_sec, float noise_scale) {
    if (!kf || !measured) {
        return;
    }

    kf->pos.x += kf->vel.x * dt_sec;
    kf->pos.y += kf->vel.y * dt_sec;
    kf->pos.z += kf->vel.z * dt_sec;

    for (int i = 0; i < 3; ++i) {
        kf->P[i][i]     += (kf->P[i+3][i+3] * dt_sec * dt_sec) + KALMAN_Q_POS;
        kf->P[i][i+3]   += (kf->P[i+3][i+3] * dt_sec);
        kf->P[i+3][i]   += (kf->P[i+3][i+3] * dt_sec);
        kf->P[i+3][i+3] += KALMAN_Q_VEL;
    }

    vec3f_t residual = {
        measured->x - kf->pos.x,
        measured->y - kf->pos.y,
        measured->z - kf->pos.z
    };

    float r_var = KALMAN_R_BASE * noise_scale;

    for (int i = 0; i < 3; ++i) {
        float S = kf->P[i][i] + r_var;
        float K_pos = kf->P[i][i] / S;
        float K_vel = kf->P[i+3][i] / S;

        float res_val = (i == 0) ? residual.x : ((i == 1) ? residual.y : residual.z);

        if (i == 0) {
            kf->pos.x += K_pos * res_val;
            kf->vel.x += K_vel * res_val;
        } else if (i == 1) {
            kf->pos.y += K_pos * res_val;
            kf->vel.y += K_vel * res_val;
        } else {
            kf->pos.z += K_pos * res_val;
            kf->vel.z += K_vel * res_val;
        }

        kf->P[i][i]     *= (1.0f - K_pos);
        kf->P[i][i+3]   *= (1.0f - K_pos);
        kf->P[i+3][i]   -= (K_vel * kf->P[i][i]);
        kf->P[i+3][i+3] -= (K_vel * kf->P[i][i+3]);
    }

    kf->active_ticks++;
    kf->missed_ticks = 0u;
    if (kf->active_ticks > 3u) {
        kf->is_locked = true;
    }
}

void tracker_compute_forcefield_intercept(spatial_tracker_t *trk, const hpve_system_state_t *sys, float dt_sec) {
    if (!trk) {
        return;
    }
    tracker_forcefield_event_t *ff = &trk->forcefield;
    memset(ff, 0, sizeof(*ff));

    if (!trk->finger_filter.is_locked) {
        ff->active = false;
        ff->zone = FORCEFIELD_ZONE_OUTSIDE;
        return;
    }

    ff->tip_pos   = trk->finger_filter.pos;
    ff->tip_vel   = trk->finger_filter.vel;
    ff->speed_mps = sqrtf(ff->tip_vel.x * ff->tip_vel.x +
                          ff->tip_vel.y * ff->tip_vel.y +
                          ff->tip_vel.z * ff->tip_vel.z);

    float wall_z = (sys && sys->forcefield_z_wall_m > 0.0f) ? sys->forcefield_z_wall_m : TRACKER_FORCEFIELD_WALL_Z;
    float trigger_enter_z = wall_z + 0.020f;

    if (ff->tip_pos.z > trigger_enter_z) {
        ff->active = false;
        ff->zone = FORCEFIELD_ZONE_OUTSIDE;
        return;
    }

    ff->active = true;
    trk->total_forcefield_triggers++;
    trk->state = TRACKER_STATE_FORCEFIELD_ACTIVE;

    float raw_pen = wall_z - ff->tip_pos.z;
    if (raw_pen < 0.0f) {
        ff->zone = FORCEFIELD_ZONE_PROXIMITY;
        ff->penetration_depth_m = 0.0f;
        ff->dynamic_stiffness_npm = SPRING_CONSTANT_K0_NPM * 0.15f;
        ff->tactile_am_depth = 40.0f;
        ff->radiation_pressure_pa = 25.0f;
        ff->normal_force_mn = 1.2f;
    } else {
        ff->penetration_depth_m = raw_pen;
        float norm_pen = raw_pen / TRACKER_FORCEFIELD_MAX_PEN;
        if (norm_pen > 1.0f) norm_pen = 1.0f;

        float k_eff = SPRING_CONSTANT_K0_NPM * (1.0f + SPRING_NONLINEAR_GAMMA * norm_pen * norm_pen);
        ff->dynamic_stiffness_npm = k_eff;

        float f_spring = k_eff * raw_pen;
        float f_damping = 0.0f;
        if (ff->tip_vel.z < 0.0f) {
            f_damping = -DAMPING_COEFFICIENT_B * ff->tip_vel.z;
        }
        float f_total_n = f_spring + f_damping;
        ff->normal_force_mn = f_total_n * 1000.0f;

        float area_eff_m2 = 0.000100f;
        ff->radiation_pressure_pa = f_total_n / area_eff_m2;
        if (ff->radiation_pressure_pa > 280.0f) {
            ff->radiation_pressure_pa = 280.0f;
        }

        float am = 40.0f + 215.0f * norm_pen;
        if (am > MAX_TACTILE_AM_DEPTH) am = MAX_TACTILE_AM_DEPTH;
        ff->tactile_am_depth = am;

        if (norm_pen >= 0.95f) {
            ff->zone = FORCEFIELD_ZONE_SATURATION;
        } else if (norm_pen >= 0.25f) {
            ff->zone = FORCEFIELD_ZONE_PENETRATION;
        } else {
            ff->zone = FORCEFIELD_ZONE_CONTACT;
        }
    }

    vec3f_t lead_pos = {
        ff->tip_pos.x + ff->tip_vel.x * (dt_sec * 1.5f),
        ff->tip_pos.y + ff->tip_vel.y * (dt_sec * 1.5f),
        wall_z - (ff->penetration_depth_m * 0.5f)
    };

    if (lead_pos.x < WORKSPACE_MIN_X_M) lead_pos.x = WORKSPACE_MIN_X_M;
    if (lead_pos.x > WORKSPACE_MAX_X_M) lead_pos.x = WORKSPACE_MAX_X_M;
    if (lead_pos.y < WORKSPACE_MIN_Y_M) lead_pos.y = WORKSPACE_MIN_Y_M;
    if (lead_pos.y > WORKSPACE_MAX_Y_M) lead_pos.y = WORKSPACE_MAX_Y_M;
    if (lead_pos.z < WORKSPACE_MIN_Z_M) lead_pos.z = WORKSPACE_MIN_Z_M;
    if (lead_pos.z > WORKSPACE_MAX_Z_M) lead_pos.z = WORKSPACE_MAX_Z_M;

    trk->acoustic_target = lead_pos;
    trk->haptic_intercept_pos = lead_pos;
}

void tracker_hook_wave_solver(spatial_tracker_t *trk, hpve_system_state_t *sys) {
    if (!trk || !sys) {
        return;
    }
    float dt = (trk->sensor_fps > 0.0f) ? (1.0f / trk->sensor_fps) : 0.01667f;
    tracker_compute_forcefield_intercept(trk, sys, dt);

    if (trk->forcefield.active) {
        sys->active_mode       = HPVE_MODE_HAPTIC_FOCUS;
        sys->active_trajectory = TRAJ_HAPTIC_FORCEFIELD_WALL;
        sys->focus_primary     = trk->acoustic_target;
        sys->finger_probe_z_m  = trk->finger_filter.pos.z;
    }
}

const spatial_tracker_t *tracker_get_instance(void) {
    return &g_spatial_tracker_instance;
}
