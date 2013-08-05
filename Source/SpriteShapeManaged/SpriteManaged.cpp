// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM		: ALL
// PRODUCT		: VISION
// VISIBILITY		: PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// NodeManaged.cpp : Defines the entry point for the DLL application.
//

#include <Vision/Samples/vForge/SpriteShape/SpriteShapeManaged/SpriteShapeManagedPCH.h>

#if (_MSC_VER < 1400)  //MSVC 8.0
  // Microsoft Knowledge Base 814472
  #include "_vcclrit.h"
#endif

/*
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

*/

VIMPORT IVisPlugin_cl* GetEnginePlugin_SpriteEnginePlugin();


namespace NodeManaged
{
  public ref class ManagedModule
  {
  public:
  
    static void InitManagedModule()
    {
#if (_MSC_VER < 1400)  //pre MSVC 8.0
      // Microsoft Knowledge Base 814472
      __crt_dll_initialize();
#endif
      //Init the entity plugin directly since we link statically to it
      GetEnginePlugin_SpriteEnginePlugin()->InitEnginePlugin();
    }

    static void DeInitManagedModule()
    {
      //Deinit the entity plugin directly since we link statically to it
      GetEnginePlugin_SpriteEnginePlugin()->DeInitEnginePlugin();

#if (_MSC_VER < 1400)  //pre MSVC 8.0
      // Microsoft Knowledge Base 814472
      __crt_dll_terminate();
#endif
    }
  };

}

