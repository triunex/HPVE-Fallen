#ifndef HPVE_CONFIG_H
#define HPVE_CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HPVE_FW_VERSION_MAJOR       2
#define HPVE_FW_VERSION_MINOR       7
#define HPVE_FW_VERSION_PATCH       0

#define SYS_CLOCK_KHZ               128000u
#define SYS_CLOCK_HZ                128000000u
#define PIO_CLKDIV_INT              1u
#define PIO_CLKDIV_FRAC             64u
#define PIO_CLOCK_HZ                102400000u

#define ACOUSTIC_CARRIER_HZ         40000u
#define ACOUSTIC_PERIOD_US          25u
#define SOUND_SPEED_MPS             343.0f
#define ACOUSTIC_WAVELENGTH_M       0.008575f
#define ACOUSTIC_WAVENUMBER_K       732.73298f

#define TILE_GRID_COLS              4u
#define TILE_GRID_ROWS              4u
#define TILE_CHANNELS               16u
#define MAX_CASCADE_TILES           4u
#define NUM_TRANSDUCERS             (TILE_CHANNELS * MAX_CASCADE_TILES)
#define GLOBAL_GRID_COLS            8u
#define GLOBAL_GRID_ROWS            8u

#define TRANSDUCER_PITCH_M          0.0165f
#define TRANSDUCER_CAN_DIAM_M       0.0162f
#define TRANSDUCER_LEAD_PITCH_M     0.0100f
#define TILE_SPAN_M                 (TILE_GRID_COLS * TRANSDUCER_PITCH_M)
#define ARRAY_SPAN_X_M              (GLOBAL_GRID_COLS * TRANSDUCER_PITCH_M)
#define ARRAY_SPAN_Y_M              (GLOBAL_GRID_ROWS * TRANSDUCER_PITCH_M)

#define NUM_PHASE_STEPS             16u
#define PHASE_STEP_MASK             (NUM_PHASE_STEPS - 1u)
#define WORDS_PER_SLICE             2u
#define SLICES_PER_CARRIER_PERIOD   NUM_PHASE_STEPS
#define CARRIER_CYCLES_PER_FRAME    200u
#define SLICES_PER_DMA_FRAME        (SLICES_PER_CARRIER_PERIOD * CARRIER_CYCLES_PER_FRAME)
#define WORDS_PER_DMA_FRAME         (SLICES_PER_DMA_FRAME * WORDS_PER_SLICE)

#define PIO_CYCLES_PER_CARRIER      2560u
#define PIO_CYCLES_PER_SLICE        (PIO_CYCLES_PER_CARRIER / NUM_PHASE_STEPS)
#define PIO_SHIFT_BITS_PER_SLICE    64u

#define HAPTIC_MOD_FREQ_HZ          200u
#define HAPTIC_CARRIER_RATIO        (ACOUSTIC_CARRIER_HZ / HAPTIC_MOD_FREQ_HZ)

#define MAX_MULTIFOCAL_NODES        7u
#define TRAJ_HISTORY_LEN            32u

#define PIN_I2C_SDA                 0u
#define PIN_I2C_SCL                 1u
#define PIN_SPI_MOSI                2u
#define PIN_SPI_SCK                 3u
#define PIN_SPI_LATCH               4u
#define PIN_DRV_OE                  5u
#define PIN_PHOTONIC_R              6u
#define PIN_PHOTONIC_G              7u
#define PIN_PHOTONIC_B              8u
#define PIN_SYNC_TRIG               9u
#define PIN_ENC_X_A                 10u
#define PIN_ENC_X_B                 11u
#define PIN_ENC_Y_A                 12u
#define PIN_ENC_Y_B                 13u
#define PIN_ENC_Z_A                 14u
#define PIN_ENC_Z_B                 15u
#define PIN_BTN_MODE                16u
#define PIN_BTN_ENABLE              17u
#define PIN_STATUS_LED              25u

#define WORKSPACE_MIN_X_M          -0.060f
#define WORKSPACE_MAX_X_M           0.060f
#define WORKSPACE_MIN_Y_M          -0.060f
#define WORKSPACE_MAX_Y_M           0.060f
#define WORKSPACE_MIN_Z_M           0.015f
#define WORKSPACE_MAX_Z_M           0.140f
#define WORKSPACE_DEFAULT_Z_M       0.055f

#define Q16_SHIFT                   16
#define Q16_ONE                     (1 << Q16_SHIFT)
#define Q16_HALF                    (1 << (Q16_SHIFT - 1))
#define FLOAT_TO_Q16(x)             ((int32_t)((x) * (float)Q16_ONE))
#define Q16_TO_FLOAT(x)             ((float)(x) / (float)Q16_ONE)
#define Q16_MUL(a, b)               ((int32_t)(((int64_t)(a) * (int64_t)(b)) >> Q16_SHIFT))
#define Q16_DIV(a, b)               ((int32_t)((((int64_t)(a)) << Q16_SHIFT) / (b)))

typedef int32_t q16_t;

typedef enum {
    HPVE_MODE_STANDBY         = 0,
    HPVE_MODE_TWIN_TRAP       = 1,
    HPVE_MODE_BOTTLE_TRAP     = 2,
    HPVE_MODE_VORTEX_BEAM     = 3,
    HPVE_MODE_HAPTIC_FOCUS    = 4,
    HPVE_MODE_VOLUMETRIC_POV  = 5,
    HPVE_MODE_DUAL_TRAP_SMF   = 6,
    HPVE_MODE_ARC_REACTOR_SMF = 7,
    HPVE_MODE_COUNT           = 8
} hpve_mode_t;

typedef enum {
    TRAJ_NONE_STATIC            = 0,
    TRAJ_ARC_REACTOR_CORE       = 1,
    TRAJ_ENDGAME_MOEBIUS        = 2,
    TRAJ_TREFOIL_KNOT           = 3,
    TRAJ_LISSAJOUS_RIBBON       = 4,
    TRAJ_HAPTIC_FORCEFIELD_WALL = 5,
    TRAJ_HAPTIC_DETENT_DIAL     = 6,
    TRAJ_HELIX_DOUBLE_STRAND    = 7,
    TRAJ_COUNT                  = 8
} hpve_trajectory_type_t;

typedef enum {
    TWIN_AXIS_X               = 0,
    TWIN_AXIS_Y               = 1,
    TWIN_AXIS_DIAGONAL        = 2,
    TWIN_AXIS_QUADRUPOLE      = 3
} hpve_twin_axis_t;

typedef struct {
    float x;
    float y;
    float z;
} vec3f_t;

typedef struct {
    q16_t x;
    q16_t y;
    q16_t z;
} vec3q16_t;

typedef struct {
    uint8_t  tile_id;
    uint8_t  local_ch;
    uint8_t  sr_chip;
    uint8_t  sr_pin_q;
    uint8_t  global_col;
    uint8_t  global_row;
    uint8_t  cascade_bit_pos;
    float    pos_x_m;
    float    pos_y_m;
    q16_t    pos_x_q16;
    q16_t    pos_y_q16;
    int8_t   phase_trim_steps;
} hpve_transducer_map_t;

typedef struct {
    uint8_t  phase_step[NUM_TRANSDUCERS];
    uint8_t  duty_steps[NUM_TRANSDUCERS];
    uint64_t active_mask;
    uint8_t  haptic_am_depth;
    uint8_t  r_pwm;
    uint8_t  g_pwm;
    uint8_t  b_pwm;
} hpve_channel_frame_t;

typedef struct {
    vec3f_t  primary_pos;
    vec3f_t  secondary_pos;
    vec3f_t  velocity_mps;
    float    speed_mps;
    float    curvature_inv_m;
    float    param_u_rad;
    float    param_v_m;
    uint8_t  num_active_nodes;
    vec3f_t  multifocal_nodes[MAX_MULTIFOCAL_NODES];
    float    multifocal_weights[MAX_MULTIFOCAL_NODES];
    uint8_t  multifocal_topological_sign[MAX_MULTIFOCAL_NODES];
    uint8_t  haptic_am_override;
    uint8_t  duty_step_override;
    uint8_t  r_pwm;
    uint8_t  g_pwm;
    uint8_t  b_pwm;
    bool     laser_blank_active;
} hpve_trajectory_sample_t;

typedef struct {
    volatile vec3f_t                focus_primary;
    volatile vec3f_t                focus_secondary;
    volatile hpve_mode_t            active_mode;
    volatile hpve_trajectory_type_t active_trajectory;
    volatile hpve_twin_axis_t       twin_axis;
    volatile int8_t                 vortex_charge;
    volatile float                  bottle_radius_m;
    volatile float                  haptic_mod_hz;
    volatile float                  orbit_radius_m;
    volatile float                  orbit_freq_hz;
    volatile float                  traj_scale;
    volatile float                  traj_yaw_rad;
    volatile float                  traj_pitch_rad;
    volatile float                  moebius_strip_width_m;
    volatile float                  forcefield_z_wall_m;
    volatile float                  finger_probe_z_m;
    volatile bool                   orbit_enabled;
    volatile bool                   output_enabled;
    volatile bool                   use_fixed_point_solver;
    volatile uint32_t               frame_counter;
    volatile uint32_t               dma_swap_counter;
    volatile uint32_t               dma_underrun_counter;
    volatile uint32_t               solver_time_us;
    volatile uint32_t               compile_time_us;
} hpve_system_state_t;

#ifdef __cplusplus
}
#endif

#endif
