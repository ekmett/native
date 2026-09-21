// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import assert from "node:assert/strict";
import { execFileSync } from "node:child_process";
import { pathToFileURL } from "node:url";
const { observe_wasm_capabilities: observe, wasm_feature_probe: probe } =
  await import(pathToFileURL(process.argv[2]).href);
const unknown = {
  simd128_observed: false, simd128: false,
  relaxed_simd_observed: false, relaxed_simd: false,
};
const names = ["simd128", "relaxed_simd"];

// Decode section lengths, value types and ULEB128 opcodes independently of the
// bridge. These are complete modules, with no imports, memory, exports or start.
// https://webassembly.github.io/spec/core/binary/modules.html
// https://webassembly.github.io/spec/core/binary/instructions.html
// https://github.com/WebAssembly/relaxed-simd/blob/main/proposals/relaxed-simd/Overview.md
function sections(bytes) {
  assert.deepEqual([...bytes.slice(0, 8)], [0, 97, 115, 109, 1, 0, 0, 0]);
  let at = 8;
  function uleb() {
    let value = 0, shift = 0;
    for (;;) {
      assert.ok(at < bytes.length && shift < 32);
      const byte = bytes[at++];
      value += (byte & 127) * 2 ** shift;
      if ((byte & 128) === 0) return value;
      shift += 7;
    }
  }
  const result = new Map();
  while (at < bytes.length) {
    const id = bytes[at++], length = uleb(), end = at + length;
    assert.ok(!result.has(id) && end <= bytes.length);
    result.set(id, bytes.slice(at, end));
    at = end;
  }
  assert.deepEqual([...result.keys()], [1, 3, 10]);
  assert.deepEqual([...result.get(3)], [1, 0]);
  return result;
}
const strict = sections(probe("simd128"));
assert.deepEqual([...strict.get(1)], [1, 0x60, 0, 1, 0x7b]);
const strictCode = strict.get(10);
assert.equal(strictCode[0], 1);
assert.equal(strictCode[1], strictCode.length - 2);
assert.equal(strictCode[2], 0);
assert.deepEqual([...strictCode.slice(3, 5)], [0xfd, 0x0c]); // v128.const
assert.equal(strictCode.length, 22);
assert.ok(strictCode.slice(5, 21).every(byte => byte === 0));
assert.equal(strictCode[21], 0x0b);
const relaxed = sections(probe("relaxed_simd"));
assert.deepEqual([...relaxed.get(1)], [1, 0x60, 2, 0x7b, 0x7b, 1, 0x7b]);
const relaxedCode = relaxed.get(10);
assert.equal(relaxedCode[0], 1);
assert.equal(relaxedCode[1], relaxedCode.length - 2);
assert.deepEqual([...relaxedCode.slice(2, 8)], [0, 0x20, 0, 0x20, 1, 0xfd]);
assert.equal((relaxedCode[8] & 127) + 128 * relaxedCode[9], 0x100); // relaxed_swizzle
assert.equal(relaxedCode[8] & 128, 128);
assert.equal(relaxedCode[10], 0x0b);
assert.equal(relaxedCode.length, 11);

// The public C++ probes and the optional installed bridge agree byte for byte.
const cpp = execFileSync(process.argv[3], ["--probes"], { encoding: "utf8" }).trim().split(/\r?\n/);
assert.deepEqual(cpp, names.map(name => Buffer.from(probe(name)).toString("hex")));
assert.throws(() => probe("unregistered"), RangeError);
const copy = probe("simd128");
copy.fill(255);
assert.notDeepEqual(copy, probe("simd128"));

for (const runtime of [undefined, null, {}, { validate: null }, { validate: 1 }])
  assert.deepEqual(observe(runtime), unknown);
assert.deepEqual(observe({ get validate() { throw new Error("unavailable"); } }), unknown);
for (const result of [false, undefined, null, 0, 1, "true", Promise.resolve(true)])
  assert.deepEqual(observe({ validate() { return result; } }), unknown);
assert.deepEqual(observe({ validate() { throw new Error("unavailable"); } }), unknown);

for (const a of [true, false, "error", "nonboolean"])
  for (const b of [true, false, "error", "nonboolean"]) {
    let calls = 0;
    const answers = [true, a, b];
    const runtime = {
      validate(bytes) {
        assert.equal(this, runtime);
        assert.ok(bytes instanceof Uint8Array);
        const answer = answers[calls++];
        if (answer === "error") throw new Error("query failed");
        return answer;
      },
      get instantiate() { throw new Error("must not instantiate"); },
      get compile() { throw new Error("must not compile through this API"); },
    };
    assert.deepEqual(observe(runtime), {
      simd128_observed: typeof a === "boolean", simd128: a === true,
      relaxed_simd_observed: typeof b === "boolean", relaxed_simd: b === true,
    });
    assert.equal(calls, 3);
  }

// A callback may mutate its input, but cannot corrupt the next observation.
observe({ validate(bytes) { bytes.fill(255); return true; } });
assert.deepEqual(cpp, names.map(name => Buffer.from(probe(name)).toString("hex")));
const actual = observe();
for (const name of names) {
  assert.equal(actual[`${name}_observed`], true);
  assert.equal(actual[name], WebAssembly.validate(probe(name)));
  assert.equal(WebAssembly.validate(probe(name).slice(0, -1)), false);
}
if (actual.relaxed_simd) assert.equal(actual.simd128, true);
console.log(`WebAssembly validation observations: ${JSON.stringify(actual)}`);
console.log("Probe decoding, C++ parity, real validation and adapter error cases passed.");
