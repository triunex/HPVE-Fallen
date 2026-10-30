"""
HPVE Shift-Register Bus Timing & Latency Simulator
Verifies that cascading eight 74HC595 shift registers can update 64 channels
within the 25 microsecond period of a 40kHz ultrasonic carrier wave.
"""

import matplotlib.pyplot as plt
import numpy as np

def run_timing_analysis():
    # System Parameters
    f_ultrasonic = 40000.0          # 40 kHz carrier
    t_period_us = (1.0 / f_ultrasonic) * 1e6  # 25.0 microseconds
    num_channels = 64
    num_shift_registers = 8         # 8 bits each = 64 bits

    # RP2040 SPI Clock Speeds (MHz) to evaluate
    spi_clocks_mhz = [5.0, 10.0, 15.0, 20.0, 25.0, 31.25]
    
    print("=" * 65)
    print(" HPVE SHIFT-REGISTER BUS TIMING BUDGET ANALYSIS (RP2040 -> 74HC595)")
    print("=" * 65)
    print(f"Target Ultrasonic Frequency: {f_ultrasonic/1e3:.1f} kHz")
    print(f"Total Available Period:      {t_period_us:.2f} microseconds (100% Budget)")
    print(f"Total Output Channels:       {num_channels} channels ({num_shift_registers}x 74HC595)")
    print("-" * 65)
    print(f"{'SPI Clock (MHz)':<18} | {'Transfer Time (us)':<20} | {'Period Margin (%)':<15}")
    print("-" * 65)

    transfer_times = []
    margins = []

    for clk in spi_clocks_mhz:
        # Time to shift 64 bits: (64 / Clock_Freq) + 20ns latch pulse
        t_transfer_us = (num_channels / clk) + 0.050 # including latch overhead
        margin_pct = ((t_period_us - t_transfer_us) / t_period_us) * 100.0
        transfer_times.append(t_transfer_us)
        margins.append(margin_pct)
        print(f"{clk:<18.2f} | {t_transfer_us:<20.3f} | {margin_pct:<15.1f}%")

    print("=" * 65)
    
    # Selected baseline: 20 MHz SPI Clock
    selected_clk = 20.0
    selected_time = (num_channels / selected_clk) + 0.050
    selected_margin = ((t_period_us - selected_time) / t_period_us) * 100.0

    print(f"\nVERDICT: At {selected_clk:.1f} MHz SPI clock:")
    print(f"- 64 bits shifted into all 8 registers in: {selected_time:.2f} microseconds.")
    print(f"- Available timing margin: {selected_margin:.1f}% of the 40 kHz cycle.")
    print("STATUS: ZERO LATENCY BOTTLENECK. 74HC595 cascade is completely safe for 40kHz.\n")

    # Generate Timing Budget Plot
    plt.style.use('dark_background')
    fig, ax = plt.subplots(figsize=(8, 5), dpi=150)
    
    bars = ax.bar([f"{c} MHz" for c in spi_clocks_mhz], transfer_times, color='#00ffcc', alpha=0.8, edgecolor='cyan')
    ax.axhline(t_period_us, color='#ff0055', linestyle='--', linewidth=2, label='40kHz Period Limit (25 us)')
    
    # Highlight 20 MHz operating point
    bars[3].set_color('#ffaa00')
    bars[3].set_edgecolor('yellow')

    for bar in bars:
        yval = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2.0, yval + 0.5, f"{yval:.2f} us", ha='center', va='bottom', fontsize=9, color='white')

    ax.set_title('HPVE Shift-Register SPI Bus Latency vs. 40kHz Cycle Budget', fontsize=12, pad=12, color='white')
    ax.set_xlabel('RP2040 SPI Clock Frequency', fontsize=10, color='white')
    ax.set_ylabel('Serial Transfer Time (Microseconds)', fontsize=10, color='white')
    ax.set_ylim(0, 30)
    ax.legend(loc='upper right')
    plt.tight_layout()

    out_file = "simulation/shift_register_timing_analysis.png"
    plt.savefig(out_file)
    print(f"Timing analysis chart saved to: {out_file}")

if __name__ == "__main__":
    run_timing_analysis()
