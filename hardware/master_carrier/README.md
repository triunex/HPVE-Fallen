# `HPVE_Master_Carrier` (`MK-I` — Rev 1.0)

Master Avionics Carrier Motherboard for the **Malibu Volumetric Engine (`HPVE`)**, designed in **KiCad 10** (`124.0 mm × 76.0 mm`, 2-layer, `3.0 mm` rounded corners) and exported **100% unrouted** (`0` pre-placed copper tracks or zones) specifically for clean autorouting in **FreeRouting**.

---

## 1. Key Architecture & Protections

- **Controller Dock (`U1`):** Dual 20-pin 2.54 mm headers for a standard **Raspberry Pi Pico (`RP2040`)**, USB port facing the top board edge for direct firmware flashing.
- **12V Input & Buck Power (`J1`, `U2`, `L1`):**
  - Vertical `AMASS XT30UPB-M` input (`J1`) → `5A` PTC polyfuse (`F1`) → `SMBJ15CA` bidirectional TVS (`D1`) → `SS34` Schottky reverse-polarity guard (`D2`).
  - `TPS54302DDCR` (`U2`) synchronous buck (`12V → 5.08V`, `3A` max, `10 uH` `CDRH104RNP-100NC` inductor `L1`, EN/UVLO divider `R1/R2 = 100k/22k`, feedback `R3/R4 = 100k/13.3k`).
  - `SS34` VSYS OR-ing diode (`D3`) powers the Pico safely alongside USB (`VBUS` isolation).
  - `R5/R6/C14` (`39k/10k/100nF`) feeds scaled 12V rail telemetry (`VSENSE_12V`) into `GP27/ADC1`.
- **3.3V → 5V High-Speed SPI Translator & FFC Link (`U4`, `F2`, `F3`, `J2`):**
  - `SN74AHCT125D` (`U4`) translates `GP2..GP5` (`PIO_SPI_MOSI`, `PIO_SPI_SCK`, `PIO_SPI_LATCH`, `PIO_DRV_OE`) from `3.3V` to `5.0V` CMOS logic through `33Ω` series damping resistors (`R10..R13`) to match the `74HC595` shift registers on [`HPVE_Tile_16Ch`](../modular_tile/HPVE_Tile_16Ch.kicad_pcb).
  - `Hirose FH12-6S-0.5SH` bottom-edge connector (`J2`) matches `HPVE_Tile_16Ch` `J_IN` pin-for-pin (**use a 6-pin, `0.50 mm` pitch Type-D / reverse-contact FFC cable**).
  - Dual `0.5A` SMD1206 polyfuses (`F2` on `/+12V_FFC`, `F3` on `/+5V_FFC`) protect both FFC power pins to match the connector's `0.5 A` contact rating and isolate fine-pitch routing (`0.30 mm` traces on `J2`).
- **8-Dial Analog Console (`U3`, `J4`, `J5`, `J6`):**
  - `CD74HC4051M` (`U3`) 8:1 analog MUX (`S0..S2` on `GP18..GP20`) reading `DIAL_0..DIAL_7` on `GP26/ADC0` with per-channel `100 nF` debounce capacitors (`C16..C23`), `100 kΩ` / `10 nF` `COM` RC filter (`R20`/`C15`), and `0 Ω` star ground bridge (`R7`) isolating `/AGND`.
- **Circular IPS & Auxiliary UI (`J3`, `J7`, `J8`, `J9`, `J10`, `SW1..SW3`, `Q1..Q3`):**
  - `J3` (`8P JST-XH`) drives a `1.28"` **GC9A01 Round IPS SPI1 Display** (`GP10..GP15`).
  - `J7` (`4P JST-XH`) & `J8` (`6P JST-XH`) share hardware `I2C0` (`GP0/GP1`, `4.7 kΩ` pull-ups `R8/R9`) for the **SSD1306 OLED HUD** and **VL53L5CX Time-of-Flight sensor** (`TOF_INT` `GP21`, `TOF_LPN` `GP22`).
  - `Q1..Q3` (`AO3400A` low-side NMOS, `100 Ω` gate + `10 kΩ` pulldown) drive **RGB Photonic Lasers** (`J9`, `GP6..GP8`).
  - `SW1` (`RESET` → `RUN`), `SW2` (`MODE` → `GP16`), `SW3` (`ENABLE` → `GP17`), plus 4 rail/status LEDs (`D4..D7`).

---

## 2. Connector Pinouts (`J1`–`J10`)

| Ref | Function / Connector | Pinout (1 → N) |
| :--- | :--- | :--- |
| **`J1`** | `XT30_12V_IN` (`AMASS_XT30UPB-M`) | `1`: `GND`, `2`: `+12V_RAW` (via `F1`/`D2`) |
| **`J2`** | `FH12-6S-0.5SH` (6P FFC Link, `0.50 mm`, **Type-D Cable**) | `1`: `/+12V_FFC`, `2`: `/+5V_FFC`, `3`: `GND`, `4`: `/FFC_MOSI`, `5`: `/FFC_SCK`, `6`: `/FFC_LATCH`, `MP`: `GND` |
| **`J3`** | `GC9A01_ROUND_IPS` (`B8B-XH-A`) | `1`: `+3V3`, `2`: `GND`, `3`: `/DISP_SCL (GP10)`, `4`: `/DISP_SDA (GP11)`, `5`: `/DISP_DC (GP12)`, `6`: `/DISP_CS (GP13)`, `7`: `/DISP_RST (GP14)`, `8`: `/DISP_BLK (GP15)` |
| **`J4`** | `LEFT_DIALS_5P` (`B5B-XH-A`) | `1`: `+3V3`, `2`: `/DIAL_0`, `3`: `/DIAL_1`, `4`: `/DIAL_2`, `5`: `/AGND` |
| **`J5`** | `RIGHT_DIALS_5P` (`B5B-XH-A`) | `1`: `+3V3`, `2`: `/DIAL_3`, `3`: `/DIAL_4`, `4`: `/DIAL_5`, `5`: `/AGND` |
| **`J6`** | `BRIDGE_DIALS_4P` (`B4B-XH-A`) | `1`: `+3V3`, `2`: `/DIAL_6`, `3`: `/DIAL_7`, `4`: `/AGND` |
| **`J7`** | `SSD1306_OLED_4P` (`B4B-XH-A`) | `1`: `+3V3`, `2`: `GND`, `3`: `/I2C0_SCL (GP1)`, `4`: `/I2C0_SDA (GP0)` |
| **`J8`** | `VL53L5CX_6P` (`B6B-XH-A`) | `1`: `+3V3`, `2`: `GND`, `3`: `/I2C0_SCL (GP1)`, `4`: `/I2C0_SDA (GP0)`, `5`: `/TOF_INT (GP21)`, `6`: `/TOF_LPN (GP22)` |
| **`J9`** | `RGB_LASERS_4P` (`B4B-XH-A`) | `1`: `+5V`, `2`: `/LASER_R_K (Q1)`, `3`: `/LASER_G_K (Q2)`, `4`: `/LASER_B_K (Q3)` |
| **`J10`** | `AUX_SYNC_4P` (`B4B-XH-A`) | `1`: `+5V`, `2`: `GND`, `3`: `/AUX_OE_5V (5V GP5)`, `4`: `/SYNC_TRIG (GP9)` |

---

## 3. Raspberry Pi Pico (`U1`) Signal Mapping

Aligned with [`../../firmware/hpve_config.h`](../../firmware/hpve_config.h) (`PIN_I2C_SDA..PIN_BTN_ENABLE`), assigning `GP10..GP15` to the GC9A01 circular IPS display and allocating `GP18..GP22`, `GP26..GP28` for the 8-dial MUX, ToF control, 12V telemetry, and status LED:

| Pico Pins | Net(s) | Subsystem |
| :--- | :--- | :--- |
| `1, 2` (`GP0, GP1`) | `/I2C0_SDA`, `/I2C0_SCL` | OLED HUD (`J7`) & VL53L5CX ToF (`J8`) |
| `4, 5, 6, 7` (`GP2..GP5`) | `/PIO_SPI_MOSI`, `SCK`, `LATCH`, `DRV_OE` | `SN74AHCT125D` (`U4`) → `J2` FFC & `J10` (`AUX_OE_5V`) |
| `9, 10, 11` (`GP6..GP8`) | `/LASER_R_PWM`, `G_PWM`, `B_PWM` | `AO3400A` low-side drivers (`Q1..Q3` → `J9`) |
| `12` (`GP9`) | `/SYNC_TRIG` | External frame sync (`J10.4`) |
| `14..17, 19, 20` (`GP10..GP15`) | `/DISP_SCL`, `SDA`, `DC`, `CS`, `RST`, `BLK` | GC9A01 Round IPS SPI1 (`J3`) |
| `21, 22` (`GP16, GP17`) | `/BTN_MODE`, `/BTN_ENABLE` | Front tactile buttons (`SW2`, `SW3`) |
| `24, 25, 26` (`GP18..GP20`) | `/MUX_S0`, `/MUX_S1`, `/MUX_S2` | `CD74HC4051M` address select (`U3`) |
| `27, 29` (`GP21, GP22`) | `/TOF_INT`, `/TOF_LPN` | VL53L5CX interrupt & shutdown (`J8`) |
| `30` (`RUN`) | `/SYS_RESET` | Hardware reset (`SW1`) |
| `31, 32, 33` (`GP26, GP27, ADC_GND`) | `/MUX_COM`, `/VSENSE_12V`, `/AGND` | 8-dial ADC & 12V sense + star ground |
| `34` (`GP28`) | `/STAT_LED` | Status LED (`D7`) |
| `36, 39` (`3V3`, `VSYS`) | `+3V3`, `/PICO_VSYS` | 3.3V logic rail & OR-ed 5V supply (`D3`) |

---

## 4. FreeRouting → KiCad Production Workflow

All 7 netclasses (`Default`, `POWER_12V`, `POWER_5V`, `POWER_FFC`, `GND_PLANE`, `POWER_3V3`, `HIGH_SPEED_SPI`) use **`0.18 mm` clearance** across `.kicad_pro`, `HPVE_Master_Carrier.dsn`, and `HPVE_Master_Carrier.rules`, with **`0.30 mm`** widths on `POWER_FFC` (`/+12V_FFC`, `/+5V_FFC`) and `GND_PLANE` (`GND`, `/AGND`) so `J2`'s `0.30 mm` (`0.50 mm`-pitch) SMD pads route cleanly without clearance bottlenecks.

> [!NOTE]
> If an earlier `.ses` file exists in this directory from prior testing, re-run FreeRouting on the latest `HPVE_Master_Carrier.dsn` before importing.

1. Launch **FreeRouting** (`freerouting`) and open **`HPVE_Master_Carrier.dsn`** (FreeRouting automatically reads **`HPVE_Master_Carrier.rules`** from the same folder).
2. Run **Autorouter** (`Auto Route`) to route 100% of nets across `F.Cu` and `B.Cu`, then export **Specctra Session File (`.ses`)** (`File → Export Specctra Session File`).
3. Open **`HPVE_Master_Carrier.kicad_pcb`** in **KiCad 10 PCB Editor**, run **`File → Import → Specctra Session...`** and select your newly exported `.ses`.
4. *(Recommended)* Draw a `GND` filled copper zone across `B.Cu` (and optionally `F.Cu`) in KiCad PCB Editor and press **`B`** to flood ground planes for low-inductance buck and SPI returns.
5. Run **DRC** (`Inspect → Design Rules Checker`), then export **Gerbers + Excellon Drill** files along with [`HPVE_Master_Carrier_bom.csv`](./HPVE_Master_Carrier_bom.csv) (`48` unique line items with LCSC numbers) and [`HPVE_Master_Carrier_cpl.csv`](./HPVE_Master_Carrier_cpl.csv) (`80` rows).

---

## 5. Verification Status (Unrouted Release)

- **ERC (`kicad-cli sch erc --severity-all`):** **`0` Errors, `0` Warnings** ([`erc_report.rpt`](./erc_report.rpt)) across `84` schematic components and `70` nets (`67` active + `3` no-connect `U1` pins).
- **DRC (`kicad-cli pcb drc --severity-all`):** **`0` DRC violations**, `0` courtyard overlaps, `201` unconnected items (expected — `0` tracks, `0` zones pre-placed; [`drc_report.rpt`](./drc_report.rpt)).
- **Deliverables Included:**
  - Schematic & Exports: [`HPVE_Master_Carrier.kicad_sch`](./HPVE_Master_Carrier.kicad_sch), [`HPVE_Master_Carrier.xml`](./HPVE_Master_Carrier.xml), [`HPVE_Master_Carrier_Schematic.pdf`](./HPVE_Master_Carrier_Schematic.pdf), [`HPVE_Master_Carrier.svg`](./HPVE_Master_Carrier.svg)
  - PCB & FreeRouting: [`HPVE_Master_Carrier.kicad_pcb`](./HPVE_Master_Carrier.kicad_pcb), [`HPVE_Master_Carrier.kicad_pro`](./HPVE_Master_Carrier.kicad_pro), [`HPVE_Master_Carrier.dsn`](./HPVE_Master_Carrier.dsn), [`HPVE_Master_Carrier.rules`](./HPVE_Master_Carrier.rules)
  - Manufacturing & Libraries: [`HPVE_Master_Carrier_bom.csv`](./HPVE_Master_Carrier_bom.csv), [`HPVE_Master_Carrier_cpl.csv`](./HPVE_Master_Carrier_cpl.csv), [`HPVE_Carrier.kicad_sym`](./HPVE_Carrier.kicad_sym), [`HPVE_Carrier.pretty/`](./HPVE_Carrier.pretty), [`sym-lib-table`](./sym-lib-table), [`fp-lib-table`](./fp-lib-table)
  - 3D Renders: [`renders/pcb_top.png`](./renders/pcb_top.png), [`renders/pcb_iso.png`](./renders/pcb_iso.png), [`renders/pcb_bottom.png`](./renders/pcb_bottom.png)
