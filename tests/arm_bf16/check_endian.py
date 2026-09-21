# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Cross-compile the real floating-instruction headers and verify byte/lane flow, without execution.

The small interpreter models only register movement and BE loads/stores. At each
floating instruction it checks the architectural operands against symbolic memory
bytes, then supplies symbolic result bytes and checks their final memory order.
It does not emulate floating arithmetic or claim big-endian hardware execution.
"""
import argparse
import pathlib
import re
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("--compiler", required=True)
parser.add_argument("--source", required=True, type=pathlib.Path)
parser.add_argument("--include", required=True)
parser.add_argument("--support", required=True)
parser.add_argument("--output", required=True, type=pathlib.Path)
args = parser.parse_args()
command = [args.compiler, "--driver-mode=g++", "--target=aarch64_be-none-elf", "-ffreestanding", "-std=c++26",
           "-fms-extensions", "-O2", "-march=armv8-a", "-Wno-nonportable-vector-initialization",
           "-I", args.support, "-I", args.include,
           "-S", str(args.source), "-o", str(args.output)]
result = subprocess.run(command, text=True, capture_output=True)
args.output.with_suffix(".log").write_text(result.stdout + result.stderr, encoding="utf-8")
if result.returncode:
    raise SystemExit(result.stderr)
assembly = args.output.read_text(encoding="utf-8")
widths = {"b": 1, "h": 2, "s": 4, "d": 8}
constants = {}
for match in re.finditer(r"(?m)^(\.LCPI\w+):\n((?:\s+\.byte[^\n]*\n)+)", assembly):
    constants[match[1]] = [int(x) for x in re.findall(r"\.byte\s+(\d+)", match[2])]


def split_elements(data, size):
    return [data[i:i + size] for i in range(0, len(data), size)]


def memory(pointer, count, start=0):
    return [(pointer, i) for i in range(start, start + count)]


def native_order(pointer, size, count, start=0):
    return sum((part[::-1] for part in split_elements(memory(pointer, count, start), size)), [])


def check(name, body):
    info = re.fullmatch(r"(?:native|acle)_(bfdot|bfmmla|bfmlalb|bfmlalt)_(2|4)_(0|4|8)_(\d+)", name)
    operation, lanes, source_lanes, selected = info.groups()
    lanes, source_lanes, selected = int(lanes), int(source_lanes), int(selected)
    count = 4 * lanes
    registers, pointers, stored, calls = {}, {}, None, 0
    for line in body.splitlines():
        line = line.split("//")[0].strip()
        if not line or line.startswith(".") or line == "ret":
            continue
        load = re.fullmatch(r"(ld1|ld1r|st1)\s+\{ (v\d+)\.(\d+)([bhsd]) \}, \[(x\d+)\]", line)
        if load:
            opcode, register, elements, fmt, pointer = load.groups()
            length, element = int(elements) * widths[fmt], widths[fmt]
            if opcode == "ld1":
                registers[register] = (constants[pointers[pointer]][:length] if pointer in pointers
                                       else native_order(pointer, element, length))
            elif opcode == "ld1r":
                registers[register] = native_order(pointer, element, element) * int(elements)
            else:
                assert pointer == "x0", (name, line)
                stored = sum((part[::-1] for part in split_elements(registers[register][:length], element)), [])
            continue
        reverse = re.fullmatch(r"rev(16|32|64)\s+(v\d+)\.(\d+)([bhsd]), (v\d+)\.\d+[bhsd]", line)
        if reverse:
            block, dest, elements, fmt, source = reverse.groups()
            block, element = int(block) // 8, widths[fmt]
            data = registers[source][:int(elements) * element]
            registers[dest] = sum((sum(split_elements(part, element)[::-1], []) for part in split_elements(data, block)), [])
            continue
        move = re.fullmatch(r"mov\s+d(\d+), (v\d+)\.d\[(\d+)\]", line)
        if move:
            dest, source, lane = move.groups()
            start = int(lane) * 8
            registers["v" + dest] = registers[source][start:start + 8]
            continue
        address = re.fullmatch(r"adrp\s+(x\d+), (\.LCPI\w+)", line)
        if address:
            pointers[address[1]] = address[2]
            continue
        address = re.fullmatch(r"add\s+(x\d+), (x\d+), :lo12:(\.LCPI\w+)", line)
        if address:
            assert pointers[address[2]] == address[3]
            pointers[address[1]] = address[3]
            continue
        table = re.fullmatch(r"tbl\s+(v\d+)\.16b, \{ (v\d+)\.16b \}, (v\d+)\.16b", line)
        if table:
            dest, source, indices = table.groups()
            registers[dest] = [registers[source][i] if i < len(registers[source]) else 0 for i in registers[indices]]
            continue
        extract = re.fullmatch(r"ext\s+(v\d+)\.16b, (v\d+)\.16b, (v\d+)\.16b, #(\d+)", line)
        if extract:
            dest, left, right, offset = extract.groups()
            registers[dest] = (registers[left] + registers[right])[int(offset):int(offset) + 16]
            continue
        instruction = re.fullmatch(r"(bfdot|bfmmla|bfmlalb|bfmlalt)\s+(v\d+)\.(2|4)s, (v\d+)\.\d+h, (v\d+)\.(?:(\d+)h(?:\[(\d+)\])?|h\[(\d+)\])", line)
        if instruction:
            opcode, dest, n, left, right, rn, pair_index, scalar_index = instruction.groups()
            assert opcode == operation and int(n) == lanes, (name, line)
            assert registers[dest][:count] == native_order("x1", 4, count), (name, "accumulator")
            assert registers[left][:count] == native_order("x2", 2, count), (name, "left", registers[left])
            if source_lanes == 0:
                assert pair_index is None and scalar_index is None
                assert registers[right][:count] == native_order("x3", 2, count), (name, "right")
            else:
                index = int(pair_index if pair_index is not None else scalar_index)
                width = 4 if opcode == "bfdot" else 2
                assert index == selected, (name, "lane")
                start = selected * width
                assert registers[right][start:start+width] == native_order("x3", 2, width, start), (name, "indexed source", registers[right])
            registers[dest] = native_order("result", 4, count)
            calls += 1
            continue
        raise AssertionError((name, "unhandled instruction", line))
    assert calls == 1 and stored == memory("result", count), (name, "result order", stored)


checked = 0
for match in re.finditer(r"(?m)^((?:native|acle)_\w+):[^\n]*\n(.*?)^\.Lfunc_end\d+:", assembly, re.S | re.M):
    name, body = match.groups()
    check(name, body)
    checked += 1
assert checked == 46, checked
print(f"Verified {checked} big-endian compiler byte/lane mappings; no target execution")
