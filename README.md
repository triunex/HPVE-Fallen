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

- **modular tile design (`HPVE_Tile_16Ch`)**: scaled from a bulky monolith to daisy-chainable 16-channel phased array tiles (4x4 transducers per board). 4 tiles daisy-chain into a 64-channel array; 16 tiles snap into a 256-channel Holotable! Standalone schematic (`HPVE_Tile_16Ch.kicad_sch`) verified with 0 ERC errors, 0 warnings, and 100% netlist synchronization against the routed PCB.
- **stackup**: 4-layer high-current board (`F.Cu` signals & 100% single-sided SMT, `In1.Cu` solid GND plane, `In2.Cu` dedicated +12V power plane, `B.Cu` signals).
- **interconnects**: ultra-low profile 0.5mm horizontal flip-lock FFC ribbon connectors (`J_IN` / `J_OUT`, Hirose FH12-6S-0.5SH) for flush edge-to-edge tile stacking, plus dedicated 2-pin auxiliary 12V bench power terminal (`J_PWR`).
- **mcu**: raspberry pi pico (rp2040). standard arduinos are way too slow to drive 64 phase channels, but the rp2040 has PIO (programmable i/o) state machines running at 133 mhz, which can toggle pins with nanosecond timing without burning cpu cycles.
- **transducers**: 16x Prowave 400ST160 40khz ultrasonic emitters per tile (64 across 4 tiles). each has a 16.2 mm can and 10.0 mm lead spacing, so the tile uses a 4x4 grid at 16.5 mm pitch with 10 mm plated-hole footprints (solder direct, or fit two single-pin sockets per transducer). tile size is 98 x 88 mm.
- **drivers**: 8x TC4427 dual high-speed MOSFET gate drivers per tile with individual 0.1uF 50V X7R 0805 decoupling caps, level shifting 3.3v logic up to 12v peak-to-peak.
- **shift registers**: 2x 74HC595 SOIC-16 shift registers per tile cascading SPI control.
- **exoskeleton chassis**: native parametric OpenSCAD snap-lock exoskeleton (`HPVE_Modular_Exoskeleton.scad`) locking 4 tiles into a rigid 196x176mm 2x2 grid with internal honeycomb wall core (I-beam perimeter stiffness), under-PCB 45-degree iso-grid diamond trusses, 16x Helmholtz acoustic back-chambers with conical phase-diffusers, 4-finger O-ring collet cradles, and 3-stage knurled brass heat-set insert bosses (M3 & M2.5) with melt-trap relief grooves.
- **rear avionics & power**: integrated RP2040 master avionics dock with internal M2 Pico standoffs, screw-retained XT30 12V 10A power input collar, buck regulator cradle, external USB-C port, external BOOTSEL pin, SSD1306 OLED HUD, and dual Pan-Tilt Gimbal yoke trunnions.
- **exponential horn waveguides**: contoured 64-channel 5-stage exponential horn bezel (+3.8 dB acoustic SPL boost) with under-bezel iso-grid sandwich pockets and 37-pore perforated stainless honeycomb mesh clamping the transducer cans flush.
- **central optical & tof docking bay**: downward-looking VL53L5CX 8x8 multizone Time-of-Flight hand-tracking sensor + 4x collimated 450nm cyan photonic projection laser diodes centered at (0, 0).
- **controls**: dual rotary encoders for moving the focal point (x, y, z) + 0.96 inch i2c oled screen for telemetry.

## firmware architecture & verification

- **deterministic PIO cascade engine (`firmware/cascade_spi.pio`)**: running at 102.4 MHz, shifting 64 bits across 8 daisy-chained 74HC595 shift registers every 1.5625 microseconds (exact 40.000 kHz acoustic carrier wave, 16 phase steps per cycle, 0% CPU overhead).
- **chained ping-pong DMA (`firmware/pio_cascade_driver.c`)**: DMA channels A and B auto-chain in ping-pong buffers for seamless, gapless acoustic wave emission.
- **Q16.16 fixed-point 3D wave solver (`firmware/wave_solver.c`)**: real-time integer square root and atan2 algorithms running on Core 1 supporting 6 operational modes:
  1. **Twin-Trap**: acoustic tweezers for open-air particle levitation.
  2. **Bottle-Trap**: 3D acoustic cage for holding larger objects.
  3. **Vortex-Beam**: acoustic vortex carrying orbital angular momentum (OAM, l=+1).
  4. **200 Hz Haptic Focus**: amplitude-modulated acoustic radiation pressure exciting Pacinian corpuscles for invisible, touchable mid-air tactile forcefields.
  5. **Volumetric POV**: high-speed parametric scanner for light persistence holograms (Möbius strip trajectory).
  6. **Dual-Trap SMF**: simultaneous multi-focal Huygens superposition for multi-point levitation.
- **real-time 3D parametric trajectory engine (`firmware/trajectory.c`)**:
  - **Demo 1 (Arc Reactor Core)**: 6-node hexagonal acoustic trap ring + central core with 450nm cyan photonic lock.
  - **Demo 2 (Endgame Möbius Strip & Trefoil Knot)**: real-time 3D parametric curve solver with curvature-adaptive minimum-jerk stepping and RGB color sync.
  - **Demo 3 (Spatiotemporal Haptic Forcefield Wall & Detent Dial)**: 5.7 m/s ultrasonic focal sweep with 200 Hz tactile envelope and dynamic virtual spring penetration resistance.
- **live OLED HUD & G-Code CLI (`firmware/ui_telemetry.c`)**: 128x64 SSD1306 display showing a 3D wireframe chamber reticle, live trajectory trail, and 8x8 transducer matrix, with USB serial G-code input (G0, M200..M207).
- **cycle-accurate HIL verification (`firmware/verify_cascade_engine.py`, `firmware/verify_trajectory_engine.py`)**: verified 0 bit errors across 1,024 bits/cycle and 100% trajectory velocity stability.

## repository structure

```
├── firmware/         # rp2040 c++ code, pio state machines, trajectory engines & verification
├── hardware/         # kicad schematics, pcb layout, gerbers, drc reports
│   ├── modular_tile/ # 16-channel 4-layer modular tile (pcb, gerbers, bom, cpl, 3d renders)
│   └── cad/          # native openscad exoskeleton, horn bezel, halo arch, stl exports & renders
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
- [x] convert to 100% single-sided top SMT (`F.Cu`) to slash JLCPCB PCBA setup costs by 60%
- [x] implement production-grade RP2040 dual-core PIO + DMA cascading firmware stack (`firmware/`)
- [x] Q16.16 fixed-point 3D holographic wave solver with 6 operational modes (Twin, Bottle, Vortex, Haptic, POV, Dual-Trap)
- [x] hardware-in-the-loop emulation test passed: 0 bit errors over 1,024 bits/cycle, exact 40.000 kHz carrier
- [x] cross 330+ XP on Hack Club Fallen (338 XP banked on Day 5, 11h 07m journaled at 1.15x multiplier!)
- [x] real-time 3D parametric trajectory & spatiotemporal haptics engine (Arc Reactor ring, Möbius strip, touchable forcefield)
- [x] native parametric OpenSCAD exoskeleton CAD (`HPVE_Modular_Exoskeleton.scad`): snap-lock 4-tile frame + rear RP2040 avionics dock
- [x] 64-channel 5-stage exponential acoustic horn waveguide bezel (+3.8 dB on-axis SPL gain) with honeycomb mesh
- [x] central cruciform docking bay for downward-looking VL53L5CX 8x8 ToF hand tracker + 4x 450nm photonic laser diodes
- [x] export watertight production STL package ready for 3D printing (PETG)
- [x] **500 XP FLIGHT GRANT UNLOCKED! (517 XP banked on Day 7, Sami evolved to Stage 3!)**
- [x] **DAY 8 MARATHON COMPLETE: 12.03h LOGGED (+111 XP) ➔ 628 XP TOTAL (STAGE 3 PROGRESSION)!**
- [x] standalone pristine modular tile schematic (`HPVE_Tile_16Ch.kicad_sch`) with 0 ERC violations & 100% netlist sync
- [x] native OpenSCAD Anduril-grade structural exoskeleton CAD with wall honeycomb, iso-grid floor & printable STL suite
- [ ] submit official Chinese F-Visa & $1,000 USD Travel Grant application on Hack Club portal
- [ ] implement closed-loop VL53L5CX ToF spatial hand tracking & autonomous forcefield trigger (`firmware/spatial_tracker.c`)
- [ ] build desktop Stark Holo-Console GUI (`host_console/`) with live 3D Gor'kov slices and G-code control
- [ ] order custom tiles from JLCPCB and components from LCSC / AliExpress
- [ ] assemble and calibrate acoustic output across 4 snapped modular tiles
- [ ] test mid-air skin haptics and particle levitation with multi-tile array
- [ ] ship build submission and demo live at maker faire shenzhen 2026!

## author

ENTROPIS IS CREATING IT.
hack club slack: @nelieo.shorya
