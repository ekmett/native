// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import { readFile } from 'node:fs/promises';
import { WASI } from 'node:wasi';
const bytes = await readFile(process.argv[2]);
if (!WebAssembly.validate(bytes)) {
  throw new Error('Engine does not admit this relaxed SIMD test module.');
}
const wasi = new WASI({ version: 'preview1', args: [process.argv[2]], env: {}, preopens: {}, returnOnExit: true });
const module = await WebAssembly.compile(bytes);
const instance = await WebAssembly.instantiate(module, { wasi_snapshot_preview1: wasi.wasiImport });
process.exit(wasi.start(instance));
