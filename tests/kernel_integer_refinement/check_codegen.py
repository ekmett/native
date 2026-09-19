# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Inspect each named pointer boundary without admitting BW instructions in F/DQ."""
import argparse
from pathlib import Path
import re
import subprocess

NAMES = [f'integer_codegen_{target}_{operation}'
         for target in ('fdq', 'fdq_vl', 'fdq_bw', 'avx512')
         for operation in ('u32', 'u64', 'pack')]
INSTRUCTION = re.compile(r'^[ \t]*[0-9a-f]+:[ \t]+((?:[0-9a-f]{2}[ \t]+)+)(\S+)[ \t]*(.*)$', re.M)
# The no-BW kernels need only these F/DQ EVEX operations. Reject additions until
# their ISA requirement is reviewed, including 128/256-bit EVEX encodings.
FDQ_EVEX = {
    'vmovdqu32', 'vmovdqu64', 'vmovdqa32', 'vmovdqa64', 'vmovups', 'vmovaps',
    'vpbroadcastd', 'vpbroadcastq', 'vpsrld', 'vpsrlq', 'vpandd', 'vpandq',
    'vpaddd', 'vpaddq', 'vpsubd', 'vpsubq', 'vpternlogd', 'vpternlogq',
    'vpmovqd', 'vinserti64x4', 'vshufi64x2'}


def check_body(name, text):
    labels = re.findall(r'^[0-9a-f]+ <([^>]+)>:', text, re.M)
    if labels not in ([name], ['_' + name]):
        raise ValueError('Missing expected function marker: ' + name)
    instructions = list(INSTRUCTION.finditer(text))
    if not instructions or not any(m[2] in ('ret', 'retq') for m in instructions):
        raise ValueError('Missing complete selected function: ' + name)
    no_bw = name.startswith(('integer_codegen_fdq_u', 'integer_codegen_fdq_pack',
                             'integer_codegen_fdq_vl_'))
    for instruction in instructions:
        encoding, opcode, operands = instruction.groups()
        if opcode.startswith(('call', 'j', 'loop')) or opcode == '<unknown>':
            raise ValueError('Outlined transfer or undecoded instruction: ' + name)
        if no_bw and encoding.split()[0] == '62' and opcode not in FDQ_EVEX:
            raise ValueError(f'Unexpected EVEX requirement in {name}: {opcode}')
        if no_bw and '_fdq_vl_' not in name and encoding.split()[0] == '62' and not re.search(r'\bzmm\d+\b', operands):
            raise ValueError('Unexpected narrow EVEX operation without VL: ' + name)
    if not any(re.search(r'\bzmm\d+\b', m[3]) for m in instructions):
        raise ValueError('Missing native 512-bit operation: ' + name)
    if no_bw:
        expected = 'vpmovqd' if name.endswith('pack') else 'vpsrld' if name.endswith('u32') else 'vpsrlq'
        native = any(m[2] == expected and re.search(r'\bzmm\d+\b', m[3]) for m in instructions)
        # LLVM recognizes the qword SWAR as ctpop and may use two AVX2 nibble
        # tables/SAD reductions. Byte instructions with VEX encoding need no BW.
        split_qwords = name.endswith('u64') and sum(m[2] == 'vpsadbw' and
            m[1].split()[0] in ('c4', 'c5') and re.search(r'\bymm\d+\b', m[3]) is not None
            for m in instructions) == 2 and any(m[2] == 'vinserti64x4' for m in instructions)
        if not native and not split_qwords:
            raise ValueError('Missing lane-local operation: ' + name)
        return 'zmm ' + expected if native else 'two VEX.256 SAD reductions joined to zmm'
    return 'native zmm boundary'


def controls():
    name = 'integer_codegen_fdq_u32'
    valid = f'0000000000000000 <{name}>:\n 0: 62 f1 7d 48 72 d0 01 vpsrld $1, %zmm0, %zmm0\n 7: c3 retq\n'
    check_body(name, valid)
    mutations = [valid + ' 8: 62 f1 7d 48 fc c0 vpaddb %zmm0, %zmm0, %zmm0\n',
        valid + ' 8: e8 00 00 00 00 callq 0x0 <helper>\n',
        valid + ' 8: e9 00 00 00 00 jmp 0x0\n 9: IMAGE_REL_AMD64_REL32 helper\n',
        valid.replace('zmm', 'ymm'), valid.replace(name, 'unrelated_neighbor'),
        valid + '0000000000000008 <unrelated_neighbor>:\n 8: c3 retq\n']
    for mutated in mutations:
        try:
            check_body(name, mutated)
        except ValueError:
            continue
        raise ValueError('Negative codegen control unexpectedly passed')
    print('Codegen controls: valid F/DQ accepted; BW, helper call, tail jump, wrong width, wrong symbol and neighboring body rejected')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--self-test', action='store_true')
    parser.add_argument('--objdump')
    parser.add_argument('--output', type=Path)
    parser.add_argument('objects', nargs='*')
    args = parser.parse_args()
    if args.self_test:
        controls()
        return
    if not args.objdump or not args.output or not args.objects:
        parser.error('--objdump, --output and objects are required')
    headers = subprocess.run([args.objdump, '--file-headers', *args.objects], capture_output=True, text=True)
    if headers.returncode:
        raise ValueError(headers.stderr)
    prefix = '_' if 'file format mach-o' in headers.stdout else ''
    output = []
    for name in NAMES:
        # Ask objdump to resolve and isolate this function; section-local alias
        # labels cannot merge neighboring bodies. Use Mach-O's C spelling there.
        result = subprocess.run([args.objdump, '--disassemble', '--reloc',
            '--disassemble-symbols=' + prefix + name, *args.objects],
            capture_output=True, text=True)
        output.append(result.stdout + result.stderr)
        args.output.write_text('\n'.join(output), encoding='utf-8')
        if result.returncode:
            raise ValueError(result.stderr)
        print(name + ': ' + check_body(name, result.stdout))
    print('12 isolated integer/packing entries: native widths, no helper transfers; '
          '6 F/DQ entries contain only reviewed F/DQ EVEX operations, with or without VL')


if __name__ == '__main__':
    try:
        main()
    except ValueError as error:
        raise SystemExit(str(error)) from error
