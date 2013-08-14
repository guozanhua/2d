// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Include For Vision Engine
#include <Vision/Runtime/Base/VBase.hpp>
#include <Vision/Runtime/Engine/System/Vision.hpp>

#ifdef SPRITE_GAME_PLUGIN_EXPORTS
#define SPRITE_GAME_IMPEXP __declspec(dllexport)
#else
#define SPRITE_GAME_IMPEXP __declspec(dllimport)
#endif

extern VModule gSpriteGameModule; // declared below with DECLARE_THIS_MODULE macro

