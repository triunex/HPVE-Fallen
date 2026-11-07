#ifndef VL53L5CX_REGS_H
#define VL53L5CX_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VL53L5CX_DEFAULT_I2C_ADDR_7BIT      0x29u
#define VL53L5CX_DEFAULT_I2C_ADDR_8BIT      (VL53L5CX_DEFAULT_I2C_ADDR_7BIT << 1)

#define VL53L5CX_RESOL_4X4                  16u
#define VL53L5CX_RESOL_8X8                  64u
#define VL53L5CX_GRID_COLS                  8u
#define VL53L5CX_GRID_ROWS                  8u
#define VL53L5CX_TOTAL_TARGETS_PER_ZONE     1u

#define VL53L5CX_FOV_DEG                    63.0f
#define VL53L5CX_HALF_FOV_RAD               0.5497787f
#define VL53L5CX_TAN_HALF_FOV               0.6128008f
#define VL53L5CX_ZONE_ANGULAR_SPAN_RAD      (VL53L5CX_HALF_FOV_RAD * 2.0f / 8.0f)

#define VL53L5CX_REG_DEVICE_ID              0x0000u
#define VL53L5CX_EXPECTED_DEVICE_ID         0xF002u
#define VL53L5CX_REG_REVISION_ID            0x0001u
#define VL53L5CX_REG_PAGE_SELECT            0x7FFFu

#define VL53L5CX_REG_SYSTEM_STATUS          0x000Eu
#define VL53L5CX_REG_FIRMWARE_STATUS        0x0024u
#define VL53L5CX_REG_MODE_CONFIG            0x0100u
#define VL53L5CX_REG_INTEGRATION_TIME_MS    0x0104u
#define VL53L5CX_REG_RANGING_FREQ_HZ        0x0108u
#define VL53L5CX_REG_ZONE_RESOLUTION        0x010Cu
#define VL53L5CX_REG_SHARPENER_PERCENT      0x0110u
#define VL53L5CX_REG_TARGET_ORDER           0x0114u
#define VL53L5CX_REG_RANGING_COMMAND        0x0118u
#define VL53L5CX_REG_DATA_READY_STATUS      0x011Cu
#define VL53L5CX_REG_DATA_STREAM_OUTPUT     0x0200u

#define VL53L5CX_CMD_ABORT_STREAM           0x0000u
#define VL53L5CX_CMD_START_STREAM           0x0001u
#define VL53L5CX_CMD_RESET_MCU              0x0002u

#define VL53L5CX_STATUS_VALID_TARGET        5u
#define VL53L5CX_STATUS_PHASE_FAIL          4u
#define VL53L5CX_STATUS_SIGMA_FAIL          6u
#define VL53L5CX_STATUS_RANGE_COMPLETE      9u

#pragma pack(push, 1)

typedef struct {
    uint16_t distance_mm;
    uint8_t  target_status;
    uint8_t  nb_targets_detected;
    uint16_t signal_rate_kcps;
    uint16_t ambient_rate_kcps;
    uint8_t  sigma_mm;
    uint8_t  reflectance_percent;
} vl53l5cx_raw_target_t;

typedef struct {
    uint32_t stream_count;
    uint8_t  silicon_temp_degc;
    uint8_t  ranging_status;
    uint16_t reserved;
    vl53l5cx_raw_target_t targets[VL53L5CX_RESOL_8X8];
} vl53l5cx_raw_data_buffer_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif
