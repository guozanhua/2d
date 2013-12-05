#include "Toolset2D_ManagedPCH.h"

#pragma managed(push, off)
#include <Common/Base/Types/hkBaseTypes.h>
#include <Common/Base/UnitTest/hkUnitTest.h>
#include <Common/Base/keycode.cxx>

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
#undef HAVOK_SIMULATION_KEYCODE
#undef HK_FEATURE_PRODUCT_MILSIM
#undef HAVOK_PHYSICS_KEYCODE
#undef HK_FEATURE_PRODUCT_PHYSICS
#undef HAVOK_DESTRUCTION_KEYCODE
#undef HK_FEATURE_PRODUCT_DESTRUCTION

#define HK_EXCLUDE_FEATURE_SerializeDeprecatedPre700
#define HK_EXCLUDE_FEATURE_RegisterVersionPatches
#define HK_EXCLUDE_FEATURE_RegisterReflectedClasses
#include <Common/Base/Config/hkProductFeatures.cxx>

#pragma managed(pop)

// -------------------------------------------------------------------------- //
// Not used at runtime; defined to prevent linker errors.                     //
// -------------------------------------------------------------------------- //
//class 
const hkTestEntry* hkUnitTestDatabase[]; 
hkBool HK_CALL hkTestReport(hkBool32 cond, const char* desc, const char* file, int line)  
{  
	return false;  
}

