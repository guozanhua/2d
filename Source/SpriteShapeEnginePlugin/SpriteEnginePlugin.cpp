// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM		: ALL
// PRODUCT		: VISION
// VISIBILITY		: PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

////////////////////////////////////////////////////////////////////////////
//
// This project is a sample engine plugin DLL that contains the classes used
// by the engine and vForge for integrating it into the Vision Engine
// 
////////////////////////////////////////////////////////////////////////////
#include <Vision/Samples/vForge/SpriteShape/SpriteShapeEnginePlugin/Source/SpriteShapeEnginePluginPCH.h>
#include <Vision/Samples/vForge/SpriteShape/SpriteShapeEnginePlugin/Source/VNode.hpp>


// This class implements the IVisPlugin_cl interface. The engine queries an instance of this class via 
// the exported GetEnginePlugin class.
class DemoEnginePlugin_cl : public IVisPlugin_cl
{
public:

  // this function is only called once since an external init counter takes care
  VOVERRIDE void OnInitEnginePlugin()
  {
    Vision::RegisterModule(&g_NodeModule);
    VNodeMananger_cl::GlobalManager().OneTimeInit();
  }

  // only called once
  VOVERRIDE void OnDeInitEnginePlugin()
  {
    Vision::UnregisterModule(&g_NodeModule);
    VNodeMananger_cl::GlobalManager().OneTimeDeInit();
  }
  
  VOVERRIDE const char *GetPluginName()
  {
    return "SpriteEnginePlugin";  //must match DLL name
  }
};


// our global instance of the plugin descriptor:
DemoEnginePlugin_cl g_DemoEnginePlugin;

// this plugin also declares a module which is needed for RTTI/serialization
DECLARE_THIS_MODULE( g_NodeModule, MAKE_VERSION(1, 0),
                     "Nodes", "Havok", "Vision Engine Nodes", &g_DemoEnginePlugin );

////////////////////////////////////////////////////////////////////////////
// These functions are called directly by modules that statically link
// against this library. We give these function unique names so we don't 
// get any naming conflicts with other modules.
////////////////////////////////////////////////////////////////////////////

//  Use this to get and initialize the plugin when you link statically
VEXPORT IVisPlugin_cl* GetEnginePlugin_SpriteEnginePlugin()
{
  return &g_DemoEnginePlugin;
}

#if (defined _DLL) || (defined _WINDLL)

//  The engine uses this to get and initialize the plugin dynamically
VEXPORT IVisPlugin_cl* GetEnginePlugin()
{
  return GetEnginePlugin_SpriteEnginePlugin();
}

#endif // _DLL or _WINDLL

