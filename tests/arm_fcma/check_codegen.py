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
checks = {'native_fcadd_32_2_90': '\\bfcadd\\s+v\\d+\\.2s,.*#(?:90|0x5a)\\b',
 'native_fcadd_32_2_270': '\\bfcadd\\s+v\\d+\\.2s,.*#(?:270|0x10e)\\b',
 'native_fcmla_32_2_0': '\\bfcmla\\s+v\\d+\\.2s,.*#(?:0|0x0)\\b',
 'native_fcmla_32_2_90': '\\bfcmla\\s+v\\d+\\.2s,.*#(?:90|0x5a)\\b',
 'native_fcmla_32_2_180': '\\bfcmla\\s+v\\d+\\.2s,.*#(?:180|0xb4)\\b',
 'native_fcmla_32_2_270': '\\bfcmla\\s+v\\d+\\.2s,.*#(?:270|0x10e)\\b',
 'native_fcadd_32_4_90': '\\bfcadd\\s+v\\d+\\.4s,.*#(?:90|0x5a)\\b',
 'native_fcadd_32_4_270': '\\bfcadd\\s+v\\d+\\.4s,.*#(?:270|0x10e)\\b',
 'native_fcmla_32_4_0': '\\bfcmla\\s+v\\d+\\.4s,.*#(?:0|0x0)\\b',
 'native_fcmla_32_4_90': '\\bfcmla\\s+v\\d+\\.4s,.*#(?:90|0x5a)\\b',
 'native_fcmla_32_4_180': '\\bfcmla\\s+v\\d+\\.4s,.*#(?:180|0xb4)\\b',
 'native_fcmla_32_4_270': '\\bfcmla\\s+v\\d+\\.4s,.*#(?:270|0x10e)\\b',
 'native_fcadd_64_2_90': '\\bfcadd\\s+v\\d+\\.2d,.*#(?:90|0x5a)\\b',
 'native_fcadd_64_2_270': '\\bfcadd\\s+v\\d+\\.2d,.*#(?:270|0x10e)\\b',
 'native_fcmla_64_2_0': '\\bfcmla\\s+v\\d+\\.2d,.*#(?:0|0x0)\\b',
 'native_fcmla_64_2_90': '\\bfcmla\\s+v\\d+\\.2d,.*#(?:90|0x5a)\\b',
 'native_fcmla_64_2_180': '\\bfcmla\\s+v\\d+\\.2d,.*#(?:180|0xb4)\\b',
 'native_fcmla_64_2_270': '\\bfcmla\\s+v\\d+\\.2d,.*#(?:270|0x10e)\\b',
 'native_fcadd_16_4_90': '\\bfcadd\\s+v\\d+\\.4h,.*#(?:90|0x5a)\\b',
 'native_fcadd_16_4_270': '\\bfcadd\\s+v\\d+\\.4h,.*#(?:270|0x10e)\\b',
 'native_fcmla_16_4_0': '\\bfcmla\\s+v\\d+\\.4h,.*#(?:0|0x0)\\b',
 'native_fcmla_16_4_90': '\\bfcmla\\s+v\\d+\\.4h,.*#(?:90|0x5a)\\b',
 'native_fcmla_16_4_180': '\\bfcmla\\s+v\\d+\\.4h,.*#(?:180|0xb4)\\b',
 'native_fcmla_16_4_270': '\\bfcmla\\s+v\\d+\\.4h,.*#(?:270|0x10e)\\b',
 'native_fcadd_16_8_90': '\\bfcadd\\s+v\\d+\\.8h,.*#(?:90|0x5a)\\b',
 'native_fcadd_16_8_270': '\\bfcadd\\s+v\\d+\\.8h,.*#(?:270|0x10e)\\b',
 'native_fcmla_16_8_0': '\\bfcmla\\s+v\\d+\\.8h,.*#(?:0|0x0)\\b',
 'native_fcmla_16_8_90': '\\bfcmla\\s+v\\d+\\.8h,.*#(?:90|0x5a)\\b',
 'native_fcmla_16_8_180': '\\bfcmla\\s+v\\d+\\.8h,.*#(?:180|0xb4)\\b',
 'native_fcmla_16_8_270': '\\bfcmla\\s+v\\d+\\.8h,.*#(?:270|0x10e)\\b'}
checks.update({
    'native_fcmla_32_2_from2_lane0_0': '\\bfcmla\\s+v\\d+\\.2s,.*#(?:0|0x0)\\b',
    'native_fcmla_32_2_from2_lane0_90': '\\bfcmla\\s+v\\d+\\.2s,.*#(?:90|0x5a)\\b',
    'native_fcmla_32_2_from2_lane0_180': '\\bfcmla\\s+v\\d+\\.2s,.*#(?:180|0xb4)\\b',
    'native_fcmla_32_2_from2_lane0_270': '\\bfcmla\\s+v\\d+\\.2s,.*#(?:270|0x10e)\\b',
    'native_fcmla_32_2_from4_lane0_0': '\\bfcmla\\s+v\\d+\\.2s,.*#(?:0|0x0)\\b',
    'native_fcmla_32_2_from4_lane0_90': '\\bfcmla\\s+v\\d+\\.2s,.*#(?:90|0x5a)\\b',
    'native_fcmla_32_2_from4_lane0_180': '\\bfcmla\\s+v\\d+\\.2s,.*#(?:180|0xb4)\\b',
    'native_fcmla_32_2_from4_lane0_270': '\\bfcmla\\s+v\\d+\\.2s,.*#(?:270|0x10e)\\b',
    'native_fcmla_32_2_from4_lane1_0': '\\bfcmla\\s+v\\d+\\.2s,.*#(?:0|0x0)\\b',
    'native_fcmla_32_2_from4_lane1_90': '\\bfcmla\\s+v\\d+\\.2s,.*#(?:90|0x5a)\\b',
    'native_fcmla_32_2_from4_lane1_180': '\\bfcmla\\s+v\\d+\\.2s,.*#(?:180|0xb4)\\b',
    'native_fcmla_32_2_from4_lane1_270': '\\bfcmla\\s+v\\d+\\.2s,.*#(?:270|0x10e)\\b',
    'native_fcmla_32_4_from2_lane0_0': '\\bfcmla\\s+v\\d+\\.4s,.*\\.s\\[0\\],\\s*#(?:0|0x0)\\b',
    'native_fcmla_32_4_from2_lane0_90': '\\bfcmla\\s+v\\d+\\.4s,.*\\.s\\[0\\],\\s*#(?:90|0x5a)\\b',
    'native_fcmla_32_4_from2_lane0_180': '\\bfcmla\\s+v\\d+\\.4s,.*\\.s\\[0\\],\\s*#(?:180|0xb4)\\b',
    'native_fcmla_32_4_from2_lane0_270': '\\bfcmla\\s+v\\d+\\.4s,.*\\.s\\[0\\],\\s*#(?:270|0x10e)\\b',
    'native_fcmla_32_4_from4_lane0_0': '\\bfcmla\\s+v\\d+\\.4s,.*\\.s\\[0\\],\\s*#(?:0|0x0)\\b',
    'native_fcmla_32_4_from4_lane0_90': '\\bfcmla\\s+v\\d+\\.4s,.*\\.s\\[0\\],\\s*#(?:90|0x5a)\\b',
    'native_fcmla_32_4_from4_lane0_180': '\\bfcmla\\s+v\\d+\\.4s,.*\\.s\\[0\\],\\s*#(?:180|0xb4)\\b',
    'native_fcmla_32_4_from4_lane0_270': '\\bfcmla\\s+v\\d+\\.4s,.*\\.s\\[0\\],\\s*#(?:270|0x10e)\\b',
    'native_fcmla_32_4_from4_lane1_0': '\\bfcmla\\s+v\\d+\\.4s,.*\\.s\\[1\\],\\s*#(?:0|0x0)\\b',
    'native_fcmla_32_4_from4_lane1_90': '\\bfcmla\\s+v\\d+\\.4s,.*\\.s\\[1\\],\\s*#(?:90|0x5a)\\b',
    'native_fcmla_32_4_from4_lane1_180': '\\bfcmla\\s+v\\d+\\.4s,.*\\.s\\[1\\],\\s*#(?:180|0xb4)\\b',
    'native_fcmla_32_4_from4_lane1_270': '\\bfcmla\\s+v\\d+\\.4s,.*\\.s\\[1\\],\\s*#(?:270|0x10e)\\b',
    'native_fcmla_16_4_from4_lane0_0': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[0\\],\\s*#(?:0|0x0)\\b',
    'native_fcmla_16_4_from4_lane0_90': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[0\\],\\s*#(?:90|0x5a)\\b',
    'native_fcmla_16_4_from4_lane0_180': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[0\\],\\s*#(?:180|0xb4)\\b',
    'native_fcmla_16_4_from4_lane0_270': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[0\\],\\s*#(?:270|0x10e)\\b',
    'native_fcmla_16_4_from4_lane1_0': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[1\\],\\s*#(?:0|0x0)\\b',
    'native_fcmla_16_4_from4_lane1_90': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[1\\],\\s*#(?:90|0x5a)\\b',
    'native_fcmla_16_4_from4_lane1_180': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[1\\],\\s*#(?:180|0xb4)\\b',
    'native_fcmla_16_4_from4_lane1_270': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[1\\],\\s*#(?:270|0x10e)\\b',
    'native_fcmla_16_4_from8_lane0_0': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[0\\],\\s*#(?:0|0x0)\\b',
    'native_fcmla_16_4_from8_lane0_90': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[0\\],\\s*#(?:90|0x5a)\\b',
    'native_fcmla_16_4_from8_lane0_180': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[0\\],\\s*#(?:180|0xb4)\\b',
    'native_fcmla_16_4_from8_lane0_270': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[0\\],\\s*#(?:270|0x10e)\\b',
    'native_fcmla_16_4_from8_lane1_0': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[1\\],\\s*#(?:0|0x0)\\b',
    'native_fcmla_16_4_from8_lane1_90': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[1\\],\\s*#(?:90|0x5a)\\b',
    'native_fcmla_16_4_from8_lane1_180': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[1\\],\\s*#(?:180|0xb4)\\b',
    'native_fcmla_16_4_from8_lane1_270': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[1\\],\\s*#(?:270|0x10e)\\b',
    'native_fcmla_16_4_from8_lane2_0': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[0\\],\\s*#(?:0|0x0)\\b',
    'native_fcmla_16_4_from8_lane2_90': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[0\\],\\s*#(?:90|0x5a)\\b',
    'native_fcmla_16_4_from8_lane2_180': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[0\\],\\s*#(?:180|0xb4)\\b',
    'native_fcmla_16_4_from8_lane2_270': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[0\\],\\s*#(?:270|0x10e)\\b',
    'native_fcmla_16_4_from8_lane3_0': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[1\\],\\s*#(?:0|0x0)\\b',
    'native_fcmla_16_4_from8_lane3_90': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[1\\],\\s*#(?:90|0x5a)\\b',
    'native_fcmla_16_4_from8_lane3_180': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[1\\],\\s*#(?:180|0xb4)\\b',
    'native_fcmla_16_4_from8_lane3_270': '\\bfcmla\\s+v\\d+\\.4h,.*\\.h\\[1\\],\\s*#(?:270|0x10e)\\b',
    'native_fcmla_16_8_from4_lane0_0': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[0\\],\\s*#(?:0|0x0)\\b',
    'native_fcmla_16_8_from4_lane0_90': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[0\\],\\s*#(?:90|0x5a)\\b',
    'native_fcmla_16_8_from4_lane0_180': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[0\\],\\s*#(?:180|0xb4)\\b',
    'native_fcmla_16_8_from4_lane0_270': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[0\\],\\s*#(?:270|0x10e)\\b',
    'native_fcmla_16_8_from4_lane1_0': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[1\\],\\s*#(?:0|0x0)\\b',
    'native_fcmla_16_8_from4_lane1_90': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[1\\],\\s*#(?:90|0x5a)\\b',
    'native_fcmla_16_8_from4_lane1_180': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[1\\],\\s*#(?:180|0xb4)\\b',
    'native_fcmla_16_8_from4_lane1_270': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[1\\],\\s*#(?:270|0x10e)\\b',
    'native_fcmla_16_8_from8_lane0_0': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[0\\],\\s*#(?:0|0x0)\\b',
    'native_fcmla_16_8_from8_lane0_90': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[0\\],\\s*#(?:90|0x5a)\\b',
    'native_fcmla_16_8_from8_lane0_180': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[0\\],\\s*#(?:180|0xb4)\\b',
    'native_fcmla_16_8_from8_lane0_270': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[0\\],\\s*#(?:270|0x10e)\\b',
    'native_fcmla_16_8_from8_lane1_0': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[1\\],\\s*#(?:0|0x0)\\b',
    'native_fcmla_16_8_from8_lane1_90': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[1\\],\\s*#(?:90|0x5a)\\b',
    'native_fcmla_16_8_from8_lane1_180': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[1\\],\\s*#(?:180|0xb4)\\b',
    'native_fcmla_16_8_from8_lane1_270': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[1\\],\\s*#(?:270|0x10e)\\b',
    'native_fcmla_16_8_from8_lane2_0': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[2\\],\\s*#(?:0|0x0)\\b',
    'native_fcmla_16_8_from8_lane2_90': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[2\\],\\s*#(?:90|0x5a)\\b',
    'native_fcmla_16_8_from8_lane2_180': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[2\\],\\s*#(?:180|0xb4)\\b',
    'native_fcmla_16_8_from8_lane2_270': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[2\\],\\s*#(?:270|0x10e)\\b',
    'native_fcmla_16_8_from8_lane3_0': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[3\\],\\s*#(?:0|0x0)\\b',
    'native_fcmla_16_8_from8_lane3_90': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[3\\],\\s*#(?:90|0x5a)\\b',
    'native_fcmla_16_8_from8_lane3_180': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[3\\],\\s*#(?:180|0xb4)\\b',
    'native_fcmla_16_8_from8_lane3_270': '\\bfcmla\\s+v\\d+\\.8h,.*\\.h\\[3\\],\\s*#(?:270|0x10e)\\b',
})
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
