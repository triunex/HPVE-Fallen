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

- **modular tile design (`HPVE_Tile_16Ch`)**: scaled from a bulky monolith to daisy-chainable 16-channel phased array tiles (4x4 transducers per board). 4 tiles daisy-chain into a 64-channel array; 16 tiles snap into a 256-channel Holotable!
- **stackup**: 4-layer high-current board (`F.Cu` signals, `In1.Cu` solid GND plane, `In2.Cu` dedicated +12V power plane, `B.Cu` signals).
- **interconnects**: ultra-low profile 0.5mm horizontal flip-lock FFC ribbon connectors (`J_IN` / `J_OUT`, Hirose FH12-6S-0.5SH) for flush edge-to-edge tile stacking, plus dedicated 2-pin auxiliary 12V bench power terminal (`J_PWR`).
- **mcu**: raspberry pi pico (rp2040). standard arduinos are way too slow to drive 64 phase channels, but the rp2040 has PIO (programmable i/o) state machines running at 133 mhz, which can toggle pins with nanosecond timing without burning cpu cycles.
- **transducers**: 16x 400ST160 40khz ultrasonic emitters per tile (64 across 4 tiles) mounted in 100% female header sockets for hot-swapping and zero soldering heat stress.
- **drivers**: 8x TC4427 dual high-speed MOSFET gate drivers per tile with individual 0.1uF 50V X7R 0805 decoupling caps, level shifting 3.3v logic up to 12v peak-to-peak.
- **shift registers**: 2x 74HC595 SOIC-16 shift registers per tile cascading SPI control.
- **controls**: dual rotary encoders for moving the focal point (x, y, z) + 0.96 inch i2c oled screen for telemetry.

## repository structure

```
├── firmware/         # rp2040 c++ code and pio state machines
├── hardware/         # kicad schematics, pcb layout, gerbers, drc reports
│   └── modular_tile/ # 16-channel 4-layer modular tile (pcb, gerbers, bom, cpl, 3d renders)
├── simulation/       # python wave physics model, 3d gor'kov trapping, impedance
├── docs/             # calculations, notes, pinout architecture
├── CAMPAIGN_ROADMAP_45_DAYS.md # master 45-day expedition manifest & live telemetry
└── README.md
```

## fallen 6-week roadmap

- [x] simulate wave interference in python (142x focal pressure verified)
- [x] 3d gor'kov acoustic trap simulation (2.0mm bead levitation & 200hz skin haptics modeled)
- [x] timing budget verified: 20MHz SPI loads 64 channels in 3.25us (87% margin)
- [x] architectural pivot: scalable 16-channel 4-layer modular tile (`HPVE_Tile_16Ch`)
- [x] route 4-layer modular tile with 0.5mm FFC connectors, bench power, and decoupling
- [x] achieve 0 DRC violations, 0 unconnected pads, and 0 warnings (`hardware/DRC.rpt`)
- [x] export 1-click JLCPCB production package (Gerbers, drills, BOM with LCSC IDs, CPL)
- [x] cross 200+ XP on Hack Club Fallen (216 XP banked on Day 4, Sami evolved to Stage 2!)
- [ ] submit official Chinese F-Visa application on Hack Club portal
- [ ] prototype RP2040 PIO cascading shift register firmware
- [ ] order custom tiles from JLCPCB and components from LCSC / AliExpress
- [ ] assemble and test 40khz acoustic output on breadboard / bench
- [ ] test mid-air skin haptics and particle levitation with multi-tile array
- [ ] design 3d printed desktop chassis with acoustic horn aperture
- [ ] ship build submission and demo live at maker faire shenzhen 2026!

## author

ENTROPIS IS CREATING IT.
hack club slack: @nelieo.shorya
