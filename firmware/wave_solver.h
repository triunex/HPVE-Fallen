#ifndef WAVE_SOLVER_H
#define WAVE_SOLVER_H

#include "hpve_config.h"

#ifdef __cplusplus
extern "C" {
#endif

void hpve_solver_init(void);

uint32_t hpve_isqrt64(uint64_t val);
q16_t    hpve_atan2_q16(q16_t y, q16_t x);
q16_t    hpve_sin_q16(q16_t angle_rad_q16);
q16_t    hpve_cos_q16(q16_t angle_rad_q16);

void hpve_solve_frame_fixed(
    const hpve_system_state_t *state,
    const hpve_transducer_map_t *geom,
    uint32_t tick_ms,
    hpve_channel_frame_t *frame_out
);

void hpve_solve_frame_float(
    const hpve_system_state_t *state,
    const hpve_transducer_map_t *geom,
    uint32_t tick_ms,
    hpve_channel_frame_t *frame_out
);

void hpve_solve_frame(
    const hpve_system_state_t *state,
    const hpve_transducer_map_t *geom,
    uint32_t tick_ms,
    hpve_channel_frame_t *frame_out
);

float hpve_eval_field_pressure_pa(
    const hpve_channel_frame_t *frame,
    const hpve_transducer_map_t *geom,
    vec3f_t eval_pt
);

#ifdef __cplusplus
}
#endif

#endif
