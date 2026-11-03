#include "ui_telemetry.h"
#include "pio_cascade_driver.h"
#include "trajectory.h"
#include "wave_solver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if defined(PICO_RP2040) || defined(PICO_BUILD)
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#define HPVE_UI_ON_HW 1
#else
#define HPVE_UI_ON_HW 0
#endif

static uint8_t  g_oled_fb[OLED_FB_BYTES];
static uint8_t  g_enc_prev_ab[3]       = {0u, 0u, 0u};
static uint32_t g_enc_last_step_ms[3]  = {0u, 0u, 0u};
static bool     g_btn_mode_prev        = true;
static bool     g_btn_en_prev          = true;
static uint32_t g_btn_last_ms          = 0u;
static char     g_serial_rx_buf[128];
static size_t   g_serial_rx_len        = 0u;

static const int8_t k_quad_table[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
};

static const uint8_t k_font5x7[96][5] = {
    {0x00,0x00,0x00,0x00,0x00}, {0x00,0x00,0x5F,0x00,0x00}, {0x00,0x07,0x00,0x07,0x00}, {0x14,0x7F,0x14,0x7F,0x14},
    {0x24,0x2A,0x7F,0x2A,0x12}, {0x23,0x13,0x08,0x64,0x62}, {0x36,0x49,0x55,0x22,0x50}, {0x00,0x05,0x03,0x00,0x00},
    {0x00,0x1C,0x22,0x41,0x00}, {0x00,0x41,0x22,0x1C,0x00}, {0x14,0x08,0x3E,0x08,0x14}, {0x08,0x08,0x3E,0x08,0x08},
    {0x00,0x50,0x30,0x00,0x00}, {0x08,0x08,0x08,0x08,0x08}, {0x00,0x60,0x60,0x00,0x00}, {0x20,0x10,0x08,0x04,0x02},
    {0x3E,0x51,0x49,0x45,0x3E}, {0x00,0x42,0x7F,0x40,0x00}, {0x42,0x61,0x51,0x49,0x46}, {0x21,0x41,0x45,0x4B,0x31},
    {0x18,0x14,0x12,0x7F,0x10}, {0x27,0x45,0x45,0x45,0x39}, {0x3C,0x4A,0x49,0x49,0x30}, {0x01,0x71,0x09,0x05,0x03},
    {0x36,0x49,0x49,0x49,0x36}, {0x06,0x49,0x49,0x29,0x1E}, {0x00,0x36,0x36,0x00,0x00}, {0x00,0x56,0x36,0x00,0x00},
    {0x08,0x14,0x22,0x41,0x00}, {0x14,0x14,0x14,0x14,0x14}, {0x00,0x41,0x22,0x14,0x08}, {0x02,0x01,0x51,0x09,0x06},
    {0x32,0x49,0x79,0x41,0x3E}, {0x7E,0x11,0x11,0x11,0x7E}, {0x7F,0x49,0x49,0x49,0x36}, {0x3E,0x41,0x41,0x41,0x22},
    {0x7F,0x41,0x41,0x22,0x1C}, {0x7F,0x49,0x49,0x49,0x41}, {0x7F,0x09,0x09,0x09,0x01}, {0x3E,0x41,0x49,0x49,0x7A},
    {0x7F,0x08,0x08,0x08,0x7F}, {0x00,0x41,0x7F,0x41,0x00}, {0x20,0x40,0x41,0x3F,0x01}, {0x7F,0x08,0x14,0x22,0x41},
    {0x7F,0x40,0x40,0x40,0x40}, {0x7F,0x02,0x0C,0x02,0x7F}, {0x7F,0x04,0x08,0x10,0x7F}, {0x3E,0x41,0x41,0x41,0x3E},
    {0x7F,0x09,0x09,0x09,0x06}, {0x3E,0x41,0x51,0x21,0x5E}, {0x7F,0x09,0x19,0x29,0x46}, {0x46,0x49,0x49,0x49,0x31},
    {0x01,0x01,0x7F,0x01,0x01}, {0x3F,0x40,0x40,0x40,0x3F}, {0x1F,0x20,0x40,0x20,0x1F}, {0x3F,0x40,0x38,0x40,0x3F},
    {0x63,0x14,0x08,0x14,0x63}, {0x07,0x08,0x70,0x08,0x07}, {0x61,0x51,0x49,0x45,0x43}, {0x00,0x7F,0x41,0x41,0x00},
    {0x02,0x04,0x08,0x10,0x20}, {0x00,0x41,0x41,0x7F,0x00}, {0x04,0x02,0x01,0x02,0x04}, {0x40,0x40,0x40,0x40,0x40},
    {0x00,0x01,0x02,0x04,0x00}, {0x20,0x54,0x54,0x54,0x78}, {0x7F,0x48,0x44,0x44,0x38}, {0x38,0x44,0x44,0x44,0x20},
    {0x38,0x44,0x44,0x48,0x7F}, {0x38,0x54,0x54,0x54,0x18}, {0x08,0x7E,0x09,0x01,0x02}, {0x0C,0x52,0x52,0x52,0x3E},
    {0x7F,0x08,0x04,0x04,0x78}, {0x00,0x44,0x7D,0x40,0x00}, {0x20,0x40,0x44,0x3D,0x00}, {0x7F,0x10,0x28,0x44,0x00},
    {0x00,0x41,0x7F,0x40,0x00}, {0x7C,0x04,0x18,0x04,0x78}, {0x7C,0x08,0x04,0x04,0x78}, {0x38,0x44,0x44,0x44,0x38},
    {0x7C,0x14,0x14,0x14,0x08}, {0x08,0x14,0x14,0x18,0x7C}, {0x7C,0x08,0x04,0x04,0x08}, {0x48,0x54,0x54,0x54,0x20},
    {0x04,0x3F,0x44,0x40,0x20}, {0x3C,0x40,0x40,0x20,0x7C}, {0x1C,0x20,0x40,0x20,0x1C}, {0x3C,0x40,0x30,0x40,0x3C},
    {0x44,0x28,0x10,0x28,0x44}, {0x0C,0x50,0x50,0x50,0x3C}, {0x44,0x64,0x54,0x4C,0x44}, {0x00,0x08,0x36,0x41,0x00},
    {0x00,0x00,0x7F,0x00,0x00}, {0x00,0x41,0x36,0x08,0x00}, {0x10,0x08,0x08,0x10,0x08}, {0x78,0x46,0x41,0x46,0x78}
};

void hpve_oled_clear(uint8_t *fb) {
    memset(fb, 0, OLED_FB_BYTES);
}

void hpve_oled_set_pixel(uint8_t *fb, int x, int y, bool on) {
    if (x < 0 || x >= (int)OLED_WIDTH || y < 0 || y >= (int)OLED_HEIGHT) {
        return;
    }
    uint32_t idx = (uint32_t)(y >> 3) * OLED_WIDTH + (uint32_t)x;
    uint8_t  bit = (uint8_t)(1u << (y & 7));
    if (on) {
        fb[idx] |= bit;
    } else {
        fb[idx] &= (uint8_t)~bit;
    }
}

void hpve_oled_draw_line(uint8_t *fb, int x0, int y0, int x1, int y1, bool on) {
    int dx = abs(x1 - x0);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    for (;;) {
        hpve_oled_set_pixel(fb, x0, y0, on);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void hpve_oled_draw_rect(uint8_t *fb, int x, int y, int w, int h, bool on) {
    hpve_oled_draw_line(fb, x, y, x + w - 1, y, on);
    hpve_oled_draw_line(fb, x, y + h - 1, x + w - 1, y + h - 1, on);
    hpve_oled_draw_line(fb, x, y, x, y + h - 1, on);
    hpve_oled_draw_line(fb, x + w - 1, y, x + w - 1, y + h - 1, on);
}

void hpve_oled_draw_char(uint8_t *fb, int x, int y, char c) {
    if (c < 32 || c > 127) {
        c = '?';
    }
    const uint8_t *glyph = k_font5x7[(int)c - 32];
    for (int col = 0; col < 5; ++col) {
        uint8_t col_bits = glyph[col];
        for (int row = 0; row < 7; ++row) {
            if (col_bits & (1u << row)) {
                hpve_oled_set_pixel(fb, x + col, y + row, true);
            }
        }
    }
}

void hpve_oled_draw_str(uint8_t *fb, int x, int y, const char *str) {
    int cur_x = x;
    while (*str) {
        hpve_oled_draw_char(fb, cur_x, y, *str++);
        cur_x += 6;
    }
}

static const char* mode_short_name(hpve_mode_t mode) {
    switch (mode) {
        case HPVE_MODE_STANDBY:         return "STBY";
        case HPVE_MODE_TWIN_TRAP:       return "TWIN";
        case HPVE_MODE_BOTTLE_TRAP:     return "BOTL";
        case HPVE_MODE_VORTEX_BEAM:     return "VRTX";
        case HPVE_MODE_HAPTIC_FOCUS:    return "HAPT";
        case HPVE_MODE_VOLUMETRIC_POV:  return "VPOV";
        case HPVE_MODE_DUAL_TRAP_SMF:   return "DUAL";
        case HPVE_MODE_ARC_REACTOR_SMF: return "ARCR";
        default:                        return "UNKN";
    }
}

static inline void project_3d_to_hud(vec3f_t pt, int *px_out, int *py_out) {
    int px = 77 + (int)(pt.x * 260.0f) + (int)(pt.y * 90.0f);
    int py = 56 - (int)((pt.z - WORKSPACE_MIN_Z_M) * 320.0f) - (int)(pt.y * 70.0f);
    if (px < 61) px = 61;
    if (px > 92) px = 92;
    if (py < 15) py = 15;
    if (py > 58) py = 58;
    *px_out = px;
    *py_out = py;
}

void hpve_ui_render_hud(
    uint8_t *fb,
    const hpve_system_state_t *state,
    const hpve_channel_frame_t *last_frame
) {
    hpve_oled_clear(fb);

    const hpve_trajectory_sample_t *tsamp = hpve_solver_get_last_trajectory_sample();
    vec3f_t disp_pt = (state->active_trajectory != TRAJ_NONE_STATIC)
                    ? tsamp->primary_pos
                    : state->focus_primary;

    char line[32];
    snprintf(line, sizeof(line), "%s %s",
             mode_short_name(state->active_mode),
             hpve_trajectory_type_name(state->active_trajectory));
    hpve_oled_draw_str(fb, 0, 0, line);
    hpve_oled_draw_line(fb, 0, 9, 127, 9, true);

    int32_t xm_tenths = (int32_t)(disp_pt.x * 10000.0f);
    int32_t ym_tenths = (int32_t)(disp_pt.y * 10000.0f);
    int32_t zm_tenths = (int32_t)(disp_pt.z * 10000.0f);

    snprintf(line, sizeof(line), "X:%+3d.%01dmm", (int)(xm_tenths / 10), abs((int)(xm_tenths % 10)));
    hpve_oled_draw_str(fb, 0, 13, line);

    snprintf(line, sizeof(line), "Y:%+3d.%01dmm", (int)(ym_tenths / 10), abs((int)(ym_tenths % 10)));
    hpve_oled_draw_str(fb, 0, 22, line);

    snprintf(line, sizeof(line), "Z:%+3d.%01dmm", (int)(zm_tenths / 10), abs((int)(zm_tenths % 10)));
    hpve_oled_draw_str(fb, 0, 31, line);

    snprintf(line, sizeof(line), "SLV:%3uus", (unsigned)state->solver_time_us);
    hpve_oled_draw_str(fb, 0, 42, line);

    snprintf(line, sizeof(line), "FRM:%5lu", (unsigned long)(state->frame_counter % 100000u));
    hpve_oled_draw_str(fb, 0, 52, line);

    hpve_oled_draw_rect(fb, 60, 14, 34, 46, true);

    const hpve_trajectory_engine_t *teng = hpve_trajectory_get_global();
    if (teng->trail_count > 1u) {
        int prev_x = 0;
        int prev_y = 0;
        for (uint8_t k = 0; k < teng->trail_count; ++k) {
            uint8_t idx = (uint8_t)((teng->trail_head + TRAJ_HISTORY_LEN - teng->trail_count + k) % TRAJ_HISTORY_LEN);
            int cur_x, cur_y;
            project_3d_to_hud(teng->trail_ring[idx], &cur_x, &cur_y);
            if (k > 0u) {
                hpve_oled_draw_line(fb, prev_x, prev_y, cur_x, cur_y, true);
            }
            prev_x = cur_x;
            prev_y = cur_y;
        }
    }

    int fx_px, fy_px;
    project_3d_to_hud(disp_pt, &fx_px, &fy_px);
    hpve_oled_draw_line(fb, fx_px - 3, fy_px, fx_px + 3, fy_px, true);
    hpve_oled_draw_line(fb, fx_px, fy_px - 3, fx_px, fy_px + 3, true);

    hpve_oled_draw_rect(fb, 97, 13, 27, 27, true);
    const hpve_transducer_map_t *geom = hpve_driver_get_geometry_table();
    for (uint8_t ch = 0; ch < NUM_TRANSDUCERS; ++ch) {
        uint8_t gc = geom[ch].global_col;
        uint8_t gr = geom[ch].global_row;
        int px = 99 + (int)gc * 3;
        int py = 15 + (int)gr * 3;
        bool ph_high = (last_frame->phase_step[ch] >= (NUM_PHASE_STEPS >> 1u));
        hpve_oled_set_pixel(fb, px, py, true);
        if (ph_high) {
            hpve_oled_set_pixel(fb, px + 1, py, true);
            hpve_oled_set_pixel(fb, px, py + 1, true);
            hpve_oled_set_pixel(fb, px + 1, py + 1, true);
        }
    }
}

const uint8_t* hpve_ui_get_framebuffer(void) {
    return g_oled_fb;
}

void hpve_ui_flush_oled(const uint8_t *fb) {
#if HPVE_UI_ON_HW
    uint8_t cmd_buf[7] = {0x00, 0x21, 0, 127, 0x22, 0, 7};
    i2c_write_blocking(i2c0, SSD1306_I2C_ADDR, cmd_buf, sizeof(cmd_buf), false);

    uint8_t tx_pkt[OLED_FB_BYTES + 1];
    tx_pkt[0] = 0x40;
    memcpy(&tx_pkt[1], fb, OLED_FB_BYTES);
    i2c_write_blocking(i2c0, SSD1306_I2C_ADDR, tx_pkt, sizeof(tx_pkt), false);
#else
    (void)fb;
#endif
}

static float clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void hpve_ui_init(hpve_system_state_t *state) {
    hpve_oled_clear(g_oled_fb);
    (void)state;

#if HPVE_UI_ON_HW
    i2c_init(i2c0, 400000u);
    gpio_set_function(PIN_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_I2C_SDA);
    gpio_pull_up(PIN_I2C_SCL);

    const uint8_t init_seq[] = {
        0x00, 0xAE, 0x20, 0x00, 0xB0, 0xC8, 0x00, 0x10, 0x40,
        0x81, 0xCF, 0xA1, 0xA6, 0xA8, 0x3F, 0xA4, 0xD3, 0x00,
        0xD5, 0x80, 0xD9, 0xF1, 0xDA, 0x12, 0xDB, 0x40, 0x8D,
        0x14, 0xAF
    };
    i2c_write_blocking(i2c0, SSD1306_I2C_ADDR, init_seq, sizeof(init_seq), false);

    const uint8_t in_pins[] = {
        PIN_ENC_X_A, PIN_ENC_X_B,
        PIN_ENC_Y_A, PIN_ENC_Y_B,
        PIN_ENC_Z_A, PIN_ENC_Z_B,
        PIN_BTN_MODE, PIN_BTN_ENABLE
    };
    for (size_t i = 0; i < sizeof(in_pins); ++i) {
        gpio_init(in_pins[i]);
        gpio_set_dir(in_pins[i], GPIO_IN);
        gpio_pull_up(in_pins[i]);
    }
#endif
}

void hpve_ui_poll_inputs(hpve_system_state_t *state, uint32_t now_ms) {
#if HPVE_UI_ON_HW
    const uint8_t pin_pairs[3][2] = {
        {PIN_ENC_X_A, PIN_ENC_X_B},
        {PIN_ENC_Y_A, PIN_ENC_Y_B},
        {PIN_ENC_Z_A, PIN_ENC_Z_B}
    };

    for (int axis = 0; axis < 3; ++axis) {
        uint8_t a = gpio_get(pin_pairs[axis][0]) ? 1u : 0u;
        uint8_t b = gpio_get(pin_pairs[axis][1]) ? 1u : 0u;
        uint8_t cur_ab = (uint8_t)((a << 1u) | b);
        uint8_t idx = (uint8_t)((g_enc_prev_ab[axis] << 2u) | cur_ab) & 0x0Fu;
        int8_t dir = k_quad_table[idx];
        g_enc_prev_ab[axis] = cur_ab;

        if (dir != 0) {
            uint32_t dt = now_ms - g_enc_last_step_ms[axis];
            g_enc_last_step_ms[axis] = now_ms;
            float step_m = (dt < 18u) ? 0.0010f : 0.00025f;
            float delta = (float)dir * step_m;

            if (state->active_trajectory == TRAJ_ENDGAME_MOEBIUS || state->active_trajectory == TRAJ_TREFOIL_KNOT) {
                if (axis == 0) {
                    state->traj_yaw_rad += (float)dir * 0.06f;
                } else if (axis == 1) {
                    state->traj_pitch_rad = clampf(state->traj_pitch_rad + (float)dir * 0.05f, -1.2f, 1.2f);
                } else {
                    state->traj_scale = clampf(state->traj_scale + (float)dir * 0.05f, 0.4f, 1.8f);
                }
            } else {
                if (axis == 0) {
                    state->focus_primary.x = clampf(state->focus_primary.x + delta, WORKSPACE_MIN_X_M, WORKSPACE_MAX_X_M);
                } else if (axis == 1) {
                    state->focus_primary.y = clampf(state->focus_primary.y + delta, WORKSPACE_MIN_Y_M, WORKSPACE_MAX_Y_M);
                } else {
                    state->focus_primary.z = clampf(state->focus_primary.z + delta, WORKSPACE_MIN_Z_M, WORKSPACE_MAX_Z_M);
                }
            }
        }
    }

    bool btn_mode = gpio_get(PIN_BTN_MODE);
    bool btn_en   = gpio_get(PIN_BTN_ENABLE);

    if (!btn_mode && g_btn_mode_prev && (now_ms - g_btn_last_ms > 180u)) {
        g_btn_last_ms = now_ms;
        uint8_t next_t = (uint8_t)state->active_trajectory + 1u;
        if (next_t >= (uint8_t)TRAJ_COUNT) {
            next_t = 0u;
        }
        hpve_trajectory_engine_t tmp_eng;
        hpve_trajectory_init(&tmp_eng);
        hpve_trajectory_set_type(&tmp_eng, state, (hpve_trajectory_type_t)next_t);
    }
    if (!btn_en && g_btn_en_prev && (now_ms - g_btn_last_ms > 180u)) {
        g_btn_last_ms = now_ms;
        hpve_driver_set_output_enable(!state->output_enabled);
    }

    g_btn_mode_prev = btn_mode;
    g_btn_en_prev   = btn_en;
#else
    (void)state;
    (void)now_ms;
#endif
}

static bool parse_float_token(const char *line, char prefix, float *out_val) {
    const char *p = line;
    while (*p) {
        if (toupper((unsigned char)*p) == toupper((unsigned char)prefix)) {
            char *endptr = NULL;
            float val = strtof(p + 1, &endptr);
            if (endptr != (p + 1)) {
                *out_val = val;
                return true;
            }
        }
        ++p;
    }
    return false;
}

void hpve_ui_process_serial_line(
    hpve_system_state_t *state,
    const char *line,
    char *reply_buf,
    size_t reply_max
) {
    while (*line == ' ' || *line == '\t') {
        ++line;
    }
    if (*line == '\0' || *line == '#') {
        if (reply_max > 0) reply_buf[0] = '\0';
        return;
    }

    if (strncmp(line, "G0", 2) == 0 || strncmp(line, "G1", 2) == 0) {
        float x_mm = state->focus_primary.x * 1000.0f;
        float y_mm = state->focus_primary.y * 1000.0f;
        float z_mm = state->focus_primary.z * 1000.0f;
        parse_float_token(line + 2, 'X', &x_mm);
        parse_float_token(line + 2, 'Y', &y_mm);
        parse_float_token(line + 2, 'Z', &z_mm);

        state->focus_primary.x = clampf(x_mm * 0.001f, WORKSPACE_MIN_X_M, WORKSPACE_MAX_X_M);
        state->focus_primary.y = clampf(y_mm * 0.001f, WORKSPACE_MIN_Y_M, WORKSPACE_MAX_Y_M);
        state->focus_primary.z = clampf(z_mm * 0.001f, WORKSPACE_MIN_Z_M, WORKSPACE_MAX_Z_M);

        snprintf(reply_buf, reply_max, "OK G0 X=%.2f Y=%.2f Z=%.2f mm\n",
                 state->focus_primary.x * 1000.0f,
                 state->focus_primary.y * 1000.0f,
                 state->focus_primary.z * 1000.0f);
        return;
    }

    if (strncmp(line, "G2", 2) == 0) {
        float x_mm = state->focus_secondary.x * 1000.0f;
        float y_mm = state->focus_secondary.y * 1000.0f;
        float z_mm = state->focus_secondary.z * 1000.0f;
        parse_float_token(line + 2, 'X', &x_mm);
        parse_float_token(line + 2, 'Y', &y_mm);
        parse_float_token(line + 2, 'Z', &z_mm);

        state->focus_secondary.x = clampf(x_mm * 0.001f, WORKSPACE_MIN_X_M, WORKSPACE_MAX_X_M);
        state->focus_secondary.y = clampf(y_mm * 0.001f, WORKSPACE_MIN_Y_M, WORKSPACE_MAX_Y_M);
        state->focus_secondary.z = clampf(z_mm * 0.001f, WORKSPACE_MIN_Z_M, WORKSPACE_MAX_Z_M);

        snprintf(reply_buf, reply_max, "OK G2 X2=%.2f Y2=%.2f Z2=%.2f mm\n",
                 state->focus_secondary.x * 1000.0f,
                 state->focus_secondary.y * 1000.0f,
                 state->focus_secondary.z * 1000.0f);
        return;
    }

    if (strncmp(line, "M100", 4) == 0) {
        state->active_trajectory = TRAJ_NONE_STATIC;
        state->active_mode       = HPVE_MODE_STANDBY;
        hpve_driver_set_output_enable(false);
        snprintf(reply_buf, reply_max, "OK MODE=STANDBY\n");
        return;
    }
    if (strncmp(line, "M101", 4) == 0) {
        state->active_trajectory = TRAJ_NONE_STATIC;
        state->active_mode       = HPVE_MODE_TWIN_TRAP;
        hpve_driver_set_output_enable(true);
        snprintf(reply_buf, reply_max, "OK MODE=TWIN_TRAP\n");
        return;
    }
    if (strncmp(line, "M102", 4) == 0) {
        state->active_trajectory = TRAJ_NONE_STATIC;
        state->active_mode       = HPVE_MODE_BOTTLE_TRAP;
        hpve_driver_set_output_enable(true);
        snprintf(reply_buf, reply_max, "OK MODE=BOTTLE_TRAP\n");
        return;
    }
    if (strncmp(line, "M103", 4) == 0) {
        state->active_trajectory = TRAJ_NONE_STATIC;
        state->active_mode       = HPVE_MODE_VORTEX_BEAM;
        float l_val = (float)state->vortex_charge;
        if (parse_float_token(line + 4, 'L', &l_val)) {
            int c = (int)l_val;
            if (c != 0 && c >= -4 && c <= 4) {
                state->vortex_charge = (int8_t)c;
            }
        }
        hpve_driver_set_output_enable(true);
        snprintf(reply_buf, reply_max, "OK MODE=VORTEX_BEAM L=%d\n", (int)state->vortex_charge);
        return;
    }
    if (strncmp(line, "M104", 4) == 0) {
        state->active_trajectory = TRAJ_NONE_STATIC;
        state->active_mode       = HPVE_MODE_HAPTIC_FOCUS;
        hpve_driver_set_output_enable(true);
        snprintf(reply_buf, reply_max, "OK MODE=HAPTIC_FOCUS\n");
        return;
    }
    if (strncmp(line, "M105", 4) == 0) {
        state->active_trajectory = TRAJ_NONE_STATIC;
        state->active_mode       = HPVE_MODE_VOLUMETRIC_POV;
        hpve_driver_set_output_enable(true);
        snprintf(reply_buf, reply_max, "OK MODE=VOLUMETRIC_POV\n");
        return;
    }
    if (strncmp(line, "M106", 4) == 0) {
        state->active_trajectory = TRAJ_NONE_STATIC;
        state->active_mode       = HPVE_MODE_DUAL_TRAP_SMF;
        hpve_driver_set_output_enable(true);
        snprintf(reply_buf, reply_max, "OK MODE=DUAL_TRAP_SMF\n");
        return;
    }

    if (strncmp(line, "M20", 3) == 0 && line[3] >= '0' && line[3] <= '7') {
        uint8_t t_idx = (uint8_t)(line[3] - '0');
        float s_val = state->traj_scale > 0.1f ? state->traj_scale : 1.0f;
        float y_deg = state->traj_yaw_rad * 57.29578f;
        float p_deg = state->traj_pitch_rad * 57.29578f;
        float w_mm  = state->moebius_strip_width_m * 1000.0f;
        float zw_mm = state->forcefield_z_wall_m * 1000.0f;
        float zf_mm = state->finger_probe_z_m * 1000.0f;

        if (parse_float_token(line + 4, 'S', &s_val)) {
            state->traj_scale = clampf(s_val, 0.3f, 2.0f);
        }
        if (parse_float_token(line + 4, 'Y', &y_deg)) {
            state->traj_yaw_rad = y_deg * 0.01745329f;
        }
        if (parse_float_token(line + 4, 'P', &p_deg)) {
            state->traj_pitch_rad = clampf(p_deg * 0.01745329f, -1.3f, 1.3f);
        }
        if (parse_float_token(line + 4, 'W', &w_mm)) {
            state->moebius_strip_width_m = clampf(w_mm * 0.001f, 0.002f, 0.020f);
        }
        if (parse_float_token(line + 4, 'Z', &zw_mm)) {
            state->forcefield_z_wall_m = clampf(zw_mm * 0.001f, 0.025f, 0.110f);
        }
        if (parse_float_token(line + 4, 'F', &zf_mm)) {
            state->finger_probe_z_m = clampf(zf_mm * 0.001f, 0.015f, 0.120f);
        }

        hpve_trajectory_engine_t tmp_eng;
        hpve_trajectory_init(&tmp_eng);
        hpve_trajectory_set_type(&tmp_eng, state, (hpve_trajectory_type_t)t_idx);
        hpve_driver_set_output_enable(true);

        snprintf(reply_buf, reply_max, "OK TRAJ=%s MODE=%s SCALE=%.2f\n",
                 hpve_trajectory_type_name(state->active_trajectory),
                 mode_short_name(state->active_mode),
                 state->traj_scale);
        return;
    }

    if (strncmp(line, "ORBIT", 5) == 0) {
        float en = state->orbit_enabled ? 1.0f : 0.0f;
        float r_mm = state->orbit_radius_m * 1000.0f;
        float f_hz = state->orbit_freq_hz;
        parse_float_token(line + 5, 'E', &en);
        parse_float_token(line + 5, 'R', &r_mm);
        parse_float_token(line + 5, 'F', &f_hz);
        state->orbit_enabled  = (en >= 0.5f);
        state->orbit_radius_m = clampf(r_mm * 0.001f, 0.001f, 0.045f);
        state->orbit_freq_hz  = clampf(f_hz, 0.2f, 60.0f);
        snprintf(reply_buf, reply_max, "OK ORBIT EN=%d R=%.1fmm F=%.1fHz\n",
                 state->orbit_enabled ? 1 : 0,
                 state->orbit_radius_m * 1000.0f,
                 state->orbit_freq_hz);
        return;
    }

    if (strncmp(line, "STATUS", 6) == 0) {
        snprintf(reply_buf, reply_max,
                 "HPVE v%u.%u.%u MODE=%s TRAJ=%s EN=%u X=%.2f Y=%.2f Z=%.2f FRM=%lu SWP=%lu UDR=%lu SLV=%luus\n",
                 HPVE_FW_VERSION_MAJOR, HPVE_FW_VERSION_MINOR, HPVE_FW_VERSION_PATCH,
                 mode_short_name(state->active_mode),
                 hpve_trajectory_type_name(state->active_trajectory),
                 state->output_enabled ? 1u : 0u,
                 state->focus_primary.x * 1000.0f,
                 state->focus_primary.y * 1000.0f,
                 state->focus_primary.z * 1000.0f,
                 (unsigned long)state->frame_counter,
                 (unsigned long)state->dma_swap_counter,
                 (unsigned long)state->dma_underrun_counter,
                 (unsigned long)state->solver_time_us);
        return;
    }

    snprintf(reply_buf, reply_max, "ERR UNKNOWN CMD: %s\n", line);
}

void hpve_ui_poll_usb_serial(hpve_system_state_t *state) {
#if HPVE_UI_ON_HW
    for (;;) {
        int ch = getchar_timeout_us(0);
        if (ch == PICO_ERROR_TIMEOUT) {
            break;
        }
        if (ch == '\r' || ch == '\n') {
            if (g_serial_rx_len > 0u) {
                g_serial_rx_buf[g_serial_rx_len] = '\0';
                char reply[160];
                hpve_ui_process_serial_line(state, g_serial_rx_buf, reply, sizeof(reply));
                if (reply[0] != '\0') {
                    printf("%s", reply);
                }
                g_serial_rx_len = 0u;
            }
        } else if (g_serial_rx_len + 1u < sizeof(g_serial_rx_buf)) {
            g_serial_rx_buf[g_serial_rx_len++] = (char)ch;
        }
    }
#else
    (void)state;
#endif
}
