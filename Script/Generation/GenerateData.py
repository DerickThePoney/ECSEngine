#!/usr/bin/env python3

import sys
import os
import subprocess
from shutil import copyfile

import argparse

def main():
    os.chdir('build')
    print(os.getcwd())
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--allowCompil', action="store_true", help='Allow compilation')

    args = parser.parse_args()
    if not os.path.exists('../bin/AssetCooker-x64-Release.exe'):
        if not args.allowCompil:
            return 1

        result = subprocess.run('py Script/Compil/BuildAll.py -e')
        if result.returncode != 0:
            return result.returncode

        if not os.path.exists('bin/AssetCooker-x64-Release.exe'):
            return 1

    result = subprocess.run('../bin/AssetCooker-x64-Release.exe')
    return result.returncode

if __name__ == "__main__":
   sys.exit(main())