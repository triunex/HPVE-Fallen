import re

pcb_path = r"n:\Iron Cave Industries\HPVE_Project\hardware\modular_tile\HPVE_Tile_16Ch.kicad_pcb"
with open(pcb_path, "r", encoding="utf-8") as f:
    content = f.read()

# Replace model with absolute path to 400ST160.wrl
model_block = """\t\t(attr through_hole)
\t\t(model "n:/Iron Cave Industries/HPVE_Project/hardware/modular_tile/400ST160.wrl"
\t\t\t(offset
\t\t\t\t(xyz 0 0 0)
\t\t\t)
\t\t\t(scale
\t\t\t\t(xyz 1 1 1)
\t\t\t)
\t\t\t(rotate
\t\t\t\t(xyz 0 0 0)
\t\t\t)
\t\t)
\t)"""

pattern = r'(\(property "Value" "400ST160_FEMALE_SOCKET"[^)]*\)(?:[^\n]*\n)*?[ \t]*\(embedded_fonts no\)\n)(?:[ \t]*\(attr [^\n]*\n)?(?:[ \t]*\(model (?:[^\n]*\n)*?[ \t]*\)\n)?(\t\))'

matches = re.findall(pattern, content)
print(f"Found {len(matches)} TX footprints to update.")

def replacer(match):
    return match.group(1) + model_block

new_content = re.sub(pattern, replacer, content)

with open(pcb_path, "w", encoding="utf-8") as f:
    f.write(new_content)

print("Updated with custom 400ST160.wrl!")
