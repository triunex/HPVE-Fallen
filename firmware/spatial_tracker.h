#ifndef SPATIAL_TRACKER_H
#define SPATIAL_TRACKER_H

#include "hpve_config.h"
#include "vl53l5cx_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TRACKER_GRID_DIM            8u
#define TRACKER_TOTAL_ZONES         64u
#define TRACKER_MAX_CLUSTERS        4u
#define TRACKER_MAX_FINGERS         5u
#define TRACKER_HISTORY_DEPTH       16u

#define TRACKER_RANGE_MIN_M         0.012f
#define TRACKER_RANGE_MAX_M         0.220f
#define TRACKER_FORCEFIELD_ENTER_Z  0.075f
#define TRACKER_FORCEFIELD_WALL_Z   0.055f
#define TRACKER_FORCEFIELD_MAX_PEN  0.035f

#define TRACKER_MAX_FINGER_SPEED    1.80f
#define TRACKER_OUTLIER_SIGMA_M     0.025f

typedef enum {
    TRACKER_STATE_BOOT              = 0,
    TRACKER_STATE_CALIBRATING       = 1,
    TRACKER_STATE_SEARCHING         = 2,
    TRACKER_STATE_HAND_DETECTED     = 3,
    TRACKER_STATE_FINGER_LOCKED     = 4,
    TRACKER_STATE_FORCEFIELD_ACTIVE = 5,
    TRACKER_STATE_LOST              = 6,
    TRACKER_STATE_ERROR             = 7
} tracker_state_t;

typedef enum {
    FORCEFIELD_ZONE_OUTSIDE         = 0,
    FORCEFIELD_ZONE_PROXIMITY       = 1,
    FORCEFIELD_ZONE_CONTACT         = 2,
    FORCEFIELD_ZONE_PENETRATION     = 3,
    FORCEFIELD_ZONE_SATURATION      = 4
} forcefield_zone_t;

typedef struct {
    float x;
    float y;
    float z;
    float distance_m;
    float signal_kcps;
    uint8_t status;
    uint8_t sigma_mm;
    bool valid;
} tracker_depth_pixel_t;

typedef struct {
    uint8_t pixel_indices[TRACKER_TOTAL_ZONES];
    uint8_t pixel_count;
    vec3f_t centroid;
    vec3f_t min_bounds;
    vec3f_t max_bounds;
    float total_mass;
    float depth_variance;
} tracker_point_cluster_t;

typedef struct {
    vec3f_t pos;
    vec3f_t vel;
    float P[6][6];
    float confidence;
    uint32_t active_ticks;
    uint32_t missed_ticks;
    bool is_locked;
} tracker_kalman_6d_t;

typedef struct {
    vec3f_t tip_pos;
    vec3f_t tip_vel;
    float speed_mps;
    float curvature;
    float penetration_depth_m;
    float dynamic_stiffness_npm;
    float tactile_am_depth;
    float radiation_pressure_pa;
    float normal_force_mn;
    forcefield_zone_t zone;
    bool active;
} tracker_forcefield_event_t;

typedef struct {
    tracker_state_t state;
    uint32_t frame_index;
    uint32_t timestamp_ms;
    uint8_t valid_pixels_count;
    tracker_depth_pixel_t depth_map[TRACKER_GRID_DIM][TRACKER_GRID_DIM];
    tracker_point_cluster_t clusters[TRACKER_MAX_CLUSTERS];
    uint8_t cluster_count;
    tracker_kalman_6d_t hand_filter;
    tracker_kalman_6d_t finger_filter;
    tracker_forcefield_event_t forcefield;
    vec3f_t acoustic_target;
    vec3f_t haptic_intercept_pos;
    float sensor_temp_degc;
    float sensor_fps;
    uint32_t total_forcefield_triggers;
    uint32_t total_frames_processed;
    uint32_t total_i2c_bus_errors;
} spatial_tracker_t;

void tracker_init(spatial_tracker_t *trk);
bool tracker_sensor_hardware_handshake(spatial_tracker_t *trk);
void tracker_configure_high_speed_ranging(spatial_tracker_t *trk, uint8_t frequency_hz);
bool tracker_poll_and_read_depth_map(spatial_tracker_t *trk, uint32_t now_ms);
void tracker_ingest_depth_buffer(spatial_tracker_t *trk, const vl53l5cx_raw_data_buffer_t *raw, uint32_t now_ms);
void tracker_execute_spatial_clustering(spatial_tracker_t *trk);
void tracker_extract_apex_fingertip(spatial_tracker_t *trk);
void tracker_update_kalman_filter(tracker_kalman_6d_t *kf, const vec3f_t *measured, float dt_sec, float noise_scale);
void tracker_compute_forcefield_intercept(spatial_tracker_t *trk, const hpve_system_state_t *sys, float dt_sec);
void tracker_hook_wave_solver(spatial_tracker_t *trk, hpve_system_state_t *sys);
const spatial_tracker_t *tracker_get_instance(void);

#ifdef __cplusplus
}
#endif

#endif
