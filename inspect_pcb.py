import pcbnew

board = pcbnew.LoadBoard("hardware/kicad/HPVE_Controller.kicad_pcb")
fps = list(board.GetFootprints())
min_x = min(fp.GetPosition().x for fp in fps) / 1e6
max_x = max(fp.GetPosition().x for fp in fps) / 1e6
min_y = min(fp.GetPosition().y for fp in fps) / 1e6
max_y = max(fp.GetPosition().y for fp in fps) / 1e6
print(f"Total footprints: {len(fps)}")
print(f"Footprints bbox: X: [{min_x:.2f}, {max_x:.2f}] mm (width {max_x-min_x:.2f} mm), Y: [{min_y:.2f}, {max_y:.2f}] mm (height {max_y-min_y:.2f} mm)")

# Check drawings on Edge_Cuts
edge_cuts = [d for d in board.GetDrawings() if d.GetLayer() == pcbnew.Edge_Cuts]
print(f"Drawings on Edge_Cuts: {len(edge_cuts)}")
for d in edge_cuts:
    bbox = d.GetBoundingBox()
    print(f"  Shape: {d.GetShapeStr()}, BBox: X=[{bbox.GetX()/1e6:.2f}, {(bbox.GetX()+bbox.GetWidth())/1e6:.2f}], Y=[{bbox.GetY()/1e6:.2f}, {(bbox.GetY()+bbox.GetHeight())/1e6:.2f}] mm")

# Sample some footprint positions:
print("\nSample footprint positions:")
for ref in ["TX1", "TX8", "TX57", "TX64", "U1", "U8", "U10", "U101", "U132"]:
    fp = board.FindFootprintByReference(ref)
    if fp:
        pos = fp.GetPosition()
        print(f"  {ref}: ({pos.x/1e6:.2f}, {pos.y/1e6:.2f}) mm")
