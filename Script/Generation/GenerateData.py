#!/usr/bin/env python3

import sys
import os
import json
import subprocess
from shutil import copyfile

import argparse

def prototype_names():
    manifest_path = os.path.join('Script', 'Generation', 'prototypes.json')
    with open(manifest_path, 'r', encoding='utf-8') as manifest_file:
        prototypes = json.load(manifest_file)
    return [item['name'] for item in prototypes]

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--game', default='Colony', help='Prototype to cook. Shared assets are included.')
    parser.add_argument('-c', '--allowCompil', action="store_true", help='Allow compilation')

    args = parser.parse_args()
    if args.game not in prototype_names():
        print('Unknown game: %s' % args.game)
        return 1

    os.chdir('build')
    print(os.getcwd())
    if not os.path.exists('../bin/AssetCooker-x64-Release.exe'):
        if not args.allowCompil:
            return 1

        result = subprocess.run('py Script/Compil/BuildAll.py -e')
        if result.returncode != 0:
            return result.returncode

        if not os.path.exists('bin/AssetCooker-x64-Release.exe'):
            return 1

    result = subprocess.run(['../bin/AssetCooker-x64-Release.exe', '--game', args.game])
    if result.returncode != 0:
        print('Asset cooking failed')
    return result.returncode

if __name__ == "__main__":
   sys.exit(main())