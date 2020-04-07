#pragma once
#include "SceneActions.h"
namespace ECSEngine
{
class ISceneAction;
namespace SceneActionManagement
{
void InitialiseFactory();

bool RegisterSceneActionFactory(const u32 parId, ISceneAction* (*parFactory)());

ISceneAction* CreateSceneAction(const u32 parId);

const std::map<u32, std::string>& GetSceneActionsList();
} // namespace SceneActionManagement
} // namespace ECSEngine
