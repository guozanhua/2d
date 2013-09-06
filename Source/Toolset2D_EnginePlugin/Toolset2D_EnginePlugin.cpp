//=======
//
// Author: Joel Van Eenwyk
// Purpose: Starting point for the 2D toolset plugin which handles initializing
//          the sprite manager and ensuring LUA is registered properly with
//          our plugin.
//
//=======
//
//  ##  ########  ##   
//    #   ####   #    
//     ##  ##   ##     
//      ##     ##      
//       ##   ##       
//       ##  ##        
//       ##  ##        
//      ###  ###       
//      ##    ##       
//     ##      ##      
//    ##        ##     
//   ##          ##    
//  ##            ##   
//  #              #   
// #                #  
//

#include "Toolset2D_EnginePluginPCH.h"

#include "SpriteManager.hpp"

// This class implements the IVisPlugin_cl interface. The engine queries an instance of this class via 
// the exported GetEnginePlugin class.
class Toolset2D_EnginePlugin_cl : public IVisPlugin_cl
{
public:
	// this function is only called once since an external init counter takes care
	VOVERRIDE void OnInitEnginePlugin()
	{
		Vision::Error.SystemMessage( "Toolset2D_EnginePlugin_cl:OnInitEnginePlugin()" );

		Vision::RegisterModule(&gToolset2D_EngineModule);

		SpriteManager *module = &SpriteManager::GlobalManager();
		if (module != NULL)
		{
			module->OneTimeInit();
		}
		
		SpriteManager::RegisterLua();
	}

	// only called once
	VOVERRIDE void OnDeInitEnginePlugin()
	{
		Vision::Error.SystemMessage( "Toolset2D_EnginePlugin_cl:OnDeInitEnginePlugin()" );

		SpriteManager *module = &SpriteManager::GlobalManager();
		if (module != NULL)
		{
			module->OneTimeDeInit();
		}

		Vision::UnregisterModule(&gToolset2D_EngineModule);
	}

	VOVERRIDE const char *GetPluginName()
	{
		// Must match DLL name
		return "Toolset2D_EnginePlugin";
	}
};

// our global instance of the plugin descriptor:
Toolset2D_EnginePlugin_cl g_Toolset2D_EnginePlugin;

// this plugin also declares a module which is needed for RTTI/serialization
DECLARE_THIS_MODULE( gToolset2D_EngineModule,
					 MAKE_VERSION(1, 0),
					 "Toolset2D_EnginePlugin",
					 "Havok",
					 "2D toolset for Project Anarchy",
					 &g_Toolset2D_EnginePlugin );

////////////////////////////////////////////////////////////////////////////
// These functions are called directly by modules that statically link
// against this library. We give these function unique names so we don't 
// get any naming conflicts with other modules.
////////////////////////////////////////////////////////////////////////////

//  Use this to get and initialize the plugin when you link statically
VEXPORT IVisPlugin_cl* GetEnginePlugin_Toolset2D_EnginePlugin()
{
	return &g_Toolset2D_EnginePlugin;
}

#if (defined _DLL) || (defined _WINDLL)

//  The engine uses this to get and initialize the plugin dynamically
VEXPORT IVisPlugin_cl* GetEnginePlugin()
{
	return GetEnginePlugin_Toolset2D_EnginePlugin();
}

#endif // _DLL or _WINDLL

