#include "SpriteShapeEnginePluginPCH.h"

#include "SpriteManager.hpp"

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>
#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>

// One global static instance
SpriteManager SpriteManager::g_SpriteManager;

void SpriteManager::OneTimeInit()
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
#endif
}

void SpriteManager::OneTimeDeInit()
{
	Vision::Callbacks.OnEditorModeChanged -= this;
	Vision::Callbacks.OnBeforeSceneLoaded -= this;
	Vision::Callbacks.OnAfterSceneLoaded -= this;
	Vision::Callbacks.OnWorldDeInit -= this;
	Vision::Callbacks.OnRenderHook -= this;

#if defined(WIN32) && !defined(_VISION_WINRT)
	Vision::Callbacks.OnUpdateSceneFinished -= this;
#endif
}

void SpriteManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
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

void SpriteManager::Render()
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
void SpriteManager::SetPlayTheGame(bool bStatus)
{
	if (m_bPlayingTheGame != bStatus)
	{
		m_bPlayingTheGame = bStatus;
	}
}