"""
HPVE KiCad 10 Automated PCB Router
Connects all 64 DRV channels, 64 SIG channels, SPI cascade bus, and power rails
directly into HPVE_Controller.kicad_pcb using the native KiCad pcbnew Python API.
"""

import sys
import os

try:
    import pcbnew
except ImportError:
    print("Error: pcbnew module not found. Run with KiCad's bundled Python.")
    sys.exit(1)

def route_two_point_trace(board, p1, p2, net, layer=pcbnew.F_Cu, width_mm=0.35):
    """Adds a clean orthogonal 2-segment PCB trace between two points."""
    x1, y1 = p1.x, p1.y
    x2, y2 = p2.x, p2.y
    width = pcbnew.FromMM(width_mm)

    # If already aligned horizontally or vertically, single segment
    if x1 == x2 or y1 == y2:
        seg = pcbnew.PCB_TRACK(board)
        seg.SetStart(p1)
        seg.SetEnd(p2)
        seg.SetWidth(width)
        seg.SetLayer(layer)
        seg.SetNet(net)
        board.Add(seg)
        return

    # 2-segment dogleg routing: (x1, y1) -> (x2, y1) -> (x2, y2)
    corner = pcbnew.VECTOR2I(x2, y1)

    seg1 = pcbnew.PCB_TRACK(board)
    seg1.SetStart(p1)
    seg1.SetEnd(corner)
    seg1.SetWidth(width)
    seg1.SetLayer(layer)
    seg1.SetNet(net)
    board.Add(seg1)

    seg2 = pcbnew.PCB_TRACK(board)
    seg2.SetStart(corner)
    seg2.SetEnd(p2)
    seg2.SetWidth(width)
    seg2.SetLayer(layer)
    seg2.SetNet(net)
    board.Add(seg2)

def get_or_create_net(board, net_name):
    """Retrieves an existing net or creates a new one in the board's NetInfo."""
    net = board.FindNet(net_name)
    if net is None or net.GetNetname() != net_name:
        net = pcbnew.NETINFO_ITEM(board, net_name)
        board.Add(net)
    return net

def main():
    pcb_path = "hardware/kicad/HPVE_Controller.kicad_pcb"
    print(f"Loading PCB: {pcb_path}")
    board = pcbnew.LoadBoard(pcb_path)

    # 1. Clear any existing loose tracks
    tracks = board.GetTracks()
    print(f"Existing tracks before routing: {len(tracks)}")
    for t in list(tracks):
        board.Remove(t)

    # Net setup
    net_gnd = get_or_create_net(board, "GND")
    net_12v = get_or_create_net(board, "+12V")
    net_3v3 = get_or_create_net(board, "+3V3")
    net_sck = get_or_create_net(board, "SPI_SCK")
    net_lat = get_or_create_net(board, "SPI_LATCH")
    net_mosi = get_or_create_net(board, "SPI_MOSI")

    print("\n--- Routing 64 DRV Channels (Driver -> Transducer) ---")
    drv_routed = 0
    for d in range(1, 33):
        drv_ref = f"U{100 + d}"
        u_drv = board.FindFootprintByReference(drv_ref)
        if not u_drv:
            continue

        ch_a = (d - 1) * 2 + 1
        ch_b = (d - 1) * 2 + 2

        # Channel A: Driver OUTA (pad 7) -> Transducer TX{ch_a} (pad 1)
        tx_a = board.FindFootprintByReference(f"TX{ch_a}")
        if tx_a:
            p_drv_outa = u_drv.FindPadByNumber("7")
            p_tx_a = tx_a.FindPadByNumber("1")
            net_drv_a = get_or_create_net(board, f"DRV_{ch_a}")
            p_drv_outa.SetNet(net_drv_a)
            p_tx_a.SetNet(net_drv_a)

            route_two_point_trace(board, p_drv_outa.GetPosition(), p_tx_a.GetPosition(), net_drv_a, layer=pcbnew.F_Cu, width_mm=0.35)
            drv_routed += 1

        # Channel B: Driver OUTB (pad 5) -> Transducer TX{ch_b} (pad 1)
        tx_b = board.FindFootprintByReference(f"TX{ch_b}")
        if tx_b:
            p_drv_outb = u_drv.FindPadByNumber("5")
            p_tx_b = tx_b.FindPadByNumber("1")
            net_drv_b = get_or_create_net(board, f"DRV_{ch_b}")
            p_drv_outb.SetNet(net_drv_b)
            p_tx_b.SetNet(net_drv_b)

            route_two_point_trace(board, p_drv_outb.GetPosition(), p_tx_b.GetPosition(), net_drv_b, layer=pcbnew.F_Cu, width_mm=0.35)
            drv_routed += 1

    print(f"Successfully routed {drv_routed} DRV tracks.")

    print("\n--- Routing 64 SIG Channels (Shift Register -> Driver) ---")
    # Shift register output pin mapping: QA..QH
    sr_out_pins = ["15", "1", "2", "3", "4", "5", "6", "7"]
    sig_routed = 0

    for ch in range(1, 65):
        sr_idx = (ch - 1) // 8 + 1
        sr_pin_idx = (ch - 1) % 8
        sr_ref = f"U{sr_idx}"
        sr_pin = sr_out_pins[sr_pin_idx]

        drv_idx = (ch - 1) // 2 + 1
        is_ch_b = ((ch - 1) % 2) == 1
        drv_ref = f"U{100 + drv_idx}"
        drv_pin = "4" if is_ch_b else "2"  # INB or INA

        u_sr = board.FindFootprintByReference(sr_ref)
        u_drv = board.FindFootprintByReference(drv_ref)

        if u_sr and u_drv:
            p_sr = u_sr.FindPadByNumber(sr_pin)
            p_drv = u_drv.FindPadByNumber(drv_pin)
            net_sig = get_or_create_net(board, f"SIG_{ch}")
            p_sr.SetNet(net_sig)
            p_drv.SetNet(net_sig)

            route_two_point_trace(board, p_sr.GetPosition(), p_drv.GetPosition(), net_sig, layer=pcbnew.B_Cu, width_mm=0.25)
            sig_routed += 1

    print(f"Successfully routed {sig_routed} SIG tracks.")

    print("\n--- Routing SPI Control Bus & Daisy Chain ---")
    # Daisy chain U1..U8: QH' (pin 9) to next SER (pin 14)
    for i in range(1, 8):
        u_curr = board.FindFootprintByReference(f"U{i}")
        u_next = board.FindFootprintByReference(f"U{i+1}")
        if u_curr and u_next:
            p_out = u_curr.FindPadByNumber("9")
            p_in = u_next.FindPadByNumber("14")
            net_cas = get_or_create_net(board, f"CASCADE_{i}")
            p_out.SetNet(net_cas)
            p_in.SetNet(net_cas)
            route_two_point_trace(board, p_out.GetPosition(), p_in.GetPosition(), net_cas, layer=pcbnew.F_Cu, width_mm=0.25)

    # Bus lines: SCK & LATCH across U1..U8
    for i in range(1, 8):
        u1 = board.FindFootprintByReference(f"U{i}")
        u2 = board.FindFootprintByReference(f"U{i+1}")
        if u1 and u2:
            # SCK (pin 11)
            p1_sck = u1.FindPadByNumber("11")
            p2_sck = u2.FindPadByNumber("11")
            p1_sck.SetNet(net_sck)
            p2_sck.SetNet(net_sck)
            route_two_point_trace(board, p1_sck.GetPosition(), p2_sck.GetPosition(), net_sck, layer=pcbnew.F_Cu, width_mm=0.25)

            # LATCH (pin 12)
            p1_lat = u1.FindPadByNumber("12")
            p2_lat = u2.FindPadByNumber("12")
            p1_lat.SetNet(net_lat)
            p2_lat.SetNet(net_lat)
            route_two_point_trace(board, p1_lat.GetPosition(), p2_lat.GetPosition(), net_lat, layer=pcbnew.F_Cu, width_mm=0.25)

    # Connect Pico U10 to Shift Register U1
    u_pico = board.FindFootprintByReference("U10")
    u_sr1 = board.FindFootprintByReference("U1")
    if u_pico and u_sr1:
        # MOSI (Pico pin 21 -> U1 pin 14)
        p_mosi_src = u_pico.FindPadByNumber("21")
        p_mosi_dst = u_sr1.FindPadByNumber("14")
        p_mosi_src.SetNet(net_mosi)
        p_mosi_dst.SetNet(net_mosi)
        route_two_point_trace(board, p_mosi_src.GetPosition(), p_mosi_dst.GetPosition(), net_mosi, layer=pcbnew.F_Cu, width_mm=0.25)

        # SCK (Pico pin 22 -> U1 pin 11)
        p_sck_src = u_pico.FindPadByNumber("22")
        p_sck_dst = u_sr1.FindPadByNumber("11")
        p_sck_src.SetNet(net_sck)
        route_two_point_trace(board, p_sck_src.GetPosition(), p_sck_dst.GetPosition(), net_sck, layer=pcbnew.F_Cu, width_mm=0.25)

        # LATCH (Pico pin 24 -> U1 pin 12)
        p_lat_src = u_pico.FindPadByNumber("24")
        p_lat_dst = u_sr1.FindPadByNumber("12")
        p_lat_src.SetNet(net_lat)
        p_lat_dst.SetNet(net_lat)
        route_two_point_trace(board, p_lat_src.GetPosition(), p_lat_dst.GetPosition(), net_lat, layer=pcbnew.F_Cu, width_mm=0.25)

    print("SPI bus & daisy-chain successfully routed.")

    print("\n--- Routing +12V Power Bus & GND Return Lines ---")
    # +12V daisy chain across U101..U132 Pin 6
    for d in range(1, 32):
        u1 = board.FindFootprintByReference(f"U{100 + d}")
        u2 = board.FindFootprintByReference(f"U{100 + d + 1}")
        if u1 and u2:
            p1_12v = u1.FindPadByNumber("6")
            p2_12v = u2.FindPadByNumber("6")
            p1_12v.SetNet(net_12v)
            p2_12v.SetNet(net_12v)
            route_two_point_trace(board, p1_12v.GetPosition(), p2_12v.GetPosition(), net_12v, layer=pcbnew.F_Cu, width_mm=0.6)

    # GND daisy chain across transducers TX1..TX64 Pin 2
    for t in range(1, 64):
        tx1 = board.FindFootprintByReference(f"TX{t}")
        tx2 = board.FindFootprintByReference(f"TX{t + 1}")
        if tx1 and tx2:
            p1_gnd = tx1.FindPadByNumber("2")
            p2_gnd = tx2.FindPadByNumber("2")
            p1_gnd.SetNet(net_gnd)
            p2_gnd.SetNet(net_gnd)
            # Route on back copper (ground return path)
            route_two_point_trace(board, p1_gnd.GetPosition(), p2_gnd.GetPosition(), net_gnd, layer=pcbnew.B_Cu, width_mm=0.5)

    print(f"\nFinal Track Count: {len(board.GetTracks())}")
    board.Save(pcb_path)
    print(f"Updated and saved board successfully to {pcb_path}!")

if __name__ == "__main__":
    main()
