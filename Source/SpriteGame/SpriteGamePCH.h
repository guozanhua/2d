#if !defined(SPRITEGAME_PCH_H_INCLUDED)
#define SPRITEGAME_PCH_H_INCLUDED

#if defined(WIN32)
	#define WIN32_LEAN_AND_MEAN    // Exclude rarely-used stuff from Windows headers
#endif

#define VISION_SAMPLEAPP_CALLBACKS

#include <Vision/Runtime/Base/VBase.hpp>
#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Runtime/Common/VisSampleApp.hpp>

#endif