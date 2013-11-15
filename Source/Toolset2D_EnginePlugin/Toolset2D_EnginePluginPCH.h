#ifndef TOOLSET2D_ENGINEPLUGIN_PCH_H_INCLUDED
#define TOOLSET2D_ENGINEPLUGIN_PCH_H_INCLUDED

// The following defines are necessary primarily on Android so that there aren't duplicate
// symbols generated for SWIG and vHavok
#ifndef WIN32
#define swig_types						t2d_swig_types
#define swig_module						t2d_swig_module
#define VSWIG_Lua_get_class_registry	T2D_VSWIG_Lua_get_class_registry
#define VSWIG_Lua_ConvertPtr			T2D_VSWIG_Lua_ConvertPtr
#define VSWIG_Lua_NewPointerObj			T2D_VSWIG_Lua_NewPointerObj
#define VSWIG_TypeCast					T2D_VSWIG_TypeCast
#define VSWIG_TypeCheckStruct			T2D_VSWIG_TypeCheckStruct
#define VSWIG_VisionLuaClassSet			T2D_VSWIG_VisionLuaClassSet
#define VSWIG_VisionLuaClassGet			T2D_VSWIG_VisionLuaClassGet
#define VSWIG_TypeCheck					T2D_VSWIG_TypeCheck
#define VSWIG_Lua_typename				T2D_VSWIG_Lua_typename
#endif

#define HK_CLASSES_FILE <Common/Serialize/Classlist/hkClasses.h>

#if defined(WIN32)
	// Exclude rarely-used stuff from Windows headers
	#define WIN32_LEAN_AND_MEAN

	#ifdef TOOLSET_2D_ENGINE_PLUGIN_EXPORTS
	#define TOOLSET_2D_IMPEXP __declspec(dllexport)
	#else
	#define TOOLSET_2D_IMPEXP __declspec(dllimport)
	#endif
#else
	#define TOOLSET_2D_IMPEXP
#endif

// Include For Vision Engine
#include <Vision/Runtime/Base/VBase.hpp>
#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>
#include <Vision/Runtime/Base/Graphics/VColor.hpp>

extern VModule gToolset2D_EngineModule;

ANALYSIS_IGNORE_ALL_START
#pragma managed(push, off)
#include <Common/Base/hkBase.h>
#pragma managed(pop)
ANALYSIS_IGNORE_ALL_END

#endif // TOOLSET2D_ENGINEPLUGIN_PCH_H_INCLUDED