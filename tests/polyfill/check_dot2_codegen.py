# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import argparse
import re
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument('--objdump', required=True)
parser.add_argument('--binary', required=True)
parser.add_argument('--architecture', required=True)
args = parser.parse_args()
assembly = subprocess.run([args.objdump, '-d', '--no-show-raw-insn', args.binary],
                          check=True, text=True, capture_output=True).stdout
match = re.search(r'^[0-9a-f]+ <_?polyfill_dot2_hardware>:\n(.*?)(?=^[0-9a-f]+ <|\Z)',
                  assembly, re.MULTILINE | re.DOTALL)
if not match:
    raise SystemExit('Native logical dot2 witness missing')
opcode = r'\bbfdot' if args.architecture in ('aarch64', 'arm64', 'ARM64') else r'\bvdpbf16ps'
if not re.search(opcode, match.group(1)):
    raise SystemExit('Permitted BF16 hardware path lost its native dot instruction')
print('Native logical BF16 dot instruction present')
