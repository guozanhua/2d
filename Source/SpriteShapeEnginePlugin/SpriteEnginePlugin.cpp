#include "SpriteShapeEnginePluginPCH.h"

#include <VNode.hpp>

// This class implements the IVisPlugin_cl interface. The engine queries an instance of this class via 
// the exported GetEnginePlugin class.
class SpriteShapeEnginePlugin_cl : public IVisPlugin_cl
{
public:

  // this function is only called once since an external init counter takes care
  VOVERRIDE void OnInitEnginePlugin()
  {
    Vision::RegisterModule(&gSpriteShapeEngineModule);
    VNodeMananger_cl::GlobalManager().OneTimeInit();
  }

  // only called once
  VOVERRIDE void OnDeInitEnginePlugin()
  {
    Vision::UnregisterModule(&gSpriteShapeEngineModule);
    VNodeMananger_cl::GlobalManager().OneTimeDeInit();
  }
  
  VOVERRIDE const char *GetPluginName()
  {
    return "SpriteShapeEnginePlugin";  //must match DLL name
  }
};


// our global instance of the plugin descriptor:
SpriteShapeEnginePlugin_cl g_SpriteShapeEnginePlugin;

// this plugin also declares a module which is needed for RTTI/serialization
DECLARE_THIS_MODULE( gSpriteShapeEngineModule,
					 MAKE_VERSION(1, 0),
					 "SpriteShapeEnginePlugin",
					 "Havok",
					 "Adds functionality for use with 2D",
					 &g_SpriteShapeEnginePlugin );

////////////////////////////////////////////////////////////////////////////
// These functions are called directly by modules that statically link
// against this library. We give these function unique names so we don't 
// get any naming conflicts with other modules.
////////////////////////////////////////////////////////////////////////////

//  Use this to get and initialize the plugin when you link statically
VEXPORT IVisPlugin_cl* GetEnginePlugin_SpriteShapeEnginePlugin()
{
  return &g_SpriteShapeEnginePlugin;
}

#if (defined _DLL) || (defined _WINDLL)

//  The engine uses this to get and initialize the plugin dynamically
VEXPORT IVisPlugin_cl* GetEnginePlugin()
{
  return GetEnginePlugin_SpriteShapeEnginePlugin();
}

#endif // _DLL or _WINDLL

