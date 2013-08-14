#include "SpriteGamePCH.h"

#include "SpriteGameApplication.hpp"
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>

#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>

#ifdef WIN32
#include <Vision/Runtime/EnginePlugins/RemoteInputEnginePlugin/IVRemoteInput.hpp>
#endif

// One global static instance
SpriteGameManager SpriteGameManager::g_GameManager;

void SpriteGameManager::OneTimeInit()
{
	m_bPlayingTheGame = false;

	// the game manager should listen to the following callbacks:
	Vision::Callbacks.OnEditorModeChanged += this;
	Vision::Callbacks.OnBeforeSceneLoaded += this;
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

	m_pPlayerSprite = reinterpret_cast<Sprite*>( Vision::Game.CreateEntity("Sprite", hkvVec3(0, 0, 170), NULL) );
}

void SpriteGameManager::OneTimeDeInit()
{
	Vision::Callbacks.OnEditorModeChanged -= this;
	Vision::Callbacks.OnBeforeSceneLoaded -= this;
	Vision::Callbacks.OnAfterSceneLoaded -= this;
	Vision::Callbacks.OnWorldDeInit -= this;
	Vision::Callbacks.OnRenderHook -= this;

#if defined(WIN32) && !defined(_VISION_WINRT)
	Vision::Callbacks.OnUpdateSceneFinished -= this;
#endif

	m_pPlayerSprite->Remove();
}

void SpriteGameManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
	if (pData->m_pSender==&Vision::Callbacks.OnEditorModeChanged)
	{
		VisEditorModeChangedDataObject_cl *pEditorModeChangedData = (VisEditorModeChangedDataObject_cl *)pData;

		// When vForge switches back from EDITORMODE_PLAYING_IN_GAME, turn off our play the game mode
		if (pEditorModeChangedData->m_eNewMode != VisEditorManager_cl::EDITORMODE_PLAYING_IN_GAME)
		{
			SetPlayTheGame(false);
		}
	}
	else if (pData->m_pSender==&Vision::Callbacks.OnBeforeSceneLoaded)
	{
		// We need accurate assignment because of nested Viszones
		Vision::World.SetUseGeometryForVisibilityClassification(true);
		return;
	}
	else if (pData->m_pSender==&Vision::Callbacks.OnAfterSceneLoaded)
	{
		// Init play-the-game only in this vForge mode (or outside vForge)
		if (Vision::Editor.IsPlayingTheGame())
		{
			SetPlayTheGame(true);
		}
	}
	else if (pData->m_pSender==&Vision::Callbacks.OnWorldDeInit)
	{
		// This is important when running outside vForge
		SetPlayTheGame(false);
	}
	else if(pData->m_pSender == &Vision::Callbacks.OnUpdateSceneFinished)
	{
	}
	else if (pData->m_pSender==&Vision::Callbacks.OnRenderHook)
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
	VSimpleRenderState_t	state;
	hkvVec2					tl(450,80);hkvVec2	br(850,850);
	hkvVec2					tl2(50,50);hkvVec2	br2(650,350);

	IVRender2DInterface *pRender = Vision::RenderLoopHelper.BeginOverlayRendering();
	pRender->SetDepth(512.f);
	pRender->SetScissorRect(NULL);
	state.SetTransparency(VIS_TRANSP_ALPHA);

	pRender->DrawSolidQuad(tl,br,VColorRef(255,0,255,100),state);
	pRender->DrawSolidQuad(tl2,br2,VColorRef(0,255,0,128),state);

	// TODO:  Render the sprites here

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
			m_spHUD = new HUDGUIContext(NULL);
			m_spHUD->SetActivate(true);

	#if defined(WIN32) && !defined(_VISION_WINRT)
			if(m_pRemoteInput)
			{
				m_pRemoteInput->InitEmulatedDevices();

				///uncomment to disable touch input smoothing
				//m_pRemoteInput->SetSmoothTouchInput(false);

				//m_pRemoteInput->StartServer("SpriteGame\\RemoteGui");
				m_pRemoteInput->AddVariable("health",100.0f);
				m_pRemoteInput->AddVariable("remaining",6);
			}

			//if(m_pPlayerEntity)
			//  m_pPlayerEntity->InitTouchInput();
	#endif
		} else
		{
			// deactivate the HUD
			if (m_spHUD)
			{
				m_spHUD->SetActivate(false);

				// Un-attach this context
				m_spHUD->GetManager()->Contexts().SafeRemove(m_spHUD);

				m_spHUD = NULL;
			}
	#if defined(WIN32) && !defined(_VISION_WINRT)
			//if(m_pPlayerEntity)
			//  m_pPlayerEntity->DeinitTouchInput();

			if(m_pRemoteInput)
			{
				m_pRemoteInput->StopServer();
				m_pRemoteInput->RemoveVariable("health");
				m_pRemoteInput->RemoveVariable("remaining");
			}
	#endif

			// clean up all pending particle effect instances
			VisParticleGroupManager_cl::GlobalManager().Instances().Purge();
		}
	}
}