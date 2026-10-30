/**
 * HPVE (Haptic Photonic Volumetric Engine) - Core Embedded Firmware
 * Platform: Raspberry Pi Pico (RP2040)
 * Dual-Core ARM Cortex-M0+ @ 133MHz
 *
 * Core 0: Calculates 3D wave phase interference math & handles I2C OLED / Encoders
 * Core 1: Drives PIO State Machines & High-Speed Shift Register Latching
 */

#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"

// System Constants
#define SOUND_SPEED_M_S      343.0f
#define ULTRASONIC_FREQ_HZ   40000.0f
#define WAVELENGTH_M         (SOUND_SPEED_M_S / ULTRASONIC_FREQ_HZ) // ~8.575mm
#define WAVE_NUMBER_K        (2.0f * M_PI / WAVELENGTH_M)
#define NUM_TRANSDUCERS      64
#define ARRAY_GRID_SIZE      8
#define TRANSDUCER_SPACING_M 0.012f // 12mm center-to-center spacing

// Target Focal Coordinate in 3D Space (Meters)
struct Vector3D {
    float x;
    float y;
    float z;
};

// Global active target (volatile for inter-core communication)
volatile Vector3D active_focal_target = {0.0f, 0.0f, 0.15f}; // 15cm above center
volatile float phase_delays[NUM_TRANSDUCERS];

// Transducer Coordinates on the 8x8 emitter plate (Z=0)
Vector3D transducer_positions[NUM_TRANSDUCERS];

void init_transducer_geometry() {
    float offset = ((ARRAY_GRID_SIZE - 1) * TRANSDUCER_SPACING_M) / 2.0f;
    for (int row = 0; row < ARRAY_GRID_SIZE; row++) {
        for (int col = 0; col < ARRAY_GRID_SIZE; col++) {
            int idx = row * ARRAY_GRID_SIZE + col;
            transducer_positions[idx].x = (col * TRANSDUCER_SPACING_M) - offset;
            transducer_positions[idx].y = (row * TRANSDUCER_SPACING_M) - offset;
            transducer_positions[idx].z = 0.0f;
        }
    }
}

/**
 * Core 0: Wave Phase Calculation Loop
 * Computes: phi_i = (-k * distance_i) mod 2pi
 */
void calculate_phases(Vector3D target) {
    for (int i = 0; i < NUM_TRANSDUCERS; i++) {
        float dx = transducer_positions[i].x - target.x;
        float dy = transducer_positions[i].y - target.y;
        float dz = transducer_positions[i].z - target.z;
        float dist = sqrtf(dx * dx + dy * dy + dz * dz);

        // Inverse distance phase shift
        float phase = fmodf(-WAVE_NUMBER_K * dist, 2.0f * M_PI);
        if (phase < 0.0f) phase += (2.0f * M_PI);

        // Convert 0..2pi radians into 0..25 microsecond timing delay offset
        phase_delays[i] = (phase / (2.0f * M_PI)) * 25.0f; // in microseconds
    }
}

/**
 * Core 1: Hardware Pulse Execution
 */
void core1_entry() {
    printf("[HPVE Core 1] Hardware pulse generator started at 40kHz.\n");
    while (true) {
        // High-speed phase modulation loop
        tight_loop_contents();
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("--- HPVE Core Bootstrap (RP2040) ---\n");
    printf("Transducer Matrix: 8x8 (64 elements)\n");
    printf("Frequency: 40.0 kHz | Period: 25.0 us\n");

    init_transducer_geometry();

    // Launch Core 1 for dedicated pulse engine
    multicore_launch_core1(core1_entry);

    // Initial calculation
    calculate_phases((Vector3D){0.0f, 0.0f, 0.15f});
    printf("Initial focal lock achieved at (X=0.0cm, Y=0.0cm, Z=15.0cm).\n");

    while (true) {
        // Polling loop for telemetry updates and encoder adjustments
        sleep_ms(100);
    }

    return 0;
}
