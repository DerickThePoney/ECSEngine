#!/usr/bin/env python3

import sys
import os
import subprocess
from shutil import copyfile

import argparse

def clean():
    command = ['git', 'clean', '-fdx']
    ret = subprocess.run(command)
    return ret.returncode

def main():
    print('Flush everything')
    ret = clean()
    if ret != 0:
        return ret

    os.chdir('Assets')
    ret = clean()
    if ret != 0:
        return ret
    os.chdir('..')

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
    os.chdir('bin')
    command = ['./DataPacker-x64-Release.exe']
    ret = subprocess.run(command)
    if ret.returncode != 0:
        return ret.returncode

if __name__ == "__main__":
   sys.exit(main())
