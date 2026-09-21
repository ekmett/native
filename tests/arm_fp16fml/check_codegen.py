# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Inspect independently targeted wrappers emitted from an ARMv8-A translation unit."""
import argparse
import pathlib
import re
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("--objdump", required=True)
parser.add_argument("--output", required=True, type=pathlib.Path)
parser.add_argument("objects", nargs="+")
args = parser.parse_args()
result = subprocess.run([args.objdump, "--syms", "--disassemble", "--no-show-raw-insn",
                         "--triple=aarch64-none-linux-gnu",
                         *args.objects], capture_output=True, text=True)
args.output.write_text(result.stdout + result.stderr, encoding="utf-8")
if result.returncode:
    raise SystemExit(result.stderr)
parts = re.split(r"(?m)^([0-9a-f]+) <([^\n]+)>:\s*$", result.stdout)
bodies = {parts[i + 1].lstrip("_"): parts[i + 2] for i in range(1, len(parts), 3)}
addresses = {int(parts[i], 16): parts[i + 2] for i in range(1, len(parts), 3)}
for address, name in re.findall(r"(?m)^([0-9a-f]+)\s+[gw]\s+F\s+\S+\s+(_?native_\w+)$", result.stdout):
    if int(address, 16) in addresses:
        bodies.setdefault(name.lstrip("_"), addresses[int(address, 16)])
checks = {'native_fmlal_2': '\\bfmlal\\s+v\\d+\\.2s,',
 'native_fmlal_2_lane': '\\bfmlal\\s+v\\d+\\.2s,.*\\.h\\[7\\]',
 'native_fmlal_4': '\\bfmlal\\s+v\\d+\\.4s,',
 'native_fmlal_4_lane': '\\bfmlal\\s+v\\d+\\.4s,.*\\.h\\[7\\]',
 'native_fmlal2_2': '\\bfmlal2\\s+v\\d+\\.2s,',
 'native_fmlal2_2_lane': '\\bfmlal2\\s+v\\d+\\.2s,.*\\.h\\[7\\]',
 'native_fmlal2_4': '\\bfmlal2\\s+v\\d+\\.4s,',
 'native_fmlal2_4_lane': '\\bfmlal2\\s+v\\d+\\.4s,.*\\.h\\[7\\]',
 'native_fmlsl_2': '\\bfmlsl\\s+v\\d+\\.2s,',
 'native_fmlsl_2_lane': '\\bfmlsl\\s+v\\d+\\.2s,.*\\.h\\[7\\]',
 'native_fmlsl_4': '\\bfmlsl\\s+v\\d+\\.4s,',
 'native_fmlsl_4_lane': '\\bfmlsl\\s+v\\d+\\.4s,.*\\.h\\[7\\]',
 'native_fmlsl2_2': '\\bfmlsl2\\s+v\\d+\\.2s,',
 'native_fmlsl2_2_lane': '\\bfmlsl2\\s+v\\d+\\.2s,.*\\.h\\[7\\]',
 'native_fmlsl2_4': '\\bfmlsl2\\s+v\\d+\\.4s,',
 'native_fmlsl2_4_lane': '\\bfmlsl2\\s+v\\d+\\.4s,.*\\.h\\[7\\]'}
for name, pattern in checks.items():
    if not re.search(pattern, bodies.get(name, "")):
        raise SystemExit("Missing expected instruction in " + name + ": " + bodies.get(name, "absent"))
    if re.search(r"\bbl\b", bodies[name]):
        raise SystemExit("Unexpected out-of-line call in " + name)
if "native_baseline" not in bodies:
    raise SystemExit("Missing baseline control")
if re.search(r"\b(?:fmlal2?|fmlsl2?|fcmla|fcadd)\b", bodies["native_baseline"]):
    raise SystemExit("Optional instruction leaked into baseline function")
print(f"Verified {len(checks)} instruction forms and baseline isolation")
