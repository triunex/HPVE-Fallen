#ifndef PIO_CASCADE_DRIVER_H
#define PIO_CASCADE_DRIVER_H

#include "hpve_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t words[WORDS_PER_DMA_FRAME];
} __attribute__((aligned(16))) hpve_dma_buffer_t;

void hpve_driver_build_geometry_table(hpve_transducer_map_t *table_out);
const hpve_transducer_map_t* hpve_driver_get_geometry_table(void);

void hpve_driver_init(hpve_system_state_t *sys_state);
void hpve_driver_set_output_enable(bool enable);
void hpve_driver_set_photonic_rgb(uint8_t r, uint8_t g, uint8_t b);

void hpve_compile_single_carrier_bitplanes(
    const hpve_channel_frame_t *frame,
    uint32_t *slice_words_out,
    bool gate_active
);

void hpve_compile_full_dma_frame(
    const hpve_channel_frame_t *frame,
    hpve_dma_buffer_t *dma_buf_out
);

hpve_dma_buffer_t* hpve_driver_acquire_backbuffer(void);
void hpve_driver_commit_backbuffer(void);
bool hpve_driver_is_swap_pending(void);

uint64_t hpve_emulate_74hc595_chain(uint32_t word_first, uint32_t word_second);

#ifdef __cplusplus
}
#endif

#endif
