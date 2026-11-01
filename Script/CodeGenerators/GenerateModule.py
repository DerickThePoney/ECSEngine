#!/usr/bin/env python3

import sys
import os
import json
import subprocess

import argparse

MODULE_LIST_FILE_NAME = 'ModuleList.inl'
MAX_MODULE_NUMBER = 32

Module_H_Code = """
#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{{
class {modulename}Template : public ModuleTemplate
{{
    DECLARE_MODULE_TEMPLATE({modulename}, {modulename}Template);

public:
    {modulename}Template()
        : ModuleTemplate()
    {{
    }}
    ~{modulename}Template() {{ }}

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE() {{ }}

protected:
    void VirtualDrawEditor() override;
}};

class {modulename} : public Module
{{
    DECLARE_MODULE({modulename});

    DECLARE_SAVELOAD_ABILITIES();

public:
    {modulename}();
    ~{modulename}();
}};

}} // namespace ECSEngine
"""

Module_CPP_Code = """
#include "stdafx.h"

#include "{modulename}.h"

#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::{modulename}Template);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::{modulename}Template);

namespace ECSEngine
{{
IMPLEMENT_MODULE_TEMPLATE({modulename}, {modulename}Template);

Module* {modulename}Template::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{{
    return NewModule<{modulename}>(this, parUnitId, parParameters);
}}

void {modulename}Template::VirtualDrawEditor()
{{
}}

IMPLEMENT_SAVELOAD_ABILITIES({modulename});
template<typename Chunk, bool isWriting>
void {modulename}::SaveLoad(Chunk& parChunk)
{{
    parent_type::SaveLoad(parChunk);
}}

{modulename}::{modulename}()
    : Module()
{{
}}

{modulename}::~{modulename}()
{{
}}

}} // namespace ECSEngine
"""


def module_directory(where):
    if where == 'Common':
        return 'SRC/ECSGameplay_Common'
    manifest_path = 'Script/Generation/prototypes.json'
    with open(manifest_path, 'r', encoding='utf-8') as manifest_file:
        prototypes = json.load(manifest_file)
    for prototype in prototypes:
        if prototype['name'] == where:
            return 'SRC/Games/%s' % where
    return None

def count_declared_modules():
    lists = ['SRC/ECSGameplay_Common/ModuleList.inl']
    with open('Script/Generation/prototypes.json', 'r', encoding='utf-8') as manifest_file:
        for prototype in json.load(manifest_file):
            lists.append('SRC/Games/%s/ModuleList.inl' % prototype['name'])
    count = 0
    for path in lists:
        with open(path, 'r', encoding='utf-8') as module_list:
            for line in module_list:
                if line.strip().startswith('DECLARE_MODULE_AND_TEMPLATE'):
                    count += 1
    return count

def main():

    parser = argparse.ArgumentParser()
    parser.add_argument('-w', '--where', type=str, help='Common, or a prototype name from prototypes.json', required=True)
    parser.add_argument('-n', '--name', type=str, help='Module name', required=True)

    args = parser.parse_args()

    directory = module_directory(args.where)
    if directory is None:
        print('Unknown module destination: %s' % args.where)
        return -1

    moduleName = args.name
    if moduleName is None or len(moduleName) == 0:
        print('Error in the module name')
        return -1

    declared = count_declared_modules()
    if declared + 1 > MAX_MODULE_NUMBER:
        print('Adding %s would exceed MaxModuleNumber (%d)' % (moduleName, MAX_MODULE_NUMBER))
        return -1

    with open('%s/%s.cpp' % (directory, moduleName), 'w') as f:
        f.write(Module_CPP_Code.format(modulename=moduleName))

    with open('%s/%s.h' % (directory, moduleName), 'w') as f:
        f.write(Module_H_Code.format(modulename=moduleName))

    with open('%s/%s' % (directory, MODULE_LIST_FILE_NAME), 'a') as f:
        f.write('DECLARE_MODULE_AND_TEMPLATE(%s, %sTemplate)\n' % (moduleName, moduleName))

    result = subprocess.run(['py', '-u', 'Script/Compil/BuildAll.py', '-sln'])

    return result.returncode

if __name__ == "__main__":
   sys.exit(main())