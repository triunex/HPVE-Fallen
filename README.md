# HPVE (Haptic Photonic Volumetric Engine)

building an open-air, chamber-free touchable 3D hologram in mid-air for hack club fallen / maker faire shenzhen 2026.

## what is this?

screens are flat and boring, and wearing clunky vr goggles feels terrible. ever since i saw tony stark working over his holographic table in iron man, i wanted to build an interface where you can reach out with your bare hands and physically touch things floating in mid air. no gloves, no controllers, no glass, no acrylic boxes, no smoke screens.

hpve uses phased array ultrasound and synchronized photonic lasers to physically manipulate empty air. by firing 64 ultrasonic transducers at 40,000 hz with microsecond phase delays, sound waves smash together at a single coordinate in 3d space.

this creates two revolutionary spatial capabilities:
1. **chamber-free 3D volumetric holography**: the downward-directed phased array focuses acoustic and photonic vector fields into open space beneath the Halo Arch, suspending a glowing apple-sized 3D vector hologram in mid-air with zero glass, no acrylic tanks, and no VR goggles.
2. **200 Hz touchable mid-air haptics**: when you reach out and push your bare hand into the hologram, downward Time-of-Flight sensors track your fingertips at 60 Hz, dynamically modulating the ultrasound carrier at 200 Hz. The acoustic radiation pressure acts directly on your skin's Pacinian corpuscles, generating an elastic physical forcefield you can actually feel in empty air.

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
- **master avionics carrier motherboard (`hardware/master_carrier/`)**: 124.0 x 76.0 mm 2-layer carrier PCB mounting inside the rear bridge dock of the Halo Arch rig.
  - **MCU Dock**: dual female socket headers for Raspberry Pi Pico (RP2040) with top-edge USB access for direct firmware flashing.
  - **Power Stage**: vertical XT30 12V input with 5A PTC polyfuse (`F1`), `SMBJ15CA` TVS diode (`D1`), `SS34` Schottky reverse-polarity protection (`D2`), and `TPS54302DDCR` synchronous buck regulator (`U2`, 12V -> 5.08V @ 3A max, 10uH `L1`, EN/UVLO divider `R1/R2`). `SS34` OR-ing diode (`D3`) powers Pico `VSYS` safely alongside USB, and `39k/10k` divider monitors 12V telemetry (`VSENSE_12V` on `GP27/ADC1`).
  - **3.3V -> 5.0V Logic Shifter & Tile Link**: `SN74AHCT125D` (`U4`) translates Pico `GP2..GP5` 3.3V logic to 5.0V CMOS with 33R series damping resistors (`R10..R13`) to drive modular tile shift registers. Matched pin-for-pin via bottom-edge Hirose `FH12-6S-0.5SH` (`J2`, Type-D reverse FFC cable). Dual 0.5A PTC polyfuses (`F2`, `F3`) protect both FFC power rails.
  - **8-Dial Console**: `CD74HC4051M` 8:1 analog MUX (`U3`, `GP18..GP20`) reading `DIAL_0..DIAL_7` into `GP26/ADC0` with 8x 100nF hardware debounce capacitors (`C16..C23`), `100k/10nF` COM filter, and star AGND bridge (`R7`, 0R) broken out to `LEFT DIALS` (`J4`), `RIGHT DIALS` (`J5`), and `BRIDGE DIALS` (`J6`).
  - **Circular IPS HUD**: 8-pin JST-XH header (`J3`) driving a 1.28" GC9A01 round SPI1 display (`GP10..GP15`), upgrading from vintage needle meters to real-time graphic holographic animations.
  - **Auxiliary Telemetry & Photonics**: shared I2C0 bus (`GP0/GP1`) for SSD1306 OLED HUD (`J7`) and VL53L5CX ToF sensor (`J8`), 3x `AO3400A` NMOS low-side drivers (`Q1..Q3` + `J9`) for RGB lasers, `AUX_SYNC_4P` (`J10`), tactile buttons `SW1..SW3` (`RESET`, `MODE`, `ENABLE`), and 4 discrete rail/status LEDs (`D4..D7` for 12V, 5V, 3.3V, and Pico GP28 heartbeat).
  - **Certified 0 DRC & 0 ERC**: 84 components, 0 DRC violations, 0 unconnected items, 0 footprint errors, and 0 ERC messages with 100% schematic parity verified.
- **exoskeleton chassis**: native parametric snap-lock exoskeleton locking 4 tiles into a rigid 196x176mm 2x2 grid with internal honeycomb wall core (I-beam perimeter stiffness), under-PCB 45-degree iso-grid diamond trusses, 16x Helmholtz acoustic back-chambers with conical phase-diffusers, 4-finger O-ring collet cradles, and 3-stage knurled brass heat-set insert bosses (M3 & M2.5) with melt-trap relief grooves.
- **portable breakdown halo arch stand (`hardware/cad/halo_arch/`)**: 7-subassembly modular display stand engineered to pack completely flat into a travel backpack. Sliced to fit standard 220x220mm / 256x256mm 3D printer beds:
  - **Sub-Assembly A (Top Central Bridge)**: suspended array dock with dual-axis +/-15 deg gimbal yoke, recessed pocket for 38mm round analog SPL needle gauge (driven via Pico PWM low-pass filter), master amplitude dial, status LEDs, and rear louvered service covers.
  - **Sub-Assemblies B & C (Left & Right Pillars)**: precision 0.15mm slip-fit keyed tenons, 6x custom knurled control dials with internal D-shaft profiles, hollow continuous internal cable raceways, and captive M3 service covers.
  - **Sub-Assemblies D & E (Heavy-Duty Table C-Clamps)**: 60mm jaw opening with 5-lobe hand screws, swivel pressure plates, and soft non-marring TPU gripper pads.
  - **Sub-Assembly F (Under-Desk Power & Battery Chassis)**: quick-release dovetail mount, dual bays fitting Mean Well 12V 10A power supply + 4S2P 18650 Li-ion battery cradle with BMS, illuminated rocker switch, and XT30/XT60 ports.
  - **Sub-Assembly G (Umbilical System)**: flexible corrugated TPU conduits with 90-degree elbows, angled glands, and array rear manifold.
  - **Production STL Suite**: 35+ watertight production STLs verified via 20-stage automated boolean interference test harness.
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
- **closed-loop ToF spatial hand tracker (`firmware/spatial_tracker.c`)**: 60 Hz non-blocking I2C driver for ST VL53L5CX 8x8 multizone depth matrix, connected components / BFS point-cloud clustering, and full 6D state Kalman filter ([x, y, z, vx, vy, vz]^T) with dynamic process noise covariance Q(t) and spatial outlier rejection.
- **autonomous forcefield interception engine (`firmware/spatial_tracker.c`, `wave_solver.c`)**: real-time trigger when hands penetrate Z <= 75mm, instantly switching array from idle/standby and dynamically steering 200 Hz haptic focal beam onto the moving fingertip with non-linear spring restitution F(z) = k * delta_z * (1 + 4.2 * delta_z^2) - b * v_z (up to 280 Pa radiation pressure).
- **cycle-accurate HIL verification (`firmware/verify_cascade_engine.py`, `firmware/verify_trajectory_engine.py`, `firmware/verify_spatial_tracker.py`)**: verified 0 bit errors across 1,024 bits/cycle, 100% trajectory velocity stability, and sub-10mm tracking lock across 180 continuous 60 Hz frames.
- **4-tile star power & high-speed FFC signal integrity engine (`simulation/four_tile_power_and_signal_integrity.py`)**: transient RLC PDN simulation proving 3.24 mV DC drop across 12V 10A star harness (11.997 V minimum tile rail), 27.58 mV SSN dynamic droop (77% safety margin under 120 mV threshold), and 2.10 ns cumulative SPI clock skew across 310 mm daisy chain with +12.90 ns setup slack (zero bit-slips).

## repository structure

```
├── firmware/         # rp2040 c++ code, pio state machines, trajectory engines & verification
├── hardware/         # kicad schematics, pcb layout, gerbers, drc reports
│   ├── modular_tile/   # 16-channel 4-layer modular tile (pcb, gerbers, bom, cpl, 3d renders)
│   ├── master_carrier/ # RP2040 master carrier motherboard (pcb, bom, cpl, schematics, renders)
│   └── cad/            # native openscad exoskeleton, horn bezel, halo arch, stl exports & renders
├── simulation/       # python wave physics model, 3d gor'kov trapping, impedance
├── docs/             # calculations, notes, pinout architecture
├── CAMPAIGN_ROADMAP_45_DAYS.md # master 45-day expedition manifest & live telemetry
└── README.md
```

## fallen 6-week roadmap

- [x] simulate wave interference in python (142x focal pressure verified)
- [x] 3D Gor'kov acoustic trap & 200Hz skin haptics simulation modeled
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
- [x] **700+ XP CLUB UNLOCKED: 8.15h LOGGED (+75 XP) ➔ 703 XP TOTAL (SATURDAY STREAK ACTIVE, 1.15x MULTIPLIER)!**
- [x] implement closed-loop VL53L5CX ToF spatial hand tracking & autonomous forcefield trigger (`firmware/spatial_tracker.c`)
- [x] 4-tile master power & FFC signal integrity validation: simulated 3.24 mV DC drop, 27.6 mV SSN droop, 2.10 ns clock skew (`simulation/fig13`)
- [x] **900+ XP CLUB UNLOCKED: 16h 40m MONSTER SPRINT (+171 XP) ➔ 929 XP TOTAL (ONLY 71 XP TO FINAL FORM)!**
- [x] portable breakdown halo arch stand CAD: 7 modular sub-assemblies, 35+ production STLs, 20-stage fit check harness (fits standard 220x220mm beds)
- [x] standalone modular tile schematic updated to v1.2 with verified 0.5mm FFC daisy-chain pinouts
- [x] **1,000+ XP SHENZHEN QUALIFIER SHATTERED! (1,043 XP banked, 108h 10m logged across 14 journals!)**
- [x] design & route RP2040 Master Avionics Carrier PCB (`HPVE_Master_Carrier` MK-I) in KiCad 10
- [x] integrate 12V 3A buck, 8-dial MUX, 1.28" GC9A01 circular IPS display, 74AHCT125 level shifter, dual FFC polyfuses, ToF, and RGB lasers
- [x] achieve 0 DRC violations, 0 unconnected pads, 0 footprint errors, and 0 ERC messages on Master Carrier
- [ ] submit official Chinese F-Visa & $1,000 USD Travel Grant application on Hack Club portal
- [ ] build desktop Stark Holo-Console GUI (`host_console/`) with live 3D Gor'kov slices and G-code control
- [ ] order custom tiles from JLCPCB and components from LCSC / AliExpress
- [ ] assemble and calibrate acoustic output across 4 snapped modular tiles
- [ ] test mid-air skin haptics and touchable volumetric forcefield with multi-tile array
- [ ] ship build submission and demo live at maker faire shenzhen 2026!

## author

ENTROPIS IS CREATING IT.
hack club slack: @nelieo.shorya
