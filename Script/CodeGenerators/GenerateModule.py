#!/usr/bin/env python3

import sys
import os
import subprocess

import argparse

WORLD_DECLARATION_FILE_NAME = 'SRC/ECSGameplay_Common/WorldDeclaration.cpp'
MODULE_LIST_FILE_NAME = 'ModuleList.inl'

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

public:
    {modulename}();
    ~{modulename}();
}};

}} // namespace ECSEngine
"""

Module_CPP_Code = """
#include "stdafx.h"

#include "{modulename}.h"

#include "ECSCore/EntityTemplateManager.h"
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

{modulename}::{modulename}()
    : Module()
{{
}}

{modulename}::~{modulename}()
{{
}}

}} // namespace ECSEngine
"""


def main():

    parser = argparse.ArgumentParser()
    parser.add_argument('-w', '--where', type=str, help='Tells where to put the module (Specific, Common)', default='Specific')
    parser.add_argument('-n', '--name', type=str, help='Module name', required=True)

    args = parser.parse_args()

    projectName = args.where

    if not (projectName == 'Specific' or projectName == 'Common'):
        print('error in the name of the project')
        return -1

    if projectName == 'Specific':
        projectName = 'ECSGameplay_Specific'
    else:
        projectName = 'ECSGameplay_Common'

    moduleName = args.name
    if moduleName is None:
        print('Error in the module name')
        return -1

    # write cpp
    with open('SRC/%s/%s.cpp' % (projectName, moduleName), 'w') as f:
        global Module_CPP_Code
        res = Module_CPP_Code.format(modulename=moduleName)
        f.write(res)

    # write h
    with open('SRC/%s/%s.h' % (projectName, moduleName), 'w') as f:
        global Module_H_Code
        res = Module_H_Code.format(modulename=moduleName)
        f.write(res)

    # write the declaration
    with open('SRC/%s/%s' % (projectName, MODULE_LIST_FILE_NAME), 'a') as f:
        f.write('DECLARE_MODULE_AND_TEMPLATE(%s, %sTemplate)\n'% (moduleName, moduleName))


    lines=[]
    with open('%s' % (WORLD_DECLARATION_FILE_NAME), 'r') as f:

        added = False
        for line in f:

            if not added and 'namespace' in line:
                lines.append('#include "%s/%s.h"\n\n' % (projectName, moduleName))
                added = True

            lines.append(line)

    with open('%s' % (WORLD_DECLARATION_FILE_NAME), 'w') as f:
        for line in lines:
            f.write(line)

    result = subprocess.run(['./tools/premake5.exe', 'vs2019'])

    return result.returncode

if __name__ == "__main__":
   sys.exit(main())