#include "wave_solver.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define TRIG_LUT_SIZE       1024u
#define TRIG_LUT_MASK       (TRIG_LUT_SIZE - 1u)
#define Q16_PI              205887
#define Q16_TWO_PI          411775
#define Q16_HALF_PI         102944
#define Q16_INV_WAVELENGTH  7642915

static q16_t                    g_sin_lut_q16[TRIG_LUT_SIZE];
static bool                     g_solver_initialized = false;
static hpve_trajectory_engine_t g_solver_traj_eng;
static hpve_trajectory_sample_t g_last_traj_sample;

void hpve_solver_init(void) {
    if (g_solver_initialized) {
        return;
    }
    for (uint32_t i = 0; i < TRIG_LUT_SIZE; ++i) {
        double theta = (2.0 * M_PI * (double)i) / (double)TRIG_LUT_SIZE;
        g_sin_lut_q16[i] = (q16_t)llround(sin(theta) * (double)Q16_ONE);
    }
    hpve_trajectory_init(&g_solver_traj_eng);
    memset(&g_last_traj_sample, 0, sizeof(g_last_traj_sample));
    g_solver_initialized = true;
}

const hpve_trajectory_sample_t* hpve_solver_get_last_trajectory_sample(void) {
    return &g_last_traj_sample;
}

uint32_t hpve_isqrt64(uint64_t val) {
    uint64_t rem  = 0ULL;
    uint64_t root = 0ULL;
    for (int i = 0; i < 32; ++i) {
        root <<= 1u;
        rem = (rem << 2u) | (val >> 62u);
        val <<= 2u;
        uint64_t test_div = (root << 1u) | 1ULL;
        if (rem >= test_div) {
            rem -= test_div;
            root |= 1ULL;
        }
    }
    return (uint32_t)root;
}

q16_t hpve_sin_q16(q16_t angle_rad_q16) {
    if (!g_solver_initialized) {
        hpve_solver_init();
    }
    int64_t scaled = ((int64_t)angle_rad_q16 * (int64_t)TRIG_LUT_SIZE) / (int64_t)Q16_TWO_PI;
    uint32_t idx = (uint32_t)scaled & TRIG_LUT_MASK;
    return g_sin_lut_q16[idx];
}

q16_t hpve_cos_q16(q16_t angle_rad_q16) {
    return hpve_sin_q16(angle_rad_q16 + Q16_HALF_PI);
}

q16_t hpve_atan2_q16(q16_t y, q16_t x) {
    if (x == 0 && y == 0) {
        return 0;
    }
    q16_t abs_y = (y < 0) ? -y : y;
    if (abs_y == 0) {
        abs_y = 1;
    }
    q16_t angle;
    if (x >= 0) {
        q16_t r = Q16_DIV(x - abs_y, x + abs_y);
        q16_t r3 = Q16_MUL(Q16_MUL(r, r), r);
        angle = (Q16_HALF_PI >> 1) - Q16_MUL(64340, r) + Q16_MUL(12868, r3);
    } else {
        q16_t r = Q16_DIV(x + abs_y, abs_y - x);
        q16_t r3 = Q16_MUL(Q16_MUL(r, r), r);
        angle = (3 * (Q16_HALF_PI >> 1)) - Q16_MUL(64340, r) + Q16_MUL(12868, r3);
    }
    return (y < 0) ? -angle : angle;
}

static inline void evaluate_current_trajectory(
    const hpve_system_state_t *state,
    uint32_t tick_ms,
    hpve_trajectory_sample_t *sample_out
) {
    if (state->active_trajectory != TRAJ_NONE_STATIC) {
        hpve_trajectory_step(&g_solver_traj_eng, state, tick_ms, sample_out);
        return;
    }

    memset(sample_out, 0, sizeof(*sample_out));
    vec3f_t target = state->focus_primary;
    if (state->orbit_enabled || state->active_mode == HPVE_MODE_VOLUMETRIC_POV) {
        float t_sec = (float)tick_ms * 0.001f;
        float freq  = (state->orbit_freq_hz > 0.1f) ? state->orbit_freq_hz : 12.0f;
        float rad   = (state->orbit_radius_m > 0.001f) ? state->orbit_radius_m : 0.015f;
        float omega = 2.0f * (float)M_PI * freq * t_sec;
        target.x += rad * cosf(omega);
        target.y += rad * sinf(omega);
        if (state->active_mode == HPVE_MODE_VOLUMETRIC_POV) {
            target.z += (rad * 0.35f) * sinf(2.0f * omega);
        }
    }

    sample_out->primary_pos              = target;
    sample_out->secondary_pos            = state->focus_secondary;
    sample_out->num_active_nodes         = 1u;
    sample_out->multifocal_nodes[0]      = target;
    sample_out->multifocal_weights[0]    = 1.0f;
    sample_out->duty_step_override       = (uint8_t)(NUM_PHASE_STEPS >> 1u);
    sample_out->haptic_am_override       = (state->active_mode == HPVE_MODE_HAPTIC_FOCUS) ? 160u : 0u;
    sample_out->r_pwm                    = 0u;
    sample_out->g_pwm                    = 220u;
    sample_out->b_pwm                    = 255u;
}

static inline q16_t compute_signature_offset_q16(
    const hpve_system_state_t *state,
    const hpve_transducer_map_t *tr,
    q16_t tx_q16,
    q16_t ty_q16
) {
    switch (state->active_mode) {
        case HPVE_MODE_TWIN_TRAP: {
            if (state->twin_axis == TWIN_AXIS_X) {
                return (tr->pos_x_q16 >= tx_q16) ? (Q16_ONE >> 1) : 0;
            } else if (state->twin_axis == TWIN_AXIS_Y) {
                return (tr->pos_y_q16 >= ty_q16) ? (Q16_ONE >> 1) : 0;
            } else if (state->twin_axis == TWIN_AXIS_DIAGONAL) {
                return ((tr->pos_x_q16 - tx_q16) + (tr->pos_y_q16 - ty_q16) >= 0) ? (Q16_ONE >> 1) : 0;
            } else {
                bool sx = (tr->pos_x_q16 >= tx_q16);
                bool sy = (tr->pos_y_q16 >= ty_q16);
                return (sx ^ sy) ? (Q16_ONE >> 1) : 0;
            }
        }
        case HPVE_MODE_BOTTLE_TRAP: {
            q16_t dx = tr->pos_x_q16 - tx_q16;
            q16_t dy = tr->pos_y_q16 - ty_q16;
            uint64_t r2 = (uint64_t)((int64_t)dx * dx + (int64_t)dy * dy);
            q16_t r_ring = FLOAT_TO_Q16(state->bottle_radius_m > 0.005f ? state->bottle_radius_m : 0.036f);
            uint64_t ring2 = (uint64_t)((int64_t)r_ring * r_ring);
            return (r2 < ring2) ? (Q16_ONE >> 1) : 0;
        }
        case HPVE_MODE_VORTEX_BEAM: {
            q16_t dx = tr->pos_x_q16 - tx_q16;
            q16_t dy = tr->pos_y_q16 - ty_q16;
            q16_t azim = hpve_atan2_q16(dy, dx);
            if (azim < 0) {
                azim += Q16_TWO_PI;
            }
            int32_t charge = (state->vortex_charge != 0) ? state->vortex_charge : 1;
            int64_t cyc = ((int64_t)azim * charge * (int64_t)Q16_ONE) / (int64_t)Q16_TWO_PI;
            return (q16_t)(cyc & (Q16_ONE - 1));
        }
        default:
            return 0;
    }
}

void hpve_solve_frame_fixed(
    const hpve_system_state_t *state,
    const hpve_transducer_map_t *geom,
    uint32_t tick_ms,
    hpve_channel_frame_t *frame_out
) {
    if (!g_solver_initialized) {
        hpve_solver_init();
    }

    if (!state->output_enabled || state->active_mode == HPVE_MODE_STANDBY) {
        memset(frame_out->phase_step, 0, sizeof(frame_out->phase_step));
        memset(frame_out->duty_steps, 0, sizeof(frame_out->duty_steps));
        frame_out->active_mask     = 0ULL;
        frame_out->haptic_am_depth = 0u;
        frame_out->r_pwm           = 0u;
        frame_out->g_pwm           = 0u;
        frame_out->b_pwm           = 16u;
        return;
    }

    evaluate_current_trajectory(state, tick_ms, &g_last_traj_sample);
    vec3f_t target_f = g_last_traj_sample.primary_pos;
    uint8_t duty_ov  = (g_last_traj_sample.duty_step_override > 0u)
                     ? g_last_traj_sample.duty_step_override
                     : (uint8_t)(NUM_PHASE_STEPS >> 1u);

    q16_t tx_q16 = FLOAT_TO_Q16(target_f.x);
    q16_t ty_q16 = FLOAT_TO_Q16(target_f.y);
    q16_t tz_q16 = FLOAT_TO_Q16(target_f.z);

    if (state->active_mode == HPVE_MODE_ARC_REACTOR_SMF && g_last_traj_sample.num_active_nodes > 1u) {
        uint8_t n_nodes = g_last_traj_sample.num_active_nodes;
        if (n_nodes > MAX_MULTIFOCAL_NODES) {
            n_nodes = MAX_MULTIFOCAL_NODES;
        }

        q16_t nx_q16[MAX_MULTIFOCAL_NODES];
        q16_t ny_q16[MAX_MULTIFOCAL_NODES];
        q16_t nz_q16[MAX_MULTIFOCAL_NODES];
        q16_t nw_q16[MAX_MULTIFOCAL_NODES];
        for (uint8_t k = 0; k < n_nodes; ++k) {
            nx_q16[k] = FLOAT_TO_Q16(g_last_traj_sample.multifocal_nodes[k].x);
            ny_q16[k] = FLOAT_TO_Q16(g_last_traj_sample.multifocal_nodes[k].y);
            nz_q16[k] = FLOAT_TO_Q16(g_last_traj_sample.multifocal_nodes[k].z);
            nw_q16[k] = FLOAT_TO_Q16(g_last_traj_sample.multifocal_weights[k]);
        }

        for (uint8_t i = 0; i < NUM_TRANSDUCERS; ++i) {
            int64_t sum_re = 0;
            int64_t sum_im = 0;

            for (uint8_t k = 0; k < n_nodes; ++k) {
                int64_t dx = (int64_t)(nx_q16[k] - geom[i].pos_x_q16);
                int64_t dy = (int64_t)(ny_q16[k] - geom[i].pos_y_q16);
                int64_t dz = (int64_t)nz_q16[k];
                uint64_t d_sq = (uint64_t)(dx * dx + dy * dy + dz * dz);
                q16_t dist_q16 = (q16_t)hpve_isqrt64(d_sq);

                q16_t cyc = Q16_MUL(-dist_q16, Q16_INV_WAVELENGTH);
                if (g_last_traj_sample.multifocal_topological_sign[k] != 0u) {
                    cyc += (Q16_ONE >> 1);
                } else if (geom[i].pos_x_q16 >= nx_q16[k]) {
                    cyc += (Q16_ONE >> 1);
                }

                q16_t ang = Q16_MUL(cyc & (Q16_ONE - 1), Q16_TWO_PI);
                sum_re += (int64_t)Q16_MUL(nw_q16[k], hpve_cos_q16(ang));
                sum_im += (int64_t)Q16_MUL(nw_q16[k], hpve_sin_q16(ang));
            }

            q16_t combined_ang = hpve_atan2_q16((q16_t)sum_im, (q16_t)sum_re);
            if (combined_ang < 0) {
                combined_ang += Q16_TWO_PI;
            }
            uint32_t step = (uint32_t)(((int64_t)combined_ang * NUM_PHASE_STEPS + (Q16_TWO_PI >> 1)) / Q16_TWO_PI) & PHASE_STEP_MASK;
            frame_out->phase_step[i] = (uint8_t)step;
            frame_out->duty_steps[i] = duty_ov;
        }
    } else if (state->active_mode == HPVE_MODE_DUAL_TRAP_SMF) {
        vec3f_t sec_f = (state->active_trajectory != TRAJ_NONE_STATIC)
                      ? g_last_traj_sample.secondary_pos
                      : state->focus_secondary;
        q16_t t2x_q16 = FLOAT_TO_Q16(sec_f.x);
        q16_t t2y_q16 = FLOAT_TO_Q16(sec_f.y);
        q16_t t2z_q16 = FLOAT_TO_Q16(sec_f.z);

        for (uint8_t i = 0; i < NUM_TRANSDUCERS; ++i) {
            int64_t dx1 = (int64_t)(tx_q16 - geom[i].pos_x_q16);
            int64_t dy1 = (int64_t)(ty_q16 - geom[i].pos_y_q16);
            int64_t dz1 = (int64_t)tz_q16;
            uint64_t d1_sq = (uint64_t)(dx1 * dx1 + dy1 * dy1 + dz1 * dz1);
            q16_t dist1_q16 = (q16_t)hpve_isqrt64(d1_sq);

            int64_t dx2 = (int64_t)(t2x_q16 - geom[i].pos_x_q16);
            int64_t dy2 = (int64_t)(t2y_q16 - geom[i].pos_y_q16);
            int64_t dz2 = (int64_t)t2z_q16;
            uint64_t d2_sq = (uint64_t)(dx2 * dx2 + dy2 * dy2 + dz2 * dz2);
            q16_t dist2_q16 = (q16_t)hpve_isqrt64(d2_sq);

            q16_t cyc1 = Q16_MUL(-dist1_q16, Q16_INV_WAVELENGTH);
            if (geom[i].pos_x_q16 >= tx_q16) {
                cyc1 += (Q16_ONE >> 1);
            }
            q16_t cyc2 = Q16_MUL(-dist2_q16, Q16_INV_WAVELENGTH);
            if (geom[i].pos_x_q16 >= t2x_q16) {
                cyc2 += (Q16_ONE >> 1);
            }

            q16_t ang1 = Q16_MUL(cyc1 & (Q16_ONE - 1), Q16_TWO_PI);
            q16_t ang2 = Q16_MUL(cyc2 & (Q16_ONE - 1), Q16_TWO_PI);

            q16_t re = hpve_cos_q16(ang1) + hpve_cos_q16(ang2);
            q16_t im = hpve_sin_q16(ang1) + hpve_sin_q16(ang2);

            q16_t combined_ang = hpve_atan2_q16(im, re);
            if (combined_ang < 0) {
                combined_ang += Q16_TWO_PI;
            }
            uint32_t step = (uint32_t)(((int64_t)combined_ang * NUM_PHASE_STEPS) / Q16_TWO_PI) & PHASE_STEP_MASK;
            frame_out->phase_step[i] = (uint8_t)step;
            frame_out->duty_steps[i] = duty_ov;
        }
    } else {
        for (uint8_t i = 0; i < NUM_TRANSDUCERS; ++i) {
            int64_t dx = (int64_t)(tx_q16 - geom[i].pos_x_q16);
            int64_t dy = (int64_t)(ty_q16 - geom[i].pos_y_q16);
            int64_t dz = (int64_t)tz_q16;

            uint64_t dist_sq = (uint64_t)(dx * dx + dy * dy + dz * dz);
            q16_t dist_q16   = (q16_t)hpve_isqrt64(dist_sq);

            q16_t cycles_q16 = Q16_MUL(dist_q16, Q16_INV_WAVELENGTH);
            q16_t frac_travel = cycles_q16 & (Q16_ONE - 1);
            q16_t emit_frac   = (Q16_ONE - frac_travel) & (Q16_ONE - 1);

            q16_t sig_offset  = compute_signature_offset_q16(state, &geom[i], tx_q16, ty_q16);
            q16_t total_frac  = (emit_frac + sig_offset) & (Q16_ONE - 1);

            uint32_t step = (uint32_t)(((uint64_t)total_frac * NUM_PHASE_STEPS + Q16_HALF) >> Q16_SHIFT) & PHASE_STEP_MASK;
            frame_out->phase_step[i] = (uint8_t)step;
            frame_out->duty_steps[i] = duty_ov;
        }
    }

    frame_out->active_mask = 0xFFFFFFFFFFFFFFFFULL;

    if (state->active_trajectory != TRAJ_NONE_STATIC) {
        frame_out->haptic_am_depth = g_last_traj_sample.haptic_am_override;
        frame_out->r_pwm           = g_last_traj_sample.r_pwm;
        frame_out->g_pwm           = g_last_traj_sample.g_pwm;
        frame_out->b_pwm           = g_last_traj_sample.b_pwm;
        return;
    }

    frame_out->haptic_am_depth = (state->active_mode == HPVE_MODE_HAPTIC_FOCUS) ? 160u : 0u;

    switch (state->active_mode) {
        case HPVE_MODE_TWIN_TRAP:
            frame_out->r_pwm = 0u;   frame_out->g_pwm = 220u; frame_out->b_pwm = 255u;
            break;
        case HPVE_MODE_BOTTLE_TRAP:
            frame_out->r_pwm = 180u; frame_out->g_pwm = 40u;  frame_out->b_pwm = 255u;
            break;
        case HPVE_MODE_VORTEX_BEAM:
            frame_out->r_pwm = 255u; frame_out->g_pwm = 120u; frame_out->b_pwm = 0u;
            break;
        case HPVE_MODE_HAPTIC_FOCUS:
            frame_out->r_pwm = 255u; frame_out->g_pwm = 20u;  frame_out->b_pwm = 60u;
            break;
        case HPVE_MODE_VOLUMETRIC_POV: {
            float phase_c = (float)(tick_ms % 1000u) * 0.006283185f;
            frame_out->r_pwm = (uint8_t)(127.0f + 127.0f * sinf(phase_c));
            frame_out->g_pwm = (uint8_t)(127.0f + 127.0f * sinf(phase_c + 2.094395f));
            frame_out->b_pwm = (uint8_t)(127.0f + 127.0f * sinf(phase_c + 4.188790f));
            break;
        }
        case HPVE_MODE_DUAL_TRAP_SMF:
        case HPVE_MODE_ARC_REACTOR_SMF:
            frame_out->r_pwm = 20u;  frame_out->g_pwm = 240u; frame_out->b_pwm = 255u;
            break;
        default:
            frame_out->r_pwm = 0u;   frame_out->g_pwm = 0u;   frame_out->b_pwm = 0u;
            break;
    }
}

void hpve_solve_frame_float(
    const hpve_system_state_t *state,
    const hpve_transducer_map_t *geom,
    uint32_t tick_ms,
    hpve_channel_frame_t *frame_out
) {
    if (!state->output_enabled || state->active_mode == HPVE_MODE_STANDBY) {
        memset(frame_out->phase_step, 0, sizeof(frame_out->phase_step));
        memset(frame_out->duty_steps, 0, sizeof(frame_out->duty_steps));
        frame_out->active_mask     = 0ULL;
        frame_out->haptic_am_depth = 0u;
        frame_out->r_pwm           = 0u;
        frame_out->g_pwm           = 0u;
        frame_out->b_pwm           = 16u;
        return;
    }

    evaluate_current_trajectory(state, tick_ms, &g_last_traj_sample);
    vec3f_t target = g_last_traj_sample.primary_pos;
    uint8_t duty_ov = (g_last_traj_sample.duty_step_override > 0u)
                    ? g_last_traj_sample.duty_step_override
                    : (uint8_t)(NUM_PHASE_STEPS >> 1u);

    if (state->active_mode == HPVE_MODE_ARC_REACTOR_SMF && g_last_traj_sample.num_active_nodes > 1u) {
        uint8_t n_nodes = g_last_traj_sample.num_active_nodes;
        if (n_nodes > MAX_MULTIFOCAL_NODES) {
            n_nodes = MAX_MULTIFOCAL_NODES;
        }
        for (uint8_t i = 0; i < NUM_TRANSDUCERS; ++i) {
            float sum_re = 0.0f;
            float sum_im = 0.0f;
            for (uint8_t k = 0; k < n_nodes; ++k) {
                vec3f_t nk = g_last_traj_sample.multifocal_nodes[k];
                float w    = g_last_traj_sample.multifocal_weights[k];
                float dx   = nk.x - geom[i].pos_x_m;
                float dy   = nk.y - geom[i].pos_y_m;
                float d    = sqrtf(dx * dx + dy * dy + nk.z * nk.z);
                float sig  = 0.0f;
                if (g_last_traj_sample.multifocal_topological_sign[k] != 0u || geom[i].pos_x_m >= nk.x) {
                    sig = (float)M_PI;
                }
                float phi  = -ACOUSTIC_WAVENUMBER_K * d + sig;
                sum_re += w * cosf(phi);
                sum_im += w * sinf(phi);
            }
            float combined = atan2f(sum_im, sum_re);
            if (combined < 0.0f) {
                combined += 2.0f * (float)M_PI;
            }
            uint32_t step = (uint32_t)lroundf((combined / (2.0f * (float)M_PI)) * (float)NUM_PHASE_STEPS) & PHASE_STEP_MASK;
            frame_out->phase_step[i] = (uint8_t)step;
            frame_out->duty_steps[i] = duty_ov;
        }
    } else if (state->active_mode == HPVE_MODE_DUAL_TRAP_SMF) {
        vec3f_t t2 = (state->active_trajectory != TRAJ_NONE_STATIC)
                   ? g_last_traj_sample.secondary_pos
                   : state->focus_secondary;
        for (uint8_t i = 0; i < NUM_TRANSDUCERS; ++i) {
            float dx1 = target.x - geom[i].pos_x_m;
            float dy1 = target.y - geom[i].pos_y_m;
            float d1  = sqrtf(dx1 * dx1 + dy1 * dy1 + target.z * target.z);
            float phi1 = -ACOUSTIC_WAVENUMBER_K * d1 + ((geom[i].pos_x_m >= target.x) ? (float)M_PI : 0.0f);

            float dx2 = t2.x - geom[i].pos_x_m;
            float dy2 = t2.y - geom[i].pos_y_m;
            float d2  = sqrtf(dx2 * dx2 + dy2 * dy2 + t2.z * t2.z);
            float phi2 = -ACOUSTIC_WAVENUMBER_K * d2 + ((geom[i].pos_x_m >= t2.x) ? (float)M_PI : 0.0f);

            float re = cosf(phi1) + cosf(phi2);
            float im = sinf(phi1) + sinf(phi2);
            float combined = atan2f(im, re);
            if (combined < 0.0f) {
                combined += 2.0f * (float)M_PI;
            }
            float norm_cyc = combined / (2.0f * (float)M_PI);
            uint32_t step  = (uint32_t)lroundf(norm_cyc * (float)NUM_PHASE_STEPS) & PHASE_STEP_MASK;
            frame_out->phase_step[i] = (uint8_t)step;
            frame_out->duty_steps[i] = duty_ov;
        }
    } else {
        for (uint8_t i = 0; i < NUM_TRANSDUCERS; ++i) {
            float dx = target.x - geom[i].pos_x_m;
            float dy = target.y - geom[i].pos_y_m;
            float dz = target.z;
            float dist = sqrtf(dx * dx + dy * dy + dz * dz);

            float cycles = dist / ACOUSTIC_WAVELENGTH_M;
            float frac   = cycles - floorf(cycles);
            float emit_frac = 1.0f - frac;

            float sig_frac = 0.0f;
            if (state->active_mode == HPVE_MODE_TWIN_TRAP) {
                if (state->twin_axis == TWIN_AXIS_X) {
                    sig_frac = (geom[i].pos_x_m >= target.x) ? 0.5f : 0.0f;
                } else if (state->twin_axis == TWIN_AXIS_Y) {
                    sig_frac = (geom[i].pos_y_m >= target.y) ? 0.5f : 0.0f;
                } else if (state->twin_axis == TWIN_AXIS_DIAGONAL) {
                    sig_frac = ((geom[i].pos_x_m - target.x) + (geom[i].pos_y_m - target.y) >= 0.0f) ? 0.5f : 0.0f;
                } else {
                    bool sx = (geom[i].pos_x_m >= target.x);
                    bool sy = (geom[i].pos_y_m >= target.y);
                    sig_frac = (sx ^ sy) ? 0.5f : 0.0f;
                }
            } else if (state->active_mode == HPVE_MODE_BOTTLE_TRAP) {
                float r_xy = sqrtf(dx * dx + dy * dy);
                float r_ring = (state->bottle_radius_m > 0.005f) ? state->bottle_radius_m : 0.036f;
                sig_frac = (r_xy < r_ring) ? 0.5f : 0.0f;
            } else if (state->active_mode == HPVE_MODE_VORTEX_BEAM) {
                float azim = atan2f(geom[i].pos_y_m - target.y, geom[i].pos_x_m - target.x);
                if (azim < 0.0f) {
                    azim += 2.0f * (float)M_PI;
                }
                int charge = (state->vortex_charge != 0) ? state->vortex_charge : 1;
                sig_frac = ((float)charge * azim) / (2.0f * (float)M_PI);
            }

            float total_frac = emit_frac + sig_frac;
            total_frac = total_frac - floorf(total_frac);

            uint32_t step = (uint32_t)lroundf(total_frac * (float)NUM_PHASE_STEPS) & PHASE_STEP_MASK;
            frame_out->phase_step[i] = (uint8_t)step;
            frame_out->duty_steps[i] = duty_ov;
        }
    }

    frame_out->active_mask = 0xFFFFFFFFFFFFFFFFULL;
    if (state->active_trajectory != TRAJ_NONE_STATIC) {
        frame_out->haptic_am_depth = g_last_traj_sample.haptic_am_override;
        frame_out->r_pwm           = g_last_traj_sample.r_pwm;
        frame_out->g_pwm           = g_last_traj_sample.g_pwm;
        frame_out->b_pwm           = g_last_traj_sample.b_pwm;
    } else {
        frame_out->haptic_am_depth = (state->active_mode == HPVE_MODE_HAPTIC_FOCUS) ? 160u : 0u;
        frame_out->r_pwm           = 0u;
        frame_out->g_pwm           = 220u;
        frame_out->b_pwm           = 255u;
    }
}

void hpve_solve_frame(
    const hpve_system_state_t *state,
    const hpve_transducer_map_t *geom,
    uint32_t tick_ms,
    hpve_channel_frame_t *frame_out
) {
    if (state->use_fixed_point_solver) {
        hpve_solve_frame_fixed(state, geom, tick_ms, frame_out);
    } else {
        hpve_solve_frame_float(state, geom, tick_ms, frame_out);
    }
}

float hpve_eval_field_pressure_pa(
    const hpve_channel_frame_t *frame,
    const hpve_transducer_map_t *geom,
    vec3f_t eval_pt
) {
    if (frame->active_mask == 0ULL) {
        return 0.0f;
    }

    const float p0_ref      = 1.25f;
    const float piston_rad  = 0.0045f;
    float sum_re            = 0.0f;
    float sum_im            = 0.0f;

    for (uint8_t i = 0; i < NUM_TRANSDUCERS; ++i) {
        if ((frame->active_mask & (1ULL << i)) == 0ULL) {
            continue;
        }
        float dx = eval_pt.x - geom[i].pos_x_m;
        float dy = eval_pt.y - geom[i].pos_y_m;
        float dz = eval_pt.z;
        float r  = sqrtf(dx * dx + dy * dy + dz * dz);
        if (r < 0.001f) {
            r = 0.001f;
        }

        float sin_theta = sqrtf(dx * dx + dy * dy) / r;
        float ka_sin    = ACOUSTIC_WAVENUMBER_K * piston_rad * sin_theta;
        float directivity = 1.0f;
        if (ka_sin > 1e-4f) {
            float x2 = ka_sin * x2;
            x2 = ka_sin * ka_sin;
            directivity = 1.0f - (x2 * 0.125f) + (x2 * x2 * 0.00520833f);
            if (directivity < 0.05f) {
                directivity = 0.05f;
            }
        }

        float amp = (p0_ref * directivity) / r;
        float emit_phi = ((float)frame->phase_step[i] * 2.0f * (float)M_PI) / (float)NUM_PHASE_STEPS;
        float total_phi = emit_phi + ACOUSTIC_WAVENUMBER_K * r;

        sum_re += amp * cosf(total_phi);
        sum_im += amp * sinf(total_phi);
    }

    return sqrtf(sum_re * sum_re + sum_im * sum_im);
}
