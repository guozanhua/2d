#ifndef SPRITE_GAME_PLUGIN_PCH_H_INCLUDED
#define SPRITE_GAME_PLUGIN_PCH_H_INCLUDED

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Include For Vision Engine
#include <Vision/Runtime/Base/VBase.hpp>
#include <Vision/Runtime/Engine/System/Vision.hpp>

#if defined(WIN32)
	#ifdef SPRITE_GAME_PLUGIN_EXPORTS
	#define SPRITE_GAME_IMPEXP __declspec(dllexport)
	#else
	#define SPRITE_GAME_IMPEXP __declspec(dllimport)
	#endif
#else
	#define SPRITE_GAME_IMPEXP
#endif

extern VModule gSpriteGameModule;

#include "Toolset2D_EnginePluginPCH.h"

#endif // SPRITE_GAME_PLUGIN_PCH_H_INCLUDED