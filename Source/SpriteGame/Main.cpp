#include "SpriteGamePCH.h"

const int kWindowSizeX   = 1024;
const int kWindowSizeY   = 768;
const int kWindowPosX    = 500;
const int kWindowPosY    = 50;

const char kApplicationName[]  = "2D Sprite Game"; 
const char kStartUpScene[]     = "Scenes\\Shooter.vscene";

// Use the following line to initialize a plugin that is statically linked. 
// Note that only Windows platform links plugins dynamically (on Windows you can comment out this line).
VIMPORT IVisPlugin_cl* GetEnginePlugin_Toolset2D_EnginePlugin();
		 
VisSampleAppPtr spApp;

//---------------------------------------------------------------------------------------------------------
// Init function. Here we trigger loading our scene
//---------------------------------------------------------------------------------------------------------
VISION_INIT
{
	// Create our new application.
	spApp = new VisSampleApp();

	Vision::Error.SystemMessage( "Sprite Game started!" );

	// set the initial starting position of our game window
	// and other properties if not in fullscreen. This is only relevant on windows
#if defined(WIN32)
	spApp->m_appConfig.m_videoConfig.m_iXPos = kWindowPosX;
	spApp->m_appConfig.m_videoConfig.m_iYPos = kWindowPosY;
	spApp->m_appConfig.m_videoConfig.m_szWindowTitle = kApplicationName;
#endif

	// Set the executable directory the current directory
	VisionAppHelpers::MakeEXEDirCurrent();
	
	// Set the paths to our stand alone version to override the VisSAampleApp paths.
	// The paths are platform dependent
#if defined(WIN32)
	const VString szRoot = "..\\..\\..\\..";

	Vision::File.AddDataDirectory( szRoot + "\\ProjectPackages\\Base.pcdx9.vArcFolder" );
	Vision::File.AddDataDirectory( szRoot + "\\ProjectPackages\\Project.pcdx9.vArcFolder" );
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

	// Init the application and point it to the start up scene.
	if ( !spApp->InitSample("Sprite Game",
		 kStartUpScene,
		 VSampleFlags::VSAMPLE_SPLASHSCREEN | VSampleFlags::VSAMPLE_USEDESKTOPRESOLUTION | VSampleFlags::VSAMPLE_SHOWEXITPROMPT | VSampleFlags::VSAMPLE_CUSTOMDATADIRECTORIES,
		 kWindowSizeX,
		 kWindowSizeY) )
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
	spApp->UnloadScene();

	// Deinit the application
	spApp->DeInitSample();
	spApp = NULL;

	return true;
}

VISION_MAIN_DEFAULT