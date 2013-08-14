#include "SpriteGamePluginPCH.h"

#include "SpriteGameApplication.hpp"

// This class implements the IVisPlugin_cl interface. The engine queries an instance of this class via 
// the exported GetEnginePlugin class.
class SpriteGamePlugin_cl : public IVisPlugin_cl
{
public:
	// this function is only called once since an external init counter takes care
	VOVERRIDE void OnInitEnginePlugin()
	{
		Vision::RegisterModule(&gSpriteGameModule);
		SpriteGameManager::GlobalManager().OneTimeInit();
	}

	// only called once
	VOVERRIDE void OnDeInitEnginePlugin()
	{
		SpriteGameManager::GlobalManager().OneTimeDeInit();
		Vision::UnregisterModule(&gSpriteGameModule);
	}

	VOVERRIDE const char *GetPluginName()
	{
		// Must match DLL name
		return "SpriteGamePlugin";
	}
};

// our global instance of the plugin descriptor:
SpriteGamePlugin_cl g_SpriteGamePlugin;

// this plugin also declares a module which is needed for RTTI/serialization
DECLARE_THIS_MODULE( gSpriteGameModule,
					 MAKE_VERSION(1, 0),
					 "SpriteGamePlugin",
					 "Havok",
					 "2D toolset for Project Anarchy",
					 &g_SpriteGamePlugin );

////////////////////////////////////////////////////////////////////////////
// These functions are called directly by modules that statically link
// against this library. We give these function unique names so we don't 
// get any naming conflicts with other modules.
////////////////////////////////////////////////////////////////////////////

//  Use this to get and initialize the plugin when you link statically
VEXPORT IVisPlugin_cl* GetEnginePlugin_SpriteGamePlugin()
{
	return &g_SpriteGamePlugin;
}

#if (defined _DLL) || (defined _WINDLL)

//  The engine uses this to get and initialize the plugin dynamically
VEXPORT IVisPlugin_cl* GetEnginePlugin()
{
	return GetEnginePlugin_SpriteGamePlugin();
}

#endif // _DLL or _WINDLL

