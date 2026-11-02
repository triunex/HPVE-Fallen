#include "hpve_config.h"
#include "pio_cascade_driver.h"
#include "wave_solver.h"
#include "ui_telemetry.h"

#include <stdio.h>
#include <string.h>

#if defined(PICO_RP2040) || defined(PICO_BUILD)
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/watchdog.h"
#include "hardware/timer.h"
#define HPVE_MAIN_ON_HW 1
#else
#include <time.h>
#define HPVE_MAIN_ON_HW 0
#endif

static hpve_system_state_t  g_system_state;
static hpve_channel_frame_t g_latest_channel_frame;

static void hpve_state_set_defaults(hpve_system_state_t *st) {
    memset((void *)st, 0, sizeof(*st));
    st->focus_primary.x        =  0.000f;
    st->focus_primary.y        =  0.000f;
    st->focus_primary.z        =  WORKSPACE_DEFAULT_Z_M;
    st->focus_secondary.x      =  0.018f;
    st->focus_secondary.y      =  0.000f;
    st->focus_secondary.z        =  WORKSPACE_DEFAULT_Z_M;
    st->active_mode            =  HPVE_MODE_TWIN_TRAP;
    st->twin_axis              =  TWIN_AXIS_X;
    st->vortex_charge          =  1;
    st->bottle_radius_m        =  0.036f;
    st->haptic_mod_hz          =  200.0f;
    st->orbit_radius_m         =  0.015f;
    st->orbit_freq_hz          =  10.0f;
    st->orbit_enabled          =  false;
    st->output_enabled         =  true;
    st->use_fixed_point_solver =  true;
}

#if HPVE_MAIN_ON_HW
static void core1_acoustic_engine_entry(void) {
    const hpve_transducer_map_t *geom = hpve_driver_get_geometry_table();

    for (;;) {
        if (!hpve_driver_is_swap_pending()) {
            uint64_t t0 = time_us_64();
            uint32_t now_ms = to_ms_since_boot(get_absolute_time());

            hpve_solve_frame(&g_system_state, geom, now_ms, &g_latest_channel_frame);
            uint64_t t1 = time_us_64();

            hpve_dma_buffer_t *backbuf = hpve_driver_acquire_backbuffer();
            hpve_compile_full_dma_frame(&g_latest_channel_frame, backbuf);
            hpve_driver_commit_backbuffer();

            uint64_t t2 = time_us_64();
            g_system_state.solver_time_us  = (uint32_t)(t1 - t0);
            g_system_state.compile_time_us = (uint32_t)(t2 - t1);

            hpve_driver_set_photonic_rgb(
                g_latest_channel_frame.r_pwm,
                g_latest_channel_frame.g_pwm,
                g_latest_channel_frame.b_pwm
            );
        } else {
            tight_loop_contents();
        }
    }
}
#endif

int main(void) {
    hpve_state_set_defaults(&g_system_state);
    hpve_solver_init();
    hpve_driver_init(&g_system_state);
    hpve_ui_init(&g_system_state);

    const hpve_transducer_map_t *geom = hpve_driver_get_geometry_table();
    hpve_solve_frame(&g_system_state, geom, 0u, &g_latest_channel_frame);
    hpve_dma_buffer_t *init_buf = hpve_driver_acquire_backbuffer();
    hpve_compile_full_dma_frame(&g_latest_channel_frame, init_buf);
    hpve_driver_commit_backbuffer();

#if HPVE_MAIN_ON_HW
    stdio_init_all();
    watchdog_enable(250, true);
    multicore_launch_core1(core1_acoustic_engine_entry);

    uint32_t last_oled_ms = 0u;
    for (;;) {
        watchdog_update();
        uint32_t now_ms = to_ms_since_boot(get_absolute_time());

        hpve_ui_poll_inputs(&g_system_state, now_ms);
        hpve_ui_poll_usb_serial(&g_system_state);

        if (now_ms - last_oled_ms >= 33u) {
            last_oled_ms = now_ms;
            uint8_t *fb = (uint8_t *)hpve_ui_get_framebuffer();
            hpve_ui_render_hud(fb, &g_system_state, &g_latest_channel_frame);
            hpve_ui_flush_oled(fb);
        }
        sleep_us(250);
    }
#else
    printf("====================================================================\n");
    printf("  IRON CAVE INDUSTRIES - HPVE 64-CH PHASED-ARRAY FIRMWARE BENCH\n");
    printf("  Version %u.%u.%u | %u Tiles x %u Ch | Pitch: %.1f mm | Carrier: %u Hz\n",
           HPVE_FW_VERSION_MAJOR, HPVE_FW_VERSION_MINOR, HPVE_FW_VERSION_PATCH,
           MAX_CASCADE_TILES, TILE_CHANNELS, TRANSDUCER_PITCH_M * 1000.0f, ACOUSTIC_CARRIER_HZ);
    printf("====================================================================\n");

    uint32_t slice_words[SLICES_PER_CARRIER_PERIOD * WORDS_PER_SLICE];
    hpve_compile_single_carrier_bitplanes(&g_latest_channel_frame, slice_words, true);

    uint32_t bit_match_errors = 0u;
    for (uint8_t ch = 0; ch < NUM_TRANSDUCERS; ++ch) {
        uint8_t phi = g_latest_channel_frame.phase_step[ch];
        uint32_t w0 = slice_words[phi * 2u + 0u];
        uint32_t w1 = slice_words[phi * 2u + 1u];
        uint64_t sr_latched = hpve_emulate_74hc595_chain(w0, w1);
        uint64_t ch_bit = (sr_latched >> ch) & 1ULL;
        if (ch_bit != 1ULL) {
            bit_match_errors++;
        }
    }

    vec3f_t focal_pt = g_system_state.focus_primary;
    vec3f_t flank_pt = { focal_pt.x + 0.0043f, focal_pt.y, focal_pt.z };
    float p_node  = hpve_eval_field_pressure_pa(&g_latest_channel_frame, geom, focal_pt);
    float p_flank = hpve_eval_field_pressure_pa(&g_latest_channel_frame, geom, flank_pt);

    uint8_t *fb = (uint8_t *)hpve_ui_get_framebuffer();
    hpve_ui_render_hud(fb, &g_system_state, &g_latest_channel_frame);

    char status_reply[160];
    hpve_ui_process_serial_line(&g_system_state, "STATUS", status_reply, sizeof(status_reply));

    printf("[SELF-TEST] 74HC595 64-Bit Cascade Shift Verification : %s (%u errors)\n",
           (bit_match_errors == 0u) ? "PASS" : "FAIL", bit_match_errors);
    printf("[SELF-TEST] Twin-Trap Node Pressure @ (0,0,55mm)     : %.2f Pa\n", p_node);
    printf("[SELF-TEST] Twin-Trap Lobe Peak     @ (+4.3,0,55mm)  : %.2f Pa\n", p_flank);
    printf("[SELF-TEST] CLI Telemetry Response                   : %s", status_reply);
    return (bit_match_errors == 0u) ? 0 : 1;
#endif
}
