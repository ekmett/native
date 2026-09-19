# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Focused Clang check before the full hub build; use an initialized toolchain."""
import argparse
import json
import pathlib
import re
import subprocess
import sys

p=argparse.ArgumentParser()
p.add_argument('--compiler',required=True)
p.add_argument('--output',required=True,type=pathlib.Path)
a=p.parse_args()
source=pathlib.Path(__file__).resolve().parent
root=source.parent.parent
out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
records=[]
def run(name,args,success=True):
    result=subprocess.run([str(x) for x in args],capture_output=True,text=True)
    (out/(name+'.log')).write_text(result.stdout+result.stderr,encoding='utf-8')
    records.append({'name':name,'command':[str(x) for x in args],'exit_code':result.returncode})
    if (result.returncode==0)!=success:
        raise RuntimeError(name+'\n'+result.stdout+result.stderr)
    return result

flags=[a.compiler,'-std=c++26','-O2','-Werror=ignored-attributes','-I'+str(root/'src')]
pcm=out/'simd_target_metadata.pcm'
run('provider',[*flags,'--precompile',source/'metadata.ccm','-o',pcm])
run('provider-object',[*flags,'-c',pcm,'-o',out/'metadata.obj'])
consumer=[*flags,'-DSIMD_TARGETS_METADATA_ONLY','-fmodule-file=simd_target_metadata='+str(pcm)]
run('metadata',[*consumer,source/'metadata.cc',out/'metadata.obj','-o',out/'metadata.exe'])
run('metadata-run',[out/'metadata.exe'])
cpuid=out/'simd.cpu.x86.pcm'
run('cpuid-provider',[*flags,'--precompile',root/'src/simd.cpu.x86.ccm','-o',cpuid])
run('cpuid-object',[*flags,'-c',cpuid,'-o',out/'cpuid.obj'])
run('observe',[*consumer,'-fmodule-file=simd.cpu.x86='+str(cpuid),source/'observe.cc',
    out/'metadata.obj',out/'cpuid.obj','-o',out/'observe.exe'])
run('observe-run',[out/'observe.exe'])
run('arm-windows-provider',[*flags,'--target=aarch64-pc-windows-msvc','--precompile',
    root/'src/simd.cpu.arm.ccm','-o',out/'simd.cpu.arm.pcm'])
run('arm-windows-object',[*flags,'--target=aarch64-pc-windows-msvc','-c',
    out/'simd.cpu.arm.pcm','-o',out/'arm.obj'])
run('macros-ir',[*consumer,'-S','-emit-llvm',source/'macros.cc','-o',out/'macros.ll'])
run('macros-assembly',[*consumer,'-S',source/'macros.cc','-o',out/'macros.s'])
run('macros-object',[*consumer,'-c',source/'macros.cc','-o',out/'macros.obj'])
run('scalar-and-empty-list',[*consumer,'-c',source/'scalar.cc','-o',out/'scalar.obj'])
run('scalar-link',[*flags,source/'scalar_main.cc',out/'scalar.obj',out/'metadata.obj',
    '-o',out/'scalar.exe'])
run('scalar-run',[out/'scalar.exe'])
objdump=pathlib.Path(a.compiler).with_name('llvm-objdump.exe')
run('object-codegen',[sys.executable,source/'check_codegen.py','--objdump',objdump,
    '--output',out/'macros.disassembly',out/'macros.obj',out/'scalar.obj'])
asan_pcm=out/'simd_target_metadata_asan.pcm'
run('asan-provider',[*flags,'-fsanitize=address','--precompile',source/'metadata.ccm','-o',asan_pcm])
run('asan-object',[*flags,'-fsanitize=address','-DSIMD_TARGETS_METADATA_ONLY',
    '-fmodule-file=simd_target_metadata='+str(asan_pcm),'-c',source/'macros.cc','-o',out/'macros-asan.obj'])
run('asan-object-codegen',[sys.executable,source/'check_codegen.py','--objdump',objdump,
    '--output',out/'macros-asan.disassembly',out/'macros-asan.obj'])
rejected=run('invalid',[*consumer,'-c',source/'invalid.cc','-o',out/'invalid.obj'],False)
assert 'unknown SIMD ISA feature' in rejected.stderr
ir=(out/'macros.ll').read_text()
definitions=re.findall(r'^define[^\n]+@[^\n]*source_kernel[^\n]+',ir,re.M)
assert len(definitions)==3,definitions
attributes=dict(re.findall(r'^attributes #(\d+) = \{([^\n]+)\}',ir,re.M))
kernel_attributes=[attributes[re.search(r'#(\d+)',line).group(1)] for line in definitions]
assert sum('+avx512f' in x for x in kernel_attributes)==1
assert sum('+f16c' in x for x in kernel_attributes)==2
assert all('+avx2' in x for x in kernel_attributes)
after=re.search(r'^define[^\n]+@source_after_scope[^\n]+',ir,re.M).group(0)
after_attributes=attributes[re.search(r'#(\d+)',after).group(1)]
assert '+avx2' not in after_attributes and '+avx512f' not in after_attributes
assembly=(out/'macros.s').read_text()
assert re.search(r'vaddps[^\n]*ymm',assembly)
assert re.search(r'vaddps[^\n]*zmm',assembly)
(out/'receipt.json').write_text(json.dumps({'commands':records,'source_kernel_definitions':definitions,
    'selected_target_attributes':kernel_attributes,'after_scope_attributes':after_attributes,
    'passed':True},indent=2),encoding='utf-8')
print('Metadata module, synthetic admission, three exact source variants, ordinary/ASan object codegen and negative registry passed.')
