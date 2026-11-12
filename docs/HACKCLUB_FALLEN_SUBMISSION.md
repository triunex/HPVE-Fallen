# HPVE MK-I: HACK CLUB FALLEN STAGE 3 GRANT SUBMISSION
## Iron Cave Industries | Project HPVE (Haptic Photonic Volumetric Engine)
### Lead Maker: nelieo.shorya | Companion: Sami (Team Sami 🦊 - 1,043 XP)

---

## 1. Executive Summary & Vision

* **Project Title:** Haptic Photonic Volumetric Engine (HPVE MK-I)
* **Tagline:** The world's first open-hardware, chamber-free, touchable 3D volumetric holographic display.
* **Target Exhibition:** Maker Faire Shenzhen 2026 (December 11–14, 2026)

### What Are We Building?
The desktop levitation box is dead. In classical high-school science experiments, ultrasound is trapped inside an acrylic shoebox with opposing top and bottom plates, holding a piece of foam inside a glass tank where nobody can touch it.

**HPVE MK-I destroys the box.**

Suspended beneath an overhead Inverted-U **Halo Arch**, the engine fires a 64-channel, 40 kHz ultrasonic phased array straight down into completely open air. In an **apple-sized ($100\text{ mm}$) volume of free space** hovering above the workbench, synchronized 450nm photonic laser modulators and acoustic wave fields project a glowing 3D vector hologram floating in mid-air.

More importantly, **you can physically touch it**:
* A downward-looking **VL53L5CX Time-of-Flight sensor** tracks your bare fingers at 60 Hz in 3D coordinate space.
* The moment your hand enters the holographic volume, the phased array modulates the 40 kHz ultrasound carrier wave at **200 Hz**—the exact resonant frequency of the **Pacinian corpuscle mechanoreceptors** in human skin.
* The non-linear acoustic radiation pressure acts as a physical demodulator on your fingertips, synthesizing an **elastic, touchable forcefield ($F = k \cdot \Delta z$) in thin air** with zero gloves, no VR headsets, and no glass.

---

## 2. Budget & Funding Request Structure

| Subsystem | Total Build Cost | Requested from Grant | Self-Funded by Maker | Key Components |
|---|---|---|---|---|
| **4x Modular Tile PCBAs** | $116.50 | **$116.50** | $0.00 | 5x 4-layer PCBs, 4x assembled SMT, 70x 400ST160 transducers, 32x TC4427 drivers, 8x 74HC595 shift registers |
| **2x Master Carrier Motherboards** | $56.80 | **$56.80** | $0.00 | 5x 2-layer PCBs, 2x assembled SMT, 2x RP2040 Pico, TPS54302 buck, CD74HC4051 8:1 MUX, 74AHCT125 level shifter |
| **Displays, Optics & Sensors** | $48.60 | **$48.60** | $0.00 | 1.28" GC9A01 Round IPS HUD, VL53L5CX ToF sensor, SSD1306 OLED, 3x 450nm lasers, 8x dials, harnesses |
| **Power Delivery Subsystem** | $33.50 | **$33.50** | $0.00 | Mean Well LRS-150-12 (12V 12.5A), 16AWG silicone star harness, XT30/XT60 connectors, optical scattering media |
| **CAD 3D Printing Filament** | $68.00 | **$0.00** | **$68.00** | 2 kg Warm White PETG, 1 kg TVA Signal Orange PETG, 0.5 kg 95A Flexible TPU (Halo Arch + snap-lock chassis) |
| **Mechanical Fasteners & Inserts**| $16.50 | **$0.00** | **$16.50** | 100x M3 & M2.5 brass heat-set threaded inserts with melt-traps, black oxide socket head cap screws |
| **Tools & Battery Hardware** | $23.40 | **$0.00** | **$23.40** | 60/40 rosin-core solder, flux pen, under-desk 4S 20A Li-ion battery cradle tray and BMS module |
| **Express Courier Freight & Buffer**| $36.70 | **$14.60** | **$22.10** | JLCPCB DHL/FedEx air express courier delivery + international customs buffer |
| **CONSOLIDATED TOTAL** | **$400.00 USD** | **$270.00 USD (67.5%)** | **$130.00 USD (32.5%)** | Complete end-to-end production build |

### Justification for the Grant Reviewers:
* **Grant Funding ($270.00 USD):** Allocated 100% to custom factory silicon fabrication, precision transducers, and optical/ToF sensors that cannot be fabricated at home.
* **Maker Co-Investment ($130.00 USD):** The maker provides full skin in the game by self-funding all 3D printing filament, mechanical fasteners, tools, and local customs buffers.

---

## 3. Engineering Deliverables & Production Readiness

1. **Modular Phased-Array Tile PCBA (`hardware/modular_tile/`):**
   * 4-layer custom PCB (98 x 88 mm) with 100% single-sided top SMT for low-cost factory assembly.
   * Driven by 8x TC4427 dual gate drivers and 2x 74HC595 shift registers per tile.
   * Interconnected via 0.5mm 6-pin Hirose FH12 FFC ribbon connectors.
   * **Certified 0 DRC errors, 0 DRC warnings, and 0 ERC messages** with full production Gerber RS-274X, BOM, and CPL files exported.

2. **Master Avionics Carrier Motherboard (`hardware/master_carrier/`):**
   * 2-layer carrier PCB (124 x 76 mm) housing the socketed Raspberry Pi Pico (RP2040).
   * Synchronous 12V $\to$ 5V 3A buck regulator (TI TPS54302) with TVS spike and reverse-polarity protection.
   * TI CD74HC4051 8:1 analog MUX reading 8 console dials into single ADC pin with hardware debounce filtering.
   * 1.28" GC9A01 round IPS digital HUD screen replacing legacy analog gauges.
   * TI 74AHCT125 level-shifter with 33$\Omega$ damping resistors feeding the 20 MHz FFC ribbon bus.
   * **Certified 0 DRC violations, 0 unconnected items, 0 footprint errors, and 0 ERC messages**.

3. **Breakdown Halo Arch Stand & Exoskeleton CAD (`hardware/cad/` & `halo_arch/`):**
   * 44 watertight production STLs verified across 7 modular subassemblies.
   * Packs completely flat into a standard backpack for transport to Maker Faire Shenzhen.
   * Snap-lock tile frames with internal hexagonal wall honeycomb, 45-degree Iso-Grid trusses, and 16x Helmholtz back-chambers.

4. **Dual-Core Embedded Firmware (`firmware/`):**
   * Deterministic RP2040 PIO state machine shifting 64 phase channels every 1.5625 $\mu\text{s}$ at 40.000 kHz carrier frequency.
   * Q16.16 fixed-point 3D holographic wave solver running twin-trap, vortex, and 200 Hz haptic modes.
   * Closed-loop VL53L5CX 60 Hz 3D hand tracking and autonomous non-linear forcefield generator.

---

## 4. Maker Milestones Achieved

* **Total XP Banked:** **1,043 XP** *(Surpassed the 1,000 XP Shenzhen Final Form qualification gate!)*
* **Total Engineering Time Logged:** **108 Hours 10 Minutes** across 14 comprehensive technical journals.
* **Hack Club Team Standings:** **Team Sami 🦊 Leading at 15,493+ XP**.
