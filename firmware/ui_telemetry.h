#ifndef UI_TELEMETRY_H
#define UI_TELEMETRY_H

#include "hpve_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OLED_WIDTH              128u
#define OLED_HEIGHT             64u
#define OLED_FB_BYTES           ((OLED_WIDTH * OLED_HEIGHT) / 8u)
#define SSD1306_I2C_ADDR        0x3Cu

void hpve_ui_init(hpve_system_state_t *state);
void hpve_ui_poll_inputs(hpve_system_state_t *state, uint32_t now_ms);
void hpve_ui_process_serial_line(hpve_system_state_t *state, const char *line, char *reply_buf, size_t reply_max);
void hpve_ui_poll_usb_serial(hpve_system_state_t *state);

void hpve_oled_clear(uint8_t *fb);
void hpve_oled_set_pixel(uint8_t *fb, int x, int y, bool on);
void hpve_oled_draw_line(uint8_t *fb, int x0, int y0, int x1, int y1, bool on);
void hpve_oled_draw_rect(uint8_t *fb, int x, int y, int w, int h, bool on);
void hpve_oled_draw_char(uint8_t *fb, int x, int y, char c);
void hpve_oled_draw_str(uint8_t *fb, int x, int y, const char *str);

void hpve_ui_render_hud(
    uint8_t *fb,
    const hpve_system_state_t *state,
    const hpve_channel_frame_t *last_frame
);
void hpve_ui_flush_oled(const uint8_t *fb);
const uint8_t* hpve_ui_get_framebuffer(void);

#ifdef __cplusplus
}
#endif

#endif
