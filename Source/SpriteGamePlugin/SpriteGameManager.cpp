#include "SpriteGamePluginPCH.h"

#include "SpriteGameManager.hpp"
#include "SpriteEntity.hpp"

#ifdef WIN32
#include <Vision/Runtime/EnginePlugins/RemoteInputEnginePlugin/IVRemoteInput.hpp>
#endif

VIMPORT IVisPlugin_cl* GetEnginePlugin_Toolset2D_EnginePlugin();

void SpriteGameManager::OneTimeInit()
{
	Vision::Error.SystemMessage("SpriteGameManager:OneTimeInit()");

	VISION_PLUGIN_ENSURE_LOADED(Toolset2D_EnginePlugin);

	m_bPlayingTheGame = false;
	
	// the game manager should listen to the following callbacks:
	Vision::Callbacks.OnEditorModeChanged += this;
	Vision::Callbacks.OnAfterSceneLoaded += this;
	Vision::Callbacks.OnWorldDeInit += this;
	Vision::Callbacks.OnRenderHook += this;

	//load the remote input plugin
#if defined(WIN32) && !defined(_VISION_WINRT)
	Vision::Callbacks.OnUpdateSceneFinished += this;
	m_pRemoteInput = NULL;

	// TODO: Add support for remote input
	//VISION_PLUGIN_ENSURE_LOADED(vRemoteInput);
	//m_pRemoteInput = (IVRemoteInput*)Vision::Plugins.GetRegisteredPlugin("vRemoteInput");
#endif
}

void SpriteGameManager::OneTimeDeInit()
{
	Vision::Callbacks.OnEditorModeChanged -= this;
	Vision::Callbacks.OnAfterSceneLoaded -= this;
	Vision::Callbacks.OnWorldDeInit -= this;
	Vision::Callbacks.OnRenderHook -= this;

#if defined(WIN32) && !defined(_VISION_WINRT)
	Vision::Callbacks.OnUpdateSceneFinished -= this;
#endif

	if (m_spHUD)
	{
		m_spHUD->SetActivate(false);
		m_spHUD = NULL;
	}
}

void SpriteGameManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
	if (pData->m_pSender == &Vision::Callbacks.OnEditorModeChanged)
	{
		VisEditorModeChangedDataObject_cl *pEditorModeChangedData = (VisEditorModeChangedDataObject_cl *)pData;

		// When vForge switches back from EDITORMODE_PLAYING_IN_GAME, turn off our play the game mode
		if (pEditorModeChangedData->m_eNewMode != VisEditorManager_cl::EDITORMODE_PLAYING_IN_GAME)
		{
			SetPlayTheGame(false);
		}
	}
	else if (pData->m_pSender == &Vision::Callbacks.OnAfterSceneLoaded)
	{
		// Init play-the-game only in this vForge mode (or outside vForge)
		if (Vision::Editor.IsPlayingTheGame())
		{
			SetPlayTheGame(true);
		}
	}
	else if (pData->m_pSender == &Vision::Callbacks.OnWorldDeInit)
	{
		// This is important when running outside vForge
		SetPlayTheGame(false);
	}
	else if (pData->m_pSender == &Vision::Callbacks.OnRenderHook)
	{
		VisRenderHookDataObject_cl *pRenderHookData = (VisRenderHookDataObject_cl *)pData;
		if ( pRenderHookData->m_iEntryConst == VRH_POST_OCCLUSION_TESTS)
		{
			Render();
		}
	}
}

void SpriteGameManager::Render()
{
	IVRender2DInterface *pRender = Vision::RenderLoopHelper.BeginOverlayRendering();
	pRender->SetDepth(512.f);
	pRender->SetScissorRect(NULL);

	VSimpleRenderState_t state;
	state.SetTransparency(VIS_TRANSP_ALPHA);

	hkvVec2	tl(10, 10);
	hkvVec2	br(60, 60);
	pRender->DrawSolidQuad(tl, br, VColorRef(0, 255, 0, 128), state);

	Vision::RenderLoopHelper.EndOverlayRendering();
}

// switch to play-the-game mode. Do some initialization such as HUD display
void SpriteGameManager::SetPlayTheGame(bool bStatus)
{
	if (m_bPlayingTheGame != bStatus)
	{
		m_bPlayingTheGame = bStatus;
		if (m_bPlayingTheGame)
		{
			m_pPlayer = static_cast<Sprite *>( Vision::Game.SearchEntity("Player") );

			m_spHUD = new HUDGUIContext(NULL);
			m_spHUD->SetActivate(true);

	#if defined(WIN32) && !defined(_VISION_WINRT)
			if(m_pRemoteInput)
			{
				m_pRemoteInput->InitEmulatedDevices();				
				m_pRemoteInput->StartServer("SpriteGame\\RemoteGui");
			}
	#endif
		}
		else
		{
			// This is automatically deleted on scene unload
			m_pPlayer = NULL;

			// Deactivate the HUD
			if (m_spHUD)
			{
				m_spHUD->SetActivate(false);
				m_spHUD = NULL;
			}

	#if defined(WIN32) && !defined(_VISION_WINRT)
			if(m_pRemoteInput)
			{
				m_pRemoteInput->StopServer();
			}
	#endif
		}
	}
}