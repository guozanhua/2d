#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Include For Vision Engine
#include <Vision/Runtime/Base/VBase.hpp>
#include <Vision/Runtime/Engine/System/Vision.hpp>

#ifdef TOOLSET_2D_ENGINE_PLUGIN_EXPORTS
#define TOOLSET_2D_IMPEXP __declspec(dllexport)
#else
#define TOOLSET_2D_IMPEXP __declspec(dllimport)
#endif

extern VModule gToolset2D_EngineModule; // declared below with DECLARE_THIS_MODULE macro

