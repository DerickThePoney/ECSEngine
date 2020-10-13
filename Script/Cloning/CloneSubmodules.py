#!/usr/bin/env python3

import sys
import os
import subprocess

import argparse

def CloneBGFX():
    print('CLONING BGFX')
    sys.stdout.flush()
    result = subprocess.run('git submodule update --depth 1 --init -- External/BGFX', shell=True)

    return result.returncode

def CloneEngine():
    print('CLONING FOR ENGINE')
    sys.stdout.flush()
    result = subprocess.run('git submodule update --depth 1 --init -- External/', shell=True)

    return result.returncode

def CloneAssets():
    print('CLONING FOR ASSETS')
    sys.stdout.flush()
    result = subprocess.run('git submodule update --depth 1 --init -- Assets/', shell=True)

    return result.returncode

def CloneAll():
    result = CloneEngine()

    if result != 0:
        return result

    return CloneAssets()

def main():
    parser = argparse.ArgumentParser()
    group = parser.add_mutually_exclusive_group()
    group.add_argument('-a', '--all', action="store_true", help='Clone all (== -b -e -s all together)')
    group.add_argument('-b', '--bgfx', action="store_true", help='Clone BGFX')
    group.add_argument('-e', '--engine', action="store_true", help='Clone all for engine (adds -b)')
    group.add_argument('-s', '--assets', action="store_true", help='Clone Assets')

    args = parser.parse_args()

    if args.all:
        print('Clone all')
        return CloneAll()
    elif args.bgfx:
        print('Clone BGFX')
        return CloneBGFX()
    elif args.engine:
        print('Clone Engine')
        return CloneEngine()
    elif args.assets:
        print('Clone Assets')
        return CloneAssets()

if __name__ == "__main__":
   sys.exit(main())