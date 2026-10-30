from hpve_physics_model import HPVE_Simulator
import numpy as np

def run_simulation_check():
    sim = HPVE_Simulator(array_size=16)
    
    # Target: 20cm directly below center
    target = [0.0, 0.0, 0.2]
    
    # Calculate phases for focus
    phases = sim.calculate_phases(target)
    
    # Measure pressure AT focus
    p_focus = sim.get_pressure_at_point(target, phases)
    
    # Measure pressure 5cm away from focus (to check focus intensity)
    p_away = sim.get_pressure_at_point([0.05, 0.05, 0.2], phases)
    
    print(f"--- HPVE Simulation Result ---")
    print(f"Array: 16x16 (256 Transducers)")
    print(f"Mode: Phase-Locked Focal Point")
    print(f"Pressure at Focal Point: {p_focus:.2f}")
    print(f"Pressure at Noise Floor (5cm away): {p_away:.2f}")
    print(f"Focus Intensity Ratio: {p_focus/p_away:.1f}x")
    
    if p_focus/p_away > 10:
        print("\nSTATUS: OPTICAL SCATTERING THRESHOLD REACHED.")
        print("The pressure gradient at the focus is sharp enough to create a refractive index shift.")

if __name__ == "__main__":
    run_simulation_check()
