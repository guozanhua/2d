#pragma once

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN

#ifdef TOOLSET_2D_ENGINE_PLUGIN_EXPORTS
#define TOOLSET_2D_IMPEXP __declspec(dllexport)
#else
#define TOOLSET_2D_IMPEXP __declspec(dllimport)
#endif

// Include For Vision Engine
#include <Vision/Runtime/Base/VBase.hpp>
#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>

extern VModule gToolset2D_EngineModule;

ANALYSIS_IGNORE_ALL_START
#pragma managed(push, off)
#include <Common/Base/hkBase.h>
#pragma managed(pop)
ANALYSIS_IGNORE_ALL_END
