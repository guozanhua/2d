#include "SpriteGamePCH.h"

#include "VisSampleApp.hpp"

//============================================================================================================
// Properties for start up. Some of the settings are not relevant for mobile devices
//============================================================================================================
int windowSizeX    = 1024;               // Set the Window size X if not in fullscreen.
int windowSizeY    = 768;                // Set the Window size Y if not in fullscreen.
int windowPosX    = 500;                 // Set the Window position X if not in fullscreen.
int windowPosy    = 50;                  // Set the Window position Y if not in fullscreen.

char name[]      = "2D Sprite Game";  // Name to be displayed in the windows title bar.
char StartUpScene[]  = "Scenes\\Default.vscene";   // Set the location of your start up scene.

float cameraInitX = 0;                    //
float cameraInitY = 0;                    //
float cameraInitZ = 170;                  // Set our camera above the ground so that we can see 
										  // the ground.

// Use the following line to initialize a plugin that is statically linked. 
// Note that only Windows platform links plugins dynamically (on Windows you can comment out this line).
VIMPORT IVisPlugin_cl* GetEnginePlugin_SpriteGame_EnginePlugin();
VIMPORT IVisPlugin_cl* GetEnginePlugin_Toolset2D_EnginePlugin();
		 
VisSampleAppPtr spApp;

//---------------------------------------------------------------------------------------------------------
// Init function. Here we trigger loading our scene
//---------------------------------------------------------------------------------------------------------
VISION_INIT
{
	// Create our new application.
	spApp = new VisSampleApp();

	// set the initial starting position of our game window
	// and other properties if not in fullscreen. This is only relevant on windows
#if defined(WIN32)
	spApp->m_appConfig.m_videoConfig.m_iXPos = windowPosX;
	spApp->m_appConfig.m_videoConfig.m_iYPos = windowPosy;
	spApp->m_appConfig.m_videoConfig.m_szWindowTitle = name;
#endif

	// Set the executable directory the current directory
	VisionAppHelpers::MakeEXEDirCurrent();

	// Set the paths to our stand alone version to override the VisSAampleApp paths.
	// The paths are platform dependent
#if defined(WIN32)
	const VString szRoot = "..\\..\\..\\..";
	Vision::File.AddDataDirectory( szRoot + "\\Assets" );
	Vision::File.AddDataDirectory( szRoot + "\\Data\\Vision\\Base" );
#elif defined(_VISION_ANDROID)
	VString szRoot = VisSampleApp::GetApkDirectory();
	szRoot += "?assets";
	Vision::File.AddDataDirectory( szRoot + "\\Assets" );

	// "/Data/Vision/Base" is always added by the sample app
#elif defined(_VISION_IOS)
	// setup directories, does nothing on platforms other than iOS,
	// pass true if you want load from the documents directory
	VISION_SET_DIRECTORIES(false);
	VString szRoot = VisSampleApp::GetRootDirectory();

	// our deploy script always copies the asset data below the "Data" folder
	Vision::File.AddDataDirectory( szRoot + "/Data/Assets" );

	// "/Data/Vision/Base" is always added by the sample app
#endif

#if defined(VISION_OUTPUT_DIR)
	// Set the output directory manually since VSAMPLE_CUSTOMDATADIRECTORIES was specified
	// at the initialization.
	Vision::File.SetOutputDirectory(VISION_OUTPUT_DIR);
	Vision::File.AddDataDirectory(VISION_OUTPUT_DIR);
#endif
	
	spApp->LoadVisionEnginePlugin();

	// Use the following line to load a plugin. Remember that, except on Windows platform, in addition
	// you still need to statically link your plugin library (e.g. on mobile platforms) through project
	// Properties, Linker, Additional Dependencies.
	VISION_PLUGIN_ENSURE_LOADED(Toolset2D_EnginePlugin);
	VISION_PLUGIN_ENSURE_LOADED(SpriteGamePlugin);

	// Init the application and point it to the start up scene.
	if ( !spApp->InitSample("Sprite Game",
		StartUpScene,
		VSAMPLE_SPLASHSCREEN | VSAMPLE_USEDESKTOPRESOLUTION | VSAMPLE_SHOWEXITPROMPT | VSAMPLE_CUSTOMDATADIRECTORIES,
		windowSizeX,
		windowSizeY) )
	{
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------------------------------------
// Gets called after the scene has been loaded
//---------------------------------------------------------------------------------------------------------

VISION_SAMPLEAPP_AFTER_LOADING
{
	// define some help text
	spApp->AddHelpText( "" );
	spApp->AddHelpText( "How to use this demo :" );
	spApp->AddHelpText( "" );

	// Create a mouse controlled camera (optionally with gravity)
	VisBaseEntity_cl *pCamera = spApp->EnableMouseCamera();
	pCamera->SetPosition( hkvVec3(cameraInitX, cameraInitY, cameraInitZ) );
}

//---------------------------------------------------------------------------------------------------------
// main loop of the application until we quit
//---------------------------------------------------------------------------------------------------------

VISION_SAMPLEAPP_RUN
{
	return spApp->Run();
}

VISION_DEINIT
{
	// Deinit the application
	spApp->DeInitSample();
	spApp = NULL;

	return true;
}

VISION_MAIN_DEFAULT