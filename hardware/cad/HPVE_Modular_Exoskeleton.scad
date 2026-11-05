// ============================================================================
// IRON CAVE INDUSTRIES // DEFENSE & SPATIAL SYSTEMS DIVISION
// PROJECT: HPVE (Haptic Photonic Volumetric Engine) - Task 7.1 (MK-II V3)
// PART:    4-Tile Interlocking Snap-Lock Exoskeleton, Exponential Horn Bezel,
//          RP2040 Master Avionics Dock (USB-C + BOOTSEL + OLED + XT30 + FFC),
//          6-Pin FFC Daisy-Chain Ribbon Routing & Overhead Halo Gimbal Mounts
// STYLE:   TVA Retro-Futurism / Anduril Skunkworks (Matched to Hero Render)
// CAD:     Native Parametric OpenSCAD Solid Modeling Architecture
// ============================================================================

$fn = 64;

// ----------------------------------------------------------------------------
// 1. RENDER MODE SELECTOR (Overridden via CLI -D "render_mode=...")
//    0 = Hero Isometric View — 2x2 Quad Array (Emitter Face + Side Louvers)
//    1 = Top Orthographic View — 64-Channel 8x8 Matrix & Zero-Gap 196x176mm Grid
//    2 = Internal Cutaway View — Front-Left & Back-Left Tiles Open showing
//        exact PCB seat, IC clearance bays, and 6-pin FFC daisy-chain ribbons!
//    3 = Rear Avionics & Halo Gimbal Mount Iso View — Shows RP2040 Pico Master
//        Dock, external USB-C programming port, XT30 12V input, OLED HUD,
//        4-branch FFC & 12V star harness, and Pan-Tilt Gimbal Yokes!
//    4 = Rear Orthographic View — Full Avionics Backplane & FFC Harness Map
//    5 = Front-Left Side Elevation — Shows External USB-C Programming Port,
//        BOOTSEL button, XT30 +12V_DRV Power Input, and Anodized Louver Fins
//    6 = Single Tile 5-Layer Exploded Engineering View
//    7 = Full 2x2 Quad Array + RP2040 Master Avionics Dock Exploded View
//    8 = Macro Close-Up — Exponential Acoustic Horn Waveguides & Honeycomb Mesh
//    9 = Printable STL Part A: Single Tile Exoskeleton Base Frame
//   10 = Printable STL Part B: Contoured 16-Ch Exponential Horn Waveguide Bezel
//   11 = Printable STL Part C: 2x2 Quad Exoskeleton + Avionics Dock Frame
// ----------------------------------------------------------------------------
render_mode = 0;

// ----------------------------------------------------------------------------
// 2. EXACT HARDWARE CONSTANTS FROM HPVE_Tile_16Ch.kicad_pcb
// ----------------------------------------------------------------------------
pcb_w               = 98.00;
pcb_h               = 88.00;
pcb_thick           = 1.60;
pcb_chamfer         = 3.00;
pcb_clearance       = 0.25;

// M3 Mounting Hole Coordinates on PCB (H1..H4, 3.2mm drill)
m3_inset            = 4.50;
m3_drill_d          = 3.20;
m3_insert_od        = 4.60;   // M3x4x4.6mm brass heat-set threaded insert
m3_boss_od          = 7.80;

// 16x Prowave 400ST160 Transducer Array Coordinates on PCB
tx_grid_x0          = 29.50;
tx_grid_y0          = 24.00;
tx_pitch            = 16.50;
tx_can_od           = 16.20;
tx_can_clear_d      = 16.50;
tx_can_h            = 12.00;
tx_mesh_d           = 12.60;
tx_lead_spacing     = 10.00;

// FFC & Power Connector Coordinates on PCB
ffc_in_x            = 4.80;
ffc_out_x           = 93.20;
ffc_y               = 36.00;
ffc_slot_w          = 12.50;
ffc_slot_h          = 3.80;

pwr_hdr_x           = 4.80;
pwr_hdr_y           = 10.00;

// ----------------------------------------------------------------------------
// 3. EXOSKELETON STRUCTURAL PARAMETERS (ZERO-GAP 196.0 x 176.0 MM PITCH)
// ----------------------------------------------------------------------------
tile_pitch_x        = 98.00;
tile_pitch_y        = 88.00;

frame_total_h       = 23.00;
base_floor_h        = 3.40;
pcb_ledge_z         = 5.80;   // 2.4mm under-PCB solder lead & wire bay + 3.4mm floor
pcb_top_z           = pcb_ledge_z + pcb_thick; // 7.40 mm
tx_top_z            = pcb_top_z + tx_can_h;    // 19.40 mm
bezel_thick         = 5.60;
bezel_bottom_z      = frame_total_h - bezel_thick; // 17.40 mm (clamps top 2.0mm of cans)

// Bezel Envelope (Symmetric margins around the 4x4 transducer grid)
bezel_x0            = 16.75;
bezel_w             = 75.00;  // X = 16.75 .. 91.75
bezel_y0            = 11.25;
bezel_h             = 75.00;  // Y = 11.25 .. 86.25

// Inter-Tile Dovetail Key Parameters
dovetail_depth      = 3.20;
dovetail_neck_w     = 8.40;
dovetail_head_w     = 11.80;
dovetail_h          = 13.50;
dovetail_z0         = 3.20;
dovetail_tol        = 0.12;

// ----------------------------------------------------------------------------
// 4. TVA RETRO-FUTURISM / ANDURIL COLOR PALETTE (Matched to Hero Render)
// ----------------------------------------------------------------------------
c_tva_cream         = [0.91, 0.88, 0.81];   // Warm NASA/TVA Ceramic Cream (#E8E0CE)
c_tva_bone          = [0.89, 0.86, 0.78];   // Contoured Bezel Cream (#E3DBCA)
c_tva_shadow        = [0.76, 0.72, 0.64];   // Recessed Panel Groove Shadow
c_tva_orange        = [0.92, 0.35, 0.08];   // TVA Burnt Signal Orange (#EB5914)
c_anduril_slate     = [0.12, 0.14, 0.16];   // Anodized Matte Tactical Slate (#1F2429)
c_carbon_bay        = [0.08, 0.09, 0.11];   // Deep Heatsink Cavity Black
c_bronze_bolt       = [0.76, 0.58, 0.32];   // Brushed Phosphor Bronze (#C29452)
c_pcb_matte         = [0.07, 0.18, 0.13];   // 4-Layer Matte Emerald-Black Solder Mask
c_pico_green        = [0.05, 0.38, 0.22];   // Raspberry Pi Pico Soldermask Green
c_alu_can           = [0.84, 0.86, 0.89];   // Brushed Aluminum 400ST160 Can
c_mesh_silver       = [0.72, 0.75, 0.78];   // Perforated Stainless Mesh Plate
c_mesh_hole         = [0.14, 0.16, 0.18];   // Dark Acoustic Cavity Behind Mesh
c_gold_pad          = [0.90, 0.74, 0.25];   // ENIG Gold Copper Pads / Kapton FFC
c_ffc_kapton        = [0.86, 0.54, 0.12];   // Polyimide Kapton 0.5mm FFC Ribbon
c_xt30_yellow       = [0.95, 0.78, 0.10];   // Amass XT30 High-Current Connector
c_copper_wire       = [0.85, 0.18, 0.14];   // 16AWG Silicone +12V_DRV Wire
c_gnd_wire          = [0.18, 0.20, 0.22];   // 16AWG Silicone GND Wire
c_oled_cyan         = [0.00, 0.88, 0.98];   // Active SSD1306 OLED Telemetry Pixel Glow

// ----------------------------------------------------------------------------
// 5. GEOMETRIC PRIMITIVES & ASYMMETRIC QUAD-CORNER CHAMFER HELPERS
// ----------------------------------------------------------------------------
module asymmetric_quad_polygon_2d(w, h, c_sw, c_se, c_ne, c_nw) {
    polygon(points=[
        [c_sw, 0],
        [w - c_se, 0],
        [w, c_se],
        [w, h - c_ne],
        [w - c_ne, h],
        [c_nw, h],
        [0, h - c_nw],
        [0, c_sw]
    ]);
}

module asymmetric_quad_prism(w, h, z_h, c_sw, c_se, c_ne, c_nw) {
    linear_extrude(height = z_h, convexity = 6)
        asymmetric_quad_polygon_2d(w, h, c_sw, c_se, c_ne, c_nw);
}

module chamfered_rect_2d(w, h, c) {
    asymmetric_quad_polygon_2d(w, h, c, c, c, c);
}

module chamfered_prism(w, h, z_h, c) {
    linear_extrude(height = z_h, convexity = 6)
        chamfered_rect_2d(w, h, c);
}

module dovetail_2d(neck_w, head_w, depth, tol=0.0) {
    nw = neck_w - 2 * tol;
    hw = head_w - 2 * tol;
    dp = depth - tol;
    polygon(points=[
        [-nw/2, 0],
        [ nw/2, 0],
        [ hw/2, dp],
        [-hw/2, dp]
    ]);
}

// ----------------------------------------------------------------------------
// 6. ANODIZED SIDE COOLING LOUVER WINDOWS (MATCHED TO HERO RENDER)
// ----------------------------------------------------------------------------
module stadium_pocket_x(y0, len_y, z0, h_z, depth_x) {
    r = h_z / 2;
    hull() {
        translate([-0.5, y0 + r, z0 + r])
            rotate([0, 90, 0])
                cylinder(r = r, h = depth_x + 0.5);
        translate([-0.5, y0 + len_y - r, z0 + r])
            rotate([0, 90, 0])
                cylinder(r = r, h = depth_x + 0.5);
    }
}

module stadium_louver_insert_x(x_pos, y0, len_y, z0, h_z, is_right=false) {
    r = (h_z - 1.2) / 2;
    x_base = is_right ? (x_pos - 2.2) : x_pos;
    color(c_carbon_bay)
    hull() {
        translate([x_base + (is_right ? 0.0 : 1.2), y0 + r + 0.6, z0 + h_z/2])
            rotate([0, 90, 0])
                cylinder(r = r, h = 1.0);
        translate([x_base + (is_right ? 0.0 : 1.2), y0 + len_y - r - 0.6, z0 + h_z/2])
            rotate([0, 90, 0])
                cylinder(r = r, h = 1.0);
    }
    color(c_anduril_slate)
    intersection() {
        hull() {
            translate([x_base, y0 + r + 0.6, z0 + h_z/2])
                rotate([0, 90, 0])
                    cylinder(r = r, h = 2.2);
            translate([x_base, y0 + len_y - r - 0.6, z0 + h_z/2])
                rotate([0, 90, 0])
                    cylinder(r = r, h = 2.2);
        }
        union() {
            difference() {
                translate([x_base, y0, z0])
                    cube([2.2, len_y, h_z]);
                translate([x_base - 0.5, y0 + 2.2, z0 + 1.8])
                    cube([3.2, len_y - 4.4, h_z - 3.6]);
            }
            for (fy = [y0 + 3.2 : 2.6 : y0 + len_y - 3.2]) {
                translate([x_base + 0.2, fy - 0.55, z0 + 1.2])
                    cube([1.8, 1.1, h_z - 2.4]);
            }
        }
    }
}

module stadium_pocket_y(x0, len_x, z0, h_z, depth_y) {
    r = h_z / 2;
    hull() {
        translate([x0 + r, -0.5, z0 + r])
            rotate([-90, 0, 0])
                cylinder(r = r, h = depth_y + 0.5);
        translate([x0 + len_x - r, -0.5, z0 + r])
            rotate([-90, 0, 0])
                cylinder(r = r, h = depth_y + 0.5);
    }
}

module stadium_louver_insert_y(y_pos, x0, len_x, z0, h_z, is_back=false) {
    r = (h_z - 1.2) / 2;
    y_base = is_back ? (y_pos - 2.2) : y_pos;
    color(c_carbon_bay)
    hull() {
        translate([x0 + r + 0.6, y_base + (is_back ? 0.0 : 1.2), z0 + h_z/2])
            rotate([-90, 0, 0])
                cylinder(r = r, h = 1.0);
        translate([x0 + len_x - r - 0.6, y_base + (is_back ? 0.0 : 1.2), z0 + h_z/2])
            rotate([-90, 0, 0])
                cylinder(r = r, h = 1.0);
    }
    color(c_anduril_slate)
    intersection() {
        hull() {
            translate([x0 + r + 0.6, y_base, z0 + h_z/2])
                rotate([-90, 0, 0])
                    cylinder(r = r, h = 2.2);
            translate([x0 + len_x - r - 0.6, y_base, z0 + h_z/2])
                rotate([-90, 0, 0])
                    cylinder(r = r, h = 2.2);
        }
        union() {
            difference() {
                translate([x0, y_base, z0])
                    cube([len_x, 2.2, h_z]);
                translate([x0 + 2.2, y_base - 0.5, z0 + 1.8])
                    cube([len_x - 4.4, 3.2, h_z - 3.6]);
            }
            for (fx = [x0 + 3.2 : 2.6 : x0 + len_x - 3.2]) {
                translate([fx - 0.55, y_base + 0.2, z0 + 1.2])
                    cube([1.1, 1.8, h_z - 2.4]);
            }
        }
    }
}

// ----------------------------------------------------------------------------
// 7. MODULAR TILE EXOSKELETON BASE CARRIER FRAME
//    Includes:
//    - Internal 98x88mm PCB Seat + 16x Transducer Centering Cradles
//    - Dedicated IC Thermal Bays for U101..U108 (TC4427) & U1..U2 (74HC595)
//    - Inter-Tile FFC Ribbon Tunnels across X=98mm and Y=88mm Mating Walls
//    - Master Feed FFC Vertical Slot into Tile 0's J_IN (4.8, 36.0) from the
//      RP2040 Master Avionics Dock on the rear backplane!
//    - U-Turn FFC Vertical Slots at Tile 1 J_OUT & Tile 2 J_IN so the 4 tiles
//      daisy-chain cleanly (Pico -> Tile 0 -> Tile 1 -> Tile 2 -> Tile 3)!
// ----------------------------------------------------------------------------
module tile_exoskeleton_base_frame(qx=0, qy=0, include_color=true) {
    is_left_outer   = (qx == 0);
    is_right_outer  = (qx == 1);
    is_front_outer  = (qy == 0);
    is_back_outer   = (qy == 1);

    c_sw = (is_left_outer  && is_front_outer) ? 7.0 : 1.0;
    c_se = (is_right_outer && is_front_outer) ? 7.0 : 1.0;
    c_ne = (is_right_outer && is_back_outer)  ? 7.0 : 1.0;
    c_nw = (is_left_outer  && is_back_outer)  ? 7.0 : 1.0;

    m3_coords = [
        [m3_inset, m3_inset],
        [pcb_w - m3_inset, m3_inset],
        [m3_inset, pcb_h - m3_inset],
        [pcb_w - m3_inset, pcb_h - m3_inset]
    ];

    BezelScrewCoords = [
        [20.25, 14.50], [54.25, 14.50], [88.25, 14.50],
        [20.25, 83.00], [54.25, 83.00], [88.25, 83.00],
        [20.25, 48.75], [88.25, 48.75]
    ];

    outer_bolt_x = is_left_outer  ? 6.20 : (pcb_w - 6.20);
    outer_bolt_y = is_front_outer ? 6.20 : (pcb_h - 6.20);

    union() {
        color(include_color ? c_tva_cream : [0.88, 0.88, 0.88])
        difference() {
            union() {
                // 1. Main Tile Structural Monocoque (98.0 x 88.0 x 20.6 mm)
                asymmetric_quad_prism(tile_pitch_x, tile_pitch_y, frame_total_h - 2.4, c_sw, c_se, c_ne, c_nw);

                // 2. Inter-Tile Sliding Dovetail Tenons ONLY on Internal Mating Interfaces
                if (!is_right_outer) {
                    for (dy = [24.0, 64.0]) {
                        translate([tile_pitch_x, dy, dovetail_z0])
                            linear_extrude(height = dovetail_h, convexity = 4)
                                rotate([0, 0, -90])
                                    dovetail_2d(dovetail_neck_w, dovetail_head_w, dovetail_depth, dovetail_tol);
                    }
                }
                if (!is_back_outer) {
                    for (dx = [29.0, 69.0]) {
                        translate([dx, tile_pitch_y, dovetail_z0])
                            linear_extrude(height = dovetail_h, convexity = 4)
                                dovetail_2d(dovetail_neck_w, dovetail_head_w, dovetail_depth, dovetail_tol);
                    }
                }

                // 3. Raised Inter-Tile Top Alignment Latch Bridges across the Inner Cross Seam
                if (!is_right_outer) {
                    for (ly = [30.0, 67.5]) {
                        translate([tile_pitch_x - 2.2, ly - 5.5, frame_total_h - 1.2])
                            chamfered_prism(4.4, 11.0, 1.4, 0.8);
                    }
                }
                if (!is_back_outer) {
                    for (lx = [38.0, 70.5]) {
                        translate([lx - 5.5, tile_pitch_y - 2.2, frame_total_h - 1.2])
                            chamfered_prism(11.0, 4.4, 1.4, 0.8);
                    }
                }
            }

            // --- SUBTRACTIVE INTERNAL MACHINING & CAVITIES ---

            // A. Negative Dovetail Mortises on Internal Mating Faces
            if (!is_left_outer) {
                for (dy = [24.0, 64.0]) {
                    translate([-0.05, dy, dovetail_z0 - 0.2])
                        linear_extrude(height = dovetail_h + 0.4, convexity = 4)
                            rotate([0, 0, -90])
                                dovetail_2d(dovetail_neck_w, dovetail_head_w, dovetail_depth + 0.15, -dovetail_tol);
                }
            }
            if (!is_front_outer) {
                for (dx = [29.0, 69.0]) {
                    translate([dx, -0.05, dovetail_z0 - 0.2])
                        linear_extrude(height = dovetail_h + 0.4, convexity = 4)
                            dovetail_2d(dovetail_neck_w, dovetail_head_w, dovetail_depth + 0.15, -dovetail_tol);
                }
            }

            // B. Visible Seam Dovetail Joint Notches at Outer Ends of the Cross Seam
            if (!is_right_outer) {
                translate([tile_pitch_x - 1.2, 0.0, 4.5])
                    cube([2.4, 3.5, 11.0]);
            }
            if (!is_left_outer) {
                translate([-1.2, 0.0, 4.5])
                    cube([2.4, 3.5, 11.0]);
            }

            // C. Upper Bezel Seat Pocket
            translate([bezel_x0 - 0.25, bezel_y0 - 0.25, bezel_bottom_z])
                chamfered_prism(bezel_w + 0.50, bezel_h + 0.50, bezel_thick + 2.0, 3.4);

            // D. Internal PCB Slip-In Pocket & Perimeter Shelf Seat
            translate([1.4, 1.4, pcb_ledge_z])
                chamfered_prism(pcb_w - 2.8, pcb_h - 2.8, pcb_thick + 0.4, 2.5);

            // E. Internal Upper Component & Transducer Chambers
            //    1) 16-Channel Transducer Matrix Chamber
            translate([20.2, 14.7, pcb_top_z - 0.1])
                chamfered_prism(68.2, 68.2, bezel_bottom_z - pcb_top_z + 0.5, 3.0);

            //    2) Dedicated IC Thermal & Routing Bay over U101..U108 (TC4427 Gate Drivers at X=13.0)
            translate([3.2, 6.5, pcb_top_z - 0.1])
                chamfered_prism(18.5, 72.0, 7.5, 2.0);

            //    3) Dedicated IC Bay over U1 & U2 (74HC595 Shift Registers at Y=9.5) + R1..R3
            translate([12.0, 4.2, pcb_top_z - 0.1])
                chamfered_prism(68.0, 11.5, 6.5, 2.0);

            //    4) FFC Connector & Ribbon Bays for J_IN (4.8, 36.0) and J_OUT (93.2, 36.0)
            translate([2.6, ffc_y - 7.5, pcb_ledge_z - 1.5])
                cube([8.6, 15.0, 7.5]);
            translate([pcb_w - 11.2, ffc_y - 7.5, pcb_ledge_z - 1.5])
                cube([8.6, 15.0, 7.5]);

            //    5) Internal Inter-Tile FFC Ribbon Pass-Through Slot across the X=98 / X=0 Mating Wall
            //       (Connects J_OUT of Left Tile directly into J_IN of Right Tile!)
            if (!is_right_outer) {
                translate([tile_pitch_x - 4.5, ffc_y - ffc_slot_w/2, pcb_ledge_z - 1.2])
                    cube([9.0, ffc_slot_w, ffc_slot_h + 2.5]);
            }
            if (!is_left_outer) {
                translate([-4.5, ffc_y - ffc_slot_w/2, pcb_ledge_z - 1.2])
                    cube([9.0, ffc_slot_w, ffc_slot_h + 2.5]);
            }

            //    6) Vertical Bottom-Entry FFC Slots through Base Floor (Z = -0.5 .. pcb_ledge_z + 2.0)
            //       Allows the RP2040 Master Dock on the rear backplane to plug its 6-pin FFC ribbon
            //       directly up into Tile 0's J_IN (4.8, 36.0), and allows the Tile 1 J_OUT -> Tile 2 J_IN
            //       cascade ribbon to route cleanly through the protected rear spine channel!
            translate([2.4, ffc_y - 5.5, -0.5])
                cube([5.2, 11.0, pcb_ledge_z + 2.5]);
            translate([pcb_w - 7.6, ffc_y - 5.5, -0.5])
                cube([5.2, 11.0, pcb_ledge_z + 2.5]);

            // F. Under-PCB Solder-Pin Clearance Cavities (2.4mm deep below PCB bottom face)
            for (r = [0 : 3]) {
                for (c = [0 : 3]) {
                    tx_x = tx_grid_x0 + c * tx_pitch;
                    tx_y = tx_grid_y0 + r * tx_pitch;
                    hull() {
                        translate([tx_x - 5.5, tx_y, base_floor_h])
                            cylinder(d = 4.2, h = pcb_ledge_z - base_floor_h + 0.5);
                        translate([tx_x + 5.5, tx_y, base_floor_h])
                            cylinder(d = 4.2, h = pcb_ledge_z - base_floor_h + 0.5);
                    }
                }
            }

            // G. High-Current +12V_DRV 16AWG Star-Power Entry Port & Raceway (at J_PWR = (4.8, 10.0))
            translate([pwr_hdr_x - 3.2, pwr_hdr_y - 3.5, -0.5])
                cube([6.4, 7.0, pcb_ledge_z + 2.0]);
            translate([pwr_hdr_x - 3.0, 7.0, 1.4])
                cube([6.0, 32.0, pcb_ledge_z - 1.0]);

            // H. Outer Stadium Louver Pockets (2.2mm deep so they never breach internal bays)
            if (is_left_outer) {
                stadium_pocket_x(18.0, 52.0, 4.2, 11.2, 2.2);
            }
            if (is_right_outer) {
                translate([tile_pitch_x - 1.7, 0, 0])
                    stadium_pocket_x(18.0, 52.0, 4.2, 11.2, 2.2);
            }
            if (is_front_outer) {
                stadium_pocket_y(21.0, 56.0, 4.2, 11.2, 2.2);
            }
            if (is_back_outer) {
                translate([0, tile_pitch_y - 1.7, 0])
                    stadium_pocket_y(21.0, 56.0, 4.2, 11.2, 2.2);
            }

            // I. Underbody / Rear Instrumentation Vent Grilles
            for (vx = [25.0, 48.0, 71.0]) {
                for (vy = [24.0, 54.0]) {
                    translate([vx - 8.0, vy - 10.0, -0.1])
                        chamfered_prism(16.0, 20.0, 1.2, 1.5);
                    for (sy = [vy - 7.5 : 3.0 : vy + 7.5]) {
                        translate([vx - 6.5, sy - 0.6, -0.5])
                            cube([13.0, 1.2, base_floor_h + 1.0]);
                    }
                }
            }

            // J. 4x Internal M3 PCB Hold-Down Boss Through-Holes & Bottom Counterbores
            for (pt = m3_coords) {
                translate([pt[0], pt[1], -0.5])
                    cylinder(d = m3_drill_d, h = pcb_ledge_z + 2.0);
                translate([pt[0], pt[1], -0.2])
                    cylinder(d = 6.0, h = 1.8);
            }

            // K. Outer Corner M4 Socket-Head Cap Screw Counterbore
            translate([outer_bolt_x, outer_bolt_y, frame_total_h - 3.2])
                cylinder(d = 6.8, h = 4.0);
            translate([outer_bolt_x, outer_bolt_y, -0.5])
                cylinder(d = 4.0, h = frame_total_h + 1.0);
        }

        // --- ADDITIVE INTERNAL BOSSES & LOUVER FIN INSERTS ---

        // 1. 4x M3 PCB Support Stand-Off Bosses with Brass Heat-Set Insert Pockets
        color(include_color ? c_tva_cream : [0.88, 0.88, 0.88])
        for (pt = m3_coords) {
            difference() {
                translate([pt[0], pt[1], 0])
                    cylinder(d = m3_boss_od, h = pcb_ledge_z);
                translate([pt[0], pt[1], -0.5])
                    cylinder(d = m3_drill_d, h = pcb_ledge_z + 1.0);
                translate([pt[0], pt[1], pcb_ledge_z - 3.6])
                    cylinder(d = m3_insert_od, h = 3.8);
            }
        }

        // 2. 16x Conical Acoustic Transducer Cradle Rings on PCB Top Shelf
        color(include_color ? c_tva_cream : [0.88, 0.88, 0.88])
        for (r = [0 : 3]) {
            for (c = [0 : 3]) {
                tx_x = tx_grid_x0 + c * tx_pitch;
                tx_y = tx_grid_y0 + r * tx_pitch;
                difference() {
                    translate([tx_x, tx_y, pcb_top_z])
                        cylinder(d = tx_can_clear_d + 1.6, h = 2.4);
                    translate([tx_x, tx_y, pcb_top_z - 0.2])
                        cylinder(d = tx_can_clear_d, h = 2.8);
                }
            }
        }

        // 3. 8x Internal Bezel Hold-Down Pillars
        color(include_color ? c_tva_cream : [0.88, 0.88, 0.88])
        for (bpt = BezelScrewCoords) {
            difference() {
                translate([bpt[0], bpt[1], pcb_top_z])
                    cylinder(d = 5.2, h = bezel_bottom_z - pcb_top_z);
                translate([bpt[0], bpt[1], pcb_top_z - 0.5])
                    cylinder(d = 2.5, h = bezel_bottom_z - pcb_top_z + 1.0);
            }
        }

        // 4. Anodized Matte Black Stadium Louver Fin Inserts on Exterior Perimeter Faces
        if (include_color) {
            if (is_left_outer) {
                stadium_louver_insert_x(0.0, 18.0, 52.0, 4.2, 11.2, false);
            }
            if (is_right_outer) {
                stadium_louver_insert_x(tile_pitch_x, 18.0, 52.0, 4.2, 11.2, true);
            }
            if (is_front_outer) {
                stadium_louver_insert_y(0.0, 21.0, 56.0, 4.2, 11.2, false);
            }
            if (is_back_outer) {
                stadium_louver_insert_y(tile_pitch_y, 21.0, 56.0, 4.2, 11.2, true);
            }
        }
    }
}

// ----------------------------------------------------------------------------
// 8. TVA SIGNAL ORANGE CHAMFERED OUTER PERIMETER BEVEL CROWN
// ----------------------------------------------------------------------------
module tva_orange_quadrant_bevel(qx=0, qy=0) {
    is_left_outer   = (qx == 0);
    is_right_outer  = (qx == 1);
    is_front_outer  = (qy == 0);
    is_back_outer   = (qy == 1);

    c_sw = (is_left_outer  && is_front_outer) ? 7.0 : 1.0;
    c_se = (is_right_outer && is_front_outer) ? 7.0 : 1.0;
    c_ne = (is_right_outer && is_back_outer)  ? 7.0 : 1.0;
    c_nw = (is_left_outer  && is_back_outer)  ? 7.0 : 1.0;

    c_sw_in = (is_left_outer  && is_front_outer) ? 5.6 : 1.0;
    c_se_in = (is_right_outer && is_front_outer) ? 5.6 : 1.0;
    c_ne_in = (is_right_outer && is_back_outer)  ? 5.6 : 1.0;
    c_nw_in = (is_left_outer  && is_back_outer)  ? 5.6 : 1.0;

    in_left   = is_left_outer  ? 3.2 : 0.0;
    in_right  = is_right_outer ? 3.2 : 0.0;
    in_front  = is_front_outer ? 3.2 : 0.0;
    in_back   = is_back_outer  ? 3.2 : 0.0;

    outer_bolt_x = is_left_outer  ? 6.20 : (pcb_w - 6.20);
    outer_bolt_y = is_front_outer ? 6.20 : (pcb_h - 6.20);

    // 1. Main 45-Degree Outer Perimeter Chamfer Crown
    color(c_tva_orange)
    difference() {
        hull() {
            translate([0, 0, frame_total_h - 2.4])
                asymmetric_quad_prism(tile_pitch_x, tile_pitch_y, 0.20, c_sw, c_se, c_ne, c_nw);
            translate([in_left, in_front, frame_total_h - 0.10])
                asymmetric_quad_prism(
                    tile_pitch_x - in_left - in_right,
                    tile_pitch_y - in_front - in_back,
                    0.10,
                    c_sw_in, c_se_in, c_ne_in, c_nw_in
                );
        }

        translate([
            is_left_outer  ? 3.25 : -1.0,
            is_front_outer ? 3.25 : -1.0,
            frame_total_h - 3.0
        ])
        asymmetric_quad_prism(
            tile_pitch_x - (is_left_outer ? 3.25 : -1.0) - (is_right_outer ? 3.25 : -1.0),
            tile_pitch_y - (is_front_outer ? 3.25 : -1.0) - (is_back_outer ? 3.25 : -1.0),
            4.0,
            (is_left_outer  && is_front_outer) ? 5.2 : 0.5,
            (is_right_outer && is_front_outer) ? 5.2 : 0.5,
            (is_right_outer && is_back_outer)  ? 5.2 : 0.5,
            (is_left_outer  && is_back_outer)  ? 5.2 : 0.5
        );

        translate([outer_bolt_x, outer_bolt_y, frame_total_h - 3.5])
            cylinder(d = 6.8, h = 5.0);
    }

    // 2. Top-Deck Ceramic Cream Frame Deck
    color(c_tva_cream)
    difference() {
        translate([
            is_left_outer  ? 3.25 : 0.0,
            is_front_outer ? 3.25 : 0.0,
            frame_total_h - 2.4
        ])
        asymmetric_quad_prism(
            tile_pitch_x - (is_left_outer ? 3.25 : 0.0) - (is_right_outer ? 3.25 : 0.0),
            tile_pitch_y - (is_front_outer ? 3.25 : 0.0) - (is_back_outer ? 3.25 : 0.0),
            2.4,
            (is_left_outer  && is_front_outer) ? 5.2 : 1.0,
            (is_right_outer && is_front_outer) ? 5.2 : 1.0,
            (is_right_outer && is_back_outer)  ? 5.2 : 1.0,
            (is_left_outer  && is_back_outer)  ? 5.2 : 1.0
        );

        translate([bezel_x0 - 0.30, bezel_y0 - 0.30, frame_total_h - 3.0])
            chamfered_prism(bezel_w + 0.60, bezel_h + 0.60, 4.0, 3.4);

        translate([outer_bolt_x, outer_bolt_y, frame_total_h - 3.2])
            cylinder(d = 6.6, h = 4.0);
    }

    // 3. Anodized Black M4 Hex-Socket Corner Anchor Bolt
    color(c_anduril_slate)
    difference() {
        translate([outer_bolt_x, outer_bolt_y, frame_total_h - 1.6])
            cylinder(d = 6.0, h = 1.55);
        translate([outer_bolt_x, outer_bolt_y, frame_total_h - 0.9])
            cylinder(d = 3.0, h = 1.2, $fn = 6);
    }
}

// ----------------------------------------------------------------------------
// 9. CONTOURED 16-CHANNEL EXPONENTIAL ACOUSTIC WAVEGUIDE HORN BEZEL
// ----------------------------------------------------------------------------
module acoustic_horn_bezel_plate(include_color=true) {
    BezelScrewCoords = [
        [20.25, 14.50], [54.25, 14.50], [88.25, 14.50],
        [20.25, 83.00], [54.25, 83.00], [88.25, 83.00],
        [20.25, 48.75], [88.25, 48.75]
    ];

    union() {
        if (include_color) {
            color(c_tva_orange)
            difference() {
                translate([bezel_x0 - 0.20, bezel_y0 - 0.20, frame_total_h - 0.85])
                    chamfered_prism(bezel_w + 0.40, bezel_h + 0.40, 0.55, 3.3);
                translate([bezel_x0 + 0.25, bezel_y0 + 0.25, frame_total_h - 1.0])
                    chamfered_prism(bezel_w - 0.50, bezel_h - 0.50, 0.9, 3.0);
            }
        }

        color(include_color ? c_tva_bone : [0.92, 0.92, 0.92])
        difference() {
            translate([bezel_x0, bezel_y0, bezel_bottom_z])
                chamfered_prism(bezel_w, bezel_h, bezel_thick, 3.1);

            for (r = [0 : 3]) {
                for (c = [0 : 3]) {
                    tx_x = tx_grid_x0 + c * tx_pitch;
                    tx_y = tx_grid_y0 + r * tx_pitch;

                    translate([tx_x, tx_y, bezel_bottom_z - 0.1])
                        cylinder(d = tx_can_clear_d, h = 2.15);

                    translate([tx_x, tx_y, bezel_bottom_z + 1.95])
                        cylinder(d1 = 12.20, d2 = 12.85, h = 0.80);
                    translate([tx_x, tx_y, bezel_bottom_z + 2.70])
                        cylinder(d1 = 12.85, d2 = 13.65, h = 0.85);
                    translate([tx_x, tx_y, bezel_bottom_z + 3.50])
                        cylinder(d1 = 13.65, d2 = 14.55, h = 0.85);
                    translate([tx_x, tx_y, bezel_bottom_z + 4.30])
                        cylinder(d1 = 14.55, d2 = 15.30, h = 0.75);
                    translate([tx_x, tx_y, bezel_bottom_z + 5.00])
                        cylinder(d1 = 15.30, d2 = 15.75, h = 0.70);
                }
            }

            for (bpt = BezelScrewCoords) {
                translate([bpt[0], bpt[1], bezel_bottom_z - 0.5])
                    cylinder(d = 2.6, h = bezel_thick + 1.0);
                translate([bpt[0], bpt[1], frame_total_h - 1.35])
                    cylinder(d1 = 2.6, d2 = 4.6, h = 1.45);
            }

            difference() {
                translate([bezel_x0 + 0.65, bezel_y0 + 0.65, frame_total_h - 0.45])
                    chamfered_prism(bezel_w - 1.30, bezel_h - 1.30, 0.55, 2.7);
                translate([bezel_x0 + 1.15, bezel_y0 + 1.15, frame_total_h - 0.55])
                    chamfered_prism(bezel_w - 2.30, bezel_h - 2.30, 0.75, 2.5);
            }

            translate([37.25, 14.50, frame_total_h - 0.40])
                linear_extrude(height = 0.55)
                    text("HPVE // 40kHz EMITTER", size = 1.50, font = "Consolas:style=Bold", halign = "center", valign = "center");
            translate([71.25, 14.50, frame_total_h - 0.40])
                linear_extrude(height = 0.55)
                    text("120dB SPL // MK-II", size = 1.50, font = "Consolas:style=Bold", halign = "center", valign = "center");
        }

        if (include_color) {
            color(c_bronze_bolt)
            for (bpt = BezelScrewCoords) {
                difference() {
                    translate([bpt[0], bpt[1], frame_total_h - 1.20])
                        cylinder(d = 4.3, h = 1.15);
                    translate([bpt[0], bpt[1], frame_total_h - 0.70])
                        cylinder(d = 2.0, h = 0.90, $fn = 6);
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------
// 10. HIGH-DETAIL HARDWARE MODEL: HPVE_Tile_16Ch PCB + HONEYCOMB MESH CANS
// ----------------------------------------------------------------------------
module perforated_transducer_can(tx_x, tx_y) {
    color(c_alu_can)
    difference() {
        translate([tx_x, tx_y, pcb_top_z])
            cylinder(d = tx_can_od, h = tx_can_h);
        translate([tx_x, tx_y, tx_top_z - 1.1])
            cylinder(d = 11.8, h = 1.3);
    }

    color(c_mesh_hole)
    translate([tx_x, tx_y, tx_top_z - 1.05])
        cylinder(d = 11.7, h = 0.45);

    color(c_mesh_silver)
    difference() {
        translate([tx_x, tx_y, tx_top_z - 0.60])
            cylinder(d = 11.7, h = 0.35);

        translate([tx_x, tx_y, tx_top_z - 0.75]) {
            cylinder(d = 1.15, h = 0.70, $fn = 12);
            for (a1 = [0 : 60 : 300]) {
                rotate([0, 0, a1]) translate([1.85, 0, 0])
                    cylinder(d = 1.10, h = 0.70, $fn = 12);
            }
            for (a2 = [0 : 30 : 330]) {
                rotate([0, 0, a2]) translate([3.60, 0, 0])
                    cylinder(d = 1.05, h = 0.70, $fn = 12);
            }
            for (a3 = [0 : 20 : 340]) {
                rotate([0, 0, a3]) translate([5.15, 0, 0])
                    cylinder(d = 0.90, h = 0.70, $fn = 12);
            }
        }
    }

    color(c_gold_pad) {
        translate([tx_x - 5.0, tx_y, pcb_ledge_z - 1.6])
            cylinder(d = 0.85, h = 1.65, $fn = 16);
        translate([tx_x + 5.0, tx_y, pcb_ledge_z - 1.6])
            cylinder(d = 0.85, h = 1.65, $fn = 16);
    }
}

module pcb_and_transducers_assembly() {
    m3_coords = [
        [m3_inset, m3_inset],
        [pcb_w - m3_inset, m3_inset],
        [m3_inset, pcb_h - m3_inset],
        [pcb_w - m3_inset, pcb_h - m3_inset]
    ];

    // 1. 4-Layer 98.0 x 88.0 mm PCB Substrate
    color(c_pcb_matte)
    difference() {
        translate([1.5, 1.5, pcb_ledge_z])
            chamfered_prism(pcb_w - 3.0, pcb_h - 3.0, pcb_thick, 2.5);
        for (pt = m3_coords) {
            translate([pt[0], pt[1], pcb_ledge_z - 0.5])
                cylinder(d = m3_drill_d, h = pcb_thick + 1.0);
        }
    }

    // 2. ENIG Gold M3 Mounting Rings
    color(c_gold_pad)
    for (pt = m3_coords) {
        difference() {
            translate([pt[0], pt[1], pcb_top_z - 0.05])
                cylinder(d = 5.6, h = 0.15);
            translate([pt[0], pt[1], pcb_top_z - 0.2])
                cylinder(d = m3_drill_d, h = 0.5);
        }
    }

    // 3. 8x TC4427 Dual Gate Drivers (U101..U108 at X=13.0mm) + 2x 74HC595 Shift Registers (U1, U2 at Y=9.5mm)
    color(c_anduril_slate) {
        for (i = [0 : 7]) {
            uy = 13.0 + i * 8.0;
            translate([13.0 - 2.0, uy - 2.45, pcb_top_z])
                cube([3.9, 4.9, 1.55]);
        }
        translate([38.0 - 4.95, 9.5 - 1.95, pcb_top_z])
            cube([9.9, 3.9, 1.55]);
        translate([66.0 - 4.95, 9.5 - 1.95, pcb_top_z])
            cube([9.9, 3.9, 1.55]);
    }

    // 4. Local SMD Decoupling Capacitors & Damping Resistors
    color([0.72, 0.55, 0.32]) {
        for (i = [0 : 7]) {
            uy = 13.0 + i * 8.0;
            translate([18.0 - 0.6, uy + 0.635 - 1.0, pcb_top_z])
                cube([1.2, 2.0, 0.9]);
        }
        translate([47.0 - 0.6, 9.5 - 1.0, pcb_top_z]) cube([1.2, 2.0, 0.9]);
        translate([57.0 - 0.6, 9.5 - 1.0, pcb_top_z]) cube([1.2, 2.0, 0.9]);
        translate([4.8 - 0.8, 17.5 - 1.6, pcb_top_z]) cube([1.6, 3.2, 1.2]);
    }

    // 5. J_IN (4.8, 36.0) & J_OUT (93.2, 36.0) 0.5mm 6-Pin Horizontal FFC Connectors
    color([0.88, 0.85, 0.78]) {
        translate([4.8 - 1.8, 36.0 - 4.2, pcb_top_z])
            cube([3.6, 8.4, 1.9]);
        translate([93.2 - 1.8, 36.0 - 4.2, pcb_top_z])
            cube([3.6, 8.4, 1.9]);
    }
    // Dark Flip-Lock Actuator Latches on J_IN & J_OUT
    color(c_anduril_slate) {
        translate([4.8 - 2.1, 36.0 - 4.0, pcb_top_z + 1.1])
            cube([1.0, 8.0, 0.9]);
        translate([93.2 + 1.1, 36.0 - 4.0, pcb_top_z + 1.1])
            cube([1.0, 8.0, 0.9]);
    }

    // 6. J_PWR High-Current +12V_DRV Header at (4.8, 10.0)
    color(c_copper_wire)
    translate([4.8 - 1.27, 10.0 - 2.54, pcb_top_z])
        cube([2.54, 5.08, 3.2]);

    // 7. 16x Prowave 400ST160 16.2mm Ultrasonic Transducer Cans with Honeycomb Mesh
    for (r = [0 : 3]) {
        for (c = [0 : 3]) {
            tx_x = tx_grid_x0 + c * tx_pitch;
            tx_y = tx_grid_y0 + r * tx_pitch;
            perforated_transducer_can(tx_x, tx_y);
        }
    }
}

// ----------------------------------------------------------------------------
// 11. COMPLETE 6-PIN FFC DAISY-CHAIN RIBBON & +12V STAR HARNESS ROUTING
//     Explicitly models every single ribbon cable & power line in 3D space:
//     - Segment 1 (Master Feed): RP2040 Master Dock FFC_OUT -> dives into
//       Tile 0 (Front-Left) J_IN at (-93.2, -52.0, 8.0)!
//     - Segment 2 (Row 0 Bridge): Tile 0 J_OUT (-4.8, -52.0) -> straight across
//       the internal X=0 slot -> Tile 1 (Front-Right) J_IN (+4.8, -52.0)!
//     - Segment 3 (Row 0 -> Row 1 U-Turn): Tile 1 J_OUT (+93.2, -52.0) -> drops
//       through the floor slot into the protected rear cable channel -> runs
//       across backplane -> rises up into Tile 2 (Back-Left) J_IN (-93.2, +36.0)!
//     - Segment 4 (Row 1 Bridge): Tile 2 J_OUT (-4.8, +36.0) -> straight across
//       the internal X=0 slot -> Tile 3 (Back-Right) J_IN (+4.8, +36.0)!
// ----------------------------------------------------------------------------
module ffc_daisy_chain_and_star_power_harness() {
    z_ffc_pcb = pcb_top_z + 0.65; // 8.05 mm inside the PCB chamber

    color(c_ffc_kapton) {
        // 1. Segment 2: Tile 0 J_OUT (-4.8, -52.0) -> Tile 1 J_IN (+4.8, -52.0)
        translate([-5.5, -52.0 - 3.0, z_ffc_pcb])
            cube([11.0, 6.0, 0.35]);

        // 2. Segment 4: Tile 2 J_OUT (-4.8, +36.0) -> Tile 3 J_IN (+4.8, +36.0)
        translate([-5.5, 36.0 - 3.0, z_ffc_pcb])
            cube([11.0, 6.0, 0.35]);

        // 3. Segment 1 (Master Feed from RP2040 Dock at X=-65, Y=-32, Z=-5.5 into Tile 0 J_IN at X=-93.2, Y=-52.0)
        //    Horizontal run on rear backplane:
        translate([-94.5, -52.0 - 3.0, -2.5])
            cube([32.0, 6.0, 0.40]);
        //    Vertical riser through Tile 0's bottom FFC slot up to J_IN:
        translate([-94.5, -52.0 - 3.0, -2.5])
            cube([0.45, 6.0, z_ffc_pcb + 2.8]);
        //    Horizontal lip entering J_IN mouth:
        translate([-94.5, -52.0 - 3.0, z_ffc_pcb])
            cube([2.2, 6.0, 0.35]);

        // 4. Segment 3 (Row 0 -> Row 1 Cascade U-Turn from Tile 1 J_OUT (+93.2, -52.0) to Tile 2 J_IN (-93.2, +36.0))
        //    Exits Tile 1 J_OUT & drops through bottom FFC slot:
        translate([92.3, -52.0 - 3.0, z_ffc_pcb])
            cube([2.2, 6.0, 0.35]);
        translate([94.1, -52.0 - 3.0, -1.8])
            cube([0.45, 6.0, z_ffc_pcb + 2.1]);
        //    Protected rear channel run along X = +90 -> Y = 0 -> X = -94.5 -> Y = +36.0:
        translate([86.0, -52.0 - 3.0, -1.8])
            cube([8.5, 6.0, 0.40]);
        translate([86.0, -52.0 - 3.0, -1.8])
            cube([6.0, 52.0, 0.40]);
        translate([-94.5, -3.0, -1.8])
            cube([186.5, 6.0, 0.40]);
        translate([-94.5, 0.0, -1.8])
            cube([6.0, 39.0, 0.40]);
        //    Vertical riser through Tile 2's bottom FFC slot into Tile 2 J_IN (-93.2, +36.0):
        translate([-94.5, 36.0 - 3.0, -1.8])
            cube([0.45, 6.0, z_ffc_pcb + 2.1]);
        translate([-94.5, 36.0 - 3.0, z_ffc_pcb])
            cube([2.2, 6.0, 0.35]);
    }

    // 5. High-Current 4-Branch +12V_DRV Star Power Harness (from Central Power Dock to all 4 Tiles' J_PWR)
    //    Tile J_PWR global coordinates:
    //    Tile 0: (-93.2, -78.0), Tile 1: (+4.8, -78.0), Tile 2: (-93.2, +10.0), Tile 3: (+4.8, +10.0)
    j_pwr_globals = [
        [-93.2, -78.0],
        [  4.8, -78.0],
        [-93.2,  10.0],
        [  4.8,  10.0]
    ];
    for (pt = j_pwr_globals) {
        // Red +12V_DRV 16AWG Silicone Feed
        color(c_copper_wire) {
            hull() {
                translate([0, -18.0, -2.0]) sphere(d = 2.2, $fn = 16);
                translate([pt[0], pt[1] + 1.27, -2.0]) sphere(d = 2.2, $fn = 16);
            }
            translate([pt[0], pt[1] + 1.27, -2.0])
                cylinder(d = 2.2, h = pcb_top_z + 3.0, $fn = 16);
        }
        // Black GND 16AWG Silicone Feed
        color(c_gnd_wire) {
            hull() {
                translate([0, -21.0, -2.0]) sphere(d = 2.2, $fn = 16);
                translate([pt[0], pt[1] - 1.27, -2.0]) sphere(d = 2.2, $fn = 16);
            }
            translate([pt[0], pt[1] - 1.27, -2.0])
                cylinder(d = 2.2, h = pcb_top_z + 3.0, $fn = 16);
        }
    }
}

// ----------------------------------------------------------------------------
// 12. INTEGRATED RP2040 PICO MASTER AVIONICS DOCK & EXTERNAL I/O BAY
//     Answers "How we put Raspberry Pi Pico, how we connect from outside to
//     code our PCB / Pico USB cable, +12V power input, and ribbon wires":
//     Mounted cleanly on the rear/top spine (Z = 0..-14mm) with an External
//     Front-Left I/O Bulkhead Bezel at Y = -88.0mm so you can plug in:
//     1) USB-C / Micro-USB Programming & G-Code Telemetry Cable from outside!
//     2) External BOOTSEL Tactile Pinhole Button (flash firmware without disassembly)!
//     3) Amass XT30 +12V_DRV 10A Main Power Input Receptacle!
//     4) Onboard 12V -> 5V 3A Buck Regulator + 1000uF Low-ESR Bulk Capacitor!
//     5) Integrated SSD1306 128x64 OLED Telemetry Window on the Rear Backplane!
// ----------------------------------------------------------------------------
module rp2040_master_avionics_dock() {
    // 1. Master Avionics Enclosure Bay along Left-Front Spine (X = -86..-26, Y = -88..-12, Z = -14..0)
    //    Reaches all the way to the front outer wall (Y = -88.0 mm) so the USB-C port and XT30 power
    //    connector are 100% accessible from the outside edge!
    color(c_anduril_slate)
    difference() {
        union() {
            // Main Avionics Bay Housing
            translate([-84.0, -88.0, -13.5])
                chamfered_prism(52.0, 72.0, 13.5, 4.0);

            // Central Cruciform Structural Lock Plate (Bolts across all 4 inner tile M3 corners!)
            translate([-24.0, -24.0, -5.5])
                chamfered_prism(48.0, 48.0, 5.5, 6.0);
            translate([-82.0, -8.0, -4.0])
                chamfered_prism(164.0, 16.0, 4.0, 3.0);
            translate([-8.0, -76.0, -4.0])
                chamfered_prism(16.0, 152.0, 4.0, 3.0);
        }

        // Internal Cavity for Raspberry Pi Pico (21.0 x 51.0 mm) & Buck Converter
        translate([-79.0, -84.0, -11.5])
            chamfered_prism(42.0, 62.0, 12.0, 2.5);

        // External Front-Edge USB-C / Micro-USB Programming Port Cutout (at X = -66.0, Y = -88.0, Z = -7.5)
        translate([-66.0 - 4.8, -89.0, -7.5 - 1.8])
            cube([9.6, 8.0, 3.6]);

        // External Recessed USB-C Cable Plug Boot Relief (allows fat USB-C cable molding to plug flush!)
        translate([-66.0 - 6.5, -88.5, -7.5 - 3.2])
            cube([13.0, 2.5, 6.4]);

        // External BOOTSEL Tactile Reset/Flash Button Pinhole on Front Edge (at X = -53.0, Y = -88.0)
        translate([-53.0, -89.0, -7.5])
            rotate([-90, 0, 0])
                cylinder(d = 3.2, h = 8.0);

        // External XT30 +12V_DRV Power Input Bay on Front Edge (at X = -42.0, Y = -88.0)
        translate([-42.0 - 5.5, -89.0, -7.5 - 2.8])
            cube([11.0, 8.0, 5.6]);

        // Rear Observation Window exposing the Raspberry Pi Pico RP2040 Silicon & Status LEDs
        translate([-75.0, -78.0, -14.5])
            chamfered_prism(22.0, 42.0, 4.0, 2.0);

        // SSD1306 128x64 OLED HUD Bezel Cutout in Central Cruciform Hub
        translate([-15.0, -11.0, -6.5])
            chamfered_prism(30.0, 22.0, 3.0, 1.5);

        // 4x Inner Tile M3 Corner Bolt Holes (+/-4.5, +/-4.5)
        for (sx = [-4.5, 4.5]) {
            for (sy = [-4.5, 4.5]) {
                translate([sx, sy, -6.5])
                    cylinder(d = 3.4, h = 7.5);
            }
        }
    }

    // 2. TVA Signal Orange Accent Rim around the Avionics Bay & USB-C I/O Bulkhead
    color(c_tva_orange)
    difference() {
        translate([-84.4, -88.0, -2.2])
            chamfered_prism(52.8, 72.4, 1.4, 4.2);
        translate([-83.5, -87.2, -2.5])
            chamfered_prism(51.0, 70.8, 2.0, 3.8);
    }

    // 3. Physical Raspberry Pi Pico (21.0 x 51.0 mm) Mounted Inside the Avionics Bay
    //    Centered at X = -66.0, Y = -86.5..-35.5, Z = -7.0
    color(c_pico_green)
    translate([-66.0 - 10.5, -86.5, -7.0])
        chamfered_prism(21.0, 51.0, 1.6, 1.5);

    //    RP2040 Dual-Core ARM Cortex-M0+ QFN-56 Silicon Die at Center of Pico
    color([0.10, 0.11, 0.13])
    translate([-66.0 - 3.5, -62.0 - 3.5, -8.2])
        cube([7.0, 7.0, 1.2]);

    //    Pico USB-C / Micro-USB Stainless Receptacle Flush with Front Bulkhead (Y = -88.0 mm)!
    color(c_alu_can)
    difference() {
        translate([-66.0 - 4.4, -88.2, -7.0 - 1.6])
            cube([8.8, 6.5, 3.2]);
        translate([-66.0 - 3.8, -88.5, -7.0 - 1.1])
            cube([7.6, 5.0, 2.2]);
    }

    //    External USB-C Braided Programming / Telemetry Cable Plugged into the Front Port (or exploded out)
    color(c_anduril_slate)
    translate([-66.0 - 5.5, -98.0, -7.0 - 2.2])
        chamfered_prism(11.0, 10.0, 4.4, 1.5);
    color(c_tva_orange)
    translate([-66.0 - 5.7, -91.5, -7.0 - 2.4])
        chamfered_prism(11.4, 2.0, 4.8, 1.5);

    //    External BOOTSEL Brass Tactile Actuator Button Pin (at X = -53.0, Y = -88.2)
    color(c_bronze_bolt)
    translate([-53.0, -88.5, -7.5])
        rotate([-90, 0, 0])
            cylinder(d = 2.8, h = 4.0);

    // 4. Amass XT30 +12V_DRV 10A High-Current Power Receptacle Flush with Front Bulkhead (at X = -42.0, Y = -88.0)
    color(c_xt30_yellow)
    difference() {
        translate([-42.0 - 5.0, -88.2, -7.5 - 2.5])
            cube([10.0, 8.5, 5.0]);
        translate([-42.0 - 3.8, -88.6, -7.5 - 1.7])
            cube([7.6, 4.0, 3.4]);
    }
    // Gold 2mm Bullet Pins inside XT30 Receptacle
    color(c_gold_pad) {
        translate([-42.0 - 2.2, -88.0, -7.5])
            rotate([-90, 0, 0]) cylinder(d = 2.0, h = 5.0, $fn = 16);
        translate([-42.0 + 2.2, -88.0, -7.5])
            rotate([-90, 0, 0]) cylinder(d = 2.0, h = 5.0, $fn = 16);
    }

    // 5. Master 6-Pin FFC Output Connector on Avionics Dock (at X = -64.0, Y = -32.0, Z = -6.0)
    //    Feeds the Master 6-Pin FFC Ribbon directly into Tile 0's J_IN!
    color([0.88, 0.85, 0.78])
    translate([-64.0 - 4.2, -33.0, -6.5])
        cube([8.4, 4.0, 2.2]);

    // 6. Integrated SSD1306 128x64 OLED Telemetry Display in Central Cruciform Hub
    color([0.04, 0.06, 0.09])
    translate([-13.5, -9.5, -6.0])
        cube([27.0, 19.0, 1.5]);
    color(c_oled_cyan)
    translate([0.0, 3.0, -6.2])
        rotate([180, 0, 0])
            linear_extrude(height = 0.4)
                text("HPVE 40.0kHz LOCK", size = 1.9, font = "Consolas:style=Bold", halign = "center", valign = "center");
    color(c_tva_orange)
    translate([0.0, -3.0, -6.2])
        rotate([180, 0, 0])
            linear_extrude(height = 0.4)
                text("64CH PIO+DMA // 0 ERR", size = 1.5, font = "Consolas:style=Bold", halign = "center", valign = "center");

    // 7. Dual Overhead Halo Arch Suspension Rails & Pan-Tilt Gimbal Clevis Yoke (at X = -54.0 and +54.0 mm)
    for (rx = [-54.0, 54.0]) {
        y_rail_0 = (rx < 0) ? -12.0 : -78.0;
        y_rail_len = (rx < 0) ? 90.0 : 156.0;

        color(c_tva_cream)
        difference() {
            translate([rx - 14.0, y_rail_0, -6.0])
                chamfered_prism(28.0, y_rail_len, 6.0, 4.0);
            for (py = [28.0, 62.0]) {
                translate([rx - 8.0, py - 12.0, -6.5])
                    chamfered_prism(16.0, 24.0, 4.0, 2.5);
            }
        }

        color(c_tva_orange)
        difference() {
            translate([rx - 14.4, y_rail_0 - 0.4, -2.2])
                chamfered_prism(28.8, y_rail_len + 0.8, 1.4, 4.2);
            translate([rx - 13.5, y_rail_0 + 0.5, -2.5])
                chamfered_prism(27.0, y_rail_len - 1.0, 2.0, 3.8);
        }

        // Anodized Tactical Slate Pan-Tilt Gimbal Trunnion Clevis Ear (Centered at Y = +24.0 mm)
        color(c_anduril_slate)
        difference() {
            hull() {
                translate([rx - 6.0, 6.0, -6.0])
                    cube([12.0, 36.0, 2.0]);
                translate([rx - 6.0, 24.0, -22.0])
                    rotate([0, 90, 0])
                        cylinder(d = 22.0, h = 12.0);
            }
            translate([rx - 8.0, 24.0, -22.0])
                rotate([0, 90, 0])
                    cylinder(d = 6.2, h = 16.0);
        }

        // Brushed Brass Knurled Gimbal Tension Knob
        color(c_bronze_bolt)
        translate([rx + (rx > 0 ? 6.0 : -14.0), 24.0, -22.0])
            rotate([0, 90, 0])
                difference() {
                    cylinder(d = 16.0, h = 8.0);
                    for (ka = [0 : 18 : 342]) {
                        rotate([0, 0, ka])
                            translate([8.0, 0, -0.5])
                                cylinder(d = 1.6, h = 9.0, $fn = 12);
                    }
                }
    }
}

// ----------------------------------------------------------------------------
// 12B. TASK 7.4: CENTRAL OPTICAL & TOF SENSOR DOCKING POD (EMITTER FACE CENTER)
//      Mounts at the central intersection (0, 0, frame_total_h) of the 4 tiles
//      and houses:
//      1) Downward-looking VL53L5CX 8x8 Multizone Time-of-Flight (ToF) Hand Tracker
//      2) 4x Angled 450nm Cyan/Blue & RGB Photonic Laser Diodes aimed at (0, 0, 55mm)
// ----------------------------------------------------------------------------
module central_optical_tof_sensor_pod(multi_body=true) {
    // 1. Anodized Tactical Slate Cruciform Sensor Pod Body seated in the 4-tile cross-seam
    color(c_anduril_slate)
    difference() {
        union() {
            translate([-11.5, -11.5, frame_total_h - 1.5])
                chamfered_prism(23.0, 23.0, 4.2, 4.0);
            // Cruciform alignment keys locking into the X=0 and Y=0 inter-tile seams
            translate([-22.0, -3.6, frame_total_h - 1.0])
                chamfered_prism(44.0, 7.2, 2.8, 1.5);
            translate([-3.6, -22.0, frame_total_h - 1.0])
                chamfered_prism(7.2, 44.0, 2.8, 1.5);
        }

        // Central Optical Window for VL53L5CX 8x8 ToF Sensor
        translate([-4.2, -4.2, frame_total_h + 1.2])
            chamfered_prism(8.4, 8.4, 2.0, 1.0);

        // 4x Angled Collimated Laser Diode Ports at (+/-7.2, +/-7.2) aimed toward focal zone
        for (lx = [-7.2, 7.2]) {
            for (ly = [-7.2, 7.2]) {
                translate([lx, ly, frame_total_h + 0.5])
                    cylinder(d = 3.8, h = 3.0);
            }
        }
    }

    if (multi_body) {
        // 2. TVA Signal Orange Bezel Collar Ring around Central Sensor Pod
        color(c_tva_orange)
        difference() {
            translate([-12.0, -12.0, frame_total_h + 0.2])
                chamfered_prism(24.0, 24.0, 1.0, 4.2);
            translate([-11.2, -11.2, frame_total_h - 0.2])
                chamfered_prism(22.4, 22.4, 1.8, 3.8);
        }

        // 3. VL53L5CX 8x8 Multizone ToF Sensor Silicon + IR Optical Filter Glass at (0, 0)
        color([0.06, 0.07, 0.09])
        translate([-3.8, -3.8, frame_total_h + 1.0])
            cube([7.6, 7.6, 1.4]);
        // Dual Transmitter/Receiver IR Aperture Lenses
        color(c_gold_pad) {
            translate([-1.6, 0, frame_total_h + 2.45]) cylinder(d = 1.8, h = 0.25, $fn = 24);
            translate([ 1.6, 0, frame_total_h + 2.45]) cylinder(d = 1.8, h = 0.25, $fn = 24);
        }

        // 4. 4x Focused 450nm Cyan/Blue Photonic Projection Laser Collimators (Brushed Brass Sleeves + Glowing Cyan Lenses)
        for (lx = [-7.2, 7.2]) {
            for (ly = [-7.2, 7.2]) {
                color(c_bronze_bolt)
                difference() {
                    translate([lx, ly, frame_total_h + 0.8])
                        cylinder(d = 3.6, h = 1.9, $fn = 24);
                    translate([lx, ly, frame_total_h + 1.5])
                        cylinder(d = 2.2, h = 1.5, $fn = 24);
                }
                color(c_oled_cyan)
                translate([lx, ly, frame_total_h + 2.2])
                    cylinder(d = 2.1, h = 0.45, $fn = 24);
            }
        }
    }
}

// ----------------------------------------------------------------------------
// 13. ASSEMBLIES: SINGLE TILE, EXPLODED, INTERNAL CUTAWAY & 2x2 HERO QUAD
// ----------------------------------------------------------------------------
module single_tile_complete(qx=0, qy=0, show_bezel=true) {
    tile_exoskeleton_base_frame(qx, qy, true);
    pcb_and_transducers_assembly();
    if (show_bezel) {
        tva_orange_quadrant_bevel(qx, qy);
        acoustic_horn_bezel_plate(true);
    }
}

module quad_2x2_hero_assembly(include_avionics=true, cutaway_left_tiles=false) {
    for (qx = [0 : 1]) {
        for (qy = [0 : 1]) {
            translate([(qx - 1) * tile_pitch_x, (qy - 1) * tile_pitch_y, 0]) {
                show_top = !(cutaway_left_tiles && qx == 0);
                single_tile_complete(qx, qy, show_top);
            }
        }
    }
    ffc_daisy_chain_and_star_power_harness();
    if (!cutaway_left_tiles) {
        central_optical_tof_sensor_pod();
    }
    if (include_avionics) {
        rp2040_master_avionics_dock();
    }
}

module single_tile_exploded_5layer() {
    tile_exoskeleton_base_frame(0, 0, true);
    translate([0, 0, 20.0])
        pcb_and_transducers_assembly();
    translate([0, 0, 40.0])
        tva_orange_quadrant_bevel(0, 0);
    translate([0, 0, 56.0])
        acoustic_horn_bezel_plate(true);
}

module quad_2x2_full_exploded_assembly() {
    // Layer -1 (z = -38mm): RP2040 Master Avionics Dock & Halo Gimbal Mounts
    translate([0, 0, -32.0])
        rp2040_master_avionics_dock();

    // Layer 0 (z = -12mm): 6-Pin FFC Daisy-Chain Ribbon & Star Power Harness
    translate([0, 0, -10.0])
        ffc_daisy_chain_and_star_power_harness();

    // Layer +5 (z = +62mm): Task 7.4 Central Optical & ToF Sensor Pod
    translate([0, 0, 56.0])
        central_optical_tof_sensor_pod();

    // Layer 1..4: 4x Exploded Modular Tiles radially & vertically separated
    for (qx = [0 : 1]) {
        for (qy = [0 : 1]) {
            sx = (qx == 0) ? -8.0 : 8.0;
            sy = (qy == 0) ? -8.0 : 8.0;
            translate([(qx - 1) * tile_pitch_x + sx, (qy - 1) * tile_pitch_y + sy, 0]) {
                tile_exoskeleton_base_frame(qx, qy, true);
                translate([0, 0, 18.0])
                    pcb_and_transducers_assembly();
                translate([0, 0, 36.0])
                    tva_orange_quadrant_bevel(qx, qy);
                translate([0, 0, 50.0])
                    acoustic_horn_bezel_plate(true);
            }
        }
    }
}

// ----------------------------------------------------------------------------
// 14. TASK 7.3: PORTABLE OVERHEAD DESKTOP HALO ARCH STAND & BENCH CLAMP RIG
//     Matches render_halo_stand_rig.jpg (hpve_overhead_desktop_halo_stand):
//     - Smooth circular-radius Inverted-U Ceramic Cream Monocoque Arch with
//       continuous TVA Signal Orange inner arch liner & top collar bands
//     - Top Bridge Instrumentation: "HPVE | HAPTIC PHOTONIC VOLUMETRIC ENGINE",
//       twin top brass calibration turrets, front brass dial, status LEDs,
//       and recessed Analog Acoustic Pressure / SPL Gauge with needle
//     - Left & Right Pillar Controls: 3x stacked knurled brass phase/gain knobs
//       per pillar, tick marks, status LEDs, lower front coiled umbilical cords,
//       and upper inner curved corrugated umbilical conduits feeding the array
//     - Heavy-Duty Tabletop C-Clamps with knurled bottom hand-wheels & pads
//     - Under-Desk "HPVE |" +12V 10A Master PSU Box with brass knob, rocker
//       switch, and twin curved under-desk braided power conduits
//     - Suspended 4-Tile 64-Channel Emitter Head + Floating 100mm Volumetric Sphere
// ----------------------------------------------------------------------------
module knurled_brass_dial(r_dial, h_dial) {
    color(c_bronze_bolt)
    difference() {
        union() {
            cylinder(r = r_dial, h = h_dial * 0.82, $fn = 40);
            translate([0, 0, h_dial * 0.82])
                cylinder(r1 = r_dial, r2 = r_dial * 0.86, h = h_dial * 0.18, $fn = 40);
        }
        for (a = [0 : 12 : 348]) {
            rotate([0, 0, a])
                translate([r_dial, 0, -0.5])
                    cylinder(r = 0.65, h = h_dial * 0.85, $fn = 10);
        }
    }
    // Brushed Gold Front Face Cap
    color(c_gold_pad)
    translate([0, 0, h_dial])
        cylinder(r = r_dial * 0.78, h = 0.6, $fn = 36);
}

// 2D Outer Boundary of the Smooth Inverted-U Halo Arch in the XZ Plane
module halo_arch_outer_2d(span_x, pillar_w, height_z, r_outer) {
    hull() {
        // Left base foot
        translate([-span_x/2 - pillar_w/2, 0])
            square([pillar_w, 10]);
        // Right base foot
        translate([span_x/2 - pillar_w/2, 0])
            square([pillar_w, 10]);
        // Left upper outer circular shoulder
        translate([-span_x/2 - pillar_w/2 + r_outer, height_z + pillar_w/2 - r_outer])
            circle(r = r_outer, $fn = 72);
        // Right upper outer circular shoulder
        translate([span_x/2 + pillar_w/2 - r_outer, height_z + pillar_w/2 - r_outer])
            circle(r = r_outer, $fn = 72);
    }
}

// 2D Inner Cutout Window of the Smooth Inverted-U Halo Arch in the XZ Plane
module halo_arch_inner_cutout_2d(span_x, pillar_w, height_z, r_inner, extra_margin=0) {
    hull() {
        translate([-span_x/2 + pillar_w/2 - extra_margin, -20])
            square([span_x - pillar_w + 2*extra_margin, 20]);
        translate([-span_x/2 + pillar_w/2 + r_inner, height_z - pillar_w/2 - r_inner])
            circle(r = r_inner + extra_margin, $fn = 72);
        translate([span_x/2 - pillar_w/2 - r_inner, height_z - pillar_w/2 - r_inner])
            circle(r = r_inner + extra_margin, $fn = 72);
    }
}

// 2D Cross-Section of the Smooth Inverted-U Halo Arch in the XZ Plane
module halo_arch_2d_profile(span_x, pillar_w, height_z, r_outer, r_inner) {
    difference() {
        halo_arch_outer_2d(span_x, pillar_w, height_z, r_outer);
        halo_arch_inner_cutout_2d(span_x, pillar_w, height_z, r_inner, 0);
    }
}

// Printable C-Clamp & Gimbal Trunnion Hanger Bracket (Task 7.5 Part E STL)
module halo_arch_c_clamp_bracket_stl() {
    difference() {
        union() {
            // Base Flange on Tabletop
            translate([-33, -40, 0])
                chamfered_prism(66, 80, 12.0, 5.0);
            // Vertical C-Clamp Spine Wrapping Table Edge
            translate([-24, -40, -76])
                chamfered_prism(48, 20, 88.0, 3.0);
            // Lower Horizontal Jaw Under Tabletop
            translate([-24, -40, -76])
                chamfered_prism(48, 56, 16.0, 3.0);
            // Upper Pillar Tenon Boss
            translate([-22, -28, 12.0])
                chamfered_prism(44, 56, 24.0, 4.0);
        }
        // Leadscrew Threaded Bore in Lower Jaw
        translate([0, -2, -80])
            cylinder(d = 12.5, h = 25.0, $fn = 32);
        // 4x M4 Mounting Counterbores in Base Flange
        for (mx = [-24, 24]) {
            for (my = [-26, 26]) {
                translate([mx, my, -1])
                    cylinder(d = 4.4, h = 15.0, $fn = 24);
            }
        }
    }
}

module desktop_halo_arch_stand_complete() {
    arch_span_x     = 476.0;  // Center-to-center span between left & right vertical pillars
    pillar_w        = 56.0;   // Arch radial wall thickness (X on pillars, Z on top bridge)
    pillar_d        = 68.0;   // Depth of arch monocoque (Y)
    arch_height_z   = 432.0;  // Centerline Z of the top horizontal instrumentation bridge
    r_arch_outer    = 108.0;  // Smooth outer shoulder radius (matched to render_halo_stand_rig.jpg)
    r_arch_inner    = 52.0;   // Smooth inner shoulder radius
    head_mount_z    = 364.0;  // Z-height of suspended 4-tile emitter array above tabletop

    // 1. WALNUT LAB WORKBENCH TABLETOP (Z = -30..0 mm) + KNURLED ALUMINUM FRONT BULLNOSE
    //    Front edge of workbench is at Y = -40 mm so the C-Clamps wrap directly around the front edge!
    color([0.40, 0.26, 0.16])
    translate([-380, -40, -30])
        cube([760, 320, 30]);
    // Knurled Brushed Aluminum Front Edge Trim Strip on Workbench (Y = -44.5..-40 mm)
    color([0.56, 0.58, 0.61])
    translate([-380, -44.5, -30])
        cube([760, 4.5, 30]);
    // Dark Wood Under-Desk Apron & Legs for Architectural Context
    color([0.24, 0.15, 0.09]) {
        translate([-350, -20, -160]) cube([36, 260, 130]);
        translate([ 314, -20, -160]) cube([36, 260, 130]);
    }

    // 2. CIRCULAR MATTE-BLACK OPTICAL & ACOUSTIC CALIBRATION TARGET PAD AT (0, 0, 0)
    color(c_anduril_slate)
    translate([0, 0, 0.1])
        cylinder(d = 96.0, h = 2.2, $fn = 72);
    //    2D ring extrusion avoids 3D negative-volume Z-fighting on the black target pad!
    color(c_tva_orange)
    translate([0, 0, 2.35])
        linear_extrude(height = 0.45)
            difference() {
                circle(d = 88.0, $fn = 72);
                circle(d = 84.5, $fn = 72);
            }
    color(c_tva_orange)
    translate([0, 0, 2.35])
        linear_extrude(height = 0.45)
            difference() {
                circle(d = 42.0, $fn = 64);
                circle(d = 39.5, $fn = 64);
            }

    // 3. SMOOTH CURVED INVERTED-U HALO ARCH MONOCOQUE (CERAMIC CREAM #E8E0CE)
    //    Uses 2D profile difference prior to linear_extrude so zero 3D negative volume touches the tabletop!
    color(c_tva_cream)
    difference() {
        union() {
            // Core Arch Monocoque Body
            translate([0, pillar_d/2 - 2.5, 0.2])
                rotate([90, 0, 0])
                    linear_extrude(height = pillar_d - 5.0)
                        halo_arch_2d_profile(arch_span_x, pillar_w, arch_height_z, r_arch_outer, r_arch_inner);
            // Mid Chamfer Step
            translate([0, pillar_d/2 - 1.2, 0.2])
                rotate([90, 0, 0])
                    linear_extrude(height = pillar_d - 2.4)
                        offset(r = -1.2)
                            halo_arch_2d_profile(arch_span_x, pillar_w, arch_height_z, r_arch_outer, r_arch_inner);
            // Outer Beveled Faceplate Step
            translate([0, pillar_d/2, 0.2])
                rotate([90, 0, 0])
                    linear_extrude(height = pillar_d)
                        offset(r = -2.4)
                            halo_arch_2d_profile(arch_span_x, pillar_w, arch_height_z, r_arch_outer, r_arch_inner);
        }

        // Front Bridge Debossed Header Typography ("HPVE | HAPTIC PHOTONIC VOLUMETRIC ENGINE")
        translate([-138, -pillar_d/2 + 1.2, arch_height_z + 7])
            rotate([90, 0, 0])
                linear_extrude(height = 2.0)
                    text("HPVE", size = 15.5, font = "Consolas:style=Bold", halign = "left", valign = "center");
        translate([-68, -pillar_d/2 + 1.2, arch_height_z + 12])
            rotate([90, 0, 0])
                linear_extrude(height = 2.0)
                    text("HAPTIC PHOTONIC VOLUMETRIC ENGINE", size = 5.2, font = "Consolas:style=Bold", halign = "left", valign = "center");
        translate([-68, -pillar_d/2 + 1.2, arch_height_z - 3])
            rotate([90, 0, 0])
                linear_extrude(height = 2.0)
                    text("UNIV. LABS / CALIBRATION PT. 04", size = 4.4, font = "Consolas:style=Bold", halign = "left", valign = "center");

        // Recessed Analog SPL Pressure Gauge Cavity on Right of Top Bridge
        translate([112, -pillar_d/2 + 3.5, arch_height_z + 3])
            rotate([90, 0, 0])
                cylinder(d = 26.0, h = 5.0, $fn = 48);
    }

    // Dark Slate Painted Infill for Header Typography & Rule Lines on Top Bridge
    color(c_anduril_slate) {
        translate([-138, -pillar_d/2 - 0.1, arch_height_z + 7])
            rotate([90, 0, 0])
                linear_extrude(height = 0.6)
                    text("HPVE", size = 15.5, font = "Consolas:style=Bold", halign = "left", valign = "center");
        translate([-68, -pillar_d/2 - 0.1, arch_height_z + 12])
            rotate([90, 0, 0])
                linear_extrude(height = 0.6)
                    text("HAPTIC PHOTONIC VOLUMETRIC ENGINE", size = 5.2, font = "Consolas:style=Bold", halign = "left", valign = "center");
        // Subtle horizontal specification rule bar between title and subtitle
        translate([-68, -pillar_d/2 - 0.2, arch_height_z + 4.5])
            cube([104.0, 0.6, 2.2]);
        translate([-68, -pillar_d/2 - 0.1, arch_height_z - 3])
            rotate([90, 0, 0])
                linear_extrude(height = 0.6)
                    text("UNIV. LABS / CALIBRATION PT. 04", size = 4.4, font = "Consolas:style=Bold", halign = "left", valign = "center");
    }
    // TVA Orange Vertical Divider Bar after "HPVE"
    color(c_tva_orange)
    translate([-76, -pillar_d/2 - 0.3, arch_height_z - 6])
        cube([2.8, 0.8, 23.0]);

    // 4. TVA SIGNAL ORANGE COLLAR BANDS & CONTINUOUS CURVED INNER ARCH LINER
    color(c_tva_orange) {
        // Left & Right Top Bridge Vertical Orange Racing Bands (X = -156 and X = +156)
        for (bx = [-156, 156]) {
            translate([bx - 8.5, -pillar_d/2 - 0.8, arch_height_z - pillar_w/2 - 0.8])
                chamfered_prism(17.0, pillar_d + 1.6, pillar_w + 1.6, 2.5);
        }

        // Continuous Curved Inner Arch Orange Liner Strip (Follows inner pillar + circular inner shoulder!)
        translate([0, pillar_d/2 - 4.0, 0])
            rotate([90, 0, 0])
                linear_extrude(height = pillar_d - 8.0)
                    difference() {
                        halo_arch_inner_cutout_2d(arch_span_x, pillar_w, arch_height_z, r_arch_inner, 4.5);
                        halo_arch_inner_cutout_2d(arch_span_x, pillar_w, arch_height_z, r_arch_inner, -0.2);
                        // Trim bottom below Z = 12 mm
                        translate([-300, -30])
                            square([600, 42]);
                        // Trim center top between the two orange collar bands where the array hangs
                        translate([-148, arch_height_z - pillar_w/2 - 10])
                            square([296, 20]);
                    }
    }

    // 5. TOP BRIDGE KNURLED BRASS TURRETS, ROTARY ENCODER, LEDS & ANALOG SPL PRESSURE GAUGE
    //    Twin Top-Mounted Brass Calibration Turret Knobs (X = +54, +94)
    for (tx = [54, 94]) {
        color(c_anduril_slate)
        translate([tx, 0, arch_height_z + pillar_w/2 - 1.0])
            cylinder(d = 24.0, h = 3.0, $fn = 36);
        translate([tx, 0, arch_height_z + pillar_w/2 + 2.0])
            knurled_brass_dial(10.5, 15.0);
    }

    //    Front Bridge Brass Calibration Knob at X = +72
    color(c_anduril_slate)
    translate([72, -pillar_d/2 + 0.5, arch_height_z + 3])
        rotate([90, 0, 0]) cylinder(d = 25.0, h = 1.5, $fn = 36);
    translate([72, -pillar_d/2 - 1.0, arch_height_z + 3])
        rotate([90, 0, 0]) knurled_brass_dial(10.8, 10.5);

    //    Dual Status Indicator LEDs (Green + Amber) at X = +50
    color([0.15, 0.88, 0.35])
    translate([50, -pillar_d/2 - 0.5, arch_height_z + 8])
        rotate([90, 0, 0]) cylinder(d = 2.8, h = 1.2, $fn = 16);
    color(c_tva_orange)
    translate([50, -pillar_d/2 - 0.5, arch_height_z - 1])
        rotate([90, 0, 0]) cylinder(d = 2.8, h = 1.2, $fn = 16);

    //    Analog Acoustic Pressure / SPL Gauge at X = +112 (Brass Ring + Bone Dial + Tick Marks + Needle)
    //    Note: rotate([90, 0, 0]) extrudes +Z along -Y (toward the front camera!)
    color(c_bronze_bolt)
    difference() {
        translate([112, -pillar_d/2 + 0.5, arch_height_z + 3])
            rotate([90, 0, 0]) cylinder(d = 27.5, h = 2.5, $fn = 48);
        translate([112, -pillar_d/2 + 1.0, arch_height_z + 3])
            rotate([90, 0, 0]) cylinder(d = 22.5, h = 4.0, $fn = 48);
    }
    color(c_tva_bone)
    translate([112, -pillar_d/2 + 0.2, arch_height_z + 3])
        rotate([90, 0, 0]) cylinder(d = 22.8, h = 1.4, $fn = 48);
    //    Gauge Needle & Center Pivot (at Y = -pillar_d/2 - 1.3 so it sits in front of the Bone Dial!)
    color(c_anduril_slate) {
        translate([112, -pillar_d/2 - 1.3, arch_height_z + 3])
            rotate([90, -38, 0]) {
                 cylinder(d = 2.8, h = 0.8, $fn = 16);
                 translate([-0.6, 0, 0]) cube([1.2, 8.8, 0.6]);
            }
        // Gauge Scale Ticks
        for (ga = [-120 : 30 : 120]) {
            translate([112, -pillar_d/2 - 1.3, arch_height_z + 3])
                rotate([90, ga, 0])
                    translate([-0.4, 7.8, 0])
                        cube([0.8, 2.2, 0.5]);
        }
    }
    //    Micro-Labels underneath Bridge Controls
    color(c_anduril_slate) {
        translate([50, -pillar_d/2 - 0.1, arch_height_z - 14])
            rotate([90, 0, 0]) linear_extrude(0.5) text("LC098", size = 2.6, font = "Consolas:style=Bold", halign = "center");
        translate([72, -pillar_d/2 - 0.1, arch_height_z - 14])
            rotate([90, 0, 0]) linear_extrude(0.5) text("ANLG SND", size = 2.6, font = "Consolas:style=Bold", halign = "center");
        translate([112, -pillar_d/2 - 0.1, arch_height_z - 14])
            rotate([90, 0, 0]) linear_extrude(0.5) text("BAR/dB", size = 2.6, font = "Consolas:style=Bold", halign = "center");
    }

    // 6. LEFT & RIGHT PILLAR CONTROLS: 3x STACKED BRASS DIALS + BULKHEAD JACKS + LOWER COILED CORDS
    for (sx = [-1, 1]) {
        px = sx * (arch_span_x/2);
        // 3x Stacked Knurled Brass Rotary Dials per Pillar (Z = 205, 258, 310 mm)
        for (idx = [0 : 2]) {
            kz = 205 + idx * 52;
            color(c_anduril_slate)
            translate([px, -pillar_d/2 + 0.5, kz])
                rotate([90, 0, 0]) cylinder(d = 21.0, h = 1.2, $fn = 32);
            translate([px, -pillar_d/2 - 0.8, kz])
                rotate([90, 0, 0]) knurled_brass_dial(9.2, 9.5);
            // Calibration Scale Dots around each pillar knob
            color(c_anduril_slate)
            for (da = [-110 : 25 : 110]) {
                translate([px, -pillar_d/2 - 0.2, kz])
                    rotate([90, da, 0])
                        translate([0, 12.2, 0])
                            cylinder(d = 1.2, h = 0.5, $fn = 12);
            }
            // Pillar Micro-Label above/below each knob
            color(c_anduril_slate)
            translate([px, -pillar_d/2 - 0.2, kz - 17])
                rotate([90, 0, 0])
                    linear_extrude(0.5)
                        text(idx == 0 ? "LED 105" : (idx == 1 ? "PHASE ALIGN" : "10B GAIN"), size = 2.5, font = "Consolas:style=Bold", halign = "center");
        }
        // Green Status LED beside lower pillar knob
        color([0.15, 0.88, 0.35])
        translate([px - 14, -pillar_d/2 - 0.4, 205])
            rotate([90, 0, 0]) cylinder(d = 2.6, h = 1.0, $fn = 16);

        // Lower Front Umbilical Bulkhead Strain-Relief Jack at Z = 152 mm
        color(c_anduril_slate) {
            translate([px, -pillar_d/2 + 1.0, 152])
                rotate([90, 0, 0]) cylinder(d = 14.5, h = 7.0, $fn = 28);
            translate([px, -pillar_d/2 - 6.0, 152])
                sphere(d = 12.5, $fn = 24);
            // Vertical Coiled Black Umbilical Cord running from Z = 150 mm down to Z = -74 mm
            for (cz = [-74 : 5.2 : 148]) {
                translate([px, -pillar_d/2 - 7.5, cz])
                    cylinder(d = 10.2, h = 3.6, $fn = 24);
                translate([px, -pillar_d/2 - 7.5, cz + 3.0])
                    cylinder(d = 6.5, h = 2.6, $fn = 20);
            }
        }
    }

    // 7. UPPER CURVED CORRUGATED UMBILICAL CONDUITS (INNER PILLAR -> SUSPENDED 4-TILE ARRAY)
    //    Matches the signature S-curved black ribbed conduits in render_halo_stand_rig.jpg!
    color(c_anduril_slate)
    for (sx = [-1, 1]) {
        // Inner pillar angled bulkhead socket at Z = 190 mm
        translate([sx * (arch_span_x/2 - pillar_w/2 + 2), 0, 190])
            rotate([0, -sx * 65, 0])
                cylinder(d = 16.0, h = 12.0, $fn = 24);

        // 32 Corrugated Rib Segments along a smooth curved spline from pillar wall to top bridge
        for (i = [0 : 32]) {
            t = i / 32.0;
            // Smooth curve hugging the inner arch shoulder and entering near the orange collar band
            cx = sx * ((arch_span_x/2 - pillar_w/2 - 14) - pow(t, 1.7) * 58 + sin(t * 180) * 6);
            cz = 194 + t * 196 + sin(t * 165) * 18;
            ang_y = -sx * (12 + t * 68);
            translate([cx, 0, cz])
                rotate([0, ang_y, 0]) {
                    cylinder(d = 15.2, h = 3.8, $fn = 24);
                    translate([0, 0, 3.2])
                        cylinder(d = 10.5, h = 3.2, $fn = 20);
                }
        }
    }

    // 8. HEAVY-DUTY TABLETOP C-CLAMPS & KNURLED LEADSCREWS AT BASE OF LEFT & RIGHT PILLARS
    for (sx = [-1, 1]) {
        px = sx * (arch_span_x/2);
        color(c_tva_cream) {
            // Base Flange sitting on Workbench Tabletop
            translate([px - pillar_w/2 - 8, -pillar_d/2 - 6, 0])
                chamfered_prism(pillar_w + 16, pillar_d + 12, 12.0, 5.0);
            // Front Vertical C-Clamp Spine Wrapping Over the Tabletop Bullnose Edge (Y = -58..-40)
            translate([px - 22, -58, -78])
                chamfered_prism(44, 18, 90.0, 3.0);
            // Lower Horizontal C-Clamp Jaw Under the Tabletop (Y = -58..+4)
            translate([px - 22, -58, -78])
                chamfered_prism(44, 62, 15.0, 3.0);
        }
        // TVA Orange Inner Base Trim Line on Pillar Foot
        color(c_tva_orange)
        translate([px - sx * (pillar_w/2 + 8.2), 0, 6.0])
            cube([2.5, pillar_d + 10, 12.0], center = true);

        // Matte-Black Threaded Clamping Leadscrew + Swivel Pad + Knurled Bottom Hand-Wheel
        color(c_anduril_slate) {
            // Threaded screw shaft at Y = -14 (under the tabletop!)
            translate([px, -14, -96])
                cylinder(d = 11.0, h = 66.0, $fn = 24);
            // Upper Swivel Clamping Pad Pressing Against Underside of Tabletop (Z = -30)
            translate([px, -14, -36])
                cylinder(d1 = 18.0, d2 = 28.0, h = 6.0, $fn = 32);
            // Lower Knurled Hand-Knob Wheel (Z = -108..-94)
            translate([px, -14, -108])
                cylinder(d = 34.0, h = 8.0, $fn = 36);
            translate([px, -14, -100])
                cylinder(d1 = 26.0, d2 = 16.0, h = 6.0, $fn = 32);
        }
    }

    // 9. UNDER-DESK "HPVE |" +12V 10A MASTER POWER SUPPLY UNIT & TWIN BRAIDED CONDUITS
    //    Mounted flush under the front right of the workbench (X = 48..180, Y = -36..+59, Z = -102..-30)
    psu_x = 48.0;
    psu_y = -36.0;
    psu_z = -102.0;
    color(c_tva_cream)
    difference() {
        translate([psu_x, psu_y, psu_z])
            chamfered_prism(132.0, 95.0, 72.0, 5.0);
        // Front Faceplate Recess Pocket
        translate([psu_x + 5.0, psu_y - 1.0, psu_z + 5.0])
            chamfered_prism(122.0, 4.0, 62.0, 3.0);
    }
    // Recessed Cream/Bone Faceplate Border
    color(c_tva_bone)
    translate([psu_x + 6.0, psu_y + 1.5, psu_z + 6.0])
        chamfered_prism(120.0, 1.5, 60.0, 2.5);
    // "HPVE |" Logo & Dual Rocker Switch on Under-Desk PSU
    color(c_anduril_slate) {
        translate([psu_x + 14.0, psu_y + 1.2, psu_z + 46.0])
            rotate([90, 0, 0])
                linear_extrude(1.0)
                    text("HPVE", size = 13.5, font = "Consolas:style=Bold", halign = "left", valign = "center");
        translate([psu_x + 14.0, psu_y + 1.2, psu_z + 31.0])
            rotate([90, 0, 0])
                linear_extrude(0.8)
                    text("12V 10A REGULATED DRV SUPPLY", size = 3.0, font = "Consolas:style=Bold", halign = "left", valign = "center");
        // Dual Black Rocker Switch & AC Module on Lower Right of PSU
        translate([psu_x + 84.0, psu_y - 0.5, psu_z + 12.0])
            cube([32.0, 4.0, 18.0]);
    }
    // Orange Bar Next to HPVE on PSU
    color(c_tva_orange)
    translate([psu_x + 68.0, psu_y + 0.8, psu_z + 38.0])
        cube([2.6, 1.2, 16.0]);
    // Knurled Brass Voltage Trim Knob on Upper Right of PSU
    translate([psu_x + 100.0, psu_y + 0.5, psu_z + 45.0])
        rotate([90, 0, 0]) knurled_brass_dial(7.8, 8.0);

    // Twin Curved Black Corrugated Power Conduits Exiting Left of PSU Box into Under-Desk
    color(c_anduril_slate)
    for (c_off = [0, 14]) {
        for (i = [0 : 16]) {
            t = i / 16.0;
            cx = psu_x - t * 68;
            cz = (psu_z + 24 + c_off) + pow(t, 1.8) * (48 - c_off);
            translate([cx, psu_y + 20, cz])
                rotate([0, -45 * t, 0]) {
                    cylinder(d = 11.5, h = 3.2, $fn = 20);
                    translate([0, 0, 2.6])
                        cylinder(d = 8.0, h = 2.4, $fn = 16);
                }
        }
    }

    // 10. OVERHEAD GIMBAL SUSPENSION STRUTS & SUSPENDED 4-TILE HPVE EMITTER HEAD
    //     Low-profile Ceramic Cream bezel collar right under the top bridge (Z = 392..404)
    color(c_tva_cream)
    difference() {
        translate([-106.0, -32.0, arch_height_z - pillar_w/2 - 10.0])
            chamfered_prism(212.0, 64.0, 12.0, 4.0);
        translate([-96.0, -28.0, arch_height_z - pillar_w/2 - 12.0])
            cube([192.0, 56.0, 16.0]);
    }
    //     Gimbal Trunnion Hanger Struts connecting Top Bridge to the 4-Tile Array's Rear Clevises
    color(c_anduril_slate)
    for (gx = [-52.0, 52.0]) {
        translate([gx - 7.0, -12.0, head_mount_z + 6.0])
            chamfered_prism(14.0, 24.0, 34.0, 2.0);
    }

    // The 4-Tile 64-Channel Emitter Array Suspended Upside-Down (Pointing Downward!)
    // Tilted forward (-22 deg around X) on its Pan-Tilt Gimbal so the 4 modular tiles,
    // 64 horn waveguides, and Central VL53L5CX ToF Sensor Pod are prominently visible!
    translate([0, -6.0, head_mount_z + 10.0])
        rotate([-22, 0, 0])
            rotate([180, 0, 0])
                quad_2x2_hero_assembly(true, false);

    // 11. FLOATING APPLE-SIZED (100 mm) VOLUMETRIC HOLOGRAM SPHERE IN MID-AIR AT Z = 175 mm!
    //     Multi-shell glowing cyan photonic boundary + warm amber-gold core + nodal rings
    translate([0, 0, 175]) {
        // Inner Amber-Gold Plasma Core
        color([0.98, 0.64, 0.14, 0.85])
            sphere(d = 62.0, $fn = 48);
        // Mid Photonic Interference Lattice Ring
        color([0.95, 0.78, 0.28, 0.65])
        rotate([25, 18, 0])
            difference() {
                sphere(d = 84.0, $fn = 48);
                sphere(d = 76.0, $fn = 48);
            }
        // Outer Glowing Cyan 450nm Volumetric Standing-Wave Shell (104 mm diameter)
        color([0.05, 0.92, 1.00, 0.38])
            sphere(d = 104.0, $fn = 64);
    }
}

// ----------------------------------------------------------------------------
// 15. TOP-LEVEL RENDER DISPATCHER (MULTI-ANGLE VIEWS + 5 PRINTABLE STL PARTS)
// ----------------------------------------------------------------------------
if (render_mode == 0) {
    // View 0: Hero Isometric View (Matched to hpve_4tile_exoskeleton_top_hero.jpg)
    quad_2x2_hero_assembly(true, false);
} else if (render_mode == 1) {
    // View 1: Top Orthographic View (64-Ch Matrix & Zero-Gap 196x176mm Grid)
    quad_2x2_hero_assembly(false, false);
} else if (render_mode == 2) {
    // View 2: Internal Cutaway View (Left Tiles Open showing PCB, ICs, and FFC Daisy-Chain)
    quad_2x2_hero_assembly(true, true);
} else if (render_mode == 3) {
    // View 3: Rear Avionics & Halo Gimbal Mount Isometric View (Flipped 180 deg)
    rotate([180, 0, 0])
        quad_2x2_hero_assembly(true, false);
} else if (render_mode == 4) {
    // View 4: Rear Orthographic View (Full Avionics Backplane, Pico, OLED, FFC & 12V Map)
    rotate([180, 0, 0])
        quad_2x2_hero_assembly(true, false);
} else if (render_mode == 5) {
    // View 5: Front-Left I/O Bulkhead Elevation (Shows USB-C Port, BOOTSEL, XT30 12V & Louvers)
    quad_2x2_hero_assembly(true, false);
} else if (render_mode == 6) {
    // View 6: Single Tile 5-Layer Exploded View
    translate([-tile_pitch_x/2, -tile_pitch_y/2, -26.0])
        single_tile_exploded_5layer();
} else if (render_mode == 7) {
    // View 7: Full 2x2 Quad Array + RP2040 Avionics Dock Exploded View
    translate([0, 0, -15.0])
        quad_2x2_full_exploded_assembly();
} else if (render_mode == 8) {
    // View 8: Macro Close-Up of Exponential Horn Waveguides & Honeycomb Mesh
    translate([-tile_pitch_x/2, -tile_pitch_y/2, 0])
        single_tile_complete(0, 0, true);
} else if (render_mode == 9) {
    // Printable STL Part A: Single Tile Exoskeleton Base Frame
    tile_exoskeleton_base_frame(0, 0, false);
} else if (render_mode == 10) {
    // Printable STL Part B: Contoured 16-Ch Exponential Horn Waveguide Bezel
    translate([-bezel_x0, -bezel_y0, -bezel_bottom_z])
        acoustic_horn_bezel_plate(false);
} else if (render_mode == 11) {
    // Printable STL Part C: 2x2 Quad Exoskeleton Base Assembly
    for (qx = [0 : 1]) {
        for (qy = [0 : 1]) {
            translate([(qx - 1) * tile_pitch_x, (qy - 1) * tile_pitch_y, 0])
                tile_exoskeleton_base_frame(qx, qy, false);
        }
    }
} else if (render_mode == 12) {
    // View 12 (Task 7.3): Complete Portable Overhead Desktop Halo Arch Stand & Suspended 4-Tile Array
    desktop_halo_arch_stand_complete();
} else if (render_mode == 13) {
    // View 13 (Task 7.4): Macro Close-Up of Central Optical & VL53L5CX ToF Sensor Docking Bay at (0, 0)
    quad_2x2_hero_assembly(true, false);
} else if (render_mode == 14) {
    // Printable STL Part D (Task 7.4): Central Optical & VL53L5CX ToF Sensor Pod
    translate([0, 0, -(frame_total_h - 1.5)])
        central_optical_tof_sensor_pod(false);
} else if (render_mode == 15) {
    // Printable STL Part E (Task 7.3): Heavy-Duty Tabletop C-Clamp & Pillar Base Bracket
    translate([0, 0, 76.0])
        halo_arch_c_clamp_bracket_stl();
}
