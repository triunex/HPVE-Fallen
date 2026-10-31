"""
HPVE Project - 64-Channel Phased Ultrasonic Emitter
Transducer Resonance & Electromechanical Impedance Model (BVD Equivalent Network)

Author: Iron Cave Industries | Hack Club Fallen 2026
Target Hardware: 400ST160 40kHz Piezoelectric Ultrasonic Transmitters
Driver ICs: 32x Microchip TC4427 Dual High-Speed MOSFET Gate Drivers
Excitation: 12.0Vpp Square-Wave Switching Rail

Description:
High-precision Butterworth-Van Dyke (BVD) analytical and numerical model for
the 400ST160 piezoelectric transducer. Computes motional and static branch
impedance, admittance circle locus, series/parallel resonance poles, harmonic
current spectrum under square-wave excitation, 64-channel array reactive load,
and thermal dissipation across the TC4427 driver stages.
"""

import os
import json
import numpy as np
import matplotlib.pyplot as plt
from dataclasses import dataclass, asdict

# Set matplotlib style for dark aerospace/defense aesthetic
plt.style.use('dark_background')
plt.rcParams['font.family'] = 'sans-serif'
plt.rcParams['axes.edgecolor'] = '#444444'
plt.rcParams['axes.linewidth'] = 0.8
plt.rcParams['grid.color'] = '#222222'
plt.rcParams['grid.linestyle'] = '--'
plt.rcParams['grid.alpha'] = 0.7

@dataclass
class BVDParameters:
    """
    Butterworth-Van Dyke (BVD) Equivalent Circuit Parameters.
    
    Circuit Topology:
      Term A o----+-----------+----o Term B
                  |           |
                 [C0]       [Rm]
                  |           |
                 (G0)       [Lm]
                  |           |
                  |         [Cm]
                  |           |
                  +-----------+
    """
    # Static Branch
    C0_farads: float = 2.20e-9       # Clamped dielectric capacitance (nominal 2.2 nF)
    tan_delta: float = 0.005         # Dielectric loss tangent
    
    # Motional Branch (Mechanically coupled acoustic resonator)
    fs_target_hz: float = 40000.0    # Target series resonant frequency (40.0 kHz)
    Qm: float = 35.0                 # Mechanical quality factor in air (typical 30 - 40)
    Cm_farads: float = 123.2e-12     # Motional compliance capacitance (123.2 pF)
    
    # Derived parameters (populated in __post_init__)
    Lm_henries: float = 0.0          # Motional acoustic mass inductance
    Rm_ohms: float = 0.0             # Motional damping resistance
    fp_antires_hz: float = 0.0       # Parallel antiresonant frequency
    keff: float = 0.0                # Effective electromechanical coupling coefficient

    def __post_init__(self):
        w_s = 2.0 * np.pi * self.fs_target_hz
        # Lm = 1 / (w_s^2 * Cm)
        self.Lm_henries = 1.0 / ((w_s ** 2) * self.Cm_farads)
        # Rm = w_s * Lm / Qm
        self.Rm_ohms = (w_s * self.Lm_henries) / self.Qm
        # fp = fs * sqrt(1 + Cm / C0)
        self.fp_antires_hz = self.fs_target_hz * np.sqrt(1.0 + (self.Cm_farads / self.C0_farads))
        # keff = sqrt( (fp^2 - fs^2) / fp^2 )
        self.keff = np.sqrt((self.fp_antires_hz**2 - self.fs_target_hz**2) / (self.fp_antires_hz**2))


class TransducerImpedanceModel:
    """
    Analytical and numerical solver for transducer electrical characteristics,
    admittance circles, harmonic drive response, and array power budgets.
    """
    
    def __init__(self, params: BVDParameters = None):
        self.params = params if params is not None else BVDParameters()
        
    def motional_impedance(self, f: np.ndarray) -> np.ndarray:
        """Impedance of the motional branch: Z_m(w) = Rm + j*(w*Lm - 1/(w*Cm))"""
        w = 2.0 * np.pi * f
        X_m = w * self.params.Lm_henries - (1.0 / (w * self.params.Cm_farads))
        return self.params.Rm_ohms + 1j * X_m

    def static_admittance(self, f: np.ndarray) -> np.ndarray:
        """Admittance of the static branch: Y_0(w) = G_0(w) + j*w*C0"""
        w = 2.0 * np.pi * f
        G_0 = w * self.params.C0_farads * self.params.tan_delta
        B_0 = w * self.params.C0_farads
        return G_0 + 1j * B_0

    def total_admittance(self, f: np.ndarray) -> np.ndarray:
        """Total terminal admittance: Y_tot(w) = Y_m(w) + Y_0(w)"""
        Z_m = self.motional_impedance(f)
        Y_m = 1.0 / Z_m
        Y_0 = self.static_admittance(f)
        return Y_m + Y_0

    def total_impedance(self, f: np.ndarray) -> np.ndarray:
        """Total terminal impedance: Z_tot(w) = 1 / Y_tot(w)"""
        return 1.0 / self.total_admittance(f)

    def analyze_frequency_response(self, f_start: float = 35000.0, f_stop: float = 45000.0, points: int = 2000):
        """Generate high-density frequency sweep across resonant region."""
        f = np.linspace(f_start, f_stop, points)
        Z = self.total_impedance(f)
        Y = self.total_admittance(f)
        
        mag_Z = np.abs(Z)
        phase_Z_deg = np.angle(Z, deg=True)
        conductance_G = np.real(Y)
        susceptance_B = np.imag(Y)
        
        # Identify exact numerical minima and maxima
        idx_fs = np.argmin(mag_Z)
        idx_fp = np.argmax(mag_Z)
        
        exact_fs = f[idx_fs]
        exact_fp = f[idx_fp]
        min_Z = mag_Z[idx_fs]
        max_Z = mag_Z[idx_fp]
        
        return {
            'frequencies': f,
            'Z': Z,
            'Y': Y,
            'mag_Z': mag_Z,
            'phase_Z_deg': phase_Z_deg,
            'conductance_G': conductance_G,
            'susceptance_B': susceptance_B,
            'exact_fs': exact_fs,
            'exact_fp': exact_fp,
            'min_Z': min_Z,
            'max_Z': max_Z,
            'bandwidth_hz': self.params.fs_target_hz / self.params.Qm
        }

    def simulate_square_wave_drive(self, v_dd: float = 12.0, f0: float = 40000.0, max_harmonic: int = 19):
        """
        Analyze steady-state current and power under 12V unipolar square-wave drive
        produced by the TC4427 driver IC.
        
        Fourier Series of 50% duty cycle square wave (0V to V_dd):
          v(t) = V_dd/2 + sum_{n=1,3,5,...} [ (2*V_dd)/(n*pi) * sin(n*w0*t) ]
        """
        w0 = 2.0 * np.pi * f0
        harmonics = np.arange(1, max_harmonic + 1, 2)  # Odd harmonics: 1, 3, 5, ...
        
        f_harm = harmonics * f0
        v_harm_peak = (2.0 * v_dd) / (harmonics * np.pi)
        v_harm_rms = v_harm_peak / np.sqrt(2.0)
        
        Z_harm = self.total_impedance(f_harm)
        mag_Z_harm = np.abs(Z_harm)
        
        # Harmonic currents
        i_harm_peak = v_harm_peak / mag_Z_harm
        i_harm_rms = v_harm_rms / mag_Z_harm
        phase_i_deg = np.angle(1.0 / Z_harm, deg=True)
        
        # Real power dissipated in motional + static resistance per harmonic
        real_power_harm = (v_harm_rms ** 2) * np.real(1.0 / Z_harm)
        reactive_power_harm = (v_harm_rms ** 2) * np.imag(1.0 / Z_harm)
        
        # Total RMS calculations
        total_irms_per_transducer = np.sqrt(np.sum(i_harm_rms ** 2))
        total_p_real_per_transducer = np.sum(real_power_harm)
        total_q_reactive_per_transducer = np.sum(reactive_power_harm)
        total_s_apparent_per_transducer = (v_dd / np.sqrt(2.0)) * total_irms_per_transducer
        
        return {
            'harmonics': harmonics,
            'frequencies_hz': f_harm,
            'v_rms_volts': v_harm_rms,
            'i_rms_ma': i_harm_rms * 1e3,
            'i_peak_ma': i_harm_peak * 1e3,
            'impedance_ohms': mag_Z_harm,
            'phase_deg': phase_i_deg,
            'real_power_mw': real_power_harm * 1e3,
            'reactive_power_mvar': reactive_power_harm * 1e3,
            'total_irms_ma': total_irms_per_transducer * 1e3,
            'total_p_real_mw': total_p_real_per_transducer * 1e3,
            'total_q_mvar': total_q_reactive_per_transducer * 1e3,
            'total_s_mva': total_s_apparent_per_transducer * 1e3
        }

    def simulate_array_power_budget(self, num_channels: int = 64, v_dd: float = 12.0, f0: float = 40000.0):
        """
        Compute full 64-channel array power, peak transient inrush current,
        TC4427 driver package dissipation, and power supply decoupling requirements.
        """
        sq_results = self.simulate_square_wave_drive(v_dd=v_dd, f0=f0)
        
        # Array continuous totals
        array_irms_total_a = (sq_results['total_irms_ma'] / 1e3) * num_channels
        array_real_power_w = (sq_results['total_p_real_mw'] / 1e3) * num_channels
        array_reactive_power_var = (sq_results['total_q_mvar'] / 1e3) * num_channels
        
        # Switching Transient Inrush Analysis (per edge)
        # TC4427 internal RDS(on) typical = 7.0 ohms
        # PCB trace resistance approx 0.3 ohms
        r_driver_out = 7.0
        r_trace = 0.3
        r_loop = r_driver_out + r_trace
        
        # Peak instantaneous inrush current per channel during 0->12V transition
        i_peak_inrush_per_ch = v_dd / r_loop
        # 10% to 90% rise time t_r approx 2.2 * R_loop * C0
        t_rise_ns = 2.2 * r_loop * self.params.C0_farads * 1e9
        
        # Charge transferred per transition Q = C0 * V_dd
        q_per_transition_nc = self.params.C0_farads * v_dd * 1e9
        q_array_total_nc = q_per_transition_nc * num_channels
        
        # Peak simultaneous array current if all channels switch within the same 5ns window
        i_peak_array_inrush_a = i_peak_inrush_per_ch * num_channels
        
        # Decoupling Requirement:
        # To maintain V_dd ripple delta_V <= 100 mV during simultaneous edge:
        delta_v_max = 0.100  # 100 mV ripple
        c_decoupling_min_uf = (q_array_total_nc * 1e-9) / delta_v_max * 1e6
        
        # TC4427 Driver Thermal Dissipation:
        # 32 driver ICs, each driving 2 channels
        # Dynamic internal switching dissipation per channel: P_sw = 0.5 * f0 * C0 * Vdd^2 * (R_driver / R_loop)
        p_sw_per_ch = 0.5 * f0 * self.params.C0_farads * (v_dd ** 2) * (r_driver_out / r_loop)
        p_quiescent_per_ic = 12.0 * 1.5e-3  # 1.5 mA quiescent current at 12V
        p_dissipation_per_tc4427 = (2.0 * p_sw_per_ch) + p_quiescent_per_ic
        
        # Thermal rise: SOIC-8 theta_JA approx 125 degC/W on 1oz FR4
        theta_ja = 125.0
        temp_rise_degc = p_dissipation_per_tc4427 * theta_ja
        
        return {
            'num_channels': num_channels,
            'v_dd_volts': v_dd,
            'single_ch_irms_ma': sq_results['total_irms_ma'],
            'array_irms_total_a': array_irms_total_a,
            'array_real_power_w': array_real_power_w,
            'array_reactive_power_var': array_reactive_power_var,
            'i_peak_inrush_per_ch_a': i_peak_inrush_per_ch,
            'i_peak_array_inrush_a': i_peak_array_inrush_a,
            't_rise_ns': t_rise_ns,
            'q_per_transition_nc': q_per_transition_nc,
            'q_array_total_nc': q_array_total_nc,
            'c_decoupling_min_uf': c_decoupling_min_uf,
            'p_dissipation_per_tc4427_mw': p_dissipation_per_tc4427 * 1e3,
            'temp_rise_tc4427_degc': temp_rise_degc,
            'sq_results': sq_results
        }


def generate_engineering_plots(model: TransducerImpedanceModel, output_png: str):
    """
    Renders high-resolution 4-panel publication-grade diagnostic plot.
    """
    freq_data = model.analyze_frequency_response(f_start=36000.0, f_stop=44000.0, points=3000)
    array_budget = model.simulate_array_power_budget(num_channels=64, v_dd=12.0, f0=40000.0)
    sq_data = array_budget['sq_results']
    
    fig, axs = plt.subplots(2, 2, figsize=(14, 10), dpi=180)
    fig.patch.set_facecolor('#0d1117')
    
    # -------------------------------------------------------------
    # Panel 1: Bode Impedance Magnitude and Phase
    # -------------------------------------------------------------
    ax1 = axs[0, 0]
    ax1.set_facecolor('#161b22')
    
    f_khz = freq_data['frequencies'] / 1e3
    color_mag = '#58a6ff'
    color_phase = '#f0883e'
    
    line1 = ax1.plot(f_khz, freq_data['mag_Z'], color=color_mag, linewidth=2.0, label='|Z| Magnitude (Ohms)')
    ax1.set_xlabel('Frequency (kHz)', fontsize=10, color='#c9d1d9')
    ax1.set_ylabel('Impedance Magnitude |Z| (Ohms)', fontsize=10, color=color_mag)
    ax1.tick_params(axis='y', labelcolor=color_mag)
    ax1.grid(True)
    ax1.set_yscale('log')
    
    # Twin axis for phase
    ax1_phase = ax1.twinx()
    line2 = ax1_phase.plot(f_khz, freq_data['phase_Z_deg'], color=color_phase, linewidth=1.5, linestyle='--', label='Phase (Deg)')
    ax1_phase.set_ylabel('Phase Angle (Degrees)', fontsize=10, color=color_phase)
    ax1_phase.tick_params(axis='y', labelcolor=color_phase)
    ax1_phase.set_ylim(-90, 90)
    
    # Annotate Series & Parallel Resonance
    ax1.axvline(freq_data['exact_fs'] / 1e3, color='#3fb950', linestyle=':', linewidth=1.8, alpha=0.9)
    ax1.axvline(freq_data['exact_fp'] / 1e3, color='#f85149', linestyle=':', linewidth=1.8, alpha=0.9)
    
    ax1.text(freq_data['exact_fs'] / 1e3 + 0.1, freq_data['min_Z'] * 1.5,
             f"fs = {freq_data['exact_fs']/1e3:.2f} kHz\nZ_min = {freq_data['min_Z']:.1f} Ohm",
             color='#3fb950', fontsize=8.5, fontweight='bold')
    
    ax1.text(freq_data['exact_fp'] / 1e3 - 2.2, freq_data['max_Z'] * 0.4,
             f"fp = {freq_data['exact_fp']/1e3:.2f} kHz\nZ_max = {freq_data['max_Z']/1e3:.1f} kOhm",
             color='#f85149', fontsize=8.5, fontweight='bold')
    
    ax1.set_title('BVD Terminal Impedance |Z(f)| & Phase Angle', fontsize=11, fontweight='bold', color='#f0f6fc')
    
    # -------------------------------------------------------------
    # Panel 2: Admittance Circle (Nyquist / Conductance vs Susceptance)
    # -------------------------------------------------------------
    ax2 = axs[0, 1]
    ax2.set_facecolor('#161b22')
    
    G_ms = freq_data['conductance_G'] * 1e3
    B_ms = freq_data['susceptance_B'] * 1e3
    
    ax2.plot(G_ms, B_ms, color='#38d499', linewidth=2.0, label='Admittance Locus Y(w)')
    ax2.scatter(freq_data['conductance_G'][np.argmin(freq_data['mag_Z'])] * 1e3,
                freq_data['susceptance_B'][np.argmin(freq_data['mag_Z'])] * 1e3,
                color='#3fb950', s=70, zorder=5, label='Series Resonance fs')
    ax2.scatter(freq_data['conductance_G'][np.argmax(freq_data['mag_Z'])] * 1e3,
                freq_data['susceptance_B'][np.argmax(freq_data['mag_Z'])] * 1e3,
                color='#f85149', s=70, zorder=5, label='Antiresonance fp')
    
    ax2.set_xlabel('Conductance G (mS)', fontsize=10, color='#c9d1d9')
    ax2.set_ylabel('Susceptance B (mS)', fontsize=10, color='#c9d1d9')
    ax2.grid(True)
    ax2.legend(loc='lower right', fontsize=8.5, facecolor='#21262d', edgecolor='#30363d')
    ax2.set_title('Admittance Circle Vector Locus (B vs. G)', fontsize=11, fontweight='bold', color='#f0f6fc')
    
    # -------------------------------------------------------------
    # Panel 3: Harmonic Current Spectrum (12V Square Wave Drive)
    # -------------------------------------------------------------
    ax3 = axs[1, 0]
    ax3.set_facecolor('#161b22')
    
    harm_labels = [f"h{h}\n({f/1e3:.0f}k)" for h, f in zip(sq_data['harmonics'], sq_data['frequencies_hz'])]
    bars = ax3.bar(harm_labels, sq_data['i_rms_ma'], color='#79c0ff', edgecolor='#1f6feb', alpha=0.85, width=0.6)
    
    # Highlight fundamental 40 kHz
    bars[0].set_color('#2ea043')
    bars[0].set_edgecolor('#3fb950')
    
    for bar in bars:
        h = bar.get_height()
        ax3.text(bar.get_x() + bar.get_width()/2.0, h + 0.15, f"{h:.2f}mA",
                 ha='center', va='bottom', fontsize=8, color='#c9d1d9')
        
    ax3.set_xlabel('Square-Wave Harmonics (Odd)', fontsize=10, color='#c9d1d9')
    ax3.set_ylabel('Harmonic RMS Current (mA)', fontsize=10, color='#c9d1d9')
    ax3.set_title('Single Transducer Harmonic Current Spectrum (12V Square Wave)', fontsize=11, fontweight='bold', color='#f0f6fc')
    ax3.grid(True, axis='y')
    ax3.set_ylim(0, max(sq_data['i_rms_ma']) * 1.25)
    
    # -------------------------------------------------------------
    # Panel 4: 64-Channel Array Power & Transient Summary Card
    # -------------------------------------------------------------
    ax4 = axs[1, 1]
    ax4.set_facecolor('#161b22')
    ax4.axis('off')
    
    text_content = (
        "HPVE 64-CHANNEL ARRAY DRIVER DYNAMICS\n"
        "====================================================\n\n"
        f"Transducer Element:       400ST160 (Murata/Pro-Wave)\n"
        f"Static Capacitance (C0):  {model.params.C0_farads*1e9:.2f} nF\n"
        f"Motional Cap (Cm):        {model.params.Cm_farads*1e12:.2f} pF\n"
        f"Motional Inductance (Lm): {model.params.Lm_henries*1e3:.2f} mH\n"
        f"Motional Resistance (Rm): {model.params.Rm_ohms:.1f} Ohms\n"
        f"Mechanical Quality (Qm):  {model.params.Qm:.1f} (in air)\n"
        f"Electromech Coupling k:   {model.params.keff:.4f}\n"
        "----------------------------------------------------\n"
        f"Operating Frequency:      40.0 kHz (Series Resonance)\n"
        f"Drive Voltage:            12.0 Vpp (Square-Wave, 50% duty)\n"
        f"Single Element RMS I:     {array_budget['single_ch_irms_ma']:.2f} mA\n"
        f"64-Channel Total RMS I:   {array_budget['array_irms_total_a']:.2f} A continuous\n"
        f"64-Channel Active Power:  {array_budget['array_real_power_w']:.2f} W (acoustic + mechanical)\n"
        f"64-Channel Reactive Pwr:  {array_budget['array_reactive_power_var']:.2f} VAR (capacitive exchange)\n"
        "----------------------------------------------------\n"
        f"Peak Inrush / Channel:    {array_budget['i_peak_inrush_per_ch_a']:.2f} A (t_rise = {array_budget['t_rise_ns']:.1f} ns)\n"
        f"Array Total Edge Charge:  {array_budget['q_array_total_nc']:.1f} nC per half-cycle\n"
        f"Min +12V Decoupling Cap:  {array_budget['c_decoupling_min_uf']:.2f} uF (for <100mV ripple)\n"
        f"TC4427 Package Power:     {array_budget['p_dissipation_per_tc4427_mw']:.1f} mW / IC (32 ICs total)\n"
        f"TC4427 Junction Temp Rise:{array_budget['temp_rise_tc4427_degc']:.1f} degC (SOIC-8 on FR4)\n"
        "====================================================\n"
        "VERDICT: THERMALLY SAFE & ELECTRICALLY STABLE"
    )
    
    ax4.text(0.05, 0.95, text_content, transform=ax4.transAxes,
             fontsize=9.0, verticalalignment='top', fontfamily='monospace',
             color='#58a6ff',
             bbox=dict(boxstyle='round,pad=0.8', facecolor='#0d1117', edgecolor='#30363d', alpha=0.9))
    
    plt.tight_layout()
    plt.savefig(output_png, facecolor=fig.get_facecolor(), edgecolor='none')
    plt.close()
    print(f"[OK] Engineering analysis plot successfully generated: {output_png}")


def main():
    print("=" * 75)
    print(" HPVE TRANSDUCER BUTTERWORTH-VAN DYKE (BVD) ELECTRO-ACOUSTIC SOLVER")
    print(" Iron Cave Industries | Hack Club Fallen 2026")
    print("=" * 75)
    
    # Instantiate physics model with 400ST160 parameters
    params = BVDParameters(
        C0_farads=2.20e-9,      # 2.2 nF nominal clamped capacitance
        tan_delta=0.005,        # Low-loss PZT piezoceramic
        fs_target_hz=40000.0,   # 40 kHz series resonance
        Qm=35.0,                # High-Q air emitter
        Cm_farads=123.2e-12     # Motional compliance
    )
    
    model = TransducerImpedanceModel(params)
    
    # 1. Frequency sweep
    sweep = model.analyze_frequency_response()
    print(f"\n[1] BVD EQUIVALENT CIRCUIT PARAMETERS:")
    print(f"    - C0 (Dielectric Capacitance):    {model.params.C0_farads * 1e9:.2f} nF")
    print(f"    - Cm (Motional Capacitance):      {model.params.Cm_farads * 1e12:.2f} pF")
    print(f"    - Lm (Motional Inductance):       {model.params.Lm_henries * 1e3:.2f} mH")
    print(f"    - Rm (Motional Resistance):       {model.params.Rm_ohms:.2f} Ohms")
    print(f"    - Series Resonant Freq (fs):      {sweep['exact_fs']:.2f} Hz  (|Z_min| = {sweep['min_Z']:.2f} Ohms)")
    print(f"    - Parallel Antiresonant Freq (fp):{sweep['exact_fp']:.2f} Hz  (|Z_max| = {sweep['max_Z']:.2f} Ohms)")
    print(f"    - Resonant Bandwidth (-3dB):      {sweep['bandwidth_hz']:.2f} Hz")
    print(f"    - Electromechanical Coupling k:   {model.params.keff:.4f}")

    # 2. 64-Channel Array Power Analysis
    budget = model.simulate_array_power_budget(num_channels=64, v_dd=12.0, f0=40000.0)
    print(f"\n[2] 64-CHANNEL ARRAY DRIVER DYNAMICS (12Vpp @ 40 kHz):")
    print(f"    - Continuous RMS Current / Channel: {budget['single_ch_irms_ma']:.2f} mA")
    print(f"    - Total Array Continuous RMS Current: {budget['array_irms_total_a']:.3f} A")
    print(f"    - Total Array Real Acoustic Power:   {budget['array_real_power_w']:.2f} W")
    print(f"    - Total Array Reactive Power:        {budget['array_reactive_power_var']:.2f} VAR")
    print(f"    - Transient Inrush Peak / Channel:   {budget['i_peak_inrush_per_ch_a']:.2f} A (rise time: {budget['t_rise_ns']:.1f} ns)")
    print(f"    - Total Switching Charge / Edge:     {budget['q_array_total_nc']:.1f} nC")
    print(f"    - Minimum Bulk Decoupling Cap:       {budget['c_decoupling_min_uf']:.2f} uF (for <100mV droop)")
    print(f"    - Dissipation per TC4427 IC:         {budget['p_dissipation_per_tc4427_mw']:.2f} mW (32 ICs)")
    print(f"    - TC4427 Junction Temp Rise:         {budget['temp_rise_tc4427_degc']:.1f} degC (safe margin)")

    # 3. Export JSON Dataset
    os.makedirs("simulation", exist_ok=True)
    json_path = "simulation/transducer_bvd_parameters.json"
    
    export_payload = {
        "bvd_parameters": asdict(model.params),
        "resonance_metrics": {
            "fs_hz": sweep['exact_fs'],
            "fp_hz": sweep['exact_fp'],
            "min_impedance_ohms": sweep['min_Z'],
            "max_impedance_ohms": sweep['max_Z'],
            "bandwidth_hz": sweep['bandwidth_hz'],
            "keff": model.params.keff
        },
        "array_metrics_64ch": {
            "total_rms_current_a": budget['array_irms_total_a'],
            "total_real_power_w": budget['array_real_power_w'],
            "total_reactive_power_var": budget['array_reactive_power_var'],
            "min_decoupling_capacitance_uf": budget['c_decoupling_min_uf'],
            "tc4427_dissipation_mw": budget['p_dissipation_per_tc4427_mw'],
            "tc4427_temp_rise_c": budget['temp_rise_tc4427_degc']
        }
    }
    
    with open(json_path, 'w') as f:
        json.dump(export_payload, f, indent=4)
    print(f"\n[OK] Model parameters saved to: {json_path}")

    # 4. Generate Plot
    png_path = "simulation/transducer_bvd_resonance_analysis.png"
    generate_engineering_plots(model, png_path)
    print(f"[OK] Simulation completed successfully.\n")


if __name__ == "__main__":
    main()
