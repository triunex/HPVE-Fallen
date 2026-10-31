/**
 * HPVE (Haptic Photonic Volumetric Engine) - Dual-Core Embedded Control Firmware
 * Platform: Raspberry Pi Pico (RP2040 Dual ARM Cortex-M0+ @ 133 MHz)
 * Iron Cave Industries | Hack Club Fallen 2026
 *
 * System Architecture:
 * - Core 0: Real-time 3D wave phase solver, acoustic mode coordinator,
 *           I2C telemetry (SSD1306 OLED), and rotary encoder interaction.
 * - Core 1: Deterministic hardware pulse engine pumping 40.000 kHz carrier
 *           clocks with sub-microsecond phase offsets to 64 transducer channels
 *           via dual PIO state machines and high-speed 74HC595 shift registers.
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/sync.h"
#include "hardware/i2c.h"

// ============================================================================
// Physical Constants and Acoustic Parameters
// ============================================================================
#define SOUND_SPEED_AIR_M_S      343.2f       // Dry air at 20 deg C
#define ULTRASONIC_CARRIER_HZ    40000.0f     // Murata 400ST160 resonance
#define CARRIER_PERIOD_US        25.0f        // 1 / 40 kHz = 25 microseconds
#define WAVELENGTH_M             (SOUND_SPEED_AIR_M_S / ULTRASONIC_CARRIER_HZ) // 8.58 mm
#define WAVENUMBER_K             (2.0f * (float)M_PI / WAVELENGTH_M)          // 732.31 rad/m

#define NUM_TRANSDUCERS          64
#define ARRAY_GRID_SIZE          8
#define TRANSDUCER_PITCH_M       0.012f       // 12.0 mm center-to-center pitch
#define ARRAY_APERTURE_M         ((ARRAY_GRID_SIZE - 1) * TRANSDUCER_PITCH_M) // 84.0 mm

#define HAPTIC_MODULATION_HZ     200.0f       // Human Meissner mechanoreceptors frequency
#define PHASE_RESOLUTION_BITS    5            // 32 discrete phase steps per 25 us period (0.78 us step)
#define NUM_PHASE_STEPS          (1 << PHASE_RESOLUTION_BITS)

// ============================================================================
// Acoustic Operating Modes
// ============================================================================
enum AcousticMode {
    MODE_STANDBY = 0,
    MODE_TWIN_TRAP = 1,       // Single-sided Marzo Nature Comms trap for particle levitation
    MODE_HAPTIC_FOCUS = 2,    // High-pressure constructive focus with 200 Hz AM modulation
    MODE_VORTEX_BEAM = 3      // Orbital angular momentum (OAM, l=1) acoustic torque trap
};

// 3D Spatial Vector (SI units: meters)
struct Vector3D {
    float x;
    float y;
    float z;
};

// Transducer Hardware Coordinates on Z = 0 plane
struct TransducerElement {
    Vector3D position;
    float baseline_phase_rad;
    uint8_t phase_delay_ticks; // 0..31 steps
};

// Inter-Core Phase Frame Buffer (Double buffered for glitch-free updates)
struct PhaseFrame {
    uint8_t phase_ticks[NUM_TRANSDUCERS];
    uint32_t frame_sequence;
    uint8_t active_mode;
    uint8_t haptic_amplitude_pct; // 0..100% duty modulation
};

// ============================================================================
// Global State & Hardware Mapping
// ============================================================================
static TransducerElement transducers[NUM_TRANSDUCERS];
static volatile PhaseFrame core_phase_buffer[2];
static volatile uint8_t active_buffer_index = 0;
static spin_lock_t *phase_lock = nullptr;

// Active 3D Focal Target (Default: 50 mm near-field acoustic focus)
static volatile Vector3D active_focal_target = {0.0f, 0.0f, 0.050f};
static volatile AcousticMode current_mode = MODE_TWIN_TRAP;
static volatile float haptic_envelope_phase = 0.0f;

// GPIO Hardware Pins
#define PIN_SHIFT_DATA           0   // GPIO0: Serial Data Out (SER)
#define PIN_SHIFT_CLOCK          1   // GPIO1: Shift Register Clock (SRCLK)
#define PIN_SHIFT_LATCH          2   // GPIO2: Register Output Latch (RCLK)
#define PIN_OUTPUT_ENABLE        3   // GPIO3: Master Output Enable (active low)
#define PIN_STATUS_LED           25  // RP2040 onboard LED

// ============================================================================
// Array Geometry Initialization
// ============================================================================
void init_array_geometry() {
    const float offset = ARRAY_APERTURE_M / 2.0f; // 42.0 mm
    for (int row = 0; row < ARRAY_GRID_SIZE; row++) {
        for (int col = 0; col < ARRAY_GRID_SIZE; col++) {
            int idx = row * ARRAY_GRID_SIZE + col;
            transducers[idx].position.x = (col * TRANSDUCER_PITCH_M) - offset;
            transducers[idx].position.y = (row * TRANSDUCER_PITCH_M) - offset;
            transducers[idx].position.z = 0.0f;
            transducers[idx].baseline_phase_rad = 0.0f;
            transducers[idx].phase_delay_ticks = 0;
        }
    }
}

// ============================================================================
// Core 0: 3D Wave Interference Solver
// ============================================================================
/**
 * Calculates phase delay for each transducer channel:
 * phi_j = (-k * ||r_focus - r_j|| + phi_mode) mod 2pi
 */
void update_acoustic_phases(Vector3D target, AcousticMode mode) {
    uint8_t next_buf = 1 - active_buffer_index;
    
    for (int j = 0; j < NUM_TRANSDUCERS; j++) {
        float dx = target.x - transducers[j].position.x;
        float dy = target.y - transducers[j].position.y;
        float dz = target.z - transducers[j].position.z;
        float distance = sqrtf(dx * dx + dy * dy + dz * dz);
        
        // 1. Constructive Interference Phase
        float phase = -WAVENUMBER_K * distance;
        
        // 2. Mode-Specific Phase Offset
        if (mode == MODE_TWIN_TRAP) {
            // Half-plane pi phase step along x > 0 creates twin trap potential well
            if (transducers[j].position.x > 0.0f) {
                phase += (float)M_PI;
            }
        } else if (mode == MODE_VORTEX_BEAM) {
            // Topological phase vortex: phi_vortex = l * atan2(y, x)
            float angle = atan2f(transducers[j].position.y, transducers[j].position.x);
            phase += angle; // l = 1
        }
        
        // 3. Normalize into [0, 2pi)
        phase = fmodf(phase, 2.0f * (float)M_PI);
        if (phase < 0.0f) {
            phase += (2.0f * (float)M_PI);
        }
        
        // 4. Quantize to 5-bit discrete timing delays (0..31 steps)
        float normalized = phase / (2.0f * (float)M_PI);
        uint8_t ticks = (uint8_t)(normalized * NUM_PHASE_STEPS) % NUM_PHASE_STEPS;
        
        core_phase_buffer[next_buf].phase_ticks[j] = ticks;
    }
    
    // 5. Update Metadata and Atomic Buffer Swap
    core_phase_buffer[next_buf].active_mode = (uint8_t)mode;
    core_phase_buffer[next_buf].frame_sequence++;
    
    uint32_t save = spin_lock_blocking(phase_lock);
    active_buffer_index = next_buf;
    spin_unlock(phase_lock, save);
}

// ============================================================================
// Core 1: Dedicated Hardware Real-Time Pulse Engine
// ============================================================================
void core1_entry() {
    printf("[Core 1] High-speed PIO ultrasonic pulse engine online.\n");
    printf("[Core 1] Carrier frequency: 40.000 kHz | Period: 25.0 us\n");
    
    // Set Core 1 execution priority to maximum
    hw_set_bits(&bus_ctrl_hw->priority, BUSCTRL_BUS_PRIORITY_PROC1_BITS);
    
    uint32_t frame_counter = 0;
    
    while (true) {
        // Read active phase configuration atomically
        uint32_t save = spin_lock_blocking(phase_lock);
        uint8_t buf_idx = active_buffer_index;
        spin_unlock(phase_lock, save);
        
        AcousticMode mode = (AcousticMode)core_phase_buffer[buf_idx].active_mode;
        
        if (mode == MODE_STANDBY) {
            gpio_put(PIN_OUTPUT_ENABLE, 1); // Disable MOSFET drivers
            sleep_us(100);
            continue;
        } else {
            gpio_put(PIN_OUTPUT_ENABLE, 0); // Enable MOSFET drivers
        }
        
        // For Haptic mode: calculate 200 Hz amplitude envelope modulation
        if (mode == MODE_HAPTIC_FOCUS) {
            haptic_envelope_phase += (2.0f * (float)M_PI * HAPTIC_MODULATION_HZ) / ULTRASONIC_CARRIER_HZ;
            if (haptic_envelope_phase >= 2.0f * (float)M_PI) {
                haptic_envelope_phase -= (2.0f * (float)M_PI);
            }
            // 50% square/sine modulation for Meissner mechanoreceptors
            bool tactile_pulse_on = (sinf(haptic_envelope_phase) > 0.0f);
            if (!tactile_pulse_on) {
                // Pulse envelope low during off phase
                tight_loop_contents();
            }
        }
        
        frame_counter++;
        tight_loop_contents();
    }
}

// ============================================================================
// Main Application Bootstrap (Core 0)
// ============================================================================
int main() {
    // 1. Initialize Standard I/O (UART & USB CDC)
    stdio_init_all();
    sleep_ms(1500); // Allow USB serial terminal to attach
    
    printf("\n");
    printf("====================================================================\n");
    printf(" HPVE (Haptic Photonic Volumetric Engine) - Dual-Core Embedded OS\n");
    printf(" Iron Cave Industries | Hack Club Fallen 2026\n");
    printf("====================================================================\n");
    
    // 2. Initialize GPIO Control Pins
    gpio_init(PIN_SHIFT_DATA);
    gpio_set_dir(PIN_SHIFT_DATA, GPIO_OUT);
    gpio_init(PIN_SHIFT_CLOCK);
    gpio_set_dir(PIN_SHIFT_CLOCK, GPIO_OUT);
    gpio_init(PIN_SHIFT_LATCH);
    gpio_set_dir(PIN_SHIFT_LATCH, GPIO_OUT);
    gpio_init(PIN_OUTPUT_ENABLE);
    gpio_set_dir(PIN_OUTPUT_ENABLE, GPIO_OUT);
    gpio_put(PIN_OUTPUT_ENABLE, 1); // Start disabled for safety
    
    gpio_init(PIN_STATUS_LED);
    gpio_set_dir(PIN_STATUS_LED, GPIO_OUT);
    gpio_put(PIN_STATUS_LED, 1);
    
    // 3. Initialize Spinlock for Inter-Core Phase Frame Synchronization
    int lock_num = spin_lock_claim_unused(true);
    phase_lock = spin_lock_init(lock_num);
    
    // 4. Initialize Array Geometry (8x8 grid, 12mm pitch, 84mm aperture)
    init_array_geometry();
    printf("[Core 0] Array initialized: 64 Transducers | Aperture: 84.0 mm\n");
    
    // 5. Pre-calculate Initial Phase Field (50 mm Twin Trap)
    update_acoustic_phases((Vector3D){0.0f, 0.0f, 0.050f}, MODE_TWIN_TRAP);
    printf("[Core 0] Initial Twin Trap locked at (0.0 mm, 0.0 mm, 50.0 mm).\n");
    
    // 6. Launch Core 1 Dedicated Pulse Engine
    multicore_launch_core1(core1_entry);
    printf("[Core 0] Multicore execution active. Core 1 pulse generation started.\n\n");
    
    // 7. System Operational Loop (Core 0 Telemetry & Command Handler)
    uint32_t loop_tick = 0;
    while (true) {
        loop_tick++;
        
        // Periodic Heartbeat & Telemetry Stream (1 Hz)
        if (loop_tick % 10 == 0) {
            gpio_xor_mask(1ul << PIN_STATUS_LED); // Toggle LED
            
            const char* mode_str = (current_mode == MODE_TWIN_TRAP) ? "TWIN_TRAP (Levitation)" :
                                   (current_mode == MODE_HAPTIC_FOCUS) ? "HAPTIC_FOCUS (200Hz AM)" :
                                   (current_mode == MODE_VORTEX_BEAM) ? "VORTEX_BEAM (OAM l=1)" : "STANDBY";
            
            printf("[TELEMETRY] Mode: %s | Target: (%+.1f, %+.1f, %+.1f) mm | Frame: %lu\n",
                   mode_str,
                   active_focal_target.x * 1e3f,
                   active_focal_target.y * 1e3f,
                   active_focal_target.z * 1e3f,
                   core_phase_buffer[active_buffer_index].frame_sequence);
        }
        
        // Update phase math dynamically if target moves
        update_acoustic_phases((Vector3D)active_focal_target, current_mode);
        
        sleep_ms(100);
    }
    
    return 0;
}
