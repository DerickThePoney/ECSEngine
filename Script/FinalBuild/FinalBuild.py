#!/usr/bin/env python3

import sys
import os
import os.path
import subprocess
from shutil import copyfile
import zipfile

import argparse

def clean():
    command = ['git', 'clean', '-fdx']
    ret = subprocess.run(command)
    return ret.returncode

releaseFolder = '../Release'
currentVersion = 0

def Finalize():

    if not os.path.isdir(releaseFolder):
        os.mkdir(releaseFolder)

    currentMinorVersion = 0
    buildFolder = releaseFolder + '/v' + str(currentVersion)
    if not os.path.isdir(buildFolder):
        os.mkdir(buildFolder)
    else:
        onlyfiles = [f for f in os.listdir(buildFolder) if os.path.isfile(os.path.join(buildFolder, f))]
        for file in onlyfiles:
            num = int(file.split('.')[1])
            currentMinorVersion = max(num+1, currentMinorVersion)

    print('version is %d.%d.zip' % (currentVersion, currentMinorVersion))

    try:
        zipFile = zipfile.ZipFile(buildFolder + ('/%d.%d.zip'%(currentVersion, currentMinorVersion)), mode='w', compression=zipfile.ZIP_LZMA)

        zipFile.write('bin/BuildingGame-x64-Final.exe','BuildingGame-x64-Final.exe')
        zipFile.write('bin/Assets.datapack','Assets.datapack')
        zipFile.write('bin/Sounds.datapack','Sounds.datapack')
        zipFile.write('bin/glfw3.dll','glfw3.dll')

        zipFile.close()
    except:
        return -1

    return 0



def ProcessVersion():

    print('Flush everything')
    ret = clean()
    if ret != 0:
        return ret

    os.chdir('Assets')
    ret = clean()
    if ret != 0:
        return ret
    os.chdir('..')

    print('Build BGFX')
    command = ['py', '-u', 'Script/Compil/BuildAll.py', '-b', '-c', 'Final']
    ret = subprocess.run(command)
    if ret.returncode != 0:
        print('Build BGFX failed')
        return ret.returncode

    print('Build all Final')
    command = ['py', '-u', 'Script/Compil/BuildAll.py', '-e', '-c', 'Final']
    ret = subprocess.run(command)
    if ret.returncode != 0:
        print('Build Game failed')
        return ret.returncode

    print('Build engine tools')
    command = ['py', '-u', 'Script/Compil/BuildAll.py', '-et']
    ret = subprocess.run(command)
    if ret.returncode != 0:
        print('Build Tools failed')
        return ret.returncode

    print('Generate all')
    command = ['py', '-u', 'Script/Generation/GenerateData.py']
    ret = subprocess.run(command)
    if ret.returncode != 0:
        print('Generate data failed')
        return ret.returncode

    print('Datapack')
    os.chdir('bin')
    command = ['./DataPacker-x64-Release.exe']
    ret = subprocess.run(command)
    if ret.returncode != 0:
        print('Datapack failed')
        return ret.returncode

    os.chdir('..')
    return 0

def main():

    ret = ProcessVersion()
    if(ret != 0):
        return ret

    print('Zipping up Version')
    ret = Finalize()

    return ret



if __name__ == "__main__":
   sys.exit(main())
