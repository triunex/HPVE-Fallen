# HPVE Controller Motherboard v1.0 - Fabrication & Assembly Specification

**Document ID:** SPEC-HPVE-HW-001  
**Revision:** 1.0  
**Project:** Haptic Photonic Volumetric Engine (HPVE)  
**Parent Initiative:** Hack Club Fallen / Maker Faire Shenzhen 2026  
**Engineering Team:** Iron Cave Industries (Lead: `nelieo.shorya`)  
**Target Manufacturers:** JLCPCB / PCBWay / OSHPark  

---

## 1. Board Physical Parameters

| Parameter | Specification | Tolerance |
| :--- | :--- | :--- |
| **Board Dimensions** | 140.00 mm x 140.00 mm | ±0.15 mm |
| **Corner Radius** | 2.0 mm | ±0.1 mm |
| **Board Thickness** | 1.60 mm | ±10% |
| **Layer Count** | 2 Copper Layers (Double Sided) | N/A |
| **Substrate Material** | High-TG FR-4 (TG150 - TG170) | N/A |
| **Dielectric Constant ($\epsilon_r$)** | 4.4 - 4.6 @ 1 MHz | Typical |
| **Finished Outer Copper** | 1.0 oz / ft² (35 µm nominal) | ±5 µm |
| **Surface Finish** | Lead-Free HASL (Default) or ENIG (Recommended) | IPC-4552 |
| **Solder Mask** | Liquid Photoimageable (LPI), Matte Black or Green | Both sides |
| **Silkscreen** | High-Resolution Liquid Ink, Brilliant White | Top & Bottom |
| **Minimum Trace Width** | 0.25 mm (9.84 mil) | ±0.02 mm |
| **Minimum Trace Clearance** | 0.25 mm (9.84 mil) | ±0.02 mm |
| **Via Treatment** | Tented vias (Solder mask covered) | Both sides |
| **Mounting Fixtures** | 4x M3 Screw Holes (Ø 3.20 mm PTH, 6.0 mm Pad) | 5 mm corner offset |

---

## 2. Layer Stackup & File Mapping

Standard RS-274X extended Gerber formats and Excellon drill schedules:

| File Name | Layer Description | Polarity | Function |
| :--- | :--- | :--- | :--- |
| `HPVE_Controller-F_Cu.gtl` | Top Copper Layer (Layer 1) | Positive | 40 kHz High-Side Drive Lines & SPI Bus |
| `HPVE_Controller-B_Cu.gbl` | Bottom Copper Layer (Layer 2) | Positive | TC4427 Driver Feeds & Return Grounds |
| `HPVE_Controller-F_Mask.gts` | Top Solder Mask | Negative | Pad Openings (0.05 mm swell) |
| `HPVE_Controller-B_Mask.gbs` | Bottom Solder Mask | Negative | SMT Driver / Shift Register Pads |
| `HPVE_Controller-F_Silkscreen.gto`| Top Legend / Silkscreen | Positive | Transducer Grid Indicators & Pinout |
| `HPVE_Controller-B_Silkscreen.gbo`| Bottom Legend / Silkscreen | Positive | IC Component Designators & Polarity Marks |
| `HPVE_Controller-F_Paste.gtp` | Top SMT Solder Paste | Positive | Stencil Apertures (Front) |
| `HPVE_Controller-B_Paste.gbp` | Bottom SMT Solder Paste | Positive | Stencil Apertures (Back) |
| `HPVE_Controller-Edge_Cuts.gm1` | Mechanical Board Outline | Positive | 140 mm x 140 mm Routed Perimeter |
| `HPVE_Controller-PTH.drl` | Plated Through Hole Drill | N/A | Transducer & Header Drill Hits (300 hits) |
| `HPVE_Controller-NPTH.drl` | Non-Plated Through Hole | N/A | Tooling / Alignment Holes |
| `HPVE_Controller-drill_report.rpt`| NC Drill Hit Summary | N/A | Tool Tooling & Quantities Report |
| `HPVE_Controller-all-pos.csv` | Component Placement (CPL) | N/A | Automated Pick-and-Place Coordinates |

---

## 3. Drill Schedule Summary

Extracted directly from KiCad CAM drill analyzer:
- **T1: Ø 0.800 mm (0.0315")** — 128 Holes (64x Ultrasonic Transducers, 2 pins each)
- **T2: Ø 1.000 mm (0.0394")** — 168 Holes (RP2040 Pico 40-pin header, bypass capacitors, power headers)
- **T3: Ø 3.200 mm (0.1260")** — 4 Holes (M3 Structural Enclosure Stand-off Mounts)
- **Total Drill Count:** 300 Plated Through-Holes

---

## 4. Electrical Verification & Quality Standards

1. **IPC Class:** Manufactured and inspected strictly per **IPC-A-600 Class 2** or Class 3.
2. **SMT Assembly:** Component soldering per **IPC-J-STD-001** and inspection per **IPC-A-610 Class 2**.
3. **Electrical Test:** 100% Flying Probe or Bed-of-Nails Netlist Electrical Test against `HPVE_Controller.net`. Zero opens, zero shorts allowed.
4. **Thermal Rating:** Transducer array operates under continuous pulsed resonant mode (40 kHz, 12V VPP). Traces sized for low loop inductance and max 10°C temperature rise at continuous 1.5A array load.
5. **Environmental:** 100% RoHS 3 (EU Directive 2015/863) and REACH compliant. Lead-Free construction.

---

## 5. Automated SMT Assembly (PCBA) Data

- **CPL / Centroid File:** `HPVE_Controller-all-pos.csv` (All units in millimeters, board origin at lower-left `(0, 0)`).
- **Bill of Materials:** `hardware/bom.csv` (Includes manufacturer part numbers, footprints, designators, and direct procurement URLs).
