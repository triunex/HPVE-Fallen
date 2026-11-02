#include "pio_cascade_driver.h"
#include <string.h>

#if defined(PICO_RP2040) || defined(PICO_BUILD)
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/sync.h"
#include "cascade_spi.pio.h"
#define HPVE_ON_HARDWARE 1
#else
#define HPVE_ON_HARDWARE 0
#endif

static hpve_transducer_map_t g_geometry_table[NUM_TRANSDUCERS];
static hpve_dma_buffer_t     g_dma_ping __attribute__((aligned(16)));
static hpve_dma_buffer_t     g_dma_pong __attribute__((aligned(16)));

static volatile uint8_t      g_active_front_idx = 0;
static volatile bool         g_backbuffer_ready = false;
static hpve_system_state_t  *g_sys_state_ptr    = NULL;

#if HPVE_ON_HARDWARE
static PIO                   g_pio_inst         = pio0;
static uint                  g_pio_sm           = 0;
static uint                  g_pio_offset       = 0;
static int                   g_dma_chan_a       = -1;
static int                   g_dma_chan_b       = -1;
static uint                  g_pwm_slice_rg     = 0;
static uint                  g_pwm_slice_b      = 0;
#endif

/*
 * Exact Hardware Netlist Mapping for HPVE_Tile_16Ch:
 * - Each tile has 2x 74HC595:
 *   U1 (receives J_IN Pin 4 SPI_MOSI_IN): Q0..Q7 -> SIG_1..SIG_8   -> TX1..TX8
 *   U2 (receives U1 Pin 9 QH' cascade):   Q0..Q7 -> SIG_9..SIG_16  -> TX9..TX16
 *   U2 Pin 9 QH' -> J_OUT Pin 4 SPI_MOSI_OUT -> next tile J_IN Pin 4.
 *
 * - In a 4-tile daisy chain (Tile 0 -> Tile 1 -> Tile 2 -> Tile 3):
 *   64 clock pulses per slice shift 64 bits out of PIO MSB-first (two 32-bit OSR words).
 *   Bit 0 shifted out (MSB of word[0], bit 31) travels through Tile 0, 1, 2 and lands in
 *   Tile 3, U2, Q7 (TX16 of Tile 3 = global index 63).
 *   Bit 63 shifted out (LSB of word[1], bit 0) lands in Tile 0, U1, Q0 (TX1 of Tile 0 = global index 0).
 *
 *   Therefore, for global channel idx in [0..63]:
 *   - idx = tile_id * 16 + local_ch  (where tile_id in 0..3, local_ch in 0..15)
 *   - shift_order_from_first = 63 - idx
 *   - If idx >= 32 (Tiles 2 and 3): placed in word[0] at bit position (idx - 32).
 *   - If idx <  32 (Tiles 0 and 1): placed in word[1] at bit position (idx).
 *
 *   Let's verify:
 *    PIO shifts MSB-first:
 *   1st bit out of PIO = bit 31 of word[0] -> corresponds to idx = 31 + 32 = 63 (Tile 3, U2, Q7 = TX16).
 *   32nd bit out of PIO = bit 0 of word[0] -> corresponds to idx = 0 + 32 = 32 (Tile 2, U1, Q0 = TX1).
 *   33rd bit out of PIO = bit 31 of word[1] -> corresponds to idx = 31 (Tile 1, U2, Q7 = TX16).
 *   64th bit out of PIO = bit 0 of word[1] -> corresponds to idx = 0 (Tile 0, U1, Q0 = TX1).
 */

void hpve_driver_build_geometry_table(hpve_transducer_map_t *table_out) {
    /*
     * 2x2 Tile Quadrant Layout (8x8 total array, 64 channels, 16.5 mm pitch):
     * Tile 0: Top-Left Quadrant     (cols 0..3, rows 0..3)
     * Tile 1: Top-Right Quadrant    (cols 4..7, rows 0..3)
     * Tile 2: Bottom-Left Quadrant  (cols 0..3, rows 4..7)
     * Tile 3: Bottom-Right Quadrant (cols 4..7, rows 4..7)
     *
     * Array center (0, 0, 0) is at the geometric center of the 8x8 grid:
     * col_center = 3.5, row_center = 3.5.
     * x = (global_col - 3.5f) * 16.5 mm
     * y = (3.5f - global_row) * 16.5 mm  (top row has +Y, bottom row has -Y)
     */
    for (uint8_t tile = 0; tile < MAX_CASCADE_TILES; ++tile) {
        uint8_t quad_col_base = (tile & 1u) ? 4u : 0u;
        uint8_t quad_row_base = (tile & 2u) ? 4u : 0u;

        for (uint8_t local_ch = 0; local_ch < TILE_CHANNELS; ++local_ch) {
            uint8_t idx = (uint8_t)(tile * TILE_CHANNELS + local_ch);
            uint8_t local_row = local_ch >> 2u;
            uint8_t local_col = local_ch & 0x03u;

            uint8_t global_col = (uint8_t)(quad_col_base + local_col);
            uint8_t global_row = (uint8_t)(quad_row_base + local_row);

            float x_m = ((float)global_col - 3.5f) * TRANSDUCER_PITCH_M;
            float y_m = (3.5f - (float)global_row) * TRANSDUCER_PITCH_M;

            hpve_transducer_map_t *entry = &table_out[idx];
            entry->tile_id          = tile;
            entry->local_ch         = local_ch;
            entry->sr_chip          = (local_ch < 8u) ? 1u : 2u;
            entry->sr_pin_q         = (uint8_t)(local_ch & 0x07u);
            entry->global_col       = global_col;
            entry->global_row       = global_row;
            entry->cascade_bit_pos  = idx;
            entry->pos_x_m          = x_m;
            entry->pos_y_m          = y_m;
            entry->pos_x_q16        = FLOAT_TO_Q16(x_m);
            entry->pos_y_q16        = FLOAT_TO_Q16(y_m);
            entry->phase_trim_steps = 0;
        }
    }
}

const hpve_transducer_map_t* hpve_driver_get_geometry_table(void) {
    return g_geometry_table;
}

void hpve_compile_single_carrier_bitplanes(
    const hpve_channel_frame_t *frame,
    uint32_t *slice_words_out,
    bool gate_active
) {
    if (!gate_active || frame->active_mask == 0ULL) {
        memset(slice_words_out, 0, SLICES_PER_CARRIER_PERIOD * WORDS_PER_SLICE * sizeof(uint32_t));
        return;
    }

    uint32_t w0_slices[SLICES_PER_CARRIER_PERIOD];
    uint32_t w1_slices[SLICES_PER_CARRIER_PERIOD];
    for (uint32_t s = 0; s < SLICES_PER_CARRIER_PERIOD; ++s) {
        w0_slices[s] = 0u;
        w1_slices[s] = 0u;
    }

    uint64_t mask = frame->active_mask;

    for (uint8_t ch = 0; ch < 32u; ++ch) {
        if ((mask & (1ULL << ch)) == 0ULL) {
            continue;
        }
        uint8_t phi  = (uint8_t)((frame->phase_step[ch] + g_geometry_table[ch].phase_trim_steps) & PHASE_STEP_MASK);
        uint8_t duty = frame->duty_steps[ch];
        if (duty > NUM_PHASE_STEPS) {
            duty = NUM_PHASE_STEPS;
        }
        uint32_t bit = (1u << ch);
        for (uint8_t d = 0; d < duty; ++d) {
            uint8_t slice_idx = (uint8_t)((phi + d) & PHASE_STEP_MASK);
            w1_slices[slice_idx] |= bit;
        }
    }

    for (uint8_t ch = 32u; ch < NUM_TRANSDUCERS; ++ch) {
        if ((mask & (1ULL << ch)) == 0ULL) {
            continue;
        }
        uint8_t phi  = (uint8_t)((frame->phase_step[ch] + g_geometry_table[ch].phase_trim_steps) & PHASE_STEP_MASK);
        uint8_t duty = frame->duty_steps[ch];
        if (duty > NUM_PHASE_STEPS) {
            duty = NUM_PHASE_STEPS;
        }
        uint32_t bit = (1u << (ch - 32u));
        for (uint8_t d = 0; d < duty; ++d) {
            uint8_t slice_idx = (uint8_t)((phi + d) & PHASE_STEP_MASK);
            w0_slices[slice_idx] |= bit;
        }
    }

    for (uint32_t s = 0; s < SLICES_PER_CARRIER_PERIOD; ++s) {
        slice_words_out[s * 2u + 0u] = w0_slices[s];
        slice_words_out[s * 2u + 1u] = w1_slices[s];
    }
}

void hpve_compile_full_dma_frame(
    const hpve_channel_frame_t *frame,
    hpve_dma_buffer_t *dma_buf_out
) {
    uint32_t active_carrier_words[SLICES_PER_CARRIER_PERIOD * WORDS_PER_SLICE];
    hpve_compile_single_carrier_bitplanes(frame, active_carrier_words, true);

    const size_t bytes_per_carrier = SLICES_PER_CARRIER_PERIOD * WORDS_PER_SLICE * sizeof(uint32_t);
    uint8_t am_depth = frame->haptic_am_depth;

    if (am_depth == 0u) {
        for (uint32_t c = 0; c < CARRIER_CYCLES_PER_FRAME; ++c) {
            uint32_t *dst = &dma_buf_out->words[c * SLICES_PER_CARRIER_PERIOD * WORDS_PER_SLICE];
            memcpy(dst, active_carrier_words, bytes_per_carrier);
        }
        return;
    }

    /*
     * 200 Hz Haptic Pacinian Envelope Modulation over 200 carrier cycles (5.0 ms window):
     * Duty-gates the carrier train proportionally to am_depth (1..255).
     * At am_depth = 128 (50% AM), first 100 carrier cycles are full amplitude,
     * remaining 100 carrier cycles are zeroed (radiation pressure square-wave modulation at 200 Hz).
     */
    uint32_t on_cycles = ((uint32_t)(255u - (am_depth >> 1u)) * CARRIER_CYCLES_PER_FRAME) >> 8u;
    if (am_depth >= 128u) {
        on_cycles = CARRIER_CYCLES_PER_FRAME >> 1u;
    }

    for (uint32_t c = 0; c < CARRIER_CYCLES_PER_FRAME; ++c) {
        uint32_t *dst = &dma_buf_out->words[c * SLICES_PER_CARRIER_PERIOD * WORDS_PER_SLICE];
        if (c < on_cycles) {
            memcpy(dst, active_carrier_words, bytes_per_carrier);
        } else {
            memset(dst, 0, bytes_per_carrier);
        }
    }
}

uint64_t hpve_emulate_74hc595_chain(uint32_t word_first, uint32_t word_second) {
    uint64_t shift_reg = 0ULL;

    for (int bit = 31; bit >= 0; --bit) {
        uint64_t mosi = (word_first >> bit) & 1u;
        shift_reg = (shift_reg << 1u) | mosi;
    }
    for (int bit = 31; bit >= 0; --bit) {
        uint64_t mosi = (word_second >> bit) & 1u;
        shift_reg = (shift_reg << 1u) | mosi;
    }
    return shift_reg;
}

#if HPVE_ON_HARDWARE
static void __isr __time_critical_func(hpve_dma_irq0_handler)(void) {
    uint32_t ints = dma_hw->ints0;
    dma_hw->ints0 = ints;

    int completed_chan = -1;
    if (ints & (1u << g_dma_chan_a)) {
        completed_chan = g_dma_chan_a;
    } else if (ints & (1u << g_dma_chan_b)) {
        completed_chan = g_dma_chan_b;
    }

    if (completed_chan < 0) {
        return;
    }

    if (g_backbuffer_ready) {
        g_active_front_idx ^= 1u;
        g_backbuffer_ready = false;
        if (g_sys_state_ptr) {
            g_sys_state_ptr->dma_swap_counter++;
        }
    } else {
        if (g_sys_state_ptr) {
            g_sys_state_ptr->dma_underrun_counter++;
        }
    }

    const hpve_dma_buffer_t *front_buf = (g_active_front_idx == 0u) ? &g_dma_ping : &g_dma_pong;
    dma_channel_set_read_addr((uint)completed_chan, front_buf->words, false);

    if (g_sys_state_ptr) {
        g_sys_state_ptr->frame_counter++;
    }
}
#endif

void hpve_driver_set_output_enable(bool enable) {
    if (g_sys_state_ptr) {
        g_sys_state_ptr->output_enabled = enable;
    }
#if HPVE_ON_HARDWARE
    gpio_put(PIN_DRV_OE, enable ? 0 : 1);
#endif
}

void hpve_driver_set_photonic_rgb(uint8_t r, uint8_t g, uint8_t b) {
#if HPVE_ON_HARDWARE
    pwm_set_gpio_level(PIN_PHOTONIC_R, (uint16_t)r * 257u);
    pwm_set_gpio_level(PIN_PHOTONIC_G, (uint16_t)g * 257u);
    pwm_set_gpio_level(PIN_PHOTONIC_B, (uint16_t)b * 257u);
#else
    (void)r;
    (void)g;
    (void)b;
#endif
}

hpve_dma_buffer_t* hpve_driver_acquire_backbuffer(void) {
    return (g_active_front_idx == 0u) ? &g_dma_pong : &g_dma_ping;
}

void hpve_driver_commit_backbuffer(void) {
#if HPVE_ON_HARDWARE
    __dmb();
#endif
    g_backbuffer_ready = true;
}

bool hpve_driver_is_swap_pending(void) {
    return g_backbuffer_ready;
}

void hpve_driver_init(hpve_system_state_t *sys_state) {
    g_sys_state_ptr = sys_state;
    hpve_driver_build_geometry_table(g_geometry_table);

    memset(&g_dma_ping, 0, sizeof(g_dma_ping));
    memset(&g_dma_pong, 0, sizeof(g_dma_pong));
    g_active_front_idx = 0u;
    g_backbuffer_ready = false;

#if HPVE_ON_HARDWARE
    set_sys_clock_khz(SYS_CLOCK_KHZ, true);

    gpio_init(PIN_DRV_OE);
    gpio_set_dir(PIN_DRV_OE, GPIO_OUT);
    gpio_put(PIN_DRV_OE, 1);

    gpio_init(PIN_SYNC_TRIG);
    gpio_set_dir(PIN_SYNC_TRIG, GPIO_OUT);
    gpio_put(PIN_SYNC_TRIG, 0);

    gpio_init(PIN_STATUS_LED);
    gpio_set_dir(PIN_STATUS_LED, GPIO_OUT);
    gpio_put(PIN_STATUS_LED, 1);

    gpio_set_function(PIN_PHOTONIC_R, GPIO_FUNC_PWM);
    gpio_set_function(PIN_PHOTONIC_G, GPIO_FUNC_PWM);
    gpio_set_function(PIN_PHOTONIC_B, GPIO_FUNC_PWM);
    g_pwm_slice_rg = pwm_gpio_to_slice_num(PIN_PHOTONIC_R);
    g_pwm_slice_b  = pwm_gpio_to_slice_num(PIN_PHOTONIC_B);
    pwm_set_wrap(g_pwm_slice_rg, 65535u);
    pwm_set_wrap(g_pwm_slice_b,  65535u);
    pwm_set_enabled(g_pwm_slice_rg, true);
    pwm_set_enabled(g_pwm_slice_b,  true);
    hpve_driver_set_photonic_rgb(0, 0, 0);

    g_pio_offset = pio_add_program(g_pio_inst, &cascade_spi_program);
    g_pio_sm     = (uint)pio_claim_unused_sm(g_pio_inst, true);

    float clkdiv = (float)PIO_CLKDIV_INT + ((float)PIO_CLKDIV_FRAC / 256.0f);
    uint32_t shift_bits = PIO_SHIFT_BITS_PER_SLICE;
    uint32_t pad_cycles = (PIO_CYCLES_PER_SLICE > (2u * shift_bits + 6u))
                        ? (PIO_CYCLES_PER_SLICE - (2u * shift_bits + 6u))
                        : 0u;

    cascade_spi_program_init(
        g_pio_inst,
        g_pio_sm,
        g_pio_offset,
        PIN_SPI_MOSI,
        PIN_SPI_SCK,
        clkdiv,
        shift_bits,
        pad_cycles
    );

    g_dma_chan_a = dma_claim_unused_channel(true);
    g_dma_chan_b = dma_claim_unused_channel(true);

    dma_channel_config cfg_a = dma_channel_get_default_config((uint)g_dma_chan_a);
    channel_config_set_transfer_data_size(&cfg_a, DMA_SIZE_32);
    channel_config_set_read_increment(&cfg_a, true);
    channel_config_set_write_increment(&cfg_a, false);
    channel_config_set_dreq(&cfg_a, pio_get_dreq(g_pio_inst, g_pio_sm, true));
    channel_config_set_chain_to(&cfg_a, (uint)g_dma_chan_b);

    dma_channel_config cfg_b = dma_channel_get_default_config((uint)g_dma_chan_b);
    channel_config_set_transfer_data_size(&cfg_b, DMA_SIZE_32);
    channel_config_set_read_increment(&cfg_b, true);
    channel_config_set_write_increment(&cfg_b, false);
    channel_config_set_dreq(&cfg_b, pio_get_dreq(g_pio_inst, g_pio_sm, true));
    channel_config_set_chain_to(&cfg_b, (uint)g_dma_chan_a);

    dma_channel_configure(
        (uint)g_dma_chan_a,
        &cfg_a,
        &g_pio_inst->txf[g_pio_sm],
        g_dma_ping.words,
        WORDS_PER_DMA_FRAME,
        false
    );

    dma_channel_configure(
        (uint)g_dma_chan_b,
        &cfg_b,
        &g_pio_inst->txf[g_pio_sm],
        g_dma_ping.words,
        WORDS_PER_DMA_FRAME,
        false
    );

    dma_channel_set_irq0_enabled((uint)g_dma_chan_a, true);
    dma_channel_set_irq0_enabled((uint)g_dma_chan_b, true);
    irq_set_exclusive_handler(DMA_IRQ_0, hpve_dma_irq0_handler);
    irq_set_priority(DMA_IRQ_0, 0x00);
    irq_set_enabled(DMA_IRQ_0, true);

    pio_sm_set_enabled(g_pio_inst, g_pio_sm, true);
    dma_channel_start((uint)g_dma_chan_a);
    hpve_driver_set_output_enable(sys_state ? sys_state->output_enabled : true);
#endif
}
