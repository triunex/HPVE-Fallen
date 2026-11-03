#include "trajectory.h"
#include "wave_solver.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static hpve_trajectory_engine_t g_global_traj_engine;

static inline float clamp_f(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static inline vec3f_t clamp_workspace(vec3f_t p) {
    p.x = clamp_f(p.x, WORKSPACE_MIN_X_M, WORKSPACE_MAX_X_M);
    p.y = clamp_f(p.y, WORKSPACE_MIN_Y_M, WORKSPACE_MAX_Y_M);
    p.z = clamp_f(p.z, WORKSPACE_MIN_Z_M, WORKSPACE_MAX_Z_M);
    return p;
}

static inline vec3f_t rotate_euler_zy(vec3f_t local, float yaw_rad, float pitch_rad) {
    float cy = cosf(yaw_rad);
    float sy = sinf(yaw_rad);
    float cp = cosf(pitch_rad);
    float sp = sinf(pitch_rad);

    float x1 =  local.x * cp + local.z * sp;
    float y1 =  local.y;
    float z1 = -local.x * sp + local.z * cp;

    vec3f_t out;
    out.x = x1 * cy - y1 * sy;
    out.y = x1 * sy + y1 * cy;
    out.z = z1;
    return out;
}

static inline vec3f_t vec3_cross(vec3f_t a, vec3f_t b) {
    vec3f_t c;
    c.x = a.y * b.z - a.z * b.y;
    c.y = a.z * b.x - a.x * b.z;
    c.z = a.x * b.y - a.y * b.x;
    return c;
}

static inline float vec3_norm(vec3f_t v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

void hpve_trajectory_init(hpve_trajectory_engine_t *eng) {
    memset(eng, 0, sizeof(*eng));
    eng->type                  = TRAJ_NONE_STATIC;
    eng->param_u               = 0.0f;
    eng->param_v               = 0.0f;
    eng->last_tick_ms          = 0u;
    eng->active_detent_idx     = 0u;
    eng->detent_click_until_ms = 0u;
    eng->trail_head            = 0u;
    eng->trail_count           = 0u;
    for (uint8_t i = 0; i < TRAJ_HISTORY_LEN; ++i) {
        eng->trail_ring[i].x = 0.0f;
        eng->trail_ring[i].y = 0.0f;
        eng->trail_ring[i].z = WORKSPACE_DEFAULT_Z_M;
    }
    if (eng != &g_global_traj_engine) {
        memcpy(&g_global_traj_engine, eng, sizeof(*eng));
    }
}

const hpve_trajectory_engine_t* hpve_trajectory_get_global(void) {
    return &g_global_traj_engine;
}

const char* hpve_trajectory_type_name(hpve_trajectory_type_t type) {
    switch (type) {
        case TRAJ_NONE_STATIC:            return "STATIC_PT";
        case TRAJ_ARC_REACTOR_CORE:       return "ARC_REACTOR";
        case TRAJ_ENDGAME_MOEBIUS:        return "MOEBIUS_3D";
        case TRAJ_TREFOIL_KNOT:           return "TREFOIL_KNOT";
        case TRAJ_LISSAJOUS_RIBBON:       return "LISSAJOUS";
        case TRAJ_HAPTIC_FORCEFIELD_WALL: return "FORCEFIELD";
        case TRAJ_HAPTIC_DETENT_DIAL:     return "HAPTIC_DIAL";
        case TRAJ_HELIX_DOUBLE_STRAND:    return "DOUBLE_HELIX";
        default:                          return "UNKNOWN";
    }
}

void hpve_trajectory_set_type(
    hpve_trajectory_engine_t *eng,
    hpve_system_state_t *state,
    hpve_trajectory_type_t type
) {
    if (type >= TRAJ_COUNT) {
        type = TRAJ_NONE_STATIC;
    }
    eng->type        = type;
    eng->param_u     = 0.0f;
    eng->param_v     = 0.0f;
    eng->trail_head  = 0u;
    eng->trail_count = 0u;

    if (state) {
        state->active_trajectory = type;
        switch (type) {
            case TRAJ_NONE_STATIC:
                state->active_mode = HPVE_MODE_TWIN_TRAP;
                break;
            case TRAJ_ARC_REACTOR_CORE:
                state->active_mode = HPVE_MODE_ARC_REACTOR_SMF;
                break;
            case TRAJ_ENDGAME_MOEBIUS:
            case TRAJ_TREFOIL_KNOT:
            case TRAJ_LISSAJOUS_RIBBON:
                state->active_mode = HPVE_MODE_VOLUMETRIC_POV;
                break;
            case TRAJ_HAPTIC_FORCEFIELD_WALL:
            case TRAJ_HAPTIC_DETENT_DIAL:
                state->active_mode = HPVE_MODE_HAPTIC_FOCUS;
                break;
            case TRAJ_HELIX_DOUBLE_STRAND:
                state->active_mode = HPVE_MODE_DUAL_TRAP_SMF;
                break;
            default:
                break;
        }
    }
}

void hpve_trajectory_eval_moebius_point(
    float u_rad,
    float v_m,
    float major_radius_m,
    float yaw_rad,
    float pitch_rad,
    vec3f_t center,
    vec3f_t *pos_out,
    vec3f_t *tangent_out,
    float *curvature_out
) {
    const float du = 0.005f;
    float half_u = 0.5f * u_rad;
    float cu  = cosf(u_rad);
    float su  = sinf(u_rad);
    float chu = cosf(half_u);
    float shu = sinf(half_u);

    float rad_eff = major_radius_m + v_m * chu;
    vec3f_t local_p;
    local_p.x = rad_eff * cu;
    local_p.y = rad_eff * su;
    local_p.z = v_m * shu + (0.22f * major_radius_m) * sinf(u_rad);

    float u_plus = u_rad + du;
    float v_plus = v_m * cosf(3.5f * u_plus) / (cosf(3.5f * u_rad) + 1e-5f);
    (void)v_plus;

    float half_up = 0.5f * u_plus;
    float rad_p   = major_radius_m + v_m * cosf(half_up);
    vec3f_t local_pp;
    local_pp.x = rad_p * cosf(u_plus);
    local_pp.y = rad_p * sinf(u_plus);
    local_pp.z = v_m * sinf(half_up) + (0.22f * major_radius_m) * sinf(u_plus);

    float u_minus = u_rad - du;
    float half_um = 0.5f * u_minus;
    float rad_m   = major_radius_m + v_m * cosf(half_um);
    vec3f_t local_pm;
    local_pm.x = rad_m * cosf(u_minus);
    local_pm.y = rad_m * sinf(u_minus);
    local_pm.z = v_m * sinf(half_um) + (0.22f * major_radius_m) * sinf(u_minus);

    float inv_2du  = 1.0f / (2.0f * du);
    float inv_du2  = 1.0f / (du * du);

    vec3f_t r_prime = {
        (local_pp.x - local_pm.x) * inv_2du,
        (local_pp.y - local_pm.y) * inv_2du,
        (local_pp.z - local_pm.z) * inv_2du
    };

    vec3f_t r_dprime = {
        (local_pp.x - 2.0f * local_p.x + local_pm.x) * inv_du2,
        (local_pp.y - 2.0f * local_p.y + local_pm.y) * inv_du2,
        (local_pp.z - 2.0f * local_p.z + local_pm.z) * inv_du2
    };

    vec3f_t rot_p = rotate_euler_zy(local_p, yaw_rad, pitch_rad);
    rot_p.x += center.x;
    rot_p.y += center.y;
    rot_p.z += center.z;

    if (pos_out) {
        *pos_out = clamp_workspace(rot_p);
    }

    vec3f_t rot_tan = rotate_euler_zy(r_prime, yaw_rad, pitch_rad);
    if (tangent_out) {
        *tangent_out = rot_tan;
    }

    if (curvature_out) {
        vec3f_t cross_v = vec3_cross(r_prime, r_dprime);
        float num = vec3_norm(cross_v);
        float spd = vec3_norm(r_prime);
        float denom = spd * spd * spd;
        *curvature_out = (denom > 1e-6f) ? (num / denom) : 0.0f;
    }
}

void hpve_trajectory_eval_trefoil_point(
    float u_rad,
    float scale_m,
    float yaw_rad,
    float pitch_rad,
    vec3f_t center,
    vec3f_t *pos_out,
    vec3f_t *tangent_out,
    float *curvature_out
) {
    float s = scale_m * 0.32f;
    float sz = scale_m * 0.28f;

    vec3f_t local_p = {
        s  * (sinf(u_rad) + 2.0f * sinf(2.0f * u_rad)),
        s  * (cosf(u_rad) - 2.0f * cosf(2.0f * u_rad)),
        sz * (-sinf(3.0f * u_rad))
    };

    vec3f_t r_prime = {
        s  * (cosf(u_rad) + 4.0f * cosf(2.0f * u_rad)),
        s  * (-sinf(u_rad) + 4.0f * sinf(2.0f * u_rad)),
        sz * (-3.0f * cosf(3.0f * u_rad))
    };

    vec3f_t r_dprime = {
        s  * (-sinf(u_rad) - 8.0f * sinf(2.0f * u_rad)),
        s  * (-cosf(u_rad) + 8.0f * cosf(2.0f * u_rad)),
        sz * (9.0f * sinf(3.0f * u_rad))
    };

    vec3f_t rot_p = rotate_euler_zy(local_p, yaw_rad, pitch_rad);
    rot_p.x += center.x;
    rot_p.y += center.y;
    rot_p.z += center.z;

    if (pos_out) {
        *pos_out = clamp_workspace(rot_p);
    }
    if (tangent_out) {
        *tangent_out = rotate_euler_zy(r_prime, yaw_rad, pitch_rad);
    }
    if (curvature_out) {
        vec3f_t cross_v = vec3_cross(r_prime, r_dprime);
        float num = vec3_norm(cross_v);
        float spd = vec3_norm(r_prime);
        float denom = spd * spd * spd;
        *curvature_out = (denom > 1e-6f) ? (num / denom) : 0.0f;
    }
}

static void push_trail_point(hpve_trajectory_engine_t *eng, vec3f_t pt) {
    eng->trail_ring[eng->trail_head] = pt;
    eng->trail_head = (uint8_t)((eng->trail_head + 1u) % TRAJ_HISTORY_LEN);
    if (eng->trail_count < TRAJ_HISTORY_LEN) {
        eng->trail_count++;
    }
}

void hpve_trajectory_step(
    hpve_trajectory_engine_t *eng,
    const hpve_system_state_t *state,
    uint32_t now_ms,
    hpve_trajectory_sample_t *sample_out
) {
    memset(sample_out, 0, sizeof(*sample_out));

    uint32_t dt_ms = (eng->last_tick_ms == 0u || now_ms < eng->last_tick_ms)
                   ? 5u
                   : (now_ms - eng->last_tick_ms);
    if (dt_ms > 50u) {
        dt_ms = 50u;
    }
    eng->last_tick_ms = now_ms;
    float dt_sec = (float)dt_ms * 0.001f;

    hpve_trajectory_type_t active_t = state->active_trajectory;
    eng->type = active_t;

    vec3f_t base_center = state->focus_primary;
    if (base_center.z < WORKSPACE_MIN_Z_M) {
        base_center.z = WORKSPACE_DEFAULT_Z_M;
    }

    float base_rad   = (state->orbit_radius_m > 0.003f) ? state->orbit_radius_m : 0.022f;
    float scale_mult = (state->traj_scale > 0.1f) ? state->traj_scale : 1.0f;
    float radius_m   = clamp_f(base_rad * scale_mult, 0.005f, 0.045f);
    float freq_hz    = (state->orbit_freq_hz > 0.2f) ? state->orbit_freq_hz : 8.0f;
    float yaw_rad    = state->traj_yaw_rad;
    float pitch_rad  = state->traj_pitch_rad;

    switch (active_t) {
        case TRAJ_NONE_STATIC: {
            sample_out->primary_pos        = clamp_workspace(base_center);
            sample_out->secondary_pos      = clamp_workspace(state->focus_secondary);
            sample_out->num_active_nodes   = 1u;
            sample_out->multifocal_nodes[0]= sample_out->primary_pos;
            sample_out->multifocal_weights[0] = 1.0f;
            sample_out->multifocal_topological_sign[0] = 0u;
            sample_out->duty_step_override = (uint8_t)(NUM_PHASE_STEPS >> 1u);
            sample_out->haptic_am_override = (state->active_mode == HPVE_MODE_HAPTIC_FOCUS) ? 160u : 0u;
            sample_out->r_pwm              = 0u;
            sample_out->g_pwm              = 220u;
            sample_out->b_pwm              = 255u;
            sample_out->laser_blank_active = false;
            push_trail_point(eng, sample_out->primary_pos);
            break;
        }

        case TRAJ_ARC_REACTOR_CORE: {
            /*
             * Maker Faire Demo 1: The Floating Arc Reactor Core
             * 6-node hexagonal ring (R = 14.0 mm * scale) + 1 central core anchor node.
             * Slow azimuthal phase rotation locks the ring against lateral drift while
             * 450nm/490nm Cherenkov-cyan photonic excitation illuminates the ring segments.
             */
            float ring_r = clamp_f(0.014f * scale_mult, 0.008f, 0.032f);
            eng->param_u += 2.0f * (float)M_PI * 1.5f * dt_sec;
            if (eng->param_u >= 2.0f * (float)M_PI) {
                eng->param_u -= 2.0f * (float)M_PI;
            }

            sample_out->num_active_nodes = 7u;
            sample_out->multifocal_nodes[0] = clamp_workspace(base_center);
            sample_out->multifocal_weights[0] = 0.65f;
            sample_out->multifocal_topological_sign[0] = 0u;

            for (uint8_t k = 0; k < 6u; ++k) {
                float theta = eng->param_u + ((float)k * (float)M_PI / 3.0f);
                vec3f_t local_n = {
                    ring_r * cosf(theta),
                    ring_r * sinf(theta),
                    0.0f
                };
                vec3f_t rot_n = rotate_euler_zy(local_n, yaw_rad, pitch_rad);
                rot_n.x += base_center.x;
                rot_n.y += base_center.y;
                rot_n.z += base_center.z;
                sample_out->multifocal_nodes[k + 1u] = clamp_workspace(rot_n);
                sample_out->multifocal_weights[k + 1u] = 1.0f;
                sample_out->multifocal_topological_sign[k + 1u] = (uint8_t)(k & 1u);
            }

            uint8_t scan_idx = (uint8_t)(((now_ms / 2u) % 6u) + 1u);
            sample_out->primary_pos        = sample_out->multifocal_nodes[scan_idx];
            sample_out->secondary_pos      = sample_out->multifocal_nodes[0];
            sample_out->param_u_rad        = eng->param_u;
            sample_out->param_v_m          = ring_r;
            sample_out->duty_step_override = (uint8_t)(NUM_PHASE_STEPS >> 1u);
            sample_out->haptic_am_override = 0u;
            sample_out->r_pwm              = 10u;
            sample_out->g_pwm              = 235u;
            sample_out->b_pwm              = 255u;
            sample_out->laser_blank_active = false;
            push_trail_point(eng, sample_out->primary_pos);
            break;
        }

        case TRAJ_ENDGAME_MOEBIUS: {
            /*
             * Maker Faire Demo 2: Tony Stark's Endgame 3D Möbius Strip Trajectory
             * Non-orientable surface with half-twist over u in [0, 4*pi].
             * Transverse strip parameter v(u) = w * cos(3.5 * u) weaves across the ribbon
             * and closes seamlessly after two revolutions (4*pi radians).
             */
            float strip_w = (state->moebius_strip_width_m > 0.002f)
                          ? state->moebius_strip_width_m
                          : 0.0085f;
            strip_w *= scale_mult;

            vec3f_t pos, tan_vec;
            float curv = 0.0f;
            float v_cur = strip_w * sinf(5.5f * eng->param_u);

            hpve_trajectory_eval_moebius_point(
                eng->param_u, v_cur, radius_m, yaw_rad, pitch_rad,
                base_center, &pos, &tan_vec, &curv
            );

            float ds_du = vec3_norm(tan_vec);
            if (ds_du < 0.005f) {
                ds_du = 0.005f;
            }

            float target_arc_speed = 2.0f * (float)M_PI * radius_m * freq_hz;
            float curv_damp = 1.0f / (1.0f + 0.012f * curv);
            float actual_speed = target_arc_speed * curv_damp;
            float du = (actual_speed * dt_sec) / ds_du;
            if (du > 0.35f) {
                du = 0.35f;
            }

            eng->param_u += du;
            if (eng->param_u >= 4.0f * (float)M_PI) {
                eng->param_u -= 4.0f * (float)M_PI;
            }
            eng->param_v = strip_w * sinf(5.5f * eng->param_u);

            hpve_trajectory_eval_moebius_point(
                eng->param_u, eng->param_v, radius_m, yaw_rad, pitch_rad,
                base_center, &pos, &tan_vec, &curv
            );

            float twist_norm = 0.5f * (1.0f + cosf(0.5f * eng->param_u));
            uint8_t r_col = (uint8_t)(255.0f * (1.0f - twist_norm));
            uint8_t g_col = (uint8_t)(215.0f * twist_norm + 40.0f * (1.0f - twist_norm));
            uint8_t b_col = (uint8_t)(255.0f * twist_norm + 140.0f * (1.0f - twist_norm));

            sample_out->primary_pos        = pos;
            sample_out->secondary_pos      = base_center;
            sample_out->velocity_mps.x     = tan_vec.x * (du / dt_sec);
            sample_out->velocity_mps.y     = tan_vec.y * (du / dt_sec);
            sample_out->velocity_mps.z     = tan_vec.z * (du / dt_sec);
            sample_out->speed_mps          = vec3_norm(sample_out->velocity_mps);
            sample_out->curvature_inv_m    = curv;
            sample_out->param_u_rad        = eng->param_u;
            sample_out->param_v_m          = eng->param_v;
            sample_out->num_active_nodes   = 1u;
            sample_out->multifocal_nodes[0]= pos;
            sample_out->multifocal_weights[0] = 1.0f;
            sample_out->duty_step_override = (uint8_t)(NUM_PHASE_STEPS >> 1u);
            sample_out->haptic_am_override = 0u;
            sample_out->r_pwm              = r_col;
            sample_out->g_pwm              = g_col;
            sample_out->b_pwm              = b_col;
            sample_out->laser_blank_active = false;
            push_trail_point(eng, pos);
            break;
        }

        case TRAJ_TREFOIL_KNOT: {
            vec3f_t pos, tan_vec;
            float curv = 0.0f;
            hpve_trajectory_eval_trefoil_point(
                eng->param_u, radius_m, yaw_rad, pitch_rad,
                base_center, &pos, &tan_vec, &curv
            );

            float ds_du = vec3_norm(tan_vec);
            if (ds_du < 0.005f) {
                ds_du = 0.005f;
            }
            float target_speed = 2.0f * (float)M_PI * radius_m * freq_hz;
            float du = (target_speed * dt_sec) / ds_du;
            if (du > 0.30f) {
                du = 0.30f;
            }

            eng->param_u += du;
            if (eng->param_u >= 2.0f * (float)M_PI) {
                eng->param_u -= 2.0f * (float)M_PI;
            }

            hpve_trajectory_eval_trefoil_point(
                eng->param_u, radius_m, yaw_rad, pitch_rad,
                base_center, &pos, &tan_vec, &curv
            );

            sample_out->primary_pos        = pos;
            sample_out->secondary_pos      = base_center;
            sample_out->speed_mps          = target_speed;
            sample_out->curvature_inv_m    = curv;
            sample_out->param_u_rad        = eng->param_u;
            sample_out->num_active_nodes   = 1u;
            sample_out->multifocal_nodes[0]= pos;
            sample_out->multifocal_weights[0] = 1.0f;
            sample_out->duty_step_override = (uint8_t)(NUM_PHASE_STEPS >> 1u);
            sample_out->haptic_am_override = 0u;
            sample_out->r_pwm              = (uint8_t)(127.0f + 127.0f * sinf(3.0f * eng->param_u));
            sample_out->g_pwm              = (uint8_t)(127.0f + 127.0f * cosf(2.0f * eng->param_u));
            sample_out->b_pwm              = 255u;
            push_trail_point(eng, pos);
            break;
        }

        case TRAJ_LISSAJOUS_RIBBON: {
            eng->param_u += 2.0f * (float)M_PI * freq_hz * dt_sec;
            if (eng->param_u >= 2.0f * (float)M_PI) {
                eng->param_u -= 2.0f * (float)M_PI;
            }
            float precession = (float)(now_ms % 8000u) * (2.0f * (float)M_PI / 8000.0f);
            vec3f_t local_p = {
                radius_m * sinf(eng->param_u + precession),
                radius_m * sinf(2.0f * eng->param_u),
                (radius_m * 0.40f) * sinf(3.0f * eng->param_u)
            };
            vec3f_t rot_p = rotate_euler_zy(local_p, yaw_rad, pitch_rad);
            rot_p.x += base_center.x;
            rot_p.y += base_center.y;
            rot_p.z += base_center.z;
            rot_p = clamp_workspace(rot_p);

            sample_out->primary_pos        = rot_p;
            sample_out->secondary_pos      = base_center;
            sample_out->param_u_rad        = eng->param_u;
            sample_out->num_active_nodes   = 1u;
            sample_out->multifocal_nodes[0]= rot_p;
            sample_out->multifocal_weights[0] = 1.0f;
            sample_out->duty_step_override = (uint8_t)(NUM_PHASE_STEPS >> 1u);
            sample_out->haptic_am_override = 0u;
            sample_out->r_pwm              = 80u;
            sample_out->g_pwm              = 255u;
            sample_out->b_pwm              = 180u;
            push_trail_point(eng, rot_p);
            break;
        }

        case TRAJ_HAPTIC_FORCEFIELD_WALL: {
            /*
             * Maker Faire Demo 3: The Touchable 200 Hz Mid-Air Forcefield Barrier
             * Combines Spatiotemporal Modulation (STM) at v_stm ~ 6.5 m/s across a
             * protective barrier patch with 200 Hz Pacinian envelope modulation and
             * penetration-depth virtual mechanical spring stiffness.
             */
            float z_wall = (state->forcefield_z_wall_m > 0.020f)
                         ? state->forcefield_z_wall_m
                         : 0.065f;
            float z_finger = (state->finger_probe_z_m > 0.015f)
                           ? state->finger_probe_z_m
                           : base_center.z;

            float penetration_m = (z_finger < z_wall) ? (z_wall - z_finger) : 0.0f;
            float norm_pen      = clamp_f(penetration_m / 0.030f, 0.0f, 1.0f);

            float stm_rad = clamp_f(0.012f * (1.0f - 0.45f * norm_pen), 0.004f, 0.020f);
            float stm_freq = 75.0f;
            eng->param_u += 2.0f * (float)M_PI * stm_freq * dt_sec;
            if (eng->param_u >= 2.0f * (float)M_PI) {
                eng->param_u -= 2.0f * (float)M_PI;
            }

            vec3f_t pos = {
                base_center.x + stm_rad * cosf(eng->param_u),
                base_center.y + stm_rad * sinf(eng->param_u),
                (penetration_m > 0.0f) ? z_finger : z_wall
            };
            pos = clamp_workspace(pos);

            uint8_t am_depth = (uint8_t)(128.0f + 127.0f * norm_pen);
            uint8_t duty_steps = (uint8_t)((NUM_PHASE_STEPS >> 1u) - (uint8_t)(2.0f * (1.0f - norm_pen)));
            if (duty_steps < 4u) {
                duty_steps = 4u;
            }

            sample_out->primary_pos        = pos;
            sample_out->secondary_pos      = pos;
            sample_out->speed_mps          = 2.0f * (float)M_PI * stm_rad * stm_freq;
            sample_out->param_u_rad        = eng->param_u;
            sample_out->param_v_m          = penetration_m;
            sample_out->num_active_nodes   = 1u;
            sample_out->multifocal_nodes[0]= pos;
            sample_out->multifocal_weights[0] = 1.0f;
            sample_out->duty_step_override = duty_steps;
            sample_out->haptic_am_override = am_depth;
            sample_out->r_pwm              = 255u;
            sample_out->g_pwm              = (uint8_t)(140.0f * (1.0f - norm_pen));
            sample_out->b_pwm              = (uint8_t)(40.0f * (1.0f - norm_pen));
            push_trail_point(eng, pos);
            break;
        }

        case TRAJ_HAPTIC_DETENT_DIAL: {
            float dial_r = clamp_f(0.018f * scale_mult, 0.010f, 0.035f);
            eng->param_u += 2.0f * (float)M_PI * 1.2f * dt_sec;
            if (eng->param_u >= 2.0f * (float)M_PI) {
                eng->param_u -= 2.0f * (float)M_PI;
            }

            uint8_t detent_idx = (uint8_t)((eng->param_u / (2.0f * (float)M_PI)) * 24.0f) % 24u;
            if (detent_idx != eng->active_detent_idx) {
                eng->active_detent_idx = detent_idx;
                eng->detent_click_until_ms = now_ms + 8u;
            }

            bool clicking = (now_ms <= eng->detent_click_until_ms);
            float snapped_angle = ((float)detent_idx * 2.0f * (float)M_PI) / 24.0f;

            vec3f_t pos = {
                base_center.x + dial_r * cosf(snapped_angle),
                base_center.y + dial_r * sinf(snapped_angle),
                base_center.z
            };
            pos = clamp_workspace(pos);

            sample_out->primary_pos        = pos;
            sample_out->secondary_pos      = base_center;
            sample_out->param_u_rad        = snapped_angle;
            sample_out->num_active_nodes   = 1u;
            sample_out->multifocal_nodes[0]= pos;
            sample_out->multifocal_weights[0] = 1.0f;
            sample_out->duty_step_override = clicking ? (uint8_t)(NUM_PHASE_STEPS >> 1u) : 5u;
            sample_out->haptic_am_override = clicking ? 220u : 90u;
            sample_out->r_pwm              = clicking ? 255u : 0u;
            sample_out->g_pwm              = clicking ? 255u : 180u;
            sample_out->b_pwm              = 255u;
            push_trail_point(eng, pos);
            break;
        }

        case TRAJ_HELIX_DOUBLE_STRAND: {
            eng->param_u += 2.0f * (float)M_PI * freq_hz * dt_sec;
            if (eng->param_u >= 2.0f * (float)M_PI) {
                eng->param_u -= 2.0f * (float)M_PI;
            }
            float h_rad = clamp_f(0.016f * scale_mult, 0.008f, 0.032f);
            float z_mod = 0.015f * sinf(0.5f * eng->param_u);

            vec3f_t strand_a = {
                base_center.x + h_rad * cosf(eng->param_u),
                base_center.y + h_rad * sinf(eng->param_u),
                base_center.z + z_mod
            };
            vec3f_t strand_b = {
                base_center.x + h_rad * cosf(eng->param_u + (float)M_PI),
                base_center.y + h_rad * sinf(eng->param_u + (float)M_PI),
                base_center.z - z_mod
            };

            sample_out->primary_pos        = clamp_workspace(strand_a);
            sample_out->secondary_pos      = clamp_workspace(strand_b);
            sample_out->param_u_rad        = eng->param_u;
            sample_out->num_active_nodes   = 2u;
            sample_out->multifocal_nodes[0]= sample_out->primary_pos;
            sample_out->multifocal_nodes[1]= sample_out->secondary_pos;
            sample_out->multifocal_weights[0] = 1.0f;
            sample_out->multifocal_weights[1] = 1.0f;
            sample_out->duty_step_override = (uint8_t)(NUM_PHASE_STEPS >> 1u);
            sample_out->haptic_am_override = 0u;
            sample_out->r_pwm              = 160u;
            sample_out->g_pwm              = 255u;
            sample_out->b_pwm              = 90u;
            push_trail_point(eng, sample_out->primary_pos);
            break;
        }

        default:
            break;
    }

    if (eng != &g_global_traj_engine) {
        memcpy(&g_global_traj_engine, eng, sizeof(*eng));
    }
}
