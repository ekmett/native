# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""The assembly comparison keeps literal contents and added operations visible."""
import contextlib
import io
from pathlib import Path
import runpy
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch


def sample(kind="plain", different=False, extra=False):
    text = ""
    if kind == "macho":
        second = "00000081" if different else "00000080"
        text += "Contents of section __TEXT,__literal16:\n"
        text += " 1000 00000080 00000080 00000080 00000080\n"
        text += f" 1010 {second} 00000080 00000080 00000080\n"
    elif kind == "elf":
        second = "00000081" if different else "00000080"
        text += "SYMBOL TABLE:\n00000000 l .rodata.cst16 00000000 .LCPI0_0\n"
        text += "00000010 l .rodata.cst16 00000000 .LCPI1_0\n"
        text += "Contents of section .rodata.cst16:\n"
        text += " 0000 00000080 00000080 00000080 00000080\n"
        text += f" 0010 {second} 00000080 00000080 00000080\n"
    text += "Disassembly of section text:\n"
    for i in range(24):
        for raw in range(2):
            address = (2*i+raw)*32
            name = f"native_float_arm4_test_{i}" + ("_raw" if raw else "")
            text += f"\n{address:08x} <{name}>:\n"
            if i == 0 and kind == "macho":
                target = 0x1000+16*raw
                text += f" {address:x}: xorps 0x{target-address-7:x}(%rip), %xmm0 ## 0x{target:x}\n"
                text += f" {address+3:x}: X86_64_RELOC_SIGNED __literal16\n"
            elif i == 0 and kind == "elf":
                text += f" {address:x}: xorps (%rip), %xmm0 # 0x{address+7:x} <{name}+0x7>\n"
                text += f" {address+3:x}: R_X86_64_PC32 .LCPI{raw}_0-0x4\n"
            else:
                immediate = 1 if different and i == 0 and raw else 0
                text += f" {address:x}: roundps $0x{immediate:x}, %xmm0, %xmm0\n"
            if extra and i == 0 and not raw:
                text += f" {address+8:x}: movups (%rax), %xmm1\n"
            text += f" {address+12:x}: retq\n"
    return text


class CodegenControls(unittest.TestCase):
    def check(self, text):
        with tempfile.TemporaryDirectory() as temporary:
            arguments = ["check_codegen.py", "--objdump", "mock", "--output", str(Path(temporary)/"assembly"), "object"]
            result = subprocess.CompletedProcess([], 0, text, "")
            with patch.object(sys, "argv", arguments), patch("subprocess.run", return_value=result), contextlib.redirect_stdout(io.StringIO()):
                runpy.run_path(str(Path(__file__).with_name("check_codegen.py")), run_name="__main__")

    def test_equal_literal_values(self):
        for kind in ("macho", "elf"):
            with self.subTest(kind=kind):
                self.check(sample(kind))

    def test_different_literal_rejected(self):
        for kind in ("macho", "elf"):
            with self.subTest(kind=kind), self.assertRaises(SystemExit):
                self.check(sample(kind, different=True))

    def test_added_memory_instruction_rejected(self):
        with self.assertRaises(SystemExit):
            self.check(sample(extra=True))

    def test_different_rounding_immediate_rejected(self):
        with self.assertRaises(SystemExit):
            self.check(sample(different=True))


if __name__ == "__main__":
    unittest.main()
