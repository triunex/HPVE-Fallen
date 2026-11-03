#ifndef HPVE_TRAJECTORY_H
#define HPVE_TRAJECTORY_H

#include "hpve_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    hpve_trajectory_type_t type;
    float                  param_u;
    float                  param_v;
    uint32_t               last_tick_ms;
    uint8_t                active_detent_idx;
    uint32_t               detent_click_until_ms;
    vec3f_t                trail_ring[TRAJ_HISTORY_LEN];
    uint8_t                trail_head;
    uint8_t                trail_count;
} hpve_trajectory_engine_t;

void hpve_trajectory_init(hpve_trajectory_engine_t *eng);

void hpve_trajectory_set_type(
    hpve_trajectory_engine_t *eng,
    hpve_system_state_t *state,
    hpve_trajectory_type_t type
);

const char* hpve_trajectory_type_name(hpve_trajectory_type_t type);

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
);

void hpve_trajectory_eval_trefoil_point(
    float u_rad,
    float scale_m,
    float yaw_rad,
    float pitch_rad,
    vec3f_t center,
    vec3f_t *pos_out,
    vec3f_t *tangent_out,
    float *curvature_out
);

void hpve_trajectory_step(
    hpve_trajectory_engine_t *eng,
    const hpve_system_state_t *state,
    uint32_t now_ms,
    hpve_trajectory_sample_t *sample_out
);

const hpve_trajectory_engine_t* hpve_trajectory_get_global(void);

#ifdef __cplusplus
}
#endif

#endif
