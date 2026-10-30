"""
Apply Real Hardware Footprints to HPVE PCB:
1. Replaces generic 2x20 header U10 with official RaspberryPi_Pico_Common_THT footprint.
   Places it on the left with USB port facing outward.
2. Removes all tall male pin 3D models from TX1..TX64 so they render as flush
   through-hole solder pads (zero male pins sticking up!).
3. Re-routes SPI signals (MOSI, SCK, LATCH) and power lines with 45-degree miters.
4. Refills GND copper zones and updates board boundary.
"""

import sys
import pcbnew

pcb_path = "hardware/kicad/HPVE_Controller.kicad_pcb"
print(f"Loading board: {pcb_path}")
board = pcbnew.LoadBoard(pcb_path)

# Helper: Route trace with 45-degree mitered chamfer
def route_45_miter(board, p1, p2, net, layer=pcbnew.F_Cu, width_mm=0.35):
    x1, y1 = p1.x, p1.y
    x2, y2 = p2.x, p2.y
    width = pcbnew.FromMM(width_mm)

    dx = x2 - x1
    dy = y2 - y1

    if dx == 0 or dy == 0:
        seg = pcbnew.PCB_TRACK(board)
        seg.SetStart(p1)
        seg.SetEnd(p2)
        seg.SetWidth(width)
        seg.SetLayer(layer)
        seg.SetNet(net)
        board.Add(seg)
        return

    d = min(abs(dx), abs(dy))
    sx = 1 if dx > 0 else -1
    sy = 1 if dy > 0 else -1

    if abs(dx) >= abs(dy):
        mid = pcbnew.VECTOR2I(x2 - sx * d, y1)
        seg1 = pcbnew.PCB_TRACK(board)
        seg1.SetStart(p1)
        seg1.SetEnd(mid)
        seg1.SetWidth(width)
        seg1.SetLayer(layer)
        seg1.SetNet(net)
        board.Add(seg1)

        seg2 = pcbnew.PCB_TRACK(board)
        seg2.SetStart(mid)
        seg2.SetEnd(p2)
        seg2.SetWidth(width)
        seg2.SetLayer(layer)
        seg2.SetNet(net)
        board.Add(seg2)
    else:
        mid = pcbnew.VECTOR2I(x1, y2 - sy * d)
        seg1 = pcbnew.PCB_TRACK(board)
        seg1.SetStart(p1)
        seg1.SetEnd(mid)
        seg1.SetWidth(width)
        seg1.SetLayer(layer)
        seg1.SetNet(net)
        board.Add(seg1)

        seg2 = pcbnew.PCB_TRACK(board)
        seg2.SetStart(mid)
        seg2.SetEnd(p2)
        seg2.SetWidth(width)
        seg2.SetLayer(layer)
        seg2.SetNet(net)
        board.Add(seg2)

# 1. Fix Transducers (TX1..TX64): Clear male pin 3D models!
for t in range(1, 65):
    tx = board.FindFootprintByReference(f"TX{t}")
    if tx:
        # Clear 3D models to eliminate tall male pins
        tx.Models().clear()

print("Cleared male pin 3D models from all 64 transducers (now flush solder pads).")

# 2. Fix Raspberry Pi Pico (U10):
# Save existing net connections on U10
old_u10 = board.FindFootprintByReference("U10")
pico_nets = {}
if old_u10:
    for pad in old_u10.Pads():
        num = pad.GetNumber()
        net = pad.GetNet()
        if net and net.GetNetname():
            pico_nets[num] = net
    board.Remove(old_u10)
    print("Removed old generic 2x20 U10 footprint.")

# Load official RaspberryPi_Pico_Common_THT
fp_pico = pcbnew.FootprintLoad("N:/resume/share/kicad/footprints/Module.pretty", "RaspberryPi_Pico_Common_THT")
if not fp_pico:
    print("Error: Could not load RaspberryPi_Pico_Common_THT footprint.")
    sys.exit(1)

fp_pico.SetReference("U10")
fp_pico.SetValue("RP2040_Pico")

# Position Pico on the left:
# Pad 1 will be at (58.0, 78.0) mm.
# Left pin row (pins 1..20) runs from (58.0, 78.0) down to (58.0, 126.26) mm.
# Right pin row (pins 21..40) runs at X = 58.0 + 17.78 = 75.78 mm!
# Driver Col 1 is at X = 86.86 mm -> 11 mm clearance!
pico_x = 58.0
pico_y = 78.0
fp_pico.SetPosition(pcbnew.VECTOR2I(pcbnew.FromMM(pico_x), pcbnew.FromMM(pico_y)))

# Re-assign net connections to Pico pads
for num, net in pico_nets.items():
    pad = fp_pico.FindPadByNumber(num)
    if pad:
        pad.SetNet(net)

# Ensure critical SPI nets are bound:
# Pad 21: SPI_MOSI
# Pad 22: SPI_SCK
# Pad 24: SPI_LATCH
# Pad 38: GND
net_mosi = board.FindNet("SPI_MOSI")
net_sck = board.FindNet("SPI_SCK")
net_lat = board.FindNet("SPI_LATCH")
net_gnd = board.FindNet("GND")

if net_mosi and fp_pico.FindPadByNumber("21"):
    fp_pico.FindPadByNumber("21").SetNet(net_mosi)
if net_sck and fp_pico.FindPadByNumber("22"):
    fp_pico.FindPadByNumber("22").SetNet(net_sck)
if net_lat and fp_pico.FindPadByNumber("24"):
    fp_pico.FindPadByNumber("24").SetNet(net_lat)
if net_gnd and fp_pico.FindPadByNumber("38"):
    fp_pico.FindPadByNumber("38").SetNet(net_gnd)

board.Add(fp_pico)
print(f"Added official Raspberry Pi Pico footprint U10 at ({pico_x}, {pico_y}) mm.")

# 3. Clean up loose tracks connected to old U10 and re-route SPI to U1
# Clear all tracks on board and re-run clean 45-degree routing
tracks = list(board.GetTracks())
for t in tracks:
    board.Remove(t)

print("Re-routing all tracks with updated Pico position...")

# Route 64 DRV Channels (Driver OUT -> Transducer) on F.Cu
for d in range(1, 33):
    u_drv = board.FindFootprintByReference(f"U{100 + d}")
    if not u_drv:
        continue
    ch_a = (d - 1) * 2 + 1
    ch_b = (d - 1) * 2 + 2

    tx_a = board.FindFootprintByReference(f"TX{ch_a}")
    if tx_a:
        p_outa = u_drv.FindPadByNumber("7")
        p_txa = tx_a.FindPadByNumber("1")
        net_a = board.FindNet(f"DRV_{ch_a}")
        if net_a:
            p_outa.SetNet(net_a)
            p_txa.SetNet(net_a)
            route_45_miter(board, p_outa.GetPosition(), p_txa.GetPosition(), net_a, layer=pcbnew.F_Cu, width_mm=0.35)

    tx_b = board.FindFootprintByReference(f"TX{ch_b}")
    if tx_b:
        p_outb = u_drv.FindPadByNumber("5")
        p_txb = tx_b.FindPadByNumber("1")
        net_b = board.FindNet(f"DRV_{ch_b}")
        if net_b:
            p_outb.SetNet(net_b)
            p_txb.SetNet(net_b)
            route_45_miter(board, p_outb.GetPosition(), p_txb.GetPosition(), net_b, layer=pcbnew.F_Cu, width_mm=0.35)

# Route 64 SIG Channels (Shift Register QA..QH -> Driver INA/INB) on B.Cu
sr_out_pins = ["15", "1", "2", "3", "4", "5", "6", "7"]
for ch in range(1, 65):
    sr_idx = (ch - 1) // 8 + 1
    sr_pin = sr_out_pins[(ch - 1) % 8]
    drv_idx = (ch - 1) // 2 + 1
    drv_pin = "4" if ((ch - 1) % 2) == 1 else "2"

    u_sr = board.FindFootprintByReference(f"U{sr_idx}")
    u_drv = board.FindFootprintByReference(f"U{100 + drv_idx}")
    if u_sr and u_drv:
        p_sr = u_sr.FindPadByNumber(sr_pin)
        p_drv = u_drv.FindPadByNumber(drv_pin)
        net_sig = board.FindNet(f"SIG_{ch}")
        if net_sig:
            p_sr.SetNet(net_sig)
            p_drv.SetNet(net_sig)
            route_45_miter(board, p_sr.GetPosition(), p_drv.GetPosition(), net_sig, layer=pcbnew.B_Cu, width_mm=0.25)

# Route SPI Daisy Chain across U1..U8 on F.Cu
for i in range(1, 8):
    u_c = board.FindFootprintByReference(f"U{i}")
    u_n = board.FindFootprintByReference(f"U{i+1}")
    if u_c and u_n:
        p_out = u_c.FindPadByNumber("9")
        p_in = u_n.FindPadByNumber("14")
        net_cas = board.FindNet(f"CASCADE_{i}")
        if net_cas:
            p_out.SetNet(net_cas)
            p_in.SetNet(net_cas)
            route_45_miter(board, p_out.GetPosition(), p_in.GetPosition(), net_cas, layer=pcbnew.F_Cu, width_mm=0.3)

        p1_sck = u_c.FindPadByNumber("11")
        p2_sck = u_n.FindPadByNumber("11")
        if net_sck:
            p1_sck.SetNet(net_sck)
            p2_sck.SetNet(net_sck)
            route_45_miter(board, p1_sck.GetPosition(), p2_sck.GetPosition(), net_sck, layer=pcbnew.F_Cu, width_mm=0.3)

        p1_lat = u_c.FindPadByNumber("12")
        p2_lat = u_n.FindPadByNumber("12")
        if net_lat:
            p1_lat.SetNet(net_lat)
            p2_lat.SetNet(net_lat)
            route_45_miter(board, p1_lat.GetPosition(), p2_lat.GetPosition(), net_lat, layer=pcbnew.F_Cu, width_mm=0.3)

# Connect Pico U10 to Shift Register U1
u_sr1 = board.FindFootprintByReference("U1")
if fp_pico and u_sr1:
    p_mosi_src = fp_pico.FindPadByNumber("21")
    p_mosi_dst = u_sr1.FindPadByNumber("14")
    if p_mosi_src and p_mosi_dst and net_mosi:
        route_45_miter(board, p_mosi_src.GetPosition(), p_mosi_dst.GetPosition(), net_mosi, layer=pcbnew.F_Cu, width_mm=0.3)

    p_sck_src = fp_pico.FindPadByNumber("22")
    p_sck_dst = u_sr1.FindPadByNumber("11")
    if p_sck_src and p_sck_dst and net_sck:
        route_45_miter(board, p_sck_src.GetPosition(), p_sck_dst.GetPosition(), net_sck, layer=pcbnew.F_Cu, width_mm=0.3)

    p_lat_src = fp_pico.FindPadByNumber("24")
    p_lat_dst = u_sr1.FindPadByNumber("12")
    if p_lat_src and p_lat_dst and net_lat:
        route_45_miter(board, p_lat_src.GetPosition(), p_lat_dst.GetPosition(), net_lat, layer=pcbnew.F_Cu, width_mm=0.3)

# Route +12V Power Line (F.Cu) and GND Return Lines (B.Cu)
net_12v = board.FindNet("+12V")
for d in range(1, 32):
    u1 = board.FindFootprintByReference(f"U{100 + d}")
    u2 = board.FindFootprintByReference(f"U{100 + d + 1}")
    if u1 and u2 and net_12v:
        p1_12v = u1.FindPadByNumber("6")
        p2_12v = u2.FindPadByNumber("6")
        p1_12v.SetNet(net_12v)
        p2_12v.SetNet(net_12v)
        route_45_miter(board, p1_12v.GetPosition(), p2_12v.GetPosition(), net_12v, layer=pcbnew.F_Cu, width_mm=0.6)

for t in range(1, 64):
    tx1 = board.FindFootprintByReference(f"TX{t}")
    tx2 = board.FindFootprintByReference(f"TX{t + 1}")
    if tx1 and tx2 and net_gnd:
        p1_g = tx1.FindPadByNumber("2")
        p2_g = tx2.FindPadByNumber("2")
        p1_g.SetNet(net_gnd)
        p2_g.SetNet(net_gnd)
        route_45_miter(board, p1_g.GetPosition(), p2_g.GetPosition(), net_gnd, layer=pcbnew.B_Cu, width_mm=0.5)

print(f"Total 45-degree mitered tracks routed: {len(board.GetTracks())}")

# 4. Update Board Outline (Edge.Cuts) & Mounting Holes
# Board encompasses Pico on left (X=50) to Transducers on right (X=162)
drawings = list(board.GetDrawings())
for d in drawings:
    if d.GetLayer() in [pcbnew.Edge_Cuts, pcbnew.F_SilkS]:
        board.Remove(d)

bx_min, bx_max = 46.0, 170.0  # Width = 124.0 mm
by_min, by_max = 46.0, 140.0  # Height = 94.0 mm

corners = [
    (pcbnew.VECTOR2I(pcbnew.FromMM(bx_min), pcbnew.FromMM(by_min)), pcbnew.VECTOR2I(pcbnew.FromMM(bx_max), pcbnew.FromMM(by_min))),
    (pcbnew.VECTOR2I(pcbnew.FromMM(bx_max), pcbnew.FromMM(by_min)), pcbnew.VECTOR2I(pcbnew.FromMM(bx_max), pcbnew.FromMM(by_max))),
    (pcbnew.VECTOR2I(pcbnew.FromMM(bx_max), pcbnew.FromMM(by_max)), pcbnew.VECTOR2I(pcbnew.FromMM(bx_min), pcbnew.FromMM(by_max))),
    (pcbnew.VECTOR2I(pcbnew.FromMM(bx_min), pcbnew.FromMM(by_max)), pcbnew.VECTOR2I(pcbnew.FromMM(bx_min), pcbnew.FromMM(by_min)))
]
for p_s, p_e in corners:
    seg = pcbnew.PCB_SHAPE(board)
    seg.SetShape(pcbnew.SHAPE_T_SEGMENT)
    seg.SetStart(p_s)
    seg.SetEnd(p_e)
    seg.SetLayer(pcbnew.Edge_Cuts)
    seg.SetWidth(pcbnew.FromMM(0.15))
    board.Add(seg)

# Remove old mounting vias and place at new 4 corners
vias = [t for t in board.GetTracks() if t.GetClass() == "PCB_VIA" and t.GetDrill() > pcbnew.FromMM(2.0)]
for v in vias:
    board.Remove(v)

m3_corners = [
    (50.0, 50.0),
    (166.0, 50.0),
    (50.0, 136.0),
    (166.0, 136.0)
]
for mx, my in m3_corners:
    via = pcbnew.PCB_VIA(board)
    via.SetPosition(pcbnew.VECTOR2I(pcbnew.FromMM(mx), pcbnew.FromMM(my)))
    via.SetViaType(pcbnew.VIATYPE_THROUGH)
    via.SetDrill(pcbnew.FromMM(3.2))
    via.SetWidth(pcbnew.FromMM(6.0))
    board.Add(via)

# 5. Add Crisp Centered Silkscreen Text
def add_silk(text, x_mm, y_mm, size_mm=1.3, thickness_mm=0.2):
    txt = pcbnew.PCB_TEXT(board)
    txt.SetText(text)
    txt.SetPosition(pcbnew.VECTOR2I(pcbnew.FromMM(x_mm), pcbnew.FromMM(y_mm)))
    txt.SetLayer(pcbnew.F_SilkS)
    txt.SetTextSize(pcbnew.VECTOR2I(pcbnew.FromMM(size_mm), pcbnew.FromMM(size_mm)))
    txt.SetTextThickness(pcbnew.FromMM(thickness_mm))
    txt.SetHorizJustify(pcbnew.GR_TEXT_H_ALIGN_CENTER)
    board.Add(txt)

board_center_x = (bx_min + bx_max) / 2.0  # 108.0 mm
add_silk("HPVE CONTROLLER MOTHERBOARD REV 1.0", board_center_x, 48.5, size_mm=1.3, thickness_mm=0.2)
add_silk("IRON CAVE INDUSTRIES | HACK CLUB FALLEN 2026", board_center_x, 51.0, size_mm=1.0, thickness_mm=0.16)
add_silk("64-CHANNEL 40kHz PHASED ULTRASONIC EMITTER", board_center_x, 137.5, size_mm=1.1, thickness_mm=0.18)

# 6. Dual-Layer GND Copper Pour Zones
old_zones = list(board.Zones())
for z in old_zones:
    board.Remove(z)

for layer in [pcbnew.F_Cu, pcbnew.B_Cu]:
    zone = pcbnew.ZONE(board)
    zone.SetLayer(layer)
    zone.SetNet(net_gnd)
    zone.SetZoneName(f"GND_POUR_{board.GetLayerName(layer)}")
    poly = pcbnew.SHAPE_LINE_CHAIN()
    poly.Append(pcbnew.FromMM(bx_min + 1.0), pcbnew.FromMM(by_min + 1.0))
    poly.Append(pcbnew.FromMM(bx_max - 1.0), pcbnew.FromMM(by_min + 1.0))
    poly.Append(pcbnew.FromMM(bx_max - 1.0), pcbnew.FromMM(by_max - 1.0))
    poly.Append(pcbnew.FromMM(bx_min + 1.0), pcbnew.FromMM(by_max - 1.0))
    poly.SetClosed(True)
    zone.AddPolygon(poly)
    board.Add(zone)

filler = pcbnew.ZONE_FILLER(board)
filler.Fill(board.Zones())
print("Filled dual-layer GND copper zones.")

board.Save(pcb_path)
print("Saved upgraded PCB layout with real Pico module and flush transducer pads!")
