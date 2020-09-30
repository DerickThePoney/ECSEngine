#!/usr/bin/env python3

import sys
import os
import subprocess

def BuildAll(args):

    configBGFX = 'Release'
    configECS = 'Release'

    if len(args) >= 1:
        configECS = args[0]

    if configECS == 'Debug':
        configBGFX = 'Debug'

    MSBUILD = '\"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe\"'
    if len(args) >= 2:
        MSBUILD = args[1]

    result = subprocess.run(['./tools/premake5.exe', 'vs2019'])

    if result.returncode != 0:
        return result.returncode

    result = subprocess.run('cd External/BGFX/bgfx/ && ..\\bx\\tools\\bin\\windows\\genie.exe --with-windows=10.0 vs2019 && {0} -m .build/projects/vs2019/bgfx.sln /verbosity:minimal /p:Configuration={1} /p:Platform=x64'.format(MSBUILD, configBGFX), shell=True)

    if result.returncode != 0:
        return result.returncode

    result = subprocess.run('{0} -m build/ECSEngine.sln /verbosity:minimal /p:Configuration={1}'.format(MSBUILD, configECS))

    if result.returncode != 0:
        return result.returncode

if __name__ == "__main__":
   sys.exit(BuildAll(sys.argv[1:]))