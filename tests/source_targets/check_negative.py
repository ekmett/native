# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
"""Build a deliberately invalid consumer, retaining its real diagnostic."""
import argparse
import pathlib
import re
import subprocess
import sys
p=argparse.ArgumentParser()
p.add_argument('--cmake',required=True)
p.add_argument('--build',required=True)
p.add_argument('--target',required=True)
p.add_argument('--config',default='')
p.add_argument('--log',required=True,type=pathlib.Path)
p.add_argument('--diagnostic',required=True)
a=p.parse_args()
command=[a.cmake,'--build',a.build,'--target',a.target,'--parallel','1']
if a.config: command.extend(['--config',a.config])
r=subprocess.run(command,text=True,capture_output=True)
text=r.stdout+r.stderr
a.log.write_text(text,encoding='utf-8')
if 'NATIVE_TARGET_NEGATIVE_UNAVAILABLE' in text:
    print('The configured project minimum already includes this target; negative is inapplicable.')
    sys.exit(77)
if r.returncode==0 or not re.search(a.diagnostic,text,re.I):
    print(text)
    raise SystemExit('Expected target diagnostic was absent.')
print('Expected target diagnostic confirmed.')
