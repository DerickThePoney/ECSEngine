#!/usr/bin/env python3

import sys
import os
import subprocess
from shutil import copyfile

import argparse


def main():
    print('Flush everything')
    command = ['git', 'clean', '-fdx', '&&', 'cd', 'Assets', '&&', 'git', 'clean', '-fdx', '&&', 'cd', '..']
    ret = subprocess.run(command)
    if ret.returncode != 0:
        return ret.returncode

    print('Build all Final')
    command = ['py', '-u', 'Script/Compil/BuildAll.py', '-a', '-c', 'Final']
    ret = subprocess.run(command)
    if ret.returncode != 0:
        return ret.returncode

    print('Build engine tools')
    command = ['py', '-u', 'Script/Compil/BuildAll.py', '-et']
    ret = subprocess.run(command)
    if ret.returncode != 0:
        return ret.returncode

    print('Generate all')
    command = ['py', '-u', 'Script/Generation/GenerateData.py']
    ret = subprocess.run(command)
    if ret.returncode != 0:
        return ret.returncode

    print('Datapack')
    command = ['cd', 'bin', '&&', './DataPacker-x64-Release.exe']
    ret = subprocess.run(command)
    if ret.returncode != 0:
        return ret.returncode

if __name__ == "__main__":
   sys.exit(main())
