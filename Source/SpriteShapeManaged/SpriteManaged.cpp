#include "SpriteShapeManagedPCH.h"

#if (_MSC_VER < 1400)  //MSVC 8.0
  // Microsoft Knowledge Base 814472
  #include "_vcclrit.h"
#endif

VIMPORT IVisPlugin_cl* GetEnginePlugin_SpriteShapeEnginePlugin();

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
      GetEnginePlugin_SpriteShapeEnginePlugin()->InitEnginePlugin();
    }

    static void DeInitManagedModule()
    {
      //Deinit the entity plugin directly since we link statically to it
      GetEnginePlugin_SpriteShapeEnginePlugin()->DeInitEnginePlugin();

#if (_MSC_VER < 1400)  //pre MSVC 8.0
      // Microsoft Knowledge Base 814472
      __crt_dll_terminate();
#endif
    }
  };

}

