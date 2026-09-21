# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check final modules against three real engine permission configurations."""
import argparse
import pathlib
import re
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ("wasmtime", "objdump", "baseline", "simd", "relaxed", "output"):
        parser.add_argument("--" + name, required=True)
    args = parser.parse_args()
    output = pathlib.Path(args.output)
    output.mkdir(parents=True, exist_ok=True)
    version = subprocess.check_output([args.wasmtime, "--version"], text=True)
    (output / "engine.txt").write_text(version)
    levels = ("baseline", "simd", "relaxed")
    vector_opcode = re.compile(r"^\s*[0-9a-f]+:\s+fd\s", re.MULTILINE)
    relaxed_opcode = re.compile(r"^\s*[0-9a-f]+:\s+fd (?:8[0-9a-f]|9[0-3]) 02\s", re.MULTILINE)
    for required, level in enumerate(levels):
        module = getattr(args, level)
        disassembly = subprocess.check_output([args.objdump, "-d", module], text=True)
        (output / (level + ".disassembly")).write_text(disassembly)
        if level == "baseline":
            assert not vector_opcode.search(disassembly), "SIMD leaked into the baseline importer"
        elif level == "simd":
            assert "i32x4.add" in disassembly, "SIMD fixture lost its native add"
            assert not relaxed_opcode.search(disassembly), "Relaxed SIMD leaked into ordinary SIMD"
        else:
            # LLVM 23 prints this finalized opcode as <unknown>; match its exact
            # 0xfd prefix and canonical ULEB128 subopcode 0x100 instead.
            assert re.search(r"^\s*[0-9a-f]+:\s+fd 80 02\s", disassembly, re.MULTILINE), \
                "Relaxed fixture lost its native swizzle"
        for admitted, engine_level in enumerate(levels):
            flags = [
                "-Wsimd=" + ("y" if admitted >= 1 else "n"),
                "-Wrelaxed-simd=" + ("y" if admitted >= 2 else "n"),
            ]
            command = [args.wasmtime, "run", "-Ccache=n", *flags, module]
            result = subprocess.run(command, text=True, capture_output=True)
            log = result.stdout + result.stderr
            (output / (level + "-engine-" + engine_level + ".log")).write_text(
                repr(command) + "\nexit=" + str(result.returncode) + "\n" + log)
            if admitted >= required:
                assert result.returncode == 0, log
            else:
                assert result.returncode != 0, "Engine accepted an unsupported module"
                assert re.search(r"(SIMD|simd|relaxed).*(disabled|support)|disabled.*(SIMD|simd)", log), log
    print("Three modules passed all nine engine-permission checks.")


if __name__ == "__main__":
    main()
