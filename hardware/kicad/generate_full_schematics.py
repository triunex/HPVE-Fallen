"""
HPVE KiCad Schematic Generator (Complete with Wires and Standard Footprints)
Fixes the "no footprint assigned" error for F8 PCB sync and adds visible wiring.
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
      (property "Footprint" "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" (at 0 -7.62 0) (effects (font (size 1.27 1.27)) hide))
      (symbol "Transducer_400ST_0_1"
        (circle (center 0 0) (radius 3.81) (stroke (width 0.254) (type solid)) (fill (type background)))
        (polyline (pts (xy -2.54 2.54) (xy 2.54 -2.54)) (stroke (width 0.254) (type solid)) (fill (type none)))
        (polyline (pts (xy -2.54 -2.54) (xy 2.54 2.54)) (stroke (width 0.254) (type solid)) (fill (type none)))
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
        (rectangle (start -8.89 8.89) (end 8.89 -8.89) (stroke (width 0.254) (type solid)) (fill (type background)))
      )
      (symbol "TC4427A_1_1"
        (pin no_connect line (at -11.43 7.62 0) (length 2.54) (name "NC" (effects (font (size 1.016 1.016)))) (number "1" (effects (font (size 1.016 1.016)))))
        (pin input line (at -11.43 3.81 0) (length 2.54) (name "INA" (effects (font (size 1.016 1.016)))) (number "2" (effects (font (size 1.016 1.016)))))
        (pin power_in line (at 0 -11.43 90) (length 2.54) (name "GND" (effects (font (size 1.016 1.016)))) (number "3" (effects (font (size 1.016 1.016)))))
        (pin input line (at -11.43 -3.81 0) (length 2.54) (name "INB" (effects (font (size 1.016 1.016)))) (number "4" (effects (font (size 1.016 1.016)))))
        (pin output line (at 11.43 -3.81 180) (length 2.54) (name "OUTB" (effects (font (size 1.016 1.016)))) (number "5" (effects (font (size 1.016 1.016)))))
        (pin power_in line (at 0 11.43 270) (length 2.54) (name "VDD" (effects (font (size 1.016 1.016)))) (number "6" (effects (font (size 1.016 1.016)))))
        (pin output line (at 11.43 3.81 180) (length 2.54) (name "OUTA" (effects (font (size 1.016 1.016)))) (number "7" (effects (font (size 1.016 1.016)))))
        (pin no_connect line (at 11.43 7.62 180) (length 2.54) (name "NC" (effects (font (size 1.016 1.016)))) (number "8" (effects (font (size 1.016 1.016)))))
      )
    )"""

def get_74hc595_lib_symbol():
    return """
    (symbol "HPVE:74HC595" (pin_names (offset 1.016)) (in_bom yes) (on_board yes)
      (property "Reference" "U" (at 0 20.32 0) (effects (font (size 1.27 1.27))))
      (property "Value" "74HC595" (at 0 -20.32 0) (effects (font (size 1.27 1.27))))
      (property "Footprint" "Package_DIP:DIP-16_W7.62mm" (at 0 -22.86 0) (effects (font (size 1.27 1.27)) hide))
      (symbol "74HC595_0_1"
        (rectangle (start -11.43 19.05) (end 11.43 -19.05) (stroke (width 0.254) (type solid)) (fill (type background)))
      )
      (symbol "74HC595_1_1"
        (pin input line (at -13.97 12.7 0) (length 2.54) (name "SER" (effects (font (size 1.016 1.016)))) (number "14" (effects (font (size 1.016 1.016)))))
        (pin input line (at -13.97 7.62 0) (length 2.54) (name "SRCLK" (effects (font (size 1.016 1.016)))) (number "11" (effects (font (size 1.016 1.016)))))
        (pin input line (at -13.97 2.54 0) (length 2.54) (name "RCLK" (effects (font (size 1.016 1.016)))) (number "12" (effects (font (size 1.016 1.016)))))
        (pin input line (at -13.97 -2.54 0) (length 2.54) (name "~{OE}" (effects (font (size 1.016 1.016)))) (number "13" (effects (font (size 1.016 1.016)))))
        (pin input line (at -13.97 -7.62 0) (length 2.54) (name "~{SRCLR}" (effects (font (size 1.016 1.016)))) (number "10" (effects (font (size 1.016 1.016)))))
        (pin power_in line (at 0 -21.59 90) (length 2.54) (name "GND" (effects (font (size 1.016 1.016)))) (number "8" (effects (font (size 1.016 1.016)))))
        (pin power_in line (at 0 21.59 270) (length 2.54) (name "VCC" (effects (font (size 1.016 1.016)))) (number "16" (effects (font (size 1.016 1.016)))))
        (pin output line (at 13.97 15.24 180) (length 2.54) (name "QA" (effects (font (size 1.016 1.016)))) (number "15" (effects (font (size 1.016 1.016)))))
        (pin output line (at 13.97 11.43 180) (length 2.54) (name "QB" (effects (font (size 1.016 1.016)))) (number "1" (effects (font (size 1.016 1.016)))))
        (pin output line (at 13.97 7.62 180) (length 2.54) (name "QC" (effects (font (size 1.016 1.016)))) (number "2" (effects (font (size 1.016 1.016)))))
        (pin output line (at 13.97 3.81 180) (length 2.54) (name "QD" (effects (font (size 1.016 1.016)))) (number "3" (effects (font (size 1.016 1.016)))))
        (pin output line (at 13.97 0 180) (length 2.54) (name "QE" (effects (font (size 1.016 1.016)))) (number "4" (effects (font (size 1.016 1.016)))))
        (pin output line (at 13.97 -3.81 180) (length 2.54) (name "QF" (effects (font (size 1.016 1.016)))) (number "5" (effects (font (size 1.016 1.016)))))
        (pin output line (at 13.97 -7.62 180) (length 2.54) (name "QG" (effects (font (size 1.016 1.016)))) (number "6" (effects (font (size 1.016 1.016)))))
        (pin output line (at 13.97 -11.43 180) (length 2.54) (name "QH" (effects (font (size 1.016 1.016)))) (number "7" (effects (font (size 1.016 1.016)))))
        (pin output line (at 13.97 -15.24 180) (length 2.54) (name "QH'" (effects (font (size 1.016 1.016)))) (number "9" (effects (font (size 1.016 1.016)))))
      )
    )"""

def get_pico_lib_symbol():
    return """
    (symbol "HPVE:Pico" (pin_names (offset 1.016)) (in_bom yes) (on_board yes)
      (property "Reference" "U" (at 0 30.48 0) (effects (font (size 1.27 1.27))))
      (property "Value" "Raspberry_Pi_Pico" (at 0 -30.48 0) (effects (font (size 1.27 1.27))))
      (property "Footprint" "Connector_PinHeader_2.54mm:PinHeader_2x20_P2.54mm_Vertical" (at 0 -33.02 0) (effects (font (size 1.27 1.27)) hide))
      (symbol "Pico_0_1"
        (rectangle (start -17.78 29.21) (end 17.78 -29.21) (stroke (width 0.254) (type solid)) (fill (type background)))
      )
      (symbol "Pico_1_1"
        (pin passive line (at -20.32 25.4 0) (length 2.54) (name "GP0" (effects (font (size 0.8 0.8)))) (number "1" (effects (font (size 0.8 0.8)))))
        (pin passive line (at -20.32 22.86 0) (length 2.54) (name "GP1" (effects (font (size 0.8 0.8)))) (number "2" (effects (font (size 0.8 0.8)))))
        (pin power_in line (at -20.32 20.32 0) (length 2.54) (name "GND" (effects (font (size 0.8 0.8)))) (number "3" (effects (font (size 0.8 0.8)))))
        (pin passive line (at -20.32 17.78 0) (length 2.54) (name "GP2" (effects (font (size 0.8 0.8)))) (number "4" (effects (font (size 0.8 0.8)))))
        (pin passive line (at -20.32 15.24 0) (length 2.54) (name "GP3" (effects (font (size 0.8 0.8)))) (number "5" (effects (font (size 0.8 0.8)))))
        (pin bidirectional line (at -20.32 12.7 0) (length 2.54) (name "GP4_SDA" (effects (font (size 0.8 0.8)))) (number "6" (effects (font (size 0.8 0.8)))))
        (pin bidirectional line (at -20.32 10.16 0) (length 2.54) (name "GP5_SCL" (effects (font (size 0.8 0.8)))) (number "7" (effects (font (size 0.8 0.8)))))
        (pin power_in line (at -20.32 7.62 0) (length 2.54) (name "GND" (effects (font (size 0.8 0.8)))) (number "8" (effects (font (size 0.8 0.8)))))
        (pin passive line (at -20.32 5.08 0) (length 2.54) (name "GP6" (effects (font (size 0.8 0.8)))) (number "9" (effects (font (size 0.8 0.8)))))
        (pin passive line (at -20.32 2.54 0) (length 2.54) (name "GP7" (effects (font (size 0.8 0.8)))) (number "10" (effects (font (size 0.8 0.8)))))
        (pin passive line (at -20.32 0 0) (length 2.54) (name "GP8" (effects (font (size 0.8 0.8)))) (number "11" (effects (font (size 0.8 0.8)))))
        (pin passive line (at -20.32 -2.54 0) (length 2.54) (name "GP9" (effects (font (size 0.8 0.8)))) (number "12" (effects (font (size 0.8 0.8)))))
        (pin power_in line (at -20.32 -5.08 0) (length 2.54) (name "GND" (effects (font (size 0.8 0.8)))) (number "13" (effects (font (size 0.8 0.8)))))
        (pin input line (at -20.32 -7.62 0) (length 2.54) (name "GP10_E1A" (effects (font (size 0.8 0.8)))) (number "14" (effects (font (size 0.8 0.8)))))
        (pin input line (at -20.32 -10.16 0) (length 2.54) (name "GP11_E1B" (effects (font (size 0.8 0.8)))) (number "15" (effects (font (size 0.8 0.8)))))
        (pin input line (at -20.32 -12.7 0) (length 2.54) (name "GP12_E2A" (effects (font (size 0.8 0.8)))) (number "16" (effects (font (size 0.8 0.8)))))
        (pin input line (at -20.32 -15.24 0) (length 2.54) (name "GP13_E2B" (effects (font (size 0.8 0.8)))) (number "17" (effects (font (size 0.8 0.8)))))
        (pin power_in line (at -20.32 -17.78 0) (length 2.54) (name "GND" (effects (font (size 0.8 0.8)))) (number "18" (effects (font (size 0.8 0.8)))))
        (pin passive line (at -20.32 -20.32 0) (length 2.54) (name "GP14" (effects (font (size 0.8 0.8)))) (number "19" (effects (font (size 0.8 0.8)))))
        (pin passive line (at -20.32 -22.86 0) (length 2.54) (name "GP15" (effects (font (size 0.8 0.8)))) (number "20" (effects (font (size 0.8 0.8)))))
        (pin output line (at 20.32 -22.86 180) (length 2.54) (name "GP16_MOSI" (effects (font (size 0.8 0.8)))) (number "21" (effects (font (size 0.8 0.8)))))
        (pin output line (at 20.32 -20.32 180) (length 2.54) (name "GP17_SCK" (effects (font (size 0.8 0.8)))) (number "22" (effects (font (size 0.8 0.8)))))
        (pin power_in line (at 20.32 -17.78 180) (length 2.54) (name "GND" (effects (font (size 0.8 0.8)))) (number "23" (effects (font (size 0.8 0.8)))))
        (pin output line (at 20.32 -15.24 180) (length 2.54) (name "GP18_LAT" (effects (font (size 0.8 0.8)))) (number "24" (effects (font (size 0.8 0.8)))))
        (pin passive line (at 20.32 -12.7 180) (length 2.54) (name "GP19" (effects (font (size 0.8 0.8)))) (number "25" (effects (font (size 0.8 0.8)))))
        (pin passive line (at 20.32 -10.16 180) (length 2.54) (name "GP20" (effects (font (size 0.8 0.8)))) (number "26" (effects (font (size 0.8 0.8)))))
        (pin passive line (at 20.32 -7.62 180) (length 2.54) (name "GP21" (effects (font (size 0.8 0.8)))) (number "27" (effects (font (size 0.8 0.8)))))
        (pin power_in line (at 20.32 -5.08 180) (length 2.54) (name "GND" (effects (font (size 0.8 0.8)))) (number "28" (effects (font (size 0.8 0.8)))))
        (pin passive line (at 20.32 -2.54 180) (length 2.54) (name "GP22" (effects (font (size 0.8 0.8)))) (number "29" (effects (font (size 0.8 0.8)))))
        (pin input line (at 20.32 0 180) (length 2.54) (name "RUN" (effects (font (size 0.8 0.8)))) (number "30" (effects (font (size 0.8 0.8)))))
        (pin passive line (at 20.32 2.54 180) (length 2.54) (name "GP26" (effects (font (size 0.8 0.8)))) (number "31" (effects (font (size 0.8 0.8)))))
        (pin passive line (at 20.32 5.08 180) (length 2.54) (name "GP27" (effects (font (size 0.8 0.8)))) (number "32" (effects (font (size 0.8 0.8)))))
        (pin power_in line (at 20.32 7.62 180) (length 2.54) (name "GND" (effects (font (size 0.8 0.8)))) (number "33" (effects (font (size 0.8 0.8)))))
        (pin passive line (at 20.32 10.16 180) (length 2.54) (name "GP28" (effects (font (size 0.8 0.8)))) (number "34" (effects (font (size 0.8 0.8)))))
        (pin power_in line (at 20.32 12.7 180) (length 2.54) (name "ADC_VREF" (effects (font (size 0.8 0.8)))) (number "35" (effects (font (size 0.8 0.8)))))
        (pin power_out line (at 20.32 15.24 180) (length 2.54) (name "3V3_OUT" (effects (font (size 0.8 0.8)))) (number "36" (effects (font (size 0.8 0.8)))))
        (pin input line (at 20.32 17.78 180) (length 2.54) (name "3V3_EN" (effects (font (size 0.8 0.8)))) (number "37" (effects (font (size 0.8 0.8)))))
        (pin power_in line (at 20.32 20.32 180) (length 2.54) (name "GND" (effects (font (size 0.8 0.8)))) (number "38" (effects (font (size 0.8 0.8)))))
        (pin power_in line (at 20.32 22.86 180) (length 2.54) (name "VSYS" (effects (font (size 0.8 0.8)))) (number "39" (effects (font (size 0.8 0.8)))))
        (pin power_in line (at 20.32 25.4 180) (length 2.54) (name "VBUS" (effects (font (size 0.8 0.8)))) (number "40" (effects (font (size 0.8 0.8)))))
      )
    )"""

def generate_sheet4_transducer_matrix():
    """Generates 8x8 Transducer Grid in Sheet 4 with Footprints and Connecting Wires"""
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
    start_x = 40.0
    start_y = 40.0
    spacing_x = 45.0
    spacing_y = 42.0

    idx = 1
    for row in range(8):
        for col in range(8):
            x = start_x + (col * spacing_x)
            y = start_y + (row * spacing_y)
            ref = f"TX{idx}"
            drv_net = f"DRV_{idx}"

            pin1_y = y - 6.35
            pin2_y = y + 6.35
            wire_top_y = pin1_y - 5.08
            wire_bot_y = pin2_y + 5.08

            out += f"""
	(symbol (lib_id "HPVE:Transducer_400ST") (at {x:.2f} {y:.2f} 0) (unit 1)
		(in_bom yes) (on_board yes) (dnp no)
		(uuid "{gen_uuid()}")
		(property "Reference" "{ref}" (at {x:.2f} {y-4.5:.2f} 0) (effects (font (size 1.27 1.27))))
		(property "Value" "400ST160" (at {x:.2f} {y+4.5:.2f} 0) (effects (font (size 1.016 1.016))))
		(property "Footprint" "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" (at {x:.2f} {y:.2f} 0) (effects (font (size 1.27 1.27)) hide))
		(pin "1" (uuid "{gen_uuid()}"))
		(pin "2" (uuid "{gen_uuid()}"))
	)
	(wire (pts (xy {x:.2f} {pin1_y:.2f}) (xy {x:.2f} {wire_top_y:.2f})) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "{drv_net}" (at {x:.2f} {wire_top_y:.2f} 90) (fields_autoplaced yes)
		(effects (font (size 1.016 1.016)) (justify left bottom))
		(uuid "{gen_uuid()}")
	)
	(wire (pts (xy {x:.2f} {pin2_y:.2f}) (xy {x:.2f} {wire_bot_y:.2f})) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "GND" (at {x:.2f} {wire_bot_y:.2f} 270) (fields_autoplaced yes)
		(effects (font (size 1.016 1.016)) (justify left top))
		(uuid "{gen_uuid()}")
	)
"""
            idx += 1

    out += ")\n"
    return out

def generate_sheet3_driver_bank():
    """Generates 32x TC4427 Driver IC Bank in Sheet 3 with Footprints and Wires"""
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
    start_x = 45.0
    start_y = 45.0
    spacing_x = 90.0
    spacing_y = 40.0

    drv_idx = 1
    for col in range(4):
        for row in range(8):
            x = start_x + (col * spacing_x)
            y = start_y + (row * spacing_y)
            ref = f"U{drv_idx + 100}"
            ch_a = (drv_idx - 1) * 2 + 1
            ch_b = (drv_idx - 1) * 2 + 2

            ina_x = x - 11.43
            outa_x = x + 11.43
            vdd_y = y - 11.43
            gnd_y = y + 11.43

            out += f"""
	(symbol (lib_id "HPVE:TC4427A") (at {x:.2f} {y:.2f} 0) (unit 1)
		(in_bom yes) (on_board yes) (dnp no)
		(uuid "{gen_uuid()}")
		(property "Reference" "{ref}" (at {x:.2f} {y-7.5:.2f} 0) (effects (font (size 1.27 1.27))))
		(property "Value" "TC4427ACOA" (at {x:.2f} {y+7.5:.2f} 0) (effects (font (size 1.016 1.016))))
		(property "Footprint" "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" (at {x:.2f} {y:.2f} 0) (effects (font (size 1.27 1.27)) hide))
		(pin "1" (uuid "{gen_uuid()}"))
		(pin "2" (uuid "{gen_uuid()}"))
		(pin "3" (uuid "{gen_uuid()}"))
		(pin "4" (uuid "{gen_uuid()}"))
		(pin "5" (uuid "{gen_uuid()}"))
		(pin "6" (uuid "{gen_uuid()}"))
		(pin "7" (uuid "{gen_uuid()}"))
		(pin "8" (uuid "{gen_uuid()}"))
	)
	(wire (pts (xy {ina_x:.2f} {y+5.08:.2f}) (xy {ina_x-7.62:.2f} {y+5.08:.2f})) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "SIG_{ch_a}" (at {ina_x-7.62:.2f} {y+5.08:.2f} 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))

	(wire (pts (xy {ina_x:.2f} {y-5.08:.2f}) (xy {ina_x-7.62:.2f} {y-5.08:.2f})) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "SIG_{ch_b}" (at {ina_x-7.62:.2f} {y-5.08:.2f} 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))

	(wire (pts (xy {outa_x:.2f} {y+5.08:.2f}) (xy {outa_x+7.62:.2f} {y+5.08:.2f})) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "DRV_{ch_a}" (at {outa_x+7.62:.2f} {y+5.08:.2f} 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))

	(wire (pts (xy {outa_x:.2f} {y-5.08:.2f}) (xy {outa_x+7.62:.2f} {y-5.08:.2f})) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "DRV_{ch_b}" (at {outa_x+7.62:.2f} {y-5.08:.2f} 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))

	(wire (pts (xy {x:.2f} {vdd_y:.2f}) (xy {x:.2f} {vdd_y-5.08:.2f})) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "+12V" (at {x:.2f} {vdd_y-5.08:.2f} 90) (effects (font (size 1.016 1.016))) (uuid "{gen_uuid()}"))

	(wire (pts (xy {x:.2f} {gnd_y:.2f}) (xy {x:.2f} {gnd_y+5.08:.2f})) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "GND" (at {x:.2f} {gnd_y+5.08:.2f} 270) (effects (font (size 1.016 1.016))) (uuid "{gen_uuid()}"))
"""
            drv_idx += 1

    out += ")\n"
    return out

def generate_sheet2_shift_registers():
    """Generates 8x 74HC595 Shift Registers in Sheet 2 with Footprints and Wires"""
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
    start_x = 55.0
    start_y = 50.0
    spacing_x = 85.0
    spacing_y = 65.0

    for i in range(8):
        row = i // 4
        col = i % 4
        x = start_x + (col * spacing_x)
        y = start_y + (row * spacing_y)
        ref = f"U{i+1}"
        base_ch = i * 8

        ser_in = "SPI_MOSI" if i == 0 else f"CASCADE_{i}"
        ser_out = f"CASCADE_{i+1}" if i < 7 else "NC_END"
        left_x = x - 13.97
        right_x = x + 13.97

        out += f"""
	(symbol (lib_id "HPVE:74HC595") (at {x:.2f} {y:.2f} 0) (unit 1)
		(in_bom yes) (on_board yes) (dnp no)
		(uuid "{gen_uuid()}")
		(property "Reference" "{ref}" (at {x:.2f} {y-16.0:.2f} 0) (effects (font (size 1.27 1.27))))
		(property "Value" "74HC595" (at {x:.2f} {y+16.0:.2f} 0) (effects (font (size 1.016 1.016))))
		(property "Footprint" "Package_DIP:DIP-16_W7.62mm" (at {x:.2f} {y:.2f} 0) (effects (font (size 1.27 1.27)) hide))
		(pin "14" (uuid "{gen_uuid()}"))
		(pin "11" (uuid "{gen_uuid()}"))
		(pin "12" (uuid "{gen_uuid()}"))
		(pin "13" (uuid "{gen_uuid()}"))
		(pin "10" (uuid "{gen_uuid()}"))
		(pin "8" (uuid "{gen_uuid()}"))
		(pin "16" (uuid "{gen_uuid()}"))
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
	(wire (pts (xy {left_x:.2f} {y+12.7:.2f}) (xy {left_x-7.62:.2f} {y+12.7:.2f})) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "{ser_in}" (at {left_x-7.62:.2f} {y+12.7:.2f} 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))

	(wire (pts (xy {left_x:.2f} {y+7.62:.2f}) (xy {left_x-7.62:.2f} {y+7.62:.2f})) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "SPI_SCK" (at {left_x-7.62:.2f} {y+7.62:.2f} 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))

	(wire (pts (xy {left_x:.2f} {y+2.54:.2f}) (xy {left_x-7.62:.2f} {y+2.54:.2f})) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "SPI_LATCH" (at {left_x-7.62:.2f} {y+2.54:.2f} 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))

	(wire (pts (xy {left_x:.2f} {y-2.54:.2f}) (xy {left_x-7.62:.2f} {y-2.54:.2f})) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "GND" (at {left_x-7.62:.2f} {y-2.54:.2f} 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))

	(wire (pts (xy {left_x:.2f} {y-7.62:.2f}) (xy {left_x-7.62:.2f} {y-7.62:.2f})) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "+3V3" (at {left_x-7.62:.2f} {y-7.62:.2f} 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))

	(wire (pts (xy {x:.2f} {y-21.59:.2f}) (xy {x:.2f} {y-26.67:.2f})) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "GND" (at {x:.2f} {y-26.67:.2f} 270) (effects (font (size 1.016 1.016))) (uuid "{gen_uuid()}"))

	(wire (pts (xy {x:.2f} {y+21.59:.2f}) (xy {x:.2f} {y+26.67:.2f})) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "+3V3" (at {x:.2f} {y+26.67:.2f} 90) (effects (font (size 1.016 1.016))) (uuid "{gen_uuid()}"))
"""
        outputs = ["QA", "QB", "QC", "QD", "QE", "QF", "QG", "QH"]
        offsets_y = [15.24, 11.43, 7.62, 3.81, 0.0, -3.81, -7.62, -11.43]
        for pin_i in range(8):
            py = y + offsets_y[pin_i]
            ch_num = base_ch + pin_i + 1
            out += f"""
	(wire (pts (xy {right_x:.2f} {py:.2f}) (xy {right_x+7.62:.2f} {py:.2f})) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "SIG_{ch_num}" (at {right_x+7.62:.2f} {py:.2f} 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))
"""
        out += f"""
	(wire (pts (xy {right_x:.2f} {y-15.24:.2f}) (xy {right_x+7.62:.2f} {y-15.24:.2f})) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "{ser_out}" (at {right_x+7.62:.2f} {y-15.24:.2f} 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))
"""

    out += ")\n"
    return out

def generate_sheet1_mcu_core():
    """Generates MCU Core with Footprints and Wires"""
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
		(property "Footprint" "Connector_PinHeader_2.54mm:PinHeader_2x20_P2.54mm_Vertical" (at 120.0 90.0 0) (effects (font (size 1.27 1.27)) hide))
"""
    for p in range(1, 41):
        out += f'\t\t(pin "{p}" (uuid "{gen_uuid()}"))\n'
    out += """	)
	(wire (pts (xy 99.68 77.30) (xy 89.68 77.30)) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "I2C_SDA" (at 89.68 77.30 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))

	(wire (pts (xy 99.68 79.84) (xy 89.68 79.84)) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "I2C_SCL" (at 89.68 79.84 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))

	(wire (pts (xy 99.68 97.62) (xy 89.68 97.62)) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "ENC_A1" (at 89.68 97.62 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))

	(wire (pts (xy 99.68 100.16) (xy 89.68 100.16)) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "ENC_B1" (at 89.68 100.16 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))

	(wire (pts (xy 99.68 102.70) (xy 89.68 102.70)) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "ENC_A2" (at 89.68 102.70 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))

	(wire (pts (xy 99.68 105.24) (xy 89.68 105.24)) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "ENC_B2" (at 89.68 105.24 180) (effects (font (size 1.016 1.016)) (justify right)) (uuid "{gen_uuid()}"))

	(wire (pts (xy 140.32 112.86) (xy 150.32 112.86)) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "SPI_MOSI" (at 150.32 112.86 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))

	(wire (pts (xy 140.32 110.32) (xy 150.32 110.32)) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "SPI_SCK" (at 150.32 110.32 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))

	(wire (pts (xy 140.32 105.24) (xy 150.32 105.24)) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "SPI_LATCH" (at 150.32 105.24 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))

	(wire (pts (xy 140.32 67.14) (xy 150.32 67.14)) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "VSYS_5V" (at 150.32 67.14 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))

	(wire (pts (xy 140.32 74.76) (xy 150.32 74.76)) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
	(label "+3V3" (at 150.32 74.76 0) (effects (font (size 1.016 1.016)) (justify left)) (uuid "{gen_uuid()}"))

	(wire (pts (xy 140.32 69.68) (xy 150.32 69.68)) (stroke (width 0) (type default)) (uuid "{gen_uuid()}"))
)
"""
    return out

def main():
    base_dir = "hardware/kicad"
    print("Generating complete KiCad schematic sheets with wires and valid footprints...")
    
    # Sheet 1: MCU Core & UI
    s1_path = os.path.join(base_dir, "sheet1_mcu_core.kicad_sch")
    with open(s1_path, "w", encoding="utf-8") as f:
        f.write(generate_sheet1_mcu_core())
    print(f"Generated Sheet 1: {s1_path}")

    # Sheet 4: 8x8 Transducer Matrix Grid
    s4_path = os.path.join(base_dir, "sheet4_transducer_matrix.kicad_sch")
    with open(s4_path, "w", encoding="utf-8") as f:
        f.write(generate_sheet4_transducer_matrix())
    print(f"Generated Sheet 4: {s4_path}")

    # Sheet 3: TC4427 Driver Bank
    s3_path = os.path.join(base_dir, "sheet3_driver_bank.kicad_sch")
    with open(s3_path, "w", encoding="utf-8") as f:
        f.write(generate_sheet3_driver_bank())
    print(f"Generated Sheet 3: {s3_path}")

    # Sheet 2: Shift Register Cascade
    s2_path = os.path.join(base_dir, "sheet2_shift_registers.kicad_sch")
    with open(s2_path, "w", encoding="utf-8") as f:
        f.write(generate_sheet2_shift_registers())
    print(f"Generated Sheet 2: {s2_path}")

    print("All 4 KiCad sheets updated with wires and official built-in footprints!")

if __name__ == "__main__":
    main()
