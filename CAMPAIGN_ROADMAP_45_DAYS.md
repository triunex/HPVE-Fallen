# HPVE 45-DAY MASTER EXPEDITION MANIFEST
## Target: 2,500+ XP | Hack Club Fallen | Maker Faire Shenzhen 2026
### Operative: nelieo.shorya | Division: Iron Cave Industries | Companion: Sami (Team Sami 🦊)

```
==================================================================================================
           HAPTIC PHOTONIC VOLUMETRIC ENGINE (HPVE) - 45-DAY CAMPAIGN ROADMAP
==================================================================================================
  MISSION TARGET: 2,500+ XP (1,000 XP Qualification + $1,500+ USD Travel Stipend Reimbursement)
  COMPANION: SAMI | STREAK STRATEGY: 1.00x -> 1.75x MAX ACCELERATOR
  CRITICAL HARD STOP: DECEMBER 9TH, 2026 (11:59 PM ET)
  EXPEDITION DATES: DECEMBER 11 - 14, 2026 @ MAKER FAIRE SHENZHEN, CHINA
==================================================================================================
```

---

## 1. Real-Time Telemetry & XP Dashboard

| Metric | Target Value | Current Value | Status |
|---|---|---|---|
| **Total XP Accumulated** | 2,500 XP | **25 XP** | 🟢 ON TRACK (HATCH COMPLETE) |
| **Current Multiplier** | 1.75x (Max) | **1.00x** (Week 1) | ⚡ 3 Freezes Remaining |
| **Visa Invitation Gate** | 100 XP (Required) | 25 / 100 XP | ⏳ Unlocks ~Day 4 (Nov 2) |
| **Need-Based Flight Gate** | 500 XP ($1,000 Grant) | 25 / 500 XP | ⏳ Unlocks ~Day 18 (Nov 16) |
| **Shenzhen Qualifier Gate** | 1,000 XP (Final Form) | 25 / 1,000 XP | ⏳ Target: Day 30 (Nov 28) |
| **Cash Stipend Surplus** | $1,500 USD (1,500 XP+) | $0.00 | ⏳ Unlocks after 1,000 XP |
| **Stage Progression** | Stage 1 ➔ Stage 2 | Stage 1 (175 XP to go) | 🟢 ACTIVE |
| **Days Remaining** | 42 Days to Qualify | 42 Days | 🟢 FULL RUNWAY |

---

## 2. The 6 Strategic Phase Banners

```
[PHASE 01: DAYS 01-07] DIGITAL ARCHITECTURE & WAVE INTERACTION MATH       (XP: 0000 -> 0200)
[PHASE 02: DAYS 08-14] KICAD SCHEMATIC, PCB ROUTING & DESIGN SUBMISSION  (XP: 0200 -> 0450)
[PHASE 03: DAYS 15-21] FIRMWARE PROTOTYPING & 500 XP FLIGHT STIPEND       (XP: 0450 -> 0800)
[PHASE 04: DAYS 22-28] HARDWARE ARRIVAL, SOLDERING & POWER STAGE PROOF   (XP: 0800 -> 1250)
[PHASE 05: DAYS 29-35] 1000 XP QUALIFIER: LEVITATION & HAPTIC TUNING     (XP: 1250 -> 1800)
[PHASE 06: DAYS 36-45] BUILD SUBMISSION, PACKING & MAKER FAIRE SHENZHEN   (XP: 1800 -> 2500+)
```

---

## PHASE 01: DIGITAL ARCHITECTURE & WAVE INTERACTION MATH (DAYS 1 - 7)
### Multiplier: 1.00x | Base Rate: 8 XP / Hour | Goal: 200 XP | Unlock: Chinese Visa Letter (>100 XP)

---

### DAY 01: Friday, October 30, 2026 — MISSION INITIATION & EGG HATCH
- **Target Hours:** 2h 04m | **Target XP:** +25 XP | **Multiplier:** 1.00x
- **Focus:** Mathematical Validation, Git Setup, Project Genesis, Sami Hatching
- [x] **Task 1.1:** Clone and initialize official repository `HPVE-Fallen` on GitHub.
- [x] **Task 1.2:** Fix syntax error in `hpve_physics_model.py` and verify focal calculation.
- [x] **Task 1.3:** Execute `verify_hpve.py` — proved 142.4x focal pressure ratio at 20cm height.
- [x] **Task 1.4:** Generate real 2D acoustic pressure heatmap (`simulation/acoustic_field_simulation.png`).
- [x] **Task 1.5:** Write lean, justified `hardware/bom.csv` with live supplier links ($187.85 budget).
- [x] **Task 1.6:** Draft RP2040 PIO state machine pulse generator (`firmware/pio_40khz_phased.pio`).
- [x] **Task 1.7:** Author system electrical wiring architecture (`hardware/PINOUT_ARCHITECTURE.md`).
- [x] **Task 1.8:** Create HPVE project on Fallen Dashboard.
- [x] **Task 1.9:** Log Session 1 (35m) in `#fallen-journals` — **SAMI HATCHED! 25 XP BANKED!**
- [x] **Task 1.10 (DAY 1 SPRINT COMPLETE):**
  - Generated KiCad hierarchical schematic architecture (`hardware/kicad/HPVE_Controller.kicad_pro`).
  - Executed shift-register timing simulation: verified 20MHz SPI loads 64 channels in 3.25us (87% timing margin).
  - Generated timing analysis chart (`simulation/shift_register_timing_analysis.png`).
  - Resolved 105 component footprints across 4 hierarchical sheets.
  - Linked all 64 channels across sheets using global net labels (`DRV_1..64`, `SIG_1..64`, `+12V`, `GND`).
  - Executed native KiCad Python auto-router (`route_hpve_board.py`): routed 392 physical copper tracks on `F.Cu` and `B.Cu`.
  - Added precise 140 mm x 140 mm board outline (`Edge.Cuts`) and 4x M3 corner mounting holes (3.2 mm drill).
  - Generated high-resolution 3D raytraced PCB renders (`hpve_pcb_routed_top.png` and `hpve_pcb_routed_3d.png`).
  - Exported complete factory-ready RS-274X Gerber layers and Excellon NC drill files (300 plated through-holes).
  - Generated automated SMT pick-and-place centroid coordinate file (`HPVE_Controller-all-pos.csv`).
  - Authored aerospace/defense-grade IPC-A-600 Class 2 fabrication specification (`hardware/gerbers/FABRICATION_NOTES.md`).
  - Packaged production zip archive: `hardware/gerbers/HPVE_Motherboard_v1.0_Production_Gerbers.zip`.
  - Committed and pushed all 34 production files to GitHub (`main`).
  - Friday daily streak locked! Completed 2h 01m total logged time across 3 development sessions!

---

### DAY 02: Saturday, October 31, 2026 — TRANSDUCER RESONANCE & IMPEDANCE DYNAMICS
- **Target Hours:** 3h 00m | **Actual Logged Today:** 4h 11m | **Actual XP Banked:** +33 XP | **Running XP Total:** 68 XP
- **Focus:** Transducer electrical characteristics and 400ST160 resonant curve
- [x] **Task 2.1:** Create `simulation/transducer_impedance_model.py`: model the 40 kHz series resonance (C0 = 2.2 nF, Cm = 123.2 pF, Lm = 128.5 mH, Rm = 922.7 Ohm).
- [x] **Task 2.2:** Calculate reactive current consumption: 11.11 mA RMS per channel, 0.711 A total continuous array RMS current at 12Vpp.
- [x] **Task 2.3:** Calculate thermal dissipation across all 32x TC4427 driver IC packages: 30.15 mW per IC, 3.8 deg C junction temperature rise.
- [x] **Task 2.4:** Determine optimum bulk decoupling capacitor values on the +12V power rail: minimum 16.90 uF for <100mV droop, validating our 100 uF low-ESR bulk rail.
- [x] **Task 2.5:** Journal entry: "Why 64 transducers act like a giant capacitive sponge (and how not to burn the lab down)". Logged 4h 11m, +33 XP banked! Total: 68 XP!

---

### DAY 03: Sunday, November 01, 2026 — 3D GOR'KOV ACOUSTIC TRAP POTENTIAL MODELING
- **Target Hours:** 3h 30m | **Target XP:** +28 XP | **Running XP Total:** ~77 XP
- **Focus:** Particle levitation physics and acoustic radiation force field
- [ ] **Task 3.1:** Write `simulation/gorkov_potential.py`: implement the Gor'kov acoustic potential equation:
  $$U = 2\pi R^3 \left( \frac{\langle p^2 \rangle}{3\rho_0 c_0^2} f_1 - \frac{\rho_0 \langle v^2 \rangle}{2} f_2 \right)$$
- [ ] **Task 3.2:** Plot 3D potential well showing the stable trapping zone for a 2mm polystyrene sphere at (0, 0, 15cm).
- [ ] **Task 3.3:** Calculate maximum axial and lateral trapping stiffness ($F = -\nabla U$).
- [ ] **Task 3.4:** Confirm maximum theoretical levitation payload weight against gravity ($F_{acoustic} > m \cdot g$).
- [ ] **Task 3.5:** Commit Gor'kov potential scripts and visual plots to `simulation/`.
- [ ] **Task 3.6:** Journal entry: "Trapping matter in mid-air with pure acoustic interference equations". Attach 3D potential plot.

---

### DAY 04: Monday, November 02, 2026 — 100 XP VISA MILESTONE UNLOCKED
- **Target Hours:** 3h 00m | **Target XP:** +24 XP | **Running XP Total:** ~101 XP
- **Focus:** Reaching 100 XP Threshold & Chinese F-Visa Application
- [ ] **Task 4.1:** Verify XP crosses 100 on dashboard.
- [ ] **Task 4.2:** **CRITICAL ACTION:** Fill out the official Hack Club Visa Form at `https://forms.hackclub.com/fallen-visa` to request the **Chinese F-Visa Invitation Letter**.
- [ ] **Task 4.3:** Gather passport scan and prepare visa submission dossier.
- [ ] **Task 4.4:** Write `hardware/CIRCUIT_SPECS.md` detailing the signal routing from RP2040 to TC4427 inputs.
- [ ] **Task 4.5:** Journal entry: "Hit 100 XP! Chinese Visa invitation requested + deep dive into driver gate delays".

---

### DAY 05: Tuesday, November 03, 2026 — SHIFT REGISTER PIPELINE BENCHMARKING
- **Target Hours:** 3h 30m | **Target XP:** +28 XP | **Running XP Total:** ~129 XP
- **Focus:** SPI vs. Bit-Bang Data Throughput for 64-Channel Output
- [ ] **Task 5.1:** Write `firmware/shift_register_bench.c` to test SPI clock rates up to 30 MHz on RP2040.
- [ ] **Task 5.2:** Measure latency to shift 64 bits through eight 74HC595 shift registers.
- [ ] **Task 5.3:** Verify that shift register reload frequency comfortably exceeds the 40 kHz acoustic period ($25\ \mu\text{s}$).
- [ ] **Task 5.4:** Design latch signal synchronization using RP2040 PWM hardware interrupt.
- [ ] **Task 5.5:** Journal entry: "Pushing 30 MHz SPI down an 8-stage shift register daisy chain".

---

### DAY 06: Wednesday, November 04, 2026 — UI & TELEMETRY SUBSYSTEM
- **Target Hours:** 3h 00m | **Target XP:** +24 XP | **Running XP Total:** ~153 XP
- **Focus:** SSD1306 OLED HUD & EC11 Rotary Encoder Interrupt Handler
- [ ] **Task 6.1:** Create `firmware/ui_controller.cpp`: implement I2C OLED driver using lightweight SSD1306 ASCII library.
- [ ] **Task 6.2:** Create custom bitmap icons: HPVE logo, 3D Target Reticle, Frequency Indicator, Active Channel Counter.
- [ ] **Task 6.3:** Write quadrature decoding ISR for dual EC11 rotary encoders on GPIO 10, 11, 12, 13 with hardware debouncing.
- [ ] **Task 6.4:** Test coordinate incrementation ($X, Y, Z$) with velocity-based acceleration.
- [ ] **Task 6.5:** Journal entry: "Designing the Stark lab UI: zero-latency rotary control on a 0.96 inch OLED HUD".

---

### DAY 07: Thursday, November 05, 2026 — WEEK 1 REVIEW & SAMI MULTIPLIER UPGRADE
- **Target Hours:** 3h 30m | **Target XP:** +28 XP | **Running XP Total:** ~181 XP
- **Focus:** Week 1 Streak Lock, Multiplier Boost, Full Phase 1 Git Tag
- [ ] **Task 7.1:** Verify 7 consecutive days of journaling.
- [ ] **Task 7.2:** **TEAM SAMI UPGRADE:** Multiplier increases from **1.00x ➔ 1.15x**!
- [ ] **Task 7.3:** Clean up code, add docstrings, and tag release `v0.1-digital-prototype` on GitHub.
- [ ] **Task 7.4:** Prepare KiCad project workspace for Week 2 schematic sprint.
- [ ] **Task 7.5:** Journal entry: "Week 1 in the books: Sami multiplier upgraded to 1.15x + digital architecture locked".

---

## PHASE 02: KICAD SCHEMATIC, PCB ROUTING & DESIGN SUBMISSION (DAYS 8 - 14)
### Multiplier: 1.15x | Base Rate: 9.2 XP / Hour | Goal: 250 XP | Total XP: 181 ➔ 431 | Unlock: Project Funding via HCB

---

### DAY 08: Friday, November 06, 2026 — KICAD SCHEMATIC: MCU CORE & POWER REGULATION
- **Target Hours:** 3h 30m | **Target XP:** +32 XP | **Running XP Total:** ~213 XP
- **Focus:** Power rail decoupling, buck regulator circuit, Pico breakout
- [ ] **Task 8.1:** Create `hardware/kicad/HPVE_Main.kicad_sch`.
- [ ] **Task 8.2:** Design DC Input stage: 5.5x2.1mm barrel jack, reverse-polarity protection Schottky diode, TVS surge protection.
- [ ] **Task 8.3:** Wire MP1584EN buck converter step-down circuit (12V in ➔ 5.0V out).
- [ ] **Task 8.4:** Wire Raspberry Pi Pico module symbol with proper bypass capacitors on VSYS and 3V3.
- [ ] **Task 8.5:** Journal entry: "Laying the electrical skeleton in KiCad: power rails and surge defense". Attach schematic screenshot.

---

### DAY 09: Saturday, November 07, 2026 — KICAD SCHEMATIC: SHIFT REGISTER & DRIVER MATRIX
- **Target Hours:** 4h 00m | **Target XP:** +37 XP | **Running XP Total:** ~250 XP
- **Focus:** 8-stage 74HC595 cascade and 32x TC4427 driver IC array
- [ ] **Task 9.1:** Create hierarchical schematic sheet `Transducer_Drivers.kicad_sch`.
- [ ] **Task 9.2:** Wire the 8x 74HC595 shift register cascade with shared clock and latch busses.
- [ ] **Task 9.3:** Wire all 32x TC4427A dual high-speed gate driver ICs (64 driver channels).
- [ ] **Task 9.4:** Place 0.1uF ceramic decoupling capacitors directly on every single TC4427 VDD pin.
- [ ] **Task 9.5:** Journal entry: "Routing 64 high-voltage gate driver channels in KiCad without losing my mind".

---

### DAY 10: Sunday, November 08, 2026 — KICAD SCHEMATIC: 8x8 TRANSDUCER ARRAY GRID
- **Target Hours:** 4h 00m | **Target XP:** +37 XP | **Running XP Total:** ~287 XP
- **Focus:** Cartesian 8x8 transducer matrix and net labels
- [ ] **Task 10.1:** Create hierarchical schematic sheet `Transducer_Array.kicad_sch`.
- [ ] **Task 10.2:** Create custom footprint for 16mm 400ST160 ultrasonic transducers (10mm pin pitch).
- [ ] **Task 10.3:** Lay out 64 transducer symbols in an 8x8 matrix (TX01 to TX64).
- [ ] **Task 10.4:** Connect each positive terminal to its designated TC4427 driver output net.
- [ ] **Task 10.5:** Run KiCad Electrical Rules Check (ERC) and eliminate all floating pin warnings.
- [ ] **Task 10.6:** Journal entry: "The 64-speaker matrix schematic is clean: 0 ERC errors, 0 warnings".

---

### DAY 11: Monday, November 09, 2026 — PCB LAYOUT: COMPONENT PLACEMENT & STACKUP
- **Target Hours:** 3h 30m | **Target XP:** +32 XP | **Running XP Total:** ~319 XP
- **Focus:** Board dimensions (120x120mm), thermal relief, 8x8 grid placement
- [ ] **Task 11.1:** Define board edge in `HPVE_Main.kicad_pcb`: 120mm x 120mm with 4mm mounting corner holes.
- [ ] **Task 11.2:** Place the 64 ultrasonic transducers in an exact 12mm pitch Cartesian grid on the top copper layer.
- [ ] **Task 11.3:** Place TC4427 drivers symmetrically on the bottom layer to minimize trace lengths.
- [ ] **Task 11.4:** Define 2-layer PCB stackup: Top layer (Signals + Local Power), Bottom layer (Solid Ground Plane).
- [ ] **Task 11.5:** Journal entry: "Packaging 64 ultrasound transducers into a 120mm square board".

---

### DAY 12: Tuesday, November 10, 2026 — PCB ROUTING: TRACES, BUSES & POWER PLANES
- **Target Hours:** 4h 00m | **Target XP:** +37 XP | **Running XP Total:** ~356 XP
- **Focus:** Routing 64 high-speed lines, +12V high-current bus, star ground
- [ ] **Task 12.1:** Route the +12V power bus with thick 1.5mm copper traces to handle 5A current spikes.
- [ ] **Task 12.2:** Route the shift register high-speed clock and data lines with matched trace lengths.
- [ ] **Task 12.3:** Route the 64 transducer driver traces with generous spacing to prevent capacitive crosstalk.
- [ ] **Task 12.4:** Pour solid ground copper planes on both top and bottom layers with stitching ground vias.
- [ ] **Task 12.5:** Run KiCad Design Rules Check (DRC) according to JLCPCB manufacturing limits (6mil track/space).
- [ ] **Task 12.6:** Journal entry: "Going to war with trace routing: 64 channels, zero DRC errors".

---

### DAY 13: Wednesday, November 11, 2026 — 3D ENCLOSURE DESIGN & STEP EXPORT
- **Target Hours:** 3h 30m | **Target XP:** +32 XP | **Running XP Total:** ~388 XP
- **Focus:** CAD Modeling (Fusion 360 / Onshape) of desktop chassis
- [ ] **Task 13.1:** Export 3D PCB model (`.step`) from KiCad.
- [ ] **Task 13.2:** Model top bezel with 64 precision acoustic horn apertures to guide acoustic beamforming.
- [ ] **Task 13.3:** Model bottom chassis with internal mounting bosses, DC barrel cutout, and OLED frame.
- [ ] **Task 13.4:** Add ventilation slots for driver thermal convection.
- [ ] **Task 13.5:** Export `.step` assembly files into `hardware/cad/`.
- [ ] **Task 13.6:** Journal entry: "Designing the matte-black desk enclosure in CAD around the 3D PCB model".

---

### DAY 14: Thursday, November 12, 2026 — DESIGN SUBMISSION & HACK CLUB GRANT APPLICATION
- **Target Hours:** 4h 30m | **Target XP:** +41 XP | **Running XP Total:** ~429 XP
- **Focus:** Submitting Official Design Submission Package for HCB Grant Card
- [ ] **Task 14.1:** Generate JLCPCB Gerber zip package (`hardware/gerbers/HPVE_Gerbers_v1.0.zip`).
- [ ] **Task 14.2:** Finalize `hardware/bom.csv` with exact supplier links and unit costs ($187.85).
- [ ] **Task 14.3:** Update GitHub repository README with 3D assembly renders, schematic images, and PCB layout preview.
- [ ] **Task 14.4:** **OFFICIAL ACTION:** Submit the **Design Submission** on the Fallen platform for mentor review.
- [ ] **Task 14.5:** **TEAM SAMI UPGRADE:** Multiplier increases from **1.15x ➔ 1.30x**!
- [ ] **Task 14.6:** Journal entry: "DESIGN SUBMISSION FIRED! Gerbers packed, BOM locked, ready for HCB funding".

---

## PHASE 03: FIRMWARE PROTOTYPING & 500 XP FLIGHT STIPEND (DAYS 15 - 21)
### Multiplier: 1.30x | Base Rate: 10.4 XP / Hour | Goal: 370 XP | Total XP: 429 ➔ 799 | Unlock: $1,000 Flight Grant

---

### DAY 15: Friday, November 13, 2026 — RP2040 PIO MULTI-CHANNEL TIMING RIG
- **Target Hours:** 4h 00m | **Target XP:** +41 XP | **Running XP Total:** ~470 XP
- **Focus:** Hardware testing on Raspberry Pi Pico breadboard
- [ ] **Task 15.1:** Flash `firmware/main.cpp` onto Raspberry Pi Pico.
- [ ] **Task 15.2:** Connect Pico GPIO outputs to a logic analyzer or oscilloscope.
- [ ] **Task 15.3:** Verify exact 40.0 kHz frequency (25.0 microsecond period) on output pins.
- [ ] **Task 15.4:** Verify programmable phase shifts: shift GPIO by 3.125 microseconds ($\pi/4$ phase delay).
- [ ] **Task 15.5:** Journal entry: "Oscilloscope traces show razor-sharp 40kHz pulses on the Pico PIO engine". Attach scope trace.

---

### DAY 16: Saturday, November 14, 2026 — 500 XP FLIGHT GRANT MILESTONE UNLOCKED!
- **Target Hours:** 4h 00m | **Target XP:** +41 XP | **Running XP Total:** ~511 XP
- **Focus:** 500 XP Milestone & Submitting the $1,000 Need-Based Flight Application
- [ ] **Task 16.1:** Verify XP crosses **500 XP** on dashboard.
- [ ] **Task 16.2:** **CRITICAL ACTION:** Submit the official **$1,000 USD Need-Based Travel Stipend Application**.
- [ ] **Task 16.3:** Pitch the HPVE project in the stipend application with the verified math, KiCad schematics, and PIO demo.
- [ ] **Task 16.4:** Email `fallen@hackclub.com` confirming 500 XP qualification and flight request.
- [ ] **Task 16.5:** Journal entry: "500 XP ACHIEVED! Submitted the $1,000 flight stipend application to Hack Club".

---

### DAY 17: Sunday, November 15, 2026 — GRANT APPROVAL & PLACING HARDWARE ORDERS
- **Target Hours:** 3h 30m | **Target XP:** +36 XP | **Running XP Total:** ~547 XP
- **Focus:** HCB Grant Card Activation & Component Ordering
- [ ] **Task 17.1:** Receive HCB virtual grant card details from Hack Club team.
- [ ] **Task 17.2:** Order custom PCBs from JLCPCB with **DHL Express shipping** (4-5 business days).
- [ ] **Task 17.3:** Order 64x 400ST160 transducers and 32x TC4427 drivers from LCSC / AliExpress.
- [ ] **Task 17.4:** Order soldering kit, multimeter, power supply, and hardware from Amazon.
- [ ] **Task 17.5:** Log all order tracking numbers and receipts in `docs/ORDERS.md`.
- [ ] **Task 17.6:** Journal entry: "Grant money landed! Parts ordered across JLCPCB, LCSC, and Amazon".

---

### DAY 18: Monday, November 16, 2026 — 3D PRINTING PROTOTYPE TEST CHASSIS
- **Target Hours:** 3h 30m | **Target XP:** +36 XP | **Running XP Total:** ~583 XP
- **Focus:** Slicing and 3D printing the enclosure components
- [ ] **Task 18.1:** Slice enclosure top and bottom plates in PrusaSlicer / Bambu Studio.
- [ ] **Task 18.2:** Set print parameters: 0.2mm layer height, 4 perimeters, 25% gyroid infill (PETG or PLA+).
- [ ] **Task 18.3:** Print test bezel and check fit with dummy 16mm cylindrical cylinders.
- [ ] **Task 18.4:** Verify M3 brass heat-set threaded insert hole dimensions.
- [ ] **Task 18.5:** Journal entry: "3D printer running overnight: printing the acoustic bezel and chassis shell".

---

### DAY 19: Tuesday, November 17, 2026 — DYNAMIC BEAM STEERING MATH ENGINE
- **Target Hours:** 4h 00m | **Target XP:** +41 XP | **Running XP Total:** ~624 XP
- **Focus:** Implementing 3D trajectory calculation in C++
- [ ] **Task 19.1:** Write `firmware/trajectory.cpp`: circular and Lissajous orbit trajectories for levitated particles.
- [ ] **Task 19.2:** Optimize phase math with integer lookup tables (LUT) for trigonometric functions on ARM Cortex-M0+.
- [ ] **Task 19.3:** Benchmark phase recalculation speed: ensure Core 0 can compute all 64 phases in under 500 microseconds.
- [ ] **Task 19.4:** Test rotary encoder coordinate input feeding into real-time phase updates.
- [ ] **Task 19.5:** Journal entry: "Cracking fast fixed-point trigonometry on the RP2040: 64 phases updated in 380us".

---

### DAY 20: Wednesday, November 18, 2026 — MID-AIR HAPTIC MODULATION SYNTHESIZER
- **Target Hours:** 4h 00m | **Target XP:** +41 XP | **Running XP Total:** ~665 XP
- **Focus:** Low-frequency tactile modulation for human mechanoreceptors
- [ ] **Task 20.1:** Human skin mechanoreceptors (Pacinian corpuscles) respond strongest to **200 Hz** vibrations.
- [ ] **Task 20.2:** Write `firmware/haptic_modulation.cpp`: amplitude-modulate the 40 kHz carrier wave with a 200 Hz square/sine envelope.
- [ ] **Task 20.3:** Implement tactile click profiles: single pulse click, continuous buzzing wall, variable stiffness spring.
- [ ] **Task 20.4:** Verify modulation envelope on oscilloscope.
- [ ] **Task 20.5:** Journal entry: "Engineering tactile sensation: modulating 40kHz ultrasound at 200Hz for skin perception".

---

### DAY 21: Thursday, November 19, 2026 — WEEK 3 MILESTONE & MULTIPLIER BOOST
- **Target Hours:** 4h 00m | **Target XP:** +41 XP | **Running XP Total:** ~706 XP
- **Focus:** Week 3 Complete, Multiplier Upgrade, Firmware Milestone Commit
- [ ] **Task 21.1:** Commit all tested firmware routines to `firmware/`.
- [ ] **Task 21.2:** **TEAM SAMI UPGRADE:** Multiplier increases from **1.30x ➔ 1.45x**!
- [ ] **Task 21.3:** Chinese Visa status check with embassy / visa agency.
- [ ] **Task 21.4:** Journal entry: "Week 3 complete! Sami multiplier hits 1.45x and firmware core is fully compiled".

---

## PHASE 04: HARDWARE ARRIVAL, SOLDERING & POWER STAGE PROOF (DAYS 22 - 28)
### Multiplier: 1.45x | Base Rate: 11.6 XP / Hour | Goal: 450 XP | Total XP: 706 ➔ 1156 | Unlock: 1,000 XP QUALIFICATION!

---

### DAY 22: Friday, November 20, 2026 — PARTS ARRIVAL & UNBOXING LAB LOG
- **Target Hours:** 4h 00m | **Target XP:** +46 XP | **Running XP Total:** ~752 XP
- **Focus:** JLCPCB boards & component inspection
- [ ] **Task 22.1:** DHL Express package arrives: inspect JLCPCB 2-layer matte black boards.
- [ ] **Task 22.2:** Verify PCB trace continuity, copper thickness, and footprint alignments with physical components.
- [ ] **Task 22.3:** Inspect 64x 400ST160 transducers and verify polarity markings (+ pin marked with red dot or longer lead).
- [ ] **Task 22.4:** Set up soldering workstation: clean iron tips, solder flux, brass wool, exhaust fan.
- [ ] **Task 22.5:** Journal entry: "THE BOARDS HAVE LANDED! Unboxing the matte-black JLCPCB PCBs and 64 ultrasound transducers". Attach photos.

---

### DAY 23: Saturday, November 21, 2026 — POWER STAGE & DC REGULATOR SOLDERING
- **Target Hours:** 4h 30m | **Target XP:** +52 XP | **Running XP Total:** ~804 XP
- **Focus:** DC jack, buck converter, decoupling capacitors
- [ ] **Task 23.1:** Solder DC barrel jack, reverse protection diode, and main bulk electrolytic capacitors.
- [ ] **Task 23.2:** Solder MP1584EN buck converter module and calibrate trim pot to exactly **5.05V**.
- [ ] **Task 23.3:** Test 12V high-current bus with digital multimeter: verify 0 ohms shorts between VDD and GND before powering.
- [ ] **Task 23.4:** Power up with 12V 5A power supply: verify clean 5V logic rail under dummy load.
- [ ] **Task 23.5:** Journal entry: "Power rail brought to life: stable 12V high-power bus and 5V logic verified with multimeter".

---

### DAY 24: Sunday, November 22, 2026 — SHIFT REGISTER & DRIVER IC ARRAY ASSEMBLY
- **Target Hours:** 5h 00m | **Target XP:** +58 XP | **Running XP Total:** ~862 XP
- **Focus:** Soldering eight 74HC595 ICs and thirty-two TC4427 driver ICs
- [ ] **Task 24.1:** Solder 8x 74HC595 shift registers onto the control section.
- [ ] **Task 24.2:** Solder 32x TC4427 MOSFET gate drivers (SOIC-8 / DIP-8) with individual decoupling caps.
- [ ] **Task 24.3:** Connect Raspberry Pi Pico to the board via female header sockets.
- [ ] **Task 24.4:** Flash test firmware and probe driver outputs: verify 12V peak-to-peak output swing on all 64 driver lines.
- [ ] **Task 24.5:** Journal entry: "Solder marathon: 32 gate drivers installed and delivering clean 12Vpp square waves". Attach PCB photo.

---

### DAY 25: Monday, November 23, 2026 — THE 64 TRANSDUCER SOLDERING SPRINT
- **Target Hours:** 5h 00m | **Target XP:** +58 XP | **Running XP Total:** ~920 XP
- **Focus:** Soldering all 64 ultrasonic transducers into the 8x8 matrix
- [ ] **Task 25.1:** Insert first 32 transducers (Rows 1–4) ensuring identical polarity orientation across the matrix.
- [ ] **Task 25.2:** Solder through-hole pins, trim leads flush to the ground plane.
- [ ] **Task 25.3:** Insert remaining 32 transducers (Rows 5–8) and solder complete array.
- [ ] **Task 25.4:** Clean board with 99% Isopropyl Alcohol (IPA) to remove conductive flux residues.
- [ ] **Task 25.5:** Journal entry: "64 transducers soldered! The acoustic emitter plate is fully assembled and looking like science fiction". Attach photos.

---

### DAY 26: Tuesday, November 24, 2026 — FIRST ACOUSTIC PULSE & SMOKE TEST
- **Target Hours:** 4h 30m | **Target XP:** +52 XP | **Running XP Total:** ~972 XP
- **Focus:** First live power-on test of the complete acoustic matrix
- [ ] **Task 26.1:** Connect current-limited 12V supply. Monitor current draw at idle (expect <100mA).
- [ ] **Task 26.2:** Enable 40 kHz phased output across all 64 channels.
- [ ] **Task 26.3:** Verify operating current at 40 kHz (expect 1.5A–2.5A continuous draw).
- [ ] **Task 26.4:** Check for thermal hotspots using IR thermometer on TC4427 driver ICs.
- [ ] **Task 26.5:** Confirm ultrasonic emission using an acoustic receiver probe or contact microphone.
- [ ] **Task 26.6:** Journal entry: "FIRST SOUND WAVE FIRED: 64 transducers humming at 40kHz, zero magic smoke".

---

### DAY 27: Wednesday, November 25, 2026 — 1,000 XP QUALIFIER THRESHOLD BREACHED!
- **Target Hours:** 4h 30m | **Target XP:** +52 XP | **Running XP Total:** ~1,024 XP
- **Focus:** CROSSING THE 1,000 XP LINE TO QUALIFY FOR SHENZHEN!
- [ ] **Task 27.1:** **HISTORIC MILESTONE:** Dashboard displays **1,000+ XP**!
- [ ] **Task 27.2:** Character reaches **Final Form Evolution** on Fallen dashboard!
- [ ] **Task 27.3:** Wire OLED telemetry display and dual rotary encoders to the chassis front panel.
- [ ] **Task 27.4:** Test real-time coordinate adjustment: turning encoder moves phase focal math from $Z=10\text{cm}$ to $Z=25\text{cm}$.
- [ ] **Task 27.5:** Journal entry: "1,000 XP CRACKED! Final character evolution unlocked, and live HUD is talking to the emitter".

---

### DAY 28: Thursday, November 26, 2026 — WEEK 4 REVIEW & MAX MULTIPLIER UNLOCKED
- **Target Hours:** 4h 00m | **Target XP:** +46 XP | **Running XP Total:** ~1,070 XP
- **Focus:** Team Sami Max Multiplier (1.60x), Flight Booking Coordination
- [ ] **Task 28.1:** Verify 4 continuous weeks of daily journaling without missing a single day.
- [ ] **Task 28.2:** **TEAM SAMI UPGRADE:** Multiplier increases from **1.45x ➔ 1.60x**!
- [ ] **Task 28.3:** Finalize flight bookings with Hack Club travel team.
- [ ] **Task 28.4:** Journal entry: "Week 4 complete: into the surplus XP zone ($1 per XP earned starts paying for China travel!)".

---

## PHASE 05: 1000 XP SURPLUS: LEVITATION & HAPTIC TUNING (DAYS 29 - 35)
### Multiplier: 1.60x | Base Rate: 12.8 XP / Hour | Goal: 500 XP | Total XP: 1,070 ➔ 1,570 | Earn Surplus Cash Stipends

---

### DAY 29: Friday, November 27, 2026 — FIRST ACOUSTIC LEVITATION TEST (THE BEAD TRAP)
- **Target Hours:** 4h 30m | **Target XP:** +57 XP | **Running XP Total:** ~1,127 XP
- **Focus:** Capturing a 2mm polystyrene particle in mid-air
- [ ] **Task 29.1:** Set focal coordinate to $(0, 0, 15\text{cm})$ directly above matrix center.
- [ ] **Task 29.2:** Use anti-static tweezers to introduce a 2mm expanded polystyrene bead into the acoustic node.
- [ ] **Task 29.3:** Adjust acoustic power until the bead stabilizes in the Gor'kov potential well.
- [ ] **Task 29.4:** **FIRST LEVITATION:** The bead floats motionless in thin air!
- [ ] **Task 29.5:** Record high-speed macro video of the suspended particle.
- [ ] **Task 29.6:** Journal entry: "IT FLOATS. Absolute zero contact acoustic levitation achieved on the bench". Attach video clips.

---

### DAY 30: Saturday, November 28, 2026 — 3D SPATIAL PARTICLE STEERING
- **Target Hours:** 5h 00m | **Target XP:** +64 XP | **Running XP Total:** ~1,191 XP
- **Focus:** Moving the levitated bead in 3D space via rotary encoder
- [ ] **Task 30.1:** Map rotary encoder 1 to lateral position ($X$-axis: $-4\text{cm}$ to $+4\text{cm}$).
- [ ] **Task 30.2:** Map rotary encoder 2 to vertical height ($Z$-axis: $10\text{cm}$ to $22\text{cm}$).
- [ ] **Task 30.3:** Test dynamic tracking: turn encoder and watch the trapped bead smoothly rise, drop, and maneuver.
- [ ] **Task 30.4:** Measure maximum traversal speed without losing acoustic trap stability.
- [ ] **Task 30.5:** Journal entry: "Steering matter in 3D: moving a levitated bead using tactile dials in real time".

---

### DAY 31: Sunday, November 29, 2026 — THE INVISIBLE FORCEFIELD: MID-AIR SKIN HAPTICS
- **Target Hours:** 5h 00m | **Target XP:** +64 XP | **Running XP Total:** ~1,255 XP
- **Focus:** Bare-skin tactile perception testing at 200 Hz modulation
- [ ] **Task 31.1:** Switch firmware into `HAPTIC_MODE` with 200 Hz square-wave modulation.
- [ ] **Task 31.2:** Place bare hand 15cm above the active emitter array.
- [ ] **Task 31.3:** **THE TOUCH TEST:** Verify tactile sensation of an invisible vibrating button against the palm.
- [ ] **Task 31.4:** Map tactile intensity thresholds across different modulation frequencies (100 Hz, 150 Hz, 200 Hz, 250 Hz).
- [ ] **Task 31.5:** Record user reaction videos touching the empty air above the machine.
- [ ] **Task 31.6:** Journal entry: "You can feel the air clicking: verified mid-air haptic forcefield on bare human skin".

---

### DAY 32: Monday, November 30, 2026 — PHOTONIC VOLUMETRIC DISPLAY ILLUMINATION
- **Target Hours:** 4h 30m | **Target XP:** +57 XP | **Running XP Total:** ~1,312 XP
- **Focus:** Synchronized RGB laser/LED lighting on the trapped particle
- [ ] **Task 32.1:** Mount high-brightness focused RGB LED onto the chassis bezel pointing at the focal node.
- [ ] **Task 32.2:** Connect LED PWM drive lines to RP2040 GPIOs.
- [ ] **Task 32.3:** Synchronize light pulses to particle trajectory positions: glowing cyan at $(X_1)$, flashing purple at $(X_2)$.
- [ ] **Task 32.4:** Darken lab room: observe brilliant, self-luminous 3D point of light floating in free space with 360-degree viewing angle.
- [ ] **Task 32.5:** Journal entry: "The glowing volumetric pixel: full photonic synchronization on a trapped levitated particle". Attach darkroom photos.

---

### DAY 33: Tuesday, December 01, 2026 — FULL MECHANICAL CHASSIS INTEGRATION
- **Target Hours:** 5h 00m | **Target XP:** +64 XP | **Running XP Total:** ~1,376 XP
- **Focus:** Final enclosure assembly, cable management, hardware aesthetics
- [ ] **Task 33.1:** Install M3 brass heat-set threaded inserts into 3D-printed bottom chassis.
- [ ] **Task 33.2:** Mount PCB into chassis with anti-vibration silicone standoffs.
- [ ] **Task 33.3:** Fasten top acoustic horn bezel with black oxide hex-socket cap screws.
- [ ] **Task 33.4:** Mount OLED display, knurled metal rotary knobs, and power toggle switch.
- [ ] **Task 33.5:** Inspect final build: clean industrial skunkworks aesthetics.
- [ ] **Task 33.6:** Journal entry: "Final assembly complete: the HPVE Desktop Emitter is bolted into its industrial chassis". Attach beauty shots.

---

### DAY 34: Wednesday, December 02, 2026 — 4-HOUR STRESS & THERMAL BURN-IN TEST
- **Target Hours:** 4h 30m | **Target XP:** +57 XP | **Running XP Total:** ~1,433 XP
- **Focus:** Continuous operation endurance testing for Maker Faire exhibition
- [ ] **Task 34.1:** Power up system for 4 hours continuous operation at full 40 kHz output.
- [ ] **Task 34.2:** Monitor internal ambient temperature inside chassis: confirm temperatures stay below 55°C.
- [ ] **Task 34.3:** Verify frequency stability: confirm RP2040 clock maintains 40.0 kHz +/- 0.05 kHz without thermal drift.
- [ ] **Task 34.4:** Test continuous particle trap stability over 30 minutes without dropping the bead.
- [ ] **Task 34.5:** Journal entry: "4-hour burn-in stress test passed: rock-solid thermal stability for the convention floor".

---

### DAY 35: Thursday, December 03, 2026 — WEEK 5 COMPLETE & MAXIMUM MULTIPLIER UNLOCKED!
- **Target Hours:** 4h 30m | **Target XP:** +57 XP | **Running XP Total:** ~1,490 XP
- **Focus:** MAXIMUM SAMI MULTIPLIER REACHED (1.75x)
- [ ] **Task 35.1:** **MAX ACCELERATOR ACHIEVED:** Multiplier hits the theoretical ceiling: **1.75x**!
- [ ] **Task 35.2:** Every 1 hour of building now generates **14.0 XP**!
- [ ] **Task 35.3:** Chinese Visa stamped in passport / digital confirmation confirmed.
- [ ] **Task 35.4:** Journal entry: "MAX MULTIPLIER 1.75x UNLOCKED! Surplus stipend climbing past $450 cash reimbursement".

---

## PHASE 06: BUILD SUBMISSION, PACKING & MAKER FAIRE SHENZHEN (DAYS 36 - 45)
### Multiplier: 1.75x (MAX) | Base Rate: 14.0 XP / Hour | Goal: 1,000+ XP | Total XP: 1,490 ➔ 2,500+ XP!

---

### DAY 36: Friday, December 04, 2026 — DEMO VIDEO PRODUCTION (CINEMATIC LAB REEL)
- **Target Hours:** 5h 00m | **Target XP:** +70 XP | **Running XP Total:** ~1,560 XP
- **Focus:** Filming the official 2-minute working demo video for Build Submission
- [ ] **Task 36.1:** Film Scene 1: The vision (brief voiceover explaining why screens suck and the Holotable vision).
- [ ] **Task 36.2:** Film Scene 2: Live acoustic levitation (macro shot of tweezers dropping bead, bead floating in thin air).
- [ ] **Task 36.3:** Film Scene 3: Rotary knob 3D steering (bead climbing and maneuvering with dial adjustments).
- [ ] **Task 36.4:** Film Scene 4: Mid-air touch test (bare hand reacting to the invisible acoustic click).
- [ ] **Task 36.5:** Film Scene 5: Hardware teardown (showing custom PCB, RP2040 Pico, TC4427 drivers).
- [ ] **Task 36.6:** Edit video to 120 seconds, export 1080p 60fps, upload to YouTube / Vimeo.
- [ ] **Task 36.7:** Journal entry: "Demo video wrapped! Macro camera caught the acoustic levitation and haptic reaction in crisp 60fps".

---

### DAY 37: Saturday, December 05, 2026 — THE OFFICIAL BUILD SUBMISSION
- **Target Hours:** 5h 00m | **Target XP:** +70 XP | **Running XP Total:** ~1,630 XP
- **Focus:** Submitting the final shipped project on Fallen platform
- [ ] **Task 37.1:** Update GitHub repository README with final high-res hardware photos, demo video link, and architecture diagrams.
- [ ] **Task 37.2:** Ensure all firmware, KiCad files, STEP models, and BOM are fully pushed to `main`.
- [ ] **Task 37.3:** **OFFICIAL ACTION:** Submit the **Build Submission** on the Fallen platform.
- [ ] **Task 37.4:** Share demo video in `#fallen` on Hack Club Slack.
- [ ] **Task 37.5:** Journal entry: "BUILD SUBMISSION OFFICIALLY SHIPPED! Hardware built, video linked, Shenzhen qualification complete".

---

### DAY 38: Sunday, December 06, 2026 — DEMO MODE & INTERACTIVE EXPOSURE FIRMWARE
- **Target Hours:** 5h 00m | **Target XP:** +70 XP | **Running XP Total:** ~1,700 XP
- **Focus:** Creating self-running interactive booth mode for Maker Faire
- [ ] **Task 38.1:** Write `firmware/demo_mode.cpp`: automated attractive mode that runs particle orbit loops when idle.
- [ ] **Task 38.2:** Add proximity detection: when an attendee walks up, OLED displays "HOLD HAND 15CM ABOVE".
- [ ] **Task 38.3:** Implement interactive mini-game: "Feel the Pulse" where attendees find the invisible acoustic coordinates.
- [ ] **Task 38.4:** Journal entry: "Writing the Maker Faire kiosk firmware: autonomous demo modes and attendee touch games".

---

### DAY 39: Monday, December 07, 2026 — SPARE PARTS KIT & TRAVEL DISASSEMBLY
- **Target Hours:** 5h 00m | **Target XP:** +70 XP | **Running XP Total:** ~1,770 XP
- **Focus:** Packing travel kit according to Hack Club shipping rules
- [ ] **Task 39.1:** Pack travel spares kit: 10x extra transducers, 4x TC4427 ICs, 2x 74HC595s, 1x spare Pico.
- [ ] **Task 39.2:** Pack field tools: portable soldering iron, solder wire, screwdrivers, multimeter, anti-static foam.
- [ ] **Task 39.3:** Pack 100x calibrated polystyrene levitation beads in protective vials.
- [ ] **Task 39.4:** Pack hardware in padded Pelican-style hard travel case for airline carry-on.
- [ ] **Task 39.5:** Journal entry: "Toolbox packed for China: field spares, travel case, and backup components ready".

---

### DAY 40: Tuesday, December 08, 2026 — SHENZHEN ELECTRONICS SOURCING MANIFEST
- **Target Hours:** 5h 00m | **Target XP:** +70 XP | **Running XP Total:** ~1,840 XP
- **Focus:** Sourcing list for Huaqiangbei wholesale electronics market
- [ ] **Task 40.1:** Research Huaqiangbei electronics market buildings (SEG Electronics Market, Huaqiang Plaza).
- [ ] **Task 40.2:** Compile Phase 2 Sourcing Wishlist for the full ceiling-mounted Holotable array:
  - 1,024x high-SPL 40kHz ultrasonic transducers (factory reel pricing: ~$0.05 - $0.10 each).
  - 512x high-speed gate driver ICs.
  - Multi-channel FPGA board / high-density shift register arrays.
  - High-precision LiDAR / Time-of-Flight spatial sensor modules.
- [ ] **Task 40.3:** Journal entry: "The Shenzhen shopping list: planning our run through Huaqiangbei to build the ceiling Holotable".

---

### DAY 41: Wednesday, December 09, 2026 — THE FINAL QUALIFICATION DEADLINE (11:59 PM ET)
- **Target Hours:** 6h 00m | **Target XP:** +84 XP | **Running XP Total:** ~1,924 XP
- **Focus:** THE CLOCK RUNS OUT — 100% SPRINT ACCOMPLISHED!
- [ ] **Task 41.1:** Official Fallen submission window closes at 11:59 PM ET.
- [ ] **Task 41.2:** Final check: 1,000 XP requirement shattered, Build Submission approved, flights confirmed.
- [ ] **Task 41.3:** Print physical exhibition posters and pinout flyers for the Maker Faire booth.
- [ ] **Task 41.4:** Journal entry: "DEADLINE REACHED! 1,900+ XP logged, project shipped, bags packed for China".

---

### DAY 42: Thursday, December 10, 2026 — DEPARTURE DAY: FLIGHT TO CHINA ✈️
- **Target Hours:** 5h 00m | **Target XP:** +70 XP | **Running XP Total:** ~1,994 XP
- **Focus:** Transit, travel journaling, arrival in Shenzhen
- [ ] **Task 42.1:** Depart for international flight to Shenzhen Bao'an International Airport (SZX) / Hong Kong (HKG).
- [ ] **Task 42.2:** Transit logging in `#fallen-journals` (travel planning counts toward stipend XP!).
- [ ] **Task 42.3:** Land in Shenzhen, China. Meet Hack Club founders (Zach, Katie, Anson, Sophia) and fellow Fallers.
- [ ] **Task 42.4:** Check into hotel, unpack hardware in the Hack Club staging workshop.
- [ ] **Task 42.5:** Journal entry: "TOUCHDOWN IN SHENZHEN: In the hardware capital of the world with Hack Club".

---

### DAY 43: Friday, December 11, 2026 — MAKER FAIRE SETUP & HUAQIANGBEI EXPEDITION
- **Target Hours:** 6h 00m | **Target XP:** +84 XP | **Running XP Total:** ~2,078 XP
- **Focus:** Reassembling hardware in staging lab + touring the world's largest electronics market
- [ ] **Task 43.1:** Reassemble HPVE Desktop Emitter at Maker Faire venue (Vanke Design Commune, Nanshan District).
- [ ] **Task 43.2:** Power up, calibrate focal standing wave, and verify 100% operation on local 220V power.
- [ ] **Task 43.3:** **THE TREASURE RUN:** Tour Huaqiangbei electronics market with Hack Club team.
- [ ] **Task 43.4:** Source ultrasonic parts, custom silicon, and tools straight from Chinese factory vendors.
- [ ] **Task 43.5:** Journal entry: "Walking through Huaqiangbei: skyscrapers filled with raw silicon, and HPVE is bench-tested in Shenzhen".

---

### DAY 44: Saturday, December 12, 2026 — MAKER FAIRE SHENZHEN: EXHIBITION DAY 1
- **Target Hours:** 8h 00m | **Target XP:** +112 XP | **Running XP Total:** ~2,190 XP
- **Focus:** Live public demonstrations, hundreds of attendees feeling the forcefield
- [ ] **Task 44.1:** Booth opens at Maker Faire Shenzhen!
- [ ] **Task 44.2:** Live demo loop: demonstrate acoustic levitation of glowing beads in free space.
- [ ] **Task 44.3:** Live haptic station: invite hundreds of international makers and engineers to hold their bare palms over the emitter to feel the mid-air tactile button.
- [ ] **Task 44.4:** Explain the wave physics, the RP2040 PIO state machines, and the open-source architecture.
- [ ] **Task 44.5:** Journal entry: "Maker Faire Day 1: A line of 50 people waiting to touch an invisible forcefield in Shenzhen".

---

### DAY 45: Sunday, December 13, 2026 — MAKER FAIRE SHENZHEN: EXHIBITION DAY 2 & VICTORY
- **Target Hours:** 8h 00m | **Target XP:** +112 XP | **Running XP Total:** ~2,302+ XP
- **Focus:** Final showcase, awards ceremony, and Iron Cave Industries expansion
- [ ] **Task 45.1:** Final exhibition day at Maker Faire Shenzhen.
- [ ] **Task 45.2:** Connect with hardware manufacturers and makers for scaled production of the ceiling Holotable.
- [ ] **Task 45.3:** Closing Hack Club awards & celebration dinner with all 30+ international teen makers.
- [ ] **Task 45.4:** **MISSION ACCOMPLISHED:** 2,300+ Total XP, project fully built, world demonstrated, life-changing trip complete.
- [ ] **Task 45.5:** Final journal entry: "MISSION COMPLETE: We built the Holotable, we brought it to Shenzhen, and we changed the game".

---

## 3. Daily Execution Protocol (How We Update This File)

Every evening when completing a work session:
1. Mark completed tasks with an `[x]`.
2. Update the **Current Value** in the Telemetry Table.
3. Update **Running XP Total**.
4. Push the updated `CAMPAIGN_ROADMAP_45_DAYS.md` to GitHub.
