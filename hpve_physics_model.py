import numpy as np

class HPVE_Simulator:
    """
    Haptic Photonic Volumetric Engine (HPVE) - Physics Model
    Simulates the interference pattern of 256 ultrasonic transducers
    to create a focal pressure point for light scattering and haptics.
    """
    def __init__(self, array_size=16, freq=40000, sound_speed=343):
        self.array_size = array_size
        self.freq = freq
        self.sound_speed = sound_speed
        self.wavelength = sound_speed / freq
        self.k = 2 * np.pi / self.wavelength  # Wave number

        # Grid of transducers (x, y, z=0)
        spacing = 0.01  # 10mm spacing
        x = np.linspace(- (array_size-1)*spacing/2, (array_size-1)*spacing/2, array_size)
        y = np.linspace(- (array_size-1)*spacing/2, (array_size-1)*spacing/2, array_size)
        self.tx, self.ty = np.meshgrid(x, y)
        self.tx = self.tx.flatten()
        self.ty = self.ty.flatten()
        self.tz = np.zeros_like(self.tx)

    def calculate_phases(self, target_xyz):
        """
        Calculates the phase delay for each transducer to focus at target_xyz.
        """
        distances = np.sqrt((self.tx - target_xyz[0])**2 +
                            (self.ty - target_xyz[1])**2 +
                            (self.tz - target_xyz[2])**2)

        # Inverse distance to get phase shift
        phases = (-self.k * distances) % (2 * np.pi)
        return phases

    def get_pressure_at_point(self, point_xyz, phases):
        """
        Calculates the acoustic pressure at a specific 3D point.
        """
        distances = np.sqrt((self.tx - point_xyz[0])**2 +
                            (self.ty - point_xyz[1])**2 +
                            (self.tz - point_xyz[2])**2)

        # Sum of complex amplitudes: P = sum( (1/d) * exp(i * (k*d + phase)) )
        complex_pressure = np.sum((1/distances) * np.exp(1j * (self.k * distances + phases)))
        return np.abs(complex_pressure)

# Example Usage:
# target = [0.1, 0.1, 0.5] # Focus 50cm below the array
# sim = HPVE_Simulator()
# phases = sim.calculate_phases(target)
# p_at_focus = sim.get_pressure_at_point(target, phases)
# print(f"Relative pressure at focus: {p_at_focus}")
