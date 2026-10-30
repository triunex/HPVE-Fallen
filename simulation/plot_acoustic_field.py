import numpy as np
import matplotlib.pyplot as plt

class HPVE_FieldVisualizer:
    def __init__(self, array_size=8, freq=40000, sound_speed=343, spacing=0.012):
        self.array_size = array_size
        self.freq = freq
        self.sound_speed = sound_speed
        self.wavelength = sound_speed / freq
        self.k = 2 * np.pi / self.wavelength
        self.spacing = spacing

        # Transducer positions in XY plane at Z=0
        coord = np.linspace(-(array_size - 1) * spacing / 2, (array_size - 1) * spacing / 2, array_size)
        tx, ty = np.meshgrid(coord, coord)
        self.tx = tx.flatten()
        self.ty = ty.flatten()
        self.tz = np.zeros_like(self.tx)

    def calculate_phases(self, target_xyz):
        dist = np.sqrt((self.tx - target_xyz[0])**2 + (self.ty - target_xyz[1])**2 + (self.tz - target_xyz[2])**2)
        phases = (-self.k * dist) % (2 * np.pi)
        return phases

    def compute_xz_slice(self, target_xyz, x_range=(-0.08, 0.08), z_range=(0.01, 0.22), res=(160, 200)):
        phases = self.calculate_phases(target_xyz)
        xs = np.linspace(x_range[0], x_range[1], res[0])
        zs = np.linspace(z_range[0], z_range[1], res[1])
        X, Z = np.meshgrid(xs, zs)
        Y = np.zeros_like(X)

        pressure_field = np.zeros_like(X, dtype=complex)

        print(f"Calculating acoustic field slice ({res[0]}x{res[1]} grid)...")
        for i in range(len(self.tx)):
            dx = X - self.tx[i]
            dy = Y - self.ty[i]
            dz = Z - self.tz[i]
            d = np.sqrt(dx**2 + dy**2 + dz**2)
            d = np.maximum(d, 0.001)  # avoid singularity near transducer surface
            # Complex acoustic pressure superposition
            pressure_field += (1.0 / d) * np.exp(1j * (self.k * d + phases[i]))

        return xs * 100, zs * 100, np.abs(pressure_field)

def main():
    target = [0.0, 0.0, 0.15]  # Target: 15 cm directly above center
    vis = HPVE_FieldVisualizer(array_size=8, freq=40000, spacing=0.012)
    
    xs, zs, field = vis.compute_xz_slice(target_xyz=target)

    # Normalize field
    field_norm = field / np.max(field)

    # Plot
    plt.style.use('dark_background')
    fig, ax = plt.subplots(figsize=(8, 10), dpi=150)

    im = ax.imshow(field_norm, extent=[xs[0], xs[-1], zs[0], zs[-1]], 
                   origin='lower', cmap='inferno', aspect='auto')

    # Draw focal target marker
    ax.scatter([target[0] * 100], [target[2] * 100], color='cyan', marker='x', s=120, linewidth=2, label='Focal Node (Target)')
    
    # Transducers positions indicator
    ax.scatter(vis.tx * 100, np.zeros_like(vis.tx), color='#00ffcc', marker='s', s=25, alpha=0.8, label='40kHz Transducers (Z=0)')

    ax.set_title('HPVE 8x8 Acoustic Phased Array - Pressure Field (XZ Slice)\nFocus Target: (0, 0, 15cm) | Frequency: 40 kHz', fontsize=12, pad=12, color='white')
    ax.set_xlabel('Lateral Position X (cm)', fontsize=11, color='white')
    ax.set_ylabel('Height Z (cm)', fontsize=11, color='white')

    cb = fig.colorbar(im, ax=ax, shrink=0.85, pad=0.04)
    cb.set_label('Normalized Acoustic Pressure (A.U.)', fontsize=10, color='white')

    ax.legend(loc='upper right', framealpha=0.4, fontsize=9)
    plt.tight_layout()

    out_path = 'simulation/acoustic_field_simulation.png'
    plt.savefig(out_path)
    print(f"Saved acoustic field visualization to: {out_path}")

if __name__ == '__main__':
    main()
