#ifndef TOOLSET2D_ENGINEPLUGIN_PCH_H_INCLUDED
#define TOOLSET2D_ENGINEPLUGIN_PCH_H_INCLUDED

// #todo : disabling for now since there are still some known issues with it
#define USE_HAVOK_PHYSICS_2D	0

// #todo : not yet implemented
#define USE_BOX_2D				0

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
#include <Vision/Runtime/Base/Graphics/VColor.hpp>

#ifdef WIN32
#include <crtdbg.h>
#endif

#if !defined(__HAVOK_PARSER__) && USE_HAVOK_PHYSICS_2D
#undef swap16
#define HK_CLASSES_FILE <Common/Serialize/Classlist/hkClasses.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>
#else
#include <Common/Base/KeyCode.h>
#include <Common/Base/hkBase.h>
#include <Common/Base/Ext/hkBaseExt.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Thread/hkThreadMemory.h>
#endif

extern VModule gToolset2D_EngineModule;

#endif // TOOLSET2D_ENGINEPLUGIN_PCH_H_INCLUDED