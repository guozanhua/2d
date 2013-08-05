// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM		: ALL
// PRODUCT		: PHYSICS_2012+VISION
// VISIBILITY		: PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// ***********************************************************************************************
// Sample Game Demo
// Copyright (C) Trinigy GmbH. All rights reserved.
// ***********************************************************************************************
// This demo shows how to create a plugin for vForge that allows to play the game in vForge.
// There is one instance of a game manager that listens to important engine callback functions
// such as switching to play-the-game mode in vForge. This sample shows which editor states
// have to be taken into account when initializing the entities. For instance, physics objects
// are only created in play-the-game mode.
// This plugin runs in vForge and also plays the game when an exported scene is started via the
// SceneViewer.
// Note that this sample by default compiles into the bin folder of the enginesamples directory.
// To compile it to the vForge project folder (that's where vForge loads the plugins from) re-
// direct the output path in the project settings (Project Properties -> Linker -> General -> Output File)
// Also note that vForge loads the RELEASE version of the plugin (.vPlugin). To run the plugin
// in DEBUG mode, you need to compile a debug version of vForge. See vForge documentation.
// Alternatively, the (debug version of the) SceneViewer can be launched from this project to run
// an exported vscene file.
// ***********************************************************************************************
#include <Vision/Samples/Engine/SpriteGame/SpriteGamePluginPCH.h>
#include <Vision/Samples/Engine/SpriteGame/SpriteGameApplication.hpp>


class SpriteGamePlugin_cl : public IVisPlugin_cl
{
public:

  void OnInitEnginePlugin();
  void OnDeInitEnginePlugin();
  
  const char *GetPluginName()
  {
    return "SpriteGamePlugin";  //must match DLL name
  }
  
};

SpriteGamePlugin_cl g_SpriteGamePlugin;

// declare a module for the serialization
DECLARE_THIS_MODULE(g_sampleModule, MAKE_VERSION(1,0),
                    "Sample Plugin", "Havok",
                    "A sample game plugin", &g_SpriteGamePlugin);


//  Use this to get and initialize the plugin when you link statically
VEXPORT IVisPlugin_cl* GetEnginePlugin_SpriteGamePlugin()
{
  return &g_SpriteGamePlugin;
}

#if ((defined _DLL) || (defined _WINDLL)) && !defined(VBASE_LIB)
//  The engine uses this to get and initialize the plugin dynamically
VEXPORT IVisPlugin_cl* GetEnginePlugin()
{
  return GetEnginePlugin_SpriteGamePlugin();
}
#endif // _DLL or _WINDLL

void SpriteGamePlugin_cl::OnInitEnginePlugin()
{
  // register our module when the plugin is initialized
  Vision::RegisterModule(&g_sampleModule);

  // make sure the compiler does not optimize away our classes 
  //FORCE_LINKDYNCLASS(Player);
  //FORCE_LINKDYNCLASS(HealthPack);

  MyGameManager::GlobalManager().OneTimeInit();
}

void SpriteGamePlugin_cl::OnDeInitEnginePlugin()
{
  MyGameManager::GlobalManager().OneTimeDeInit();

  // de-register our module when the plugin is de-initialized
  Vision::UnregisterModule(&g_sampleModule);
}


