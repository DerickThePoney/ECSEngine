#!/usr/bin/env python3

import sys
import os
import subprocess
from shutil import copyfile

import argparse

def BuildBGFX(config, MSBUILD, tools, samples):
    configBGFX = 'Release'
    if config == 'Debug':
        configBGFX = 'Debug'

    addoptions = ''
    if tools:
        addoptions = '--with-tools '

    if samples:
        addoptions += '--with-examples '

    print('BUILDING BGFX')
    sys.stdout.flush()
    result = subprocess.run('cd External/BGFX/bgfx/ && ..\\bx\\tools\\bin\\windows\\genie.exe {2}--with-windows=10.0 vs2019 && {0} -m .build/projects/vs2019/bgfx.sln /verbosity:minimal /p:Configuration={1} /p:Platform=x64'.format(MSBUILD, configBGFX, addoptions), shell=True)

    if tools and result.returncode == 0 and config != 'Debug':
        copyfile('External/BGFX/bgfx/.build/win64_vs2019/bin/texturecRelease.exe', 'External/BGFX/ToolBinaries/texturecRelease.exe')
        copyfile('External/BGFX/bgfx/.build/win64_vs2019/bin/shadercRelease.exe', 'External/BGFX/ToolBinaries/shadercRelease.exe')

    return result.returncode

def BuildEngine(config, MSBUILD):
    print('BUILDING ECSEngine')
    sys.stdout.flush()

    command = ['py', '-u', 'Script/FastBuild/fastbuild.py']

    result = subprocess.run(command)

    if result.returncode != 0:
        return result.returncode

    command = ['./tools/FBuild.exe', '-summary', 'Build-BuildingGame-x64-%s'%(config), 'Build-AssetCooker-x64-%s'%(config)]

    result = subprocess.run(command)

    return result.returncode

def BuildSolution():
    print('BUILDING Solution')
    sys.stdout.flush()

    command = ['py', '-u', 'Script/FastBuild/fastbuild.py']

    result = subprocess.run(command)

    if result.returncode != 0:
        return result.returncode

    command = ['./tools/FBuild.exe', '-summary', 'ECSEngine']

    result = subprocess.run(command)

    return result.returncode

def BuildAll(config, MSBUILD, tools, samples):

    res = BuildBGFX(config, MSBUILD, tools, samples)

    if res != 0:
        return res

    return BuildEngine(config, MSBUILD)


def main():
    parser = argparse.ArgumentParser()
    group = parser.add_mutually_exclusive_group()
    group.add_argument('-a', '--all', action="store_true", help='Build all')
    group.add_argument('-b', '--bgfx', action="store_true", help='Build BGFX')
    group.add_argument('-e', '--engine', action="store_true", help='Build Engine')
    group.add_argument('-sln', '--solution', action="store_true", help='Build Solution')
    parser.add_argument('-c', '--config', type=str, help='Configuration to build', default='Release')
    parser.add_argument('-m', '--msbuild', type=str, help='Path to MSBuild')
    parser.add_argument('-t', '--bgfxtools', action="store_true", help='Build BGFX Tools')
    parser.add_argument('-s', '--bgfxsamples', action="store_true", help='Build BGFX Samples')

    args = parser.parse_args()

    MSBUILD = '\"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe\"'
    if args.msbuild:
        MSBUILD = args.msbuild

    if args.solution:
        res = BuildSolution()
        if res != 0:
            return res

    if args.all:
        print('Build all')
        return BuildAll(args.config, MSBUILD, args.bgfxtools, args.bgfxsamples)
    elif args.bgfx:
        print('Build BGFX')
        return BuildBGFX(args.config, MSBUILD, args.bgfxtools, args.bgfxsamples)
    elif args.engine:
        print('Build Engine')
        return BuildEngine(args.config, MSBUILD)

if __name__ == "__main__":
   sys.exit(main())