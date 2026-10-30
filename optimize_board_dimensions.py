import pcbnew
import math

pcb_path = "hardware/kicad/HPVE_Controller.kicad_pcb"
print(f"Loading board: {pcb_path}")
board = pcbnew.LoadBoard(pcb_path)

# 1. Remove existing Edge_Cuts drawings and silkscreen text
drawings_to_remove = []
for d in board.GetDrawings():
    layer = d.GetLayer()
    if layer == pcbnew.Edge_Cuts:
        drawings_to_remove.append(d)
    elif layer == pcbnew.F_SilkS and d.GetClass() == "PCB_TEXT":
        drawings_to_remove.append(d)

for d in drawings_to_remove:
    board.Remove(d)
print(f"Removed {len(drawings_to_remove)} old drawings/Edge_Cuts.")

# 2. Remove old 3.2mm mounting vias
vias_to_remove = []
for t in board.GetTracks():
    if t.GetClass() == "PCB_VIA":
        if t.GetDrill() > pcbnew.FromMM(2.0):
            vias_to_remove.append(t)

for v in vias_to_remove:
    board.Remove(v)
print(f"Removed {len(vias_to_remove)} old mounting vias.")

# 3. Define new compact board boundary
# Footprint range: X: [75.08, 166.64], Y: [57.55, 128.82]
# Tight, perfectly balanced 108mm x 88mm outline
x_min = 67.0
x_max = 175.0
y_min = 49.0
y_max = 137.0

# Create 4 boundary lines on Edge_Cuts
corners = [
    (pcbnew.VECTOR2I(pcbnew.FromMM(x_min), pcbnew.FromMM(y_min)), pcbnew.VECTOR2I(pcbnew.FromMM(x_max), pcbnew.FromMM(y_min))),
    (pcbnew.VECTOR2I(pcbnew.FromMM(x_max), pcbnew.FromMM(y_min)), pcbnew.VECTOR2I(pcbnew.FromMM(x_max), pcbnew.FromMM(y_max))),
    (pcbnew.VECTOR2I(pcbnew.FromMM(x_max), pcbnew.FromMM(y_max)), pcbnew.VECTOR2I(pcbnew.FromMM(x_min), pcbnew.FromMM(y_max))),
    (pcbnew.VECTOR2I(pcbnew.FromMM(x_min), pcbnew.FromMM(y_max)), pcbnew.VECTOR2I(pcbnew.FromMM(x_min), pcbnew.FromMM(y_min)))
]

for p_start, p_end in corners:
    seg = pcbnew.PCB_SHAPE(board)
    seg.SetShape(pcbnew.SHAPE_T_SEGMENT)
    seg.SetStart(p_start)
    seg.SetEnd(p_end)
    seg.SetLayer(pcbnew.Edge_Cuts)
    seg.SetWidth(pcbnew.FromMM(0.15))
    board.Add(seg)

print(f"Added tight 108mm x 88mm Edge_Cuts outline.")

# 4. Add 4 M3 mounting holes (3.2mm drill, 6.0mm pad ring)
m3_positions = [
    (71.0, 53.0),   # Top-Left
    (171.0, 53.0),  # Top-Right
    (71.0, 133.0),  # Bottom-Left
    (171.0, 133.0)  # Bottom-Right
]

for mx, my in m3_positions:
    via = pcbnew.PCB_VIA(board)
    via.SetPosition(pcbnew.VECTOR2I(pcbnew.FromMM(mx), pcbnew.FromMM(my)))
    via.SetViaType(pcbnew.VIATYPE_THROUGH)
    via.SetDrill(pcbnew.FromMM(3.2))
    via.SetWidth(pcbnew.FromMM(6.0))
    board.Add(via)

print(f"Added 4x M3 mounting holes at corners.")

# 5. Add beautiful silkscreen labels
def add_silk_text(text, x_mm, y_mm, size_mm=1.3, thickness_mm=0.2):
    txt = pcbnew.PCB_TEXT(board)
    txt.SetText(text)
    txt.SetPosition(pcbnew.VECTOR2I(pcbnew.FromMM(x_mm), pcbnew.FromMM(y_mm)))
    txt.SetLayer(pcbnew.F_SilkS)
    txt.SetTextSize(pcbnew.VECTOR2I(pcbnew.FromMM(size_mm), pcbnew.FromMM(size_mm)))
    txt.SetTextThickness(pcbnew.FromMM(thickness_mm))
    txt.SetHorizJustify(pcbnew.GR_TEXT_H_ALIGN_CENTER)
    board.Add(txt)

# Perfectly centered, professional silkscreen markings
add_silk_text("HPVE CONTROLLER MOTHERBOARD REV 1.0", 121.0, 51.5, size_mm=1.3, thickness_mm=0.2)
add_silk_text("IRON CAVE INDUSTRIES | HACK CLUB FALLEN 2026", 121.0, 53.8, size_mm=1.0, thickness_mm=0.16)
add_silk_text("64-CHANNEL 40kHz PHASED ULTRASONIC EMITTER", 121.0, 134.5, size_mm=1.1, thickness_mm=0.18)

# Save board
board.Save(pcb_path)
print("Board saved with compact dimensions and clean silkscreen!")
