//
// THIS FILE WILL NOT COMPILE ON ITS OWN! ONLY INCLUDE THIS IN *ONE* CPP FILE FOR THE PROJECT.
//

#ifdef __cplusplus_cli
#pragma managed(push, off)
#endif

#if USE_HAVOK_PHYSICS_2D

#include <Common/Base/KeyCode.h>
#include <Common/Base/Types/hkBaseTypes.h>
#include <Common/Base/Config/hkConfigVersion.h>

#define HK_EXCLUDE_FEATURE_SerializeDeprecatedPre700
#define HK_EXCLUDE_FEATURE_MemoryTracker
#define HK_EXCLUDE_FEATURE_RegisterVersionPatches 

// Plugins have to reg their own classes, always, even if not a DLL
#undef HAVOK_ANIMATION_KEYCODE
#undef HK_FEATURE_PRODUCT_ANIMATION
#undef HAVOK_BEHAVIOR_KEYCODE
#undef HK_FEATURE_PRODUCT_BEHAVIOR
#undef HAVOK_CLOTH_KEYCODE
#undef HK_FEATURE_PRODUCT_CLOTH
#undef HAVOK_DESTRUCTION_2012_KEYCODE
#undef HK_FEATURE_PRODUCT_DESTRUCTION_2012
#undef HAVOK_AI_KEYCODE 
#undef HK_FEATURE_PRODUCT_AI 
#undef HAVOK_PHYSICS_KEYCODE
#undef HK_FEATURE_PRODUCT_PHYSICS
#undef HAVOK_DESTRUCTION_KEYCODE
#undef HK_FEATURE_PRODUCT_DESTRUCTION
#undef HAVOK_SIMULATION_KEYCODE
#undef HK_FEATURE_PRODUCT_MILSIM

#include <Common/Base/keycode.cxx>
#include <Common/Base/Config/hkProductFeatures.cxx>

#endif // USE_HAVOK_PHYSICS_2D

#ifdef __cplusplus_cli
#pragma managed(pop)
#endif