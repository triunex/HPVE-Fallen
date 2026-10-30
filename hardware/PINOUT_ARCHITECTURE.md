# HPVE Hardware Pinout & Wiring Architecture

This document defines the electrical interconnects between the Raspberry Pi Pico (RP2040), the high-speed shift registers, MOSFET gate drivers, and the 64-element ultrasonic transducer matrix.

---

## 1. System Block Diagram

```
+-------------------------------------------------------------------------+
|                              RP2040 PICO                                |
|                                                                         |
|  [GPIO 4: SDA] -------------> SSD1306 0.96" I2C OLED (Telemetry)        |
|  [GPIO 5: SCL] -------------> SSD1306 0.96" I2C OLED (Clock)            |
|                                                                         |
|  [GPIO 10: ENC_A1] ---------> Rotary Encoder 1 (X/Y Target Position)    |
|  [GPIO 11: ENC_B1] ---------> Rotary Encoder 1                         |
|  [GPIO 12: ENC_A2] ---------> Rotary Encoder 2 (Z Height / Focus)       |
|  [GPIO 13: ENC_B2] ---------> Rotary Encoder 2                         |
|                                                                         |
|  [GPIO 16: SR_DATA] --------> 74HC595 Cascade (Serial Data In)          |
|  [GPIO 17: SR_CLK] ---------> 74HC595 Cascade (Shift Clock)             |
|  [GPIO 18: SR_LATCH] -------> 74HC595 Cascade (Storage Latch)           |
|  [GPIO 19: PWM_SYNC] -------> TC4427 Output Enable / Phase Gate         |
+-------------------------------------------------------------------------+
                                    │
                                    │ 64 Parallel Logic Lines (0-3.3V)
                                    ▼
+-------------------------------------------------------------------------+
|                  32x TC4427 DUAL MOSFET GATE DRIVERS                    |
|                                                                         |
|  - VDD: +12V DC Rail (Supplied by 12V 5A power supply)                  |
|  - GND: Common System Ground (Star Ground topology)                     |
|  - Inputs: INA, INB driven by shift register Q0-Q7 parallel outputs     |
|  - Outputs: OUTA, OUTB boost 3.3V logic to 12V Peak-to-Peak (12Vpp)     |
|  - Decoupling: 1x 0.1uF ceramic capacitor per IC placed directly at VDD |
+-------------------------------------------------------------------------+
                                    │
                                    │ 64 Channel 12Vpp 40kHz AC Pulses
                                    ▼
+-------------------------------------------------------------------------+
|               64x 400ST160 ULTRASONIC TRANSDUCER MATRIX                 |
|                                                                         |
|  - Grid: 8x8 Cartesian Matrix (12mm pitch center-to-center)             |
|  - Positive Pins: Driven by individual TC4427 OUTA/OUTB lines           |
|  - Negative Pins: Tied to Common Ground bus plane                       |
|  - Operating Frequency: 40.0 kHz +/- 1.0 kHz resonant peak              |
+-------------------------------------------------------------------------+
```

---

## 2. Power Distribution

1. **Main DC Input (12V 5A):**
   - Connected via 5.5mm x 2.1mm DC barrel jack.
   - Powers the VDD rails of all 32x TC4427 driver ICs.
   - High-frequency bulk capacitance: 470uF 25V electrolytic + 10x 10uF ceramic capacitors distributed across the board.

2. **Logic Power (5V & 3.3V):**
   - 12V input feeds the **MP1584EN DC-DC Buck Converter**.
   - Stepped down to **5.0V** to feed the VSYS pin of the Raspberry Pi Pico and OLED display.
   - Pico's internal RT6150 buck-boost provides clean **3.3V** for Pico GPIO logic and 74HC595 VCC.

---

## 3. 74HC595 Cascade Daisy-Chain

Eight 74HC595 8-bit shift registers are daisy-chained to provide 64 discrete outputs:

| Shift Register # | Outputs | Controlled Transducers |
|---|---|---|
| SR 1 (U1) | Q0 - Q7 | Transducers 1 to 8 (Row 1) |
| SR 2 (U2) | Q0 - Q7 | Transducers 9 to 16 (Row 2) |
| SR 3 (U3) | Q0 - Q7 | Transducers 17 to 24 (Row 3) |
| SR 4 (U4) | Q0 - Q7 | Transducers 25 to 32 (Row 4) |
| SR 5 (U5) | Q0 - Q7 | Transducers 33 to 40 (Row 5) |
| SR 6 (U6) | Q0 - Q7 | Transducers 41 to 48 (Row 6) |
| SR 7 (U7) | Q0 - Q7 | Transducers 49 to 56 (Row 7) |
| SR 8 (U8) | Q0 - Q7 | Transducers 57 to 64 (Row 8) |

- **Serial Data Chain:** Pico GPIO 16 -> U1 `SER`, U1 `QH'` -> U2 `SER` ... U7 `QH'` -> U8 `SER`.
- **Clock Bus:** Pico GPIO 17 tied in parallel to all eight `SRCLK` pins.
- **Latch Bus:** Pico GPIO 18 tied in parallel to all eight `RCLK` pins.
