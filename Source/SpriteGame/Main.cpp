// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM       : ALL 
// PRODUCT        : VISION 
// VISIBILITY     : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

#include "SpriteGamePCH.h"

#include <Common/Base/KeyCode.h>

#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VHelp.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VLoadingScreen.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>

VIMPORT IVisPlugin_cl* GetEnginePlugin_vFmodEnginePlugin();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavok();
VIMPORT IVisPlugin_cl* GetEnginePlugin_Toolset2D_EnginePlugin();

class SpriteApp : public VAppImpl
{
public:SpriteApp()
	{
	}

	virtual ~SpriteApp()
	{
	}

	virtual void SetupAppConfig(VisAppConfig_cl& config) HKV_OVERRIDE;
	virtual void PreloadPlugins() HKV_OVERRIDE;

	virtual void Init() HKV_OVERRIDE;
	virtual void OnAfterSceneLoaded(bool bLoadingSuccessful);
	virtual bool Run() HKV_OVERRIDE;
	virtual void DeInit() HKV_OVERRIDE;

protected:
	bool AddFileSystems();
};

VAPP_IMPLEMENT_SAMPLE(SpriteApp);

void SpriteApp::SetupAppConfig(VisAppConfig_cl& config)
{
  // Set custom file system root name ("havok_sdk" by default)
  config.m_sFileSystemRootName = "template_root";

  // Set the initial starting position of our game window and other properties
  // if not in fullscreen. This is only relevant on windows
  config.m_videoConfig.m_iXRes = 1280; // Set the Window size X if not in fullscreen.
  config.m_videoConfig.m_iYRes = 720;  // Set the Window size Y if not in fullscreen.
  config.m_videoConfig.m_iXPos = 50;   // Set the Window position X if not in fullscreen.
  config.m_videoConfig.m_iYPos = 50;   // Set the Window position Y if not in fullscreen.

  // Name to be displayed in the windows title bar.
  config.m_videoConfig.m_szWindowTitle = "One Shot - LD 28";

  config.m_videoConfig.m_bWaitVRetrace = true;

  // Fullscreen mode with current desktop resolution
  
#if defined(WIN32)
  /*
  DEVMODEA deviceMode;
  deviceMode = Vision::Video.GetAdapterMode(config.m_videoConfig.m_iAdapter);
  config.m_videoConfig.m_iXRes = deviceMode.dmPelsWidth;
  config.m_videoConfig.m_iYRes = deviceMode.dmPelsHeight;
  config.m_videoConfig.m_bFullScreen = true;
  */
#endif  
}

void SpriteApp::PreloadPlugins()
{
	VISION_PLUGIN_ENSURE_LOADED(vHavok);
	VISION_PLUGIN_ENSURE_LOADED(vFmodEnginePlugin);
	VISION_PLUGIN_ENSURE_LOADED(Toolset2D_EnginePlugin);

	//AddFileSystems();
}

//---------------------------------------------------------------------------------------------------------
// Init function. Here we trigger loading our scene
//---------------------------------------------------------------------------------------------------------
void SpriteApp::Init()
{
	VLoadingScreen *loadingScreen = GetAppModule<VLoadingScreen>();

	VLoadingScreenBase::Settings loadingScreenSettings("Textures/Anarchy_Splash_1024x512.dds");
	loadingScreen->SetSettings(loadingScreenSettings);

	VisAppLoadSettings settings("Scenes/Shooter.pcdx9.vscene");
	settings.m_customSearchPaths.Append(":template_root/Assets");

	//AddFileSystems();

	LoadScene(settings);
}

//---------------------------------------------------------------------------------------------------------
// Gets called after the scene has been loaded
//---------------------------------------------------------------------------------------------------------
void SpriteApp::OnAfterSceneLoaded(bool bLoadingSuccessful)
{
}

//---------------------------------------------------------------------------------------------------------
// Main Loop of the application until we quit
//---------------------------------------------------------------------------------------------------------
bool SpriteApp::Run()
{
  return true;
}

void SpriteApp::DeInit()
{
  // De-Initialization
  // [...]
}

bool SpriteApp::AddFileSystems()
{
	bool failed = false;

	const VString szRoot = "..\\..\\..\\..";

	/*
	VStaticString<FS_MAX_PATH> sPackagePath = "/Project.pcdx9.vArc";
	VStaticString<FS_MAX_PATH> sProjectPath;


	VStaticString<FS_MAX_PATH> sRootPath;
	if (VPathHelper::MakeAbsoluteDir("", sRootPath.AsChar()) != NULL)
	{
		sProjectPath = sRootPath;
		sProjectPath += sPackagePath;
		if (!VFileHelper::Exists(sProjectPath))
		{
			if (VPathHelper::MakeAbsoluteDir("../../../../Assets", sRootPath.AsChar()) != NULL)
			{
				sProjectPath = sRootPath;
				sProjectPath += sPackagePath;
				if (!VFileHelper::Exists(sProjectPath))
				{
					failed = true;
				}
			}
		}
	}

	if (!failed)
	{
		bool success = Vision::File.AddFileSystem("template_root", sProjectPath, VFileSystemFlags::ADD_SEARCH_PATH);
		VASSERT(success);
	}

	*/

	return !failed;
}