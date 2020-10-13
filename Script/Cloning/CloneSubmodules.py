#!/usr/bin/env python3

import sys
import os
import subprocess

import argparse

def CloneBGFX():
    print('CLONING BGFX')
    sys.stdout.flush()
    result = subprocess.run('git submodule update --init -- External/BGFX', shell=True)

    return result.returncode

# def BuildEngine(config, MSBUILD):
#     print('Making solution')
#     sys.stdout.flush()
#     result = subprocess.run(['./tools/premake5.exe', 'vs2019'])

#     if result.returncode != 0:
#         return result.returncode

#     print('BUILDING ECSEngine')
#     sys.stdout.flush()
#     result = subprocess.run('{0} -m build/ECSEngine.sln /verbosity:minimal /p:Configuration={1}'.format(MSBUILD, config))

#     return result.returncode

# def BuildAll(config, MSBUILD, tools, samples):

#     res = BuildBGFX(config, MSBUILD, tools, samples)

#     if res != 0:
#         return res

#     return BuildEngine(config, MSBUILD)


def main():
    parser = argparse.ArgumentParser()
    group = parser.add_mutually_exclusive_group()
    group.add_argument('-a', '--all', action="store_true", help='Clone all (== -b -e -s all together)')
    group.add_argument('-b', '--bgfx', action="store_true", help='Clone BGFX')
    group.add_argument('-e', '--engine', action="store_true", help='Clone all for engine (adds -b)')
    group.add_argument('-s', '--assets', action="store_true", help='Clone Assets')

    args = parser.parse_args()

    # if args.all:
    #     print('Clone all')
    #     return BuildAll(args.config, MSBUILD, args.bgfxtools, args.bgfxsamples)
    if args.bgfx:
        print('Clone BGFX')
        return CloneBGFX()
    # elif args.engine:
    #     print('Clone Engine')
    #     return BuildEngine(args.config, MSBUILD)
    # elif args.assets:
    #     print('Clone Assets')
    #     return BuildEngine(args.config, MSBUILD)

if __name__ == "__main__":
   sys.exit(main())