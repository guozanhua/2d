#include "Toolset2D_EnginePluginPCH.h"

#include <Common/Base/Config/hkConfigVersion.h>
#if (HAVOK_SDK_VERSION_MAJOR >= 2010)

// Serialization and patches
#include <Common/Base/KeyCode.h>

#define HK_EXCLUDE_FEATURE_SerializeDeprecatedPre700
//#define HK_EXCLUDE_FEATURE_MemoryTracker
//#define HK_EXCLUDE_FEATURE_RegisterVersionPatches 

// Plugins have to reg thier own classes, always, even if not a dll.
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

#include <Common/Base/Config/hkProductFeatures.cxx>

#else // 7.1 etc

// Register Havok classes that are appropriate for the current keycode strings
#include <Common/Base/KeyCode.h>
#define HK_CLASSES_FILE <Common/Serialize/Classlist/hkKeyCodeClasses.h>
#include <Common/Serialize/Util/hkBuiltinTypeRegistry.cxx>

// Register versioning information.
// If you don't want the ability to load previous Havok versions then use hkCompat_None instead to reduce code size
#define HK_COMPAT_FILE <Common/Compat/hkCompat.h>

#endif

