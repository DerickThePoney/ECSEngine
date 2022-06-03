#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import os.path
import subprocess
import codecs
import glob
import platform

import re


fbuildtemplate = """
.GameName = 'BuildingGame'
.VSPATH = '%(vspath)s'
.VSINSTALLVERSION = '%(vsversion)s'
.VCTOOLVERSION = '%(vc_tools_version)s'
.VCREDISTVERSION = '%(vc_redist_version)s'
.WindowsSDKVersion10 = '%(vc_sdk_value)s'
.PCHName = 'stdafx'
.SolutionName = 'ECSEngine'
.SolutionOutput = 'build'
.ObjDir = "$SolutionOutput$/Intermediary"
.OutputDir = "bin"
.ExternalLibraries = {}

#include "BFF/Game.bff"
"""

def GetVSStuff():
    vswhere = "C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe"
    vswhere_params = [vswhere, '-products', '*', '-version', '[16.7,18.0)', '-property', 'installationPath', '-latest', '-format', 'value']
    vswhere_popen = subprocess.Popen(vswhere_params, stdout = subprocess.PIPE)
    (vs2017_path, vswhere_stderr) = vswhere_popen.communicate(None)

    vswhere = "C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe"
    vswhere_params = [vswhere, '-products', '*', '-version', '[16.7,18.0)', '-property', 'installationVersion', '-latest']
    vswhere_popen = subprocess.Popen(vswhere_params, stdout = subprocess.PIPE)
    (vs2017_version, vswhere_stderr) = vswhere_popen.communicate(None)
    vs2017_version = vs2017_version.decode().split('\r\n')[0]
    print(vs2017_version)

    windowsSDKBasePath10 = "C:\\Program Files (x86)\\Windows Kits\\10\\Include"
    sdks = os.listdir(windowsSDKBasePath10)
    sdk = 0
    vc_sdk_value = ''
    for d in sdks:
        splited = d.split('.')
        print(splited)
        if len(splited) > 3 and sdk < int(splited[2]):
            vc_sdk_value = d

    if vc_sdk_value == '':
        return False, vs2017_path, vs2017_version, vc_tools_version, vc_redist_version, vc_sdk_value


    if vs2017_path is None or len(vs2017_path) == 0:
        print("Impossible de récuperer le chemin d'installation de VS avec vswhere: " + str(vswhere_stderr))
    else:
        vs2017_path = vs2017_path.rstrip().decode("utf-8")
        print("Le path de visual VS est : " + vs2017_path)
        print("La version de visual VS est : " + vs2017_version)

        vc_version_file = open(vs2017_path + "\\VC\\Auxiliary\\Build\\Microsoft.VCToolsVersion.default.txt", 'r')
        vc_tools_version = vc_version_file.read().rstrip()
        vc_version_file.close()

        if vc_tools_version is None or len(vc_tools_version) == 0:
            print("Impossible de récuperer VCToolsVersion: " + str(cat_vs_version_stderr))
        else:
            print("VCToolsVersion : " + vc_tools_version)

        vc_version_file = open(vs2017_path + "\\VC\\Auxiliary\\Build\\Microsoft.VCRedistVersion.default.txt", 'r')
        vc_redist_version = vc_version_file.read().rstrip()
        vc_version_file.close()

        if vc_redist_version is None or len(vc_redist_version) == 0:
            print("Impossible de récuperer VCRedistVersion: " + str(cat_vs_version_stderr))
        else:
            print("VCRedistVersion : " + vc_redist_version)
            return True, vs2017_path, vs2017_version, vc_tools_version, vc_redist_version, vc_sdk_value

    return False, vs2017_path, vs2017_version, vc_tools_version, vc_redist_version, vc_sdk_value

def main():
    success, vs_path, vs_version, vc_tools_version, vc_redist_version, vc_sdk_value = GetVSStuff()

    if not success:
        print('error')
        return -1

    buffer = fbuildtemplate % {
        'vspath': vs_path,
        'vsversion': vs_version,
        'vc_tools_version': vc_tools_version,
        'vc_redist_version': vc_redist_version,
        'vc_sdk_value': vc_sdk_value
    }

    solution_config_filename = 'fbuild.bff'

    try:
        solution_config_file = codecs.open(solution_config_filename, "r", "utf-8")
        current_file = solution_config_file.read()
        solution_config_file.close()
    except IOError:
        current_file = ''

    if current_file != buffer:
        print("Recreating fbuild.bff")
        sys.stdout.flush()
        solution_config_file = codecs.open(solution_config_filename, "w", "utf-8")
        solution_config_file.write(buffer)
        solution_config_file.close()
    else:
        print("Keeping current fbuild.bff")
        sys.stdout.flush()

    return 0

if __name__ == "__main__":
    sys.exit(main())