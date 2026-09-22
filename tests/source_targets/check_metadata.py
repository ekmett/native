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
isa_pcm=out/'native.isa.pcm'
run('isa-provider',[*flags,'--precompile',root/'src/native.isa.ccm','-o',isa_pcm])
run('isa-object',[*flags,'-c',isa_pcm,'-o',out/'isa.obj'])
isa_flags=[*flags,'-fmodule-file=native.isa='+str(isa_pcm)]
pcm=out/'native_target_metadata.pcm'
run('provider',[*isa_flags,'--precompile',source/'metadata.ccm','-o',pcm])
run('provider-object',[*isa_flags,'-c',pcm,'-o',out/'metadata.obj'])
consumer=[*isa_flags,'-DNATIVE_TARGETS_METADATA_ONLY','-fmodule-file=native_target_metadata='+str(pcm)]
run('metadata',[*consumer,source/'metadata.cc',out/'metadata.obj',out/'isa.obj','-o',out/'metadata.exe'])
run('metadata-run',[out/'metadata.exe'])
cpuid=out/'native.x86.features.pcm'
run('cpuid-provider',[*isa_flags,'--precompile',root/'src/native.x86.features.ccm','-o',cpuid])
run('cpuid-object',[*isa_flags,'-c',cpuid,'-o',out/'cpuid.obj'])
run('observe',[*consumer,'-fmodule-file=native.x86.features='+str(cpuid),source/'observe.cc',
    out/'metadata.obj',out/'cpuid.obj',out/'isa.obj','-o',out/'observe.exe'])
run('observe-run',[out/'observe.exe'])
arm_flags=[*flags,'--target=aarch64-pc-windows-msvc']
arm_isa_pcm=out/'native.isa.arm.pcm'
run('arm-windows-isa-provider',[*arm_flags,'--precompile',root/'src/native.isa.ccm','-o',arm_isa_pcm])
arm_flags+=['-fmodule-file=native.isa='+str(arm_isa_pcm)]
run('arm-windows-provider',[*arm_flags,'--precompile',
    root/'src/native.arm.features.ccm','-o',out/'native.arm.features.pcm'])
run('arm-windows-object',[*arm_flags,'-c',
    out/'native.arm.features.pcm','-o',out/'arm.obj'])
# The metadata-only fixture omits its runtime dispatcher. Explicitly instantiate
# each selected function so the codegen checks inspect the target variants.
macros_source=out/'metadata-macros.cc'
macros_source.write_text(
    '#include "'+(source/'macros.cc').as_posix()+'"\n'
    '#define INSTANTIATE_TARGET(unused, name) template void source_kernel<NATIVE_TARGET_ISA(name)>(float *, float const *);\n'
    'NATIVE_DETAIL_TARGET_MAP(INSTANTIATE_TARGET,unused,SELECTED_TARGETS)\n'
    '#undef INSTANTIATE_TARGET\n'
    '#define INSTANTIATE_TARGET(unused, name) template void source_superset<NATIVE_TARGET_ISA(name)&extra_feature>(float *, float const *);\n'
    'NATIVE_DETAIL_TARGET_MAP(INSTANTIATE_TARGET,unused,SELECTED_TARGETS)\n',encoding='utf-8')
run('macros-ir',[*consumer,'-S','-emit-llvm',macros_source,'-o',out/'macros.ll'])
run('macros-assembly',[*consumer,'-S',macros_source,'-o',out/'macros.s'])
run('macros-object',[*consumer,'-c',macros_source,'-o',out/'macros.obj'])
run('scalar-and-empty-list',[*consumer,'-c',source/'scalar.cc','-o',out/'scalar.obj'])
run('scalar-link',[*flags,source/'scalar_main.cc',out/'scalar.obj',out/'metadata.obj',out/'isa.obj',
    '-o',out/'scalar.exe'])
run('scalar-run',[out/'scalar.exe'])
objdump=pathlib.Path(a.compiler).with_name('llvm-objdump.exe')
run('object-codegen',[sys.executable,source/'check_codegen.py','--objdump',objdump,
    '--output',out/'macros.disassembly',out/'macros.obj',out/'scalar.obj'])
asan_pcm=out/'native_target_metadata_asan.pcm'
asan_isa_pcm=out/'native.isa.asan.pcm'
asan_flags=[*flags,'-fsanitize=address']
run('asan-isa-provider',[*asan_flags,'--precompile',root/'src/native.isa.ccm','-o',asan_isa_pcm])
asan_flags+=['-fmodule-file=native.isa='+str(asan_isa_pcm)]
run('asan-provider',[*asan_flags,'--precompile',source/'metadata.ccm','-o',asan_pcm])
run('asan-object',[*asan_flags,'-DNATIVE_TARGETS_METADATA_ONLY',
    '-fmodule-file=native_target_metadata='+str(asan_pcm),'-c',macros_source,'-o',out/'macros-asan.obj'])
run('asan-object-codegen',[sys.executable,source/'check_codegen.py','--objdump',objdump,
    '--output',out/'macros-asan.disassembly',out/'macros-asan.obj'])
rejected=run('invalid',[*consumer,'-c',source/'invalid.cc','-o',out/'invalid.obj'],False)
assert 'source target contains an unregistered ISA feature' in rejected.stderr
ir=(out/'macros.ll').read_text()
definitions=re.findall(r'^define[^\n]+@[^\n]*source_kernel[^\n]+',ir,re.M)
assert len(definitions)==3,definitions
attributes=dict(re.findall(r'^attributes #(\d+) = \{([^\n]+)\}',ir,re.M))
kernel_attributes=[attributes[re.search(r'#(\d+)',line).group(1)] for line in definitions]
superset_definitions=re.findall(r'^define[^\n]+@[^\n]*source_superset[^\n]+',ir,re.M)
assert len(superset_definitions)==3,superset_definitions
superset_attributes=[attributes[re.search(r'#(\d+)',line).group(1)] for line in superset_definitions]
assert sorted(superset_attributes)==sorted(kernel_attributes)
assert all('+gfni' not in x for x in superset_attributes)
assert sum('+avx512f' in x for x in kernel_attributes)==1
assert sum('+f16c' in x for x in kernel_attributes)==2
assert sum('+bmi2' in x for x in kernel_attributes)==1
assert all('+avx2' in x for x in kernel_attributes)
after=re.search(r'^define[^\n]+@source_after_scope[^\n]+',ir,re.M).group(0)
after_attributes=attributes[re.search(r'#(\d+)',after).group(1)]
assert all(feature not in after_attributes for feature in ('+avx2', '+avx512f', '+bmi2'))
assembly=(out/'macros.s').read_text()
assert re.search(r'vaddps[^\n]*ymm',assembly)
assert re.search(r'vaddps[^\n]*zmm',assembly)
(out/'receipt.json').write_text(json.dumps({'commands':records,'source_kernel_definitions':definitions,
    'selected_target_attributes':kernel_attributes,'superset_target_attributes':superset_attributes,
    'after_scope_attributes':after_attributes,
    'passed':True},indent=2),encoding='utf-8')
print('Metadata module, synthetic admission, three exact and three superset source variants, ordinary/ASan object codegen and negative registry passed.')
