#include "hpve_config.h"
#include "pio_cascade_driver.h"
#include "wave_solver.h"
#include "trajectory.h"
#include "ui_telemetry.h"
#include "spatial_tracker.h"

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
    st->focus_secondary.z      =  WORKSPACE_DEFAULT_Z_M;
    st->active_mode            =  HPVE_MODE_TWIN_TRAP;
    st->active_trajectory      =  TRAJ_NONE_STATIC;
    st->twin_axis              =  TWIN_AXIS_X;
    st->vortex_charge          =  1;
    st->bottle_radius_m        =  0.036f;
    st->haptic_mod_hz          =  200.0f;
    st->orbit_radius_m         =  0.022f;
    st->orbit_freq_hz          =  8.0f;
    st->traj_scale             =  1.0f;
    st->traj_yaw_rad           =  0.0f;
    st->traj_pitch_rad         =  0.26f;
    st->moebius_strip_width_m  =  0.0085f;
    st->forcefield_z_wall_m    =  0.065f;
    st->finger_probe_z_m       =  0.065f;
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
    tracker_init(NULL);

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

        static uint32_t last_tof_poll_ms = 0u;
        if (now_ms - last_tof_poll_ms >= 16u) {
            last_tof_poll_ms = now_ms;
            spatial_tracker_t *trk = (spatial_tracker_t *)tracker_get_instance();
            if (tracker_poll_and_read_depth_map(trk, now_ms)) {
                tracker_hook_wave_solver(trk, &g_system_state);
            }
        }

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

    char cmd_reply[160];
    hpve_ui_process_serial_line(&g_system_state, "M202 S1.0 Y15 P20", cmd_reply, sizeof(cmd_reply));
    printf("[TRAJ-TEST ] %s", cmd_reply);

    for (uint32_t step = 1u; step <= 32u; ++step) {
        hpve_solve_frame(&g_system_state, geom, step * 5u, &g_latest_channel_frame);
    }

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

    uint8_t *fb = (uint8_t *)hpve_ui_get_framebuffer();
    hpve_ui_render_hud(fb, &g_system_state, &g_latest_channel_frame);

    char status_reply[160];
    hpve_ui_process_serial_line(&g_system_state, "STATUS", status_reply, sizeof(status_reply));

    printf("[SELF-TEST ] 74HC595 64-Bit Cascade Shift Verification : %s (%u errors)\n",
           (bit_match_errors == 0u) ? "PASS" : "FAIL", bit_match_errors);
    printf("[SELF-TEST ] CLI Telemetry Response                   : %s", status_reply);
    return (bit_match_errors == 0u) ? 0 : 1;
#endif
}
