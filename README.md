# HPVE (Haptic Photonic Volumetric Engine)

building mid-air touch and acoustic levitation for hack club fallen / maker faire shenzhen 2026.

## what is this?

screens are flat and boring, and wearing clunky vr goggles feels terrible. ever since i saw tony stark working over his holographic table in iron man, i wanted to build an interface where you can reach out with your bare hands and physically touch things floating in mid air. no gloves, no controllers, no smoke screens.

hpve uses phased array ultrasound to physically manipulate empty air. by firing 64 ultrasonic transducers at 40,000 hz with microsecond phase delays, sound waves smash together at a single coordinate in 3d space.

this does two things:
1. **mid-air haptics**: when the sound waves meet at your hand, acoustic radiation pressure pushes against your skin. modulated at 200 hz, it feels like an invisible button or forcefield clicking against your palm.
2. **acoustic levitation**: the pressure creates an acoustic standing wave trap (gor'kov potential well) that can hold tiny lightweight particles floating in mid air.

## how the physics works

normal sound spreads out everywhere. to make sound focus into a tiny point in mid air, each transducer has to fire at a slightly different time depending on its distance to the target point:

phase delay equation:
phi_i = (-k * distance_i) % (2 * pi)

where:
- k = 2 * pi / wavelength (wave number)
- distance_i = sqrt((x_i - target_x)^2 + (y_i - target_y)^2 + (z_i - target_z)^2)

i already wrote a python simulation (`verify_hpve.py`) to test this. running a 16x16 array model showed a 142x pressure ratio at the focus compared to ambient noise 5cm away, which proves the interference works.

## hardware architecture

- **mcu**: raspberry pi pico (rp2040). standard arduinos are way too slow to drive 64 phase channels, but the rp2040 has PIO (programmable i/o) state machines running at 133 mhz, which can toggle pins with nanosecond timing without burning cpu cycles.
- **transducers**: 64x 400ST160 40khz ultrasonic emitters arranged in an 8x8 grid.
- **drivers**: TC4427 / L293DD dual half-bridge mosfet drivers to level shift 3.3v logic up to 12v-16v peak-to-peak for enough acoustic power.
- **controls**: dual rotary encoders for moving the focal point (x, y, z) + 0.96 inch i2c oled screen for telemetry.
- **power**: 12v 3a dc power supply with a buck converter stepping down to 5v for logic.

## repository structure

```
├── firmware/         # rp2040 c++ code and pio state machines (wip)
├── hardware/         # kicad schematics, pcb layout, gerbers
├── simulation/       # python wave physics model and pressure verification
├── docs/             # calculations, notes, bom
└── README.md
```

## fallen 6-week roadmap

- [x] simulate wave interference in python (142x focal pressure verified)
- [ ] draft kicad schematic for 64-transducer driver board
- [ ] route pcb and submit design submission for hack club grant
- [ ] prototype 8-channel pio square wave generator on rp2040
- [ ] order boards from jlcpcb and get parts
- [ ] solder driver board and test 40khz output with oscilloscope
- [ ] test mid-air skin haptics and particle levitation
- [ ] design 3d printed desktop chassis
- [ ] ship build submission and demo at maker faire shenzhen!

## author

ENTROPIS IS CREATING IT.
hack club slack: @nelieo.shorya
