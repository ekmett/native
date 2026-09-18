# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Check the native object that also supplies the admitted execution tests."""
import argparse
from pathlib import Path
import re
import subprocess

TARGETS = (
    'exp_full_half', 'avx512_bf16', 'avx512_fp16', 'avx512', 'exp_bw_half',
    'exp_bw_bf16', 'exp_bw_fp16', 'exp_bw', 'exp_vl', 'exp_base', 'avx2')
EXPECTED = {f'refined_codegen_{target}_{width}': (target, width)
            for target in TARGETS for width in ('narrow', 'native')}
VL = {'exp_full_half', 'avx512_bf16', 'avx512_fp16', 'avx512', 'exp_vl'}
LABEL = re.compile(r'(?m)^([0-9a-f]+) <([^\n]+)>:[ \t]*$')
INSTRUCTION = re.compile(
    r'^\s*([0-9a-f]+):\s+((?:[0-9a-f]{2}[ \t]+)+)([a-z][a-z0-9_.]*)\b(.*)$')
RELOCATION = re.compile(
    r'^\s*([0-9a-f]+):?\s+((?:R_|IMAGE_REL_|X86_64_RELOC_)\S+)\s+(.*)$')


def public_name(name):
    # C symbols retain a leading underscore in Mach-O objdump output.
    return name[1:] if name.startswith(('_refined_codegen_', '_exp_refinement_')) else name


def instructions(text):
    result, relocations = [], []
    for line in text.splitlines():
        match = INSTRUCTION.match(line)
        if match:
            address, encoding, opcode, operands = match.groups()
            result.append((int(address, 16), len(encoding.split()), opcode, operands.strip()))
        else:
            match = RELOCATION.match(line)
            if match:
                relocations.append((int(match[1], 16), match[2], match[3]))
    return result, relocations


def function_bodies(text):
    """Resolve symbols inside their own object and section, including local aliases."""
    bodies = []
    objects = re.split(r'(?m)^.+:\s+file format ([^\n]+)$', text)
    for i in range(1, len(objects), 2):
        file_format, contents = objects[i:i+2]
        sections = re.split(r'(?m)^Disassembly of section ([^\n]+):[ \t]*$', contents)
        symbols = []
        if file_format.startswith('coff'):
            for section, address, name in re.findall(
                    r'(?m)^\[.*?\]\(sec\s+(-?\d+)\).*?\(ty\s+20\).*?'
                    r'0x([0-9a-f]+) (.+)$', sections[0]):
                if int(section) > 0:
                    symbols.append((section, int(address, 16), name))
        elif file_format.startswith(('elf', 'mach-o')):
            for address, section, name in re.findall(
                    r'(?m)^([0-9a-f]+)\s+[lgw]\s+F\s+(\S+)\s+'
                    r'(?:[0-9a-f]+\s+)?(.+)$', sections[0]):
                name = re.sub(r'^\.(?:hidden|protected|internal)\s+', '', name)
                symbols.append((section, int(address, 16), name))
        else:
            raise ValueError('Unsupported object format: ' + file_format)
        names = {name for _, _, name in symbols}
        instrumented = any(public_name(name) == 'exp_refinement_asan_instrumented' for name in names)
        for j in range(1, len(sections), 2):
            section, assembly = sections[j:j+2]
            if file_format.startswith('coff'):
                # COFF repeats .text for COMDAT sections. Resolve each displayed
                # function name within this occurrence, never by address alone.
                functions = [(int(m[1], 16), m[2]) for m in LABEL.finditer(assembly)
                             if m[2] in names]
            else:
                functions = [(address, name) for location, address, name in symbols
                             if location == section or
                             (',' not in location and location == section.rsplit(',', 1)[-1])]
            selected = [(address, name) for address, name in functions
                        if public_name(name).startswith('refined_codegen_')]
            if not selected:
                continue
            code, relocations = instructions(assembly)
            if not code:
                raise ValueError('Entry section has no instructions: ' + section)
            section_end = max(address + size for address, size, _, _ in code)
            boundaries = sorted({address for address, _ in functions} | {section_end})
            for start, raw_name in selected:
                end = next((address for address in boundaries if address > start), section_end)
                body = [item for item in code if start <= item[0] < end]
                if not body or body[0][0] != start:
                    raise ValueError('Entry has no complete body: ' + raw_name)
                body_relocations = [item for item in relocations if start <= item[0] < end]
                bodies.append((public_name(raw_name), body, body_relocations, instrumented))
    return bodies


def check(text):
    bodies = function_bodies(text)
    names = [name for name, _, _, _ in bodies]
    if len(names) != len(set(names)):
        raise ValueError('Duplicate exp codegen entry')
    if set(names) != set(EXPECTED):
        raise ValueError(f'Exp entry mismatch: missing={sorted(set(EXPECTED)-set(names))}, '
                         f'unexpected={sorted(set(names)-set(EXPECTED))}')
    instrumented_count = 0
    for name, body, relocations, instrumented in bodies:
        target, width = EXPECTED[name]
        register = 'zmm' if width == 'native' and target != 'avx2' else 'ymm'
        fmas = [item for item in body if re.fullmatch(r'vfmadd\d*ps', item[2])]
        scales = [item for item in body if item[2] == 'vscalefps']
        if not fmas:
            raise ValueError('Missing polynomial packed FMA: ' + name)
        native_scale = (width == 'native' and target != 'avx2') or target in VL
        if bool(scales) != native_scale:
            raise ValueError('SCALEF cut mismatch: ' + name)
        for _, _, opcode, operands in fmas + scales:
            widths = set(re.findall(r'\b(xmm|ymm|zmm)\d+\b', operands))
            if widths != {register}:
                raise ValueError(f'Wrong {opcode} operand width in {name}: {operands}')
        addresses = {item[0] for item in body}
        for address, size, opcode, operands in body:
            if opcode in ('call', 'callq') and not instrumented:
                raise ValueError('Unexpected outlined helper call: ' + name)
            if opcode.startswith('j') or opcode.startswith('loop'):
                # A relocation can turn an apparently local placeholder address
                # into an external tail transfer. Do not infer its final target.
                if any(address <= offset < address + size for offset, _, _ in relocations):
                    raise ValueError('Unresolved/external branch relocation: ' + name)
                match = re.fullmatch(r'(?:0x)?([0-9a-f]+)(?:\s+<([^>]+)>)?', operands)
                if not match or int(match[1], 16) not in addresses:
                    raise ValueError('External or indirect tail/branch transfer: ' + name)
                if match[2]:
                    destination = public_name(match[2].split('+0x', 1)[0])
                    if destination != name and not destination.startswith(('.L', 'LBB', 'ltmp')):
                        raise ValueError('Branch to another function: ' + name)
        instrumented_count += instrumented
    return ('22 exact exp entries: packed FMA/SCALEF operand widths and F/VL cuts; '
            'no external tail transfers; ' +
            (f'instrumentation calls permitted in {instrumented_count} entries.'
             if instrumented_count else 'no helper calls.'))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--objdump', required=True)
    parser.add_argument('--output', required=True, type=Path)
    parser.add_argument('objects', nargs='+')
    args = parser.parse_args()
    result = subprocess.run(
        [args.objdump, '--demangle', '--syms', '--disassemble', '--reloc', *args.objects],
        capture_output=True, text=True)
    args.output.write_text(result.stdout + result.stderr, encoding='utf-8')
    if result.returncode:
        raise SystemExit(result.stderr)
    try:
        print(check(result.stdout))
    except ValueError as error:
        raise SystemExit(str(error)) from error


if __name__ == '__main__':
    main()
