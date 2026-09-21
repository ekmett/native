#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Exercise baseline rejection through the ADX disassembly checker's entry modes."""
from contextlib import redirect_stdout
import io
from pathlib import Path
import runpy
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch


class BaselineChecks(unittest.TestCase):
    entries = (
        ((), "native_adx_baseline"),
        (("--baseline-only",), "native_adx_baseline_import"),
        (("--baseline-only",), "main"),
        (("--admission-only",), "native_adx_admission"),
    )

    def check(self, flags, entry, opcode):
        bodies = {
            "native_adx_baseline": ["movl %edi, %eax"],
            "native_adx_baseline_import": ["movl %edi, %eax"],
            "native_adx_admission": ["movl %edi, %eax"],
            "main": ["xorl %eax, %eax"],
            "native_addcarryx_32": ["adcl %edx, %eax"],
            "native_addcarryx_64": ["adcq %rdx, %rax"],
            "native_chain_32": ["adcl %edx, %eax"] * 3,
            "native_chain_64": ["adcq %rdx, %rax"] * 3,
        }
        suffix = "%rdx, %rax" if opcode.endswith("q") else "%edx, %eax"
        bodies[entry] = [f"{opcode} {suffix}"]
        disassembly = "fixture.o: file format elf64-x86-64\n\n"
        for name, instructions in bodies.items():
            disassembly += f"0000000000000000 <{name}>:\n"
            for index, instruction in enumerate(instructions + ["retq"]):
                disassembly += f"  {index:x}: {instruction}\n"
            disassembly += "\n"
        completed = subprocess.CompletedProcess([], 0, disassembly, "")
        checker = Path(__file__).with_name("check_codegen.py")
        with tempfile.TemporaryDirectory() as directory:
            command = [str(checker), "--objdump", "controlled-objdump", "--output",
                       str(Path(directory) / "output.disassembly"), *flags, "fixture.o"]
            with patch.object(sys, "argv", command), \
                 patch("subprocess.run", return_value=completed), \
                 redirect_stdout(io.StringIO()):
                try:
                    runpy.run_path(str(checker), run_name="__main__")
                except SystemExit as error:
                    return error.code
        return 0

    def test_adcx_and_adox_rejected_from_every_baseline_entry(self):
        for flags, entry in self.entries:
            for opcode in ("adcxl", "adcxq", "adoxl", "adoxq"):
                with self.subTest(entry=entry, opcode=opcode):
                    result = self.check(flags, entry, opcode)
                    self.assertIsInstance(result, str)
                    self.assertIn("optional instruction", result)
                    self.assertIn(opcode, result)

    def test_ordinary_adc_remains_available_at_baseline(self):
        for flags, entry in self.entries:
            for opcode in ("adcl", "adcq"):
                with self.subTest(entry=entry, opcode=opcode):
                    self.assertEqual(self.check(flags, entry, opcode), 0)


if __name__ == "__main__":
    unittest.main()
