"""
HPVE KiCad Schematic Generator
Generates self-contained, fully populated KiCad 8/10 schematic sheets
with embedded lib_symbols, component placements, net labels, and power connections.
"""

import uuid
import os

def gen_uuid():
    return str(uuid.uuid4())

def get_transducer_lib_symbol():
    return """
    (symbol "HPVE:Transducer_400ST" (pin_names (offset 1.016)) (in_bom yes) (on_board yes)
      (property "Reference" "TX" (at 0 5.08 0) (effects (font (size 1.27 1.27))))
      (property "Value" "400ST160" (at 0 -5.08 0) (effects (font (size 1.27 1.27))))
      (property "Footprint" "HPVE:Transducer_16mm_Pitch10mm" (at 0 -7.62 0) (effects (font (size 1.27 1.27)) hide))
      (symbol "Transducer_400ST_0_1"
        (circle (center 0 0) (radius 3.81) (stroke (width 0.254) (type solid)) (fill (type none)))
        (polyline (pts (xy -1.905 1.905) (xy 1.905 -1.905)) (stroke (width 0.1524) (type solid)) (fill (type none)))
        (polyline (pts (xy -1.905 -1.905) (xy 1.905 1.905)) (stroke (width 0.1524) (type solid)) (fill (type none)))
      )
      (symbol "Transducer_400ST_1_1"
        (pin passive line (at 0 6.35 270) (length 2.54) (name "+" (effects (font (size 1.016 1.016)))) (number "1" (effects (font (size 1.016 1.016)))))
        (pin passive line (at 0 -6.35 90) (length 2.54) (name "-" (effects (font (size 1.016 1.016)))) (number "2" (effects (font (size 1.016 1.016)))))
      )
    )"""

def get_tc4427_lib_symbol():
    return """
    (symbol "HPVE:TC4427A" (pin_names (offset 1.016)) (in_bom yes) (on_board yes)
      (property "Reference" "U" (at 0 8.89 0) (effects (font (size 1.27 1.27))))
      (property "Value" "TC4427ACOA" (at 0 -8.89 0) (effects (font (size 1.27 1.27))))
      (property "Footprint" "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" (at 0 -11.43 0) (effects (font (size 1.27 1.27)) hide))
      (symbol "TC4427A_0_1"
        (rectangle (start -7.62 7.62) (end 7.62 -7.62) (stroke (width 0.254) (type solid)) (fill (type background)))
      )
      (symbol "TC4427A_1_1"
        (pin input line (at -10.16 5.08 0) (length 2.54) (name "INA" (effects (font (size 1.016 1.016)))) (number "2" (effects (font (size 1.016 1.016)))))
        (pin power_in line (at 0 -10.16 90) (length 2.54) (name "GND" (effects (font (size 1.016 1.016)))) (number "3" (effects (font (size 1.016 1.016)))))
        (pin input line (at -10.16 -5.08 0) (length 2.54) (name "INB" (effects (font (size 1.016 1.016)))) (number "4" (effects (font (size 1.016 1.016)))))
        (pin output line (at 10.16 -5.08 180) (length 2.54) (name "OUTB" (effects (font (size 1.016 1.016)))) (number "5" (effects (font (size 1.016 1.016)))))
        (pin power_in line (at 0 10.16 270) (length 2.54) (name "VDD" (effects (font (size 1.016 1.016)))) (number "6" (effects (font (size 1.016 1.016)))))
        (pin output line (at 10.16 5.08 180) (length 2.54) (name "OUTA" (effects (font (size 1.016 1.016)))) (number "7" (effects (font (size 1.016 1.016)))))
      )
    )"""

def get_74hc595_lib_symbol():
    return """
    (symbol "HPVE:74HC595" (pin_names (offset 1.016)) (in_bom yes) (on_board yes)
      (property "Reference" "U" (at 0 16.51 0) (effects (font (size 1.27 1.27))))
      (property "Value" "74HC595" (at 0 -16.51 0) (effects (font (size 1.27 1.27))))
      (property "Footprint" "Package_DIP:DIP-16_W7.62mm" (at 0 -19.05 0) (effects (font (size 1.27 1.27)) hide))
      (symbol "74HC595_0_1"
        (rectangle (start -10.16 15.24) (end 10.16 -15.24) (stroke (width 0.254) (type solid)) (fill (type background)))
      )
      (symbol "74HC595_1_1"
        (pin input line (at -12.7 10.16 0) (length 2.54) (name "SER" (effects (font (size 1.016 1.016)))) (number "14" (effects (font (size 1.016 1.016)))))
        (pin input line (at -12.7 5.08 0) (length 2.54) (name "SRCLK" (effects (font (size 1.016 1.016)))) (number "11" (effects (font (size 1.016 1.016)))))
        (pin input line (at -12.7 0 0) (length 2.54) (name "RCLK" (effects (font (size 1.016 1.016)))) (number "12" (effects (font (size 1.016 1.016)))))
        (pin input line (at -12.7 -5.08 0) (length 2.54) (name "~{OE}" (effects (font (size 1.016 1.016)))) (number "13" (effects (font (size 1.016 1.016)))))
        (pin input line (at -12.7 -10.16 0) (length 2.54) (name "~{SRCLR}" (effects (font (size 1.016 1.016)))) (number "10" (effects (font (size 1.016 1.016)))))
        (pin output line (at 12.7 12.7 180) (length 2.54) (name "QA" (effects (font (size 1.016 1.016)))) (number "15" (effects (font (size 1.016 1.016)))))
        (pin output line (at 12.7 8.89 180) (length 2.54) (name "QB" (effects (font (size 1.016 1.016)))) (number "1" (effects (font (size 1.016 1.016)))))
        (pin output line (at 12.7 5.08 180) (length 2.54) (name "QC" (effects (font (size 1.016 1.016)))) (number "2" (effects (font (size 1.016 1.016)))))
        (pin output line (at 12.7 1.27 180) (length 2.54) (name "QD" (effects (font (size 1.016 1.016)))) (number "3" (effects (font (size 1.016 1.016)))))
        (pin output line (at 12.7 -2.54 180) (length 2.54) (name "QE" (effects (font (size 1.016 1.016)))) (number "4" (effects (font (size 1.016 1.016)))))
        (pin output line (at 12.7 -6.35 180) (length 2.54) (name "QF" (effects (font (size 1.016 1.016)))) (number "5" (effects (font (size 1.016 1.016)))))
        (pin output line (at 12.7 -10.16 180) (length 2.54) (name "QG" (effects (font (size 1.016 1.016)))) (number "6" (effects (font (size 1.016 1.016)))))
        (pin output line (at 12.7 -13.97 180) (length 2.54) (name "QH" (effects (font (size 1.016 1.016)))) (number "7" (effects (font (size 1.016 1.016)))))
        (pin output line (at 0 -17.78 90) (length 2.54) (name "QH'" (effects (font (size 1.016 1.016)))) (number "9" (effects (font (size 1.016 1.016)))))
      )
    )"""

def generate_sheet4_transducer_matrix():
    """Generates 8x8 Transducer Grid in Sheet 4 with Net Labels and Ground Connections"""
    out = """(kicad_sch (version 20230121) (generator "HPVE_Generator") (generator_version "8.0")
	(uuid "00000000-0000-0000-0000-000067200400")
	(paper "A2")
	(title_block
		(title "Sheet 4: 8x8 Transducer Matrix Grid (64 Elements)")
		(date "2026-10-30")
		(rev "v1.0")
		(company "Iron Cave Industries")
		(comment 1 "40kHz Ultrasonic Emitters TX1 to TX64")
	)
	(lib_symbols
""" + get_transducer_lib_symbol() + """
	)
"""
    # 8x8 grid coordinates
    start_x = 40.0
    start_y = 40.0
    spacing_x = 45.0
    spacing_y = 40.0

    idx = 1
    for row in range(8):
        for col in range(8):
            x = start_x + (col * spacing_x)
            y = start_y + (row * spacing_y)
            ref = f"TX{idx}"
            drv_net = f"DRV_{idx}"

            # Transducer symbol
            out += f"""
	(symbol (lib_id "HPVE:Transducer_400ST") (at {x:.2f} {y:.2f} 0) (unit 1)
		(in_bom yes) (on_board yes) (dnp no)
		(uuid "{gen_uuid()}")
		(property "Reference" "{ref}" (at {x:.2f} {y-5.08:.2f} 0) (effects (font (size 1.27 1.27))))
		(property "Value" "400ST160" (at {x:.2f} {y+5.08:.2f} 0) (effects (font (size 1.016 1.016))))
		(pin "1" (uuid "{gen_uuid()}"))
		(pin "2" (uuid "{gen_uuid()}"))
	)
	(label "{drv_net}" (at {x:.2f} {y-6.35:.2f} 90) (fields_autoplaced yes)
		(effects (font (size 1.016 1.016)) (justify left bottom))
		(uuid "{gen_uuid()}")
	)
	(label "GND" (at {x:.2f} {y+6.35:.2f} 270) (fields_autoplaced yes)
		(effects (font (size 1.016 1.016)) (justify left top))
		(uuid "{gen_uuid()}")
	)
"""
            idx += 1

    out += ")\n"
    return out

def generate_sheet3_driver_bank():
    """Generates 32x TC4427 Driver IC Bank in Sheet 3"""
    out = """(kicad_sch (version 20230121) (generator "HPVE_Generator") (generator_version "8.0")
	(uuid "00000000-0000-0000-0000-000067200300")
	(paper "A2")
	(title_block
		(title "Sheet 3: TC4427 Dual High-Speed Driver Bank (32 ICs / 64 Channels)")
		(date "2026-10-30")
		(rev "v1.0")
		(company "Iron Cave Industries")
		(comment 1 "Levels 3.3V Logic to 12V Peak-to-Peak into Transducers")
	)
	(lib_symbols
""" + get_tc4427_lib_symbol() + """
	)
"""
    # 4 columns of 8 drivers
    start_x = 45.0
    start_y = 40.0
    spacing_x = 85.0
    spacing_y = 35.0

    drv_idx = 1
    for col in range(4):
        for row in range(8):
            x = start_x + (col * spacing_x)
            y = start_y + (row * spacing_y)
            ref = f"U{drv_idx + 100}"
            ch_a = (drv_idx - 1) * 2 + 1
            ch_b = (drv_idx - 1) * 2 + 2

            out += f"""
	(symbol (lib_id "HPVE:TC4427A") (at {x:.2f} {y:.2f} 0) (unit 1)
		(in_bom yes) (on_board yes) (dnp no)
		(uuid "{gen_uuid()}")
		(property "Reference" "{ref}" (at {x:.2f} {y-8.89:.2f} 0) (effects (font (size 1.27 1.27))))
		(property "Value" "TC4427ACOA" (at {x:.2f} {y+8.89:.2f} 0) (effects (font (size 1.016 1.016))))
		(pin "2" (uuid "{gen_uuid()}"))
		(pin "3" (uuid "{gen_uuid()}"))
		(pin "4" (uuid "{gen_uuid()}"))
		(pin "5" (uuid "{gen_uuid()}"))
		(pin "6" (uuid "{gen_uuid()}"))
		(pin "7" (uuid "{gen_uuid()}"))
	)
	(label "SIG_{ch_a}" (at {x-10.16:.2f} {y+5.08:.2f} 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))
	(label "SIG_{ch_b}" (at {x-10.16:.2f} {y-5.08:.2f} 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))
	(label "DRV_{ch_a}" (at {x+10.16:.2f} {y+5.08:.2f} 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))
	(label "DRV_{ch_b}" (at {x+10.16:.2f} {y-5.08:.2f} 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))
	(label "+12V" (at {x:.2f} {y-10.16:.2f} 90) (effects (font (size 1.016 1.016))) (uuid "{gen_uuid()}"))
	(label "GND" (at {x:.2f} {y+10.16:.2f} 270) (effects (font (size 1.016 1.016))) (uuid "{gen_uuid()}"))
"""
            drv_idx += 1

    out += ")\n"
    return out

def generate_sheet2_shift_registers():
    """Generates 8x 74HC595 Shift Register Cascade in Sheet 2"""
    out = """(kicad_sch (version 20230121) (generator "HPVE_Generator") (generator_version "8.0")
	(uuid "00000000-0000-0000-0000-000067200200")
	(paper "A3")
	(title_block
		(title "Sheet 2: 74HC595 High-Speed Shift Register Cascade (8 Stages)")
		(date "2026-10-30")
		(rev "v1.0")
		(company "Iron Cave Industries")
		(comment 1 "Deserializes High-Speed SPI to 64 Parallel Logic Drives")
	)
	(lib_symbols
""" + get_74hc595_lib_symbol() + """
	)
"""
    start_x = 45.0
    start_y = 45.0
    spacing_x = 70.0
    spacing_y = 55.0

    for i in range(8):
        row = i // 4
        col = i % 4
        x = start_x + (col * spacing_x)
        y = start_y + (row * spacing_y)
        ref = f"U{i+1}"
        base_ch = i * 8

        ser_in = "SPI_MOSI" if i == 0 else f"CASCADE_{i}"
        ser_out = f"CASCADE_{i+1}" if i < 7 else "NC_END"

        out += f"""
	(symbol (lib_id "HPVE:74HC595") (at {x:.2f} {y:.2f} 0) (unit 1)
		(in_bom yes) (on_board yes) (dnp no)
		(uuid "{gen_uuid()}")
		(property "Reference" "{ref}" (at {x:.2f} {y-16.51:.2f} 0) (effects (font (size 1.27 1.27))))
		(property "Value" "74HC595" (at {x:.2f} {y+16.51:.2f} 0) (effects (font (size 1.016 1.016))))
		(pin "14" (uuid "{gen_uuid()}"))
		(pin "11" (uuid "{gen_uuid()}"))
		(pin "12" (uuid "{gen_uuid()}"))
		(pin "13" (uuid "{gen_uuid()}"))
		(pin "10" (uuid "{gen_uuid()}"))
		(pin "15" (uuid "{gen_uuid()}"))
		(pin "1" (uuid "{gen_uuid()}"))
		(pin "2" (uuid "{gen_uuid()}"))
		(pin "3" (uuid "{gen_uuid()}"))
		(pin "4" (uuid "{gen_uuid()}"))
		(pin "5" (uuid "{gen_uuid()}"))
		(pin "6" (uuid "{gen_uuid()}"))
		(pin "7" (uuid "{gen_uuid()}"))
		(pin "9" (uuid "{gen_uuid()}"))
	)
	(label "{ser_in}" (at {x-12.7:.2f} {y+10.16:.2f} 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))
	(label "SPI_SCK" (at {x-12.7:.2f} {y+5.08:.2f} 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))
	(label "SPI_LATCH" (at {x-12.7:.2f} {y:.2f} 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))
	(label "GND" (at {x-12.7:.2f} {y-5.08:.2f} 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))
	(label "+3V3" (at {x-12.7:.2f} {y-10.16:.2f} 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))
	
	(label "SIG_{base_ch+1}" (at {x+12.7:.2f} {y+12.7:.2f} 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))
	(label "SIG_{base_ch+2}" (at {x+12.7:.2f} {y+8.89:.2f} 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))
	(label "SIG_{base_ch+3}" (at {x+12.7:.2f} {y+5.08:.2f} 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))
	(label "SIG_{base_ch+4}" (at {x+12.7:.2f} {y+1.27:.2f} 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))
	(label "SIG_{base_ch+5}" (at {x+12.7:.2f} {y-2.54:.2f} 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))
	(label "SIG_{base_ch+6}" (at {x+12.7:.2f} {y-6.35:.2f} 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))
	(label "SIG_{base_ch+7}" (at {x+12.7:.2f} {y-10.16:.2f} 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))
	(label "SIG_{base_ch+8}" (at {x+12.7:.2f} {y-13.97:.2f} 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))
	(label "{ser_out}" (at {x:.2f} {y-17.78:.2f} 270) (effects (font (size 1.016 1.016))) (uuid "{gen_uuid()}"))
"""

    out += ")\n"
    return out

def get_pico_lib_symbol():
    return """
    (symbol "HPVE:Pico" (pin_names (offset 1.016)) (in_bom yes) (on_board yes)
      (property "Reference" "U" (at 0 26.67 0) (effects (font (size 1.27 1.27))))
      (property "Value" "Raspberry_Pi_Pico" (at 0 -26.67 0) (effects (font (size 1.27 1.27))))
      (symbol "Pico_0_1"
        (rectangle (start -15.24 25.4) (end 15.24 -25.4) (stroke (width 0.254) (type solid)) (fill (type background)))
      )
      (symbol "Pico_1_1"
        (pin input line (at -17.78 20.32 0) (length 2.54) (name "GP4_SDA" (effects (font (size 1.016 1.016)))) (number "6" (effects (font (size 1.016 1.016)))))
        (pin input line (at -17.78 15.24 0) (length 2.54) (name "GP5_SCL" (effects (font (size 1.016 1.016)))) (number "7" (effects (font (size 1.016 1.016)))))
        (pin input line (at -17.78 7.62 0) (length 2.54) (name "GP10_ENC_A1" (effects (font (size 1.016 1.016)))) (number "14" (effects (font (size 1.016 1.016)))))
        (pin input line (at -17.78 2.54 0) (length 2.54) (name "GP11_ENC_B1" (effects (font (size 1.016 1.016)))) (number "15" (effects (font (size 1.016 1.016)))))
        (pin input line (at -17.78 -5.08 0) (length 2.54) (name "GP12_ENC_A2" (effects (font (size 1.016 1.016)))) (number "16" (effects (font (size 1.016 1.016)))))
        (pin input line (at -17.78 -10.16 0) (length 2.54) (name "GP13_ENC_B2" (effects (font (size 1.016 1.016)))) (number "17" (effects (font (size 1.016 1.016)))))
        (pin output line (at 17.78 20.32 180) (length 2.54) (name "GP16_MOSI" (effects (font (size 1.016 1.016)))) (number "21" (effects (font (size 1.016 1.016)))))
        (pin output line (at 17.78 15.24 180) (length 2.54) (name "GP17_SCK" (effects (font (size 1.016 1.016)))) (number "22" (effects (font (size 1.016 1.016)))))
        (pin output line (at 17.78 10.16 180) (length 2.54) (name "GP18_LATCH" (effects (font (size 1.016 1.016)))) (number "24" (effects (font (size 1.016 1.016)))))
        (pin power_in line (at 0 27.94 270) (length 2.54) (name "VSYS_5V" (effects (font (size 1.016 1.016)))) (number "39" (effects (font (size 1.016 1.016)))))
        (pin power_out line (at 17.78 -20.32 180) (length 2.54) (name "3V3_OUT" (effects (font (size 1.016 1.016)))) (number "36" (effects (font (size 1.016 1.016)))))
        (pin power_in line (at 0 -27.94 90) (length 2.54) (name "GND" (effects (font (size 1.016 1.016)))) (number "38" (effects (font (size 1.016 1.016)))))
      )
    )"""

def generate_sheet1_mcu_core():
    """Generates MCU Core, OLED HUD, and Encoders in Sheet 1"""
    out = """(kicad_sch (version 20230121) (generator "HPVE_Generator") (generator_version "8.0")
	(uuid "00000000-0000-0000-0000-000067200100")
	(paper "A3")
	(title_block
		(title "Sheet 1: RP2040 Pico Core, Power & I2C UI")
		(date "2026-10-30")
		(rev "v1.0")
		(company "Iron Cave Industries")
		(comment 1 "Master System Brain & Telemetry Subsystem")
	)
	(lib_symbols
""" + get_pico_lib_symbol() + """
	)
	(symbol (lib_id "HPVE:Pico") (at 120.0 90.0 0) (unit 1)
		(in_bom yes) (on_board yes) (dnp no)
		(uuid "{gen_uuid()}")
		(property "Reference" "U10" (at 120.0 60.0 0) (effects (font (size 1.27 1.27))))
		(property "Value" "Raspberry_Pi_Pico" (at 120.0 120.0 0) (effects (font (size 1.27 1.27))))
		(pin "6" (uuid "{gen_uuid()}"))
		(pin "7" (uuid "{gen_uuid()}"))
		(pin "14" (uuid "{gen_uuid()}"))
		(pin "15" (uuid "{gen_uuid()}"))
		(pin "16" (uuid "{gen_uuid()}"))
		(pin "17" (uuid "{gen_uuid()}"))
		(pin "21" (uuid "{gen_uuid()}"))
		(pin "22" (uuid "{gen_uuid()}"))
		(pin "24" (uuid "{gen_uuid()}"))
		(pin "39" (uuid "{gen_uuid()}"))
		(pin "36" (uuid "{gen_uuid()}"))
		(pin "38" (uuid "{gen_uuid()}"))
	)
	(label "I2C_SDA" (at 102.22 110.32 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))
	(label "I2C_SCL" (at 102.22 105.24 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))
	(label "ENC_A1" (at 102.22 97.62 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))
	(label "ENC_B1" (at 102.22 92.54 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))
	(label "ENC_A2" (at 102.22 84.92 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))
	(label "ENC_B2" (at 102.22 79.84 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))
	
	(label "SPI_MOSI" (at 137.78 110.32 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))
	(label "SPI_SCK" (at 137.78 105.24 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))
	(label "SPI_LATCH" (at 137.78 100.16 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))
	(label "+5V" (at 120.0 62.06 90) (effects (font (size 1.016 1.016))) (uuid "{gen_uuid()}"))
	(label "+3V3" (at 137.78 69.68 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))
	(label "GND" (at 120.0 117.94 270) (effects (font (size 1.016 1.016))) (uuid "{gen_uuid()}"))
)
"""
    return out

def main():
    base_dir = "hardware/kicad"
    print("Generating complete KiCad schematic sheets...")
    
    # Sheet 1: MCU Core & UI
    s1_path = os.path.join(base_dir, "sheet1_mcu_core.kicad_sch")
    with open(s1_path, "w", encoding="utf-8") as f:
        f.write(generate_sheet1_mcu_core())
    print(f"Generated Sheet 1 (RP2040 Pico Core & UI): {s1_path}")

    # Sheet 4: 8x8 Transducer Matrix Grid
    s4_path = os.path.join(base_dir, "sheet4_transducer_matrix.kicad_sch")
    with open(s4_path, "w", encoding="utf-8") as f:
        f.write(generate_sheet4_transducer_matrix())
    print(f"Generated Sheet 4 (64 Transducers): {s4_path}")

    # Sheet 3: TC4427 Driver Bank
    s3_path = os.path.join(base_dir, "sheet3_driver_bank.kicad_sch")
    with open(s3_path, "w", encoding="utf-8") as f:
        f.write(generate_sheet3_driver_bank())
    print(f"Generated Sheet 3 (32x TC4427 Drivers): {s3_path}")

    # Sheet 2: Shift Register Cascade
    s2_path = os.path.join(base_dir, "sheet2_shift_registers.kicad_sch")
    with open(s2_path, "w", encoding="utf-8") as f:
        f.write(generate_sheet2_shift_registers())
    print(f"Generated Sheet 2 (8x 74HC595 Registers): {s2_path}")

    print("All 4 KiCad sheets populated successfully!")

if __name__ == "__main__":
    main()

