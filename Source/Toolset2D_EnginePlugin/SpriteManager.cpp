#include "Toolset2D_EnginePluginPCH.h"

#include "SpriteManager.hpp"
#include "SpriteEntity.hpp"

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>
#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>

// One global static instance
SpriteManager SpriteManager::g_SpriteManager;

void SpriteManager::OneTimeInit()
{
	Vision::Callbacks.OnRenderHook += this;
}

void SpriteManager::OneTimeDeInit()
{
	Vision::Callbacks.OnRenderHook -= this;

	VASSERT(m_sprites.GetSize() == 0);
}

void SpriteManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
	if (pData->m_pSender==&Vision::Callbacks.OnRenderHook)
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
	IVRender2DInterface *pRender = Vision::RenderLoopHelper.BeginOverlayRendering();	
	pRender->SetDepth(512.f);
	pRender->SetScissorRect(NULL);

	VSimpleRenderState_t state;
	state.SetTransparency(VIS_TRANSP_ALPHA);
	
	for (int spriteIndex = 0; spriteIndex < m_sprites.GetSize(); spriteIndex++)
	{
		m_sprites[spriteIndex]->Render(pRender, state);
	}

	Vision::RenderLoopHelper.EndOverlayRendering();
}

void SpriteManager::AddSprite(Sprite *sprite)
{
	const int spriteIndex = m_sprites.Find(sprite, 0);
	if (spriteIndex == -1)
	{
		m_sprites.Append(sprite);
	}
}

void SpriteManager::RemoveSprite(Sprite *sprite)
{
	const int spriteIndex = m_sprites.Find(sprite, 0);
	if (spriteIndex != -1)
	{
		m_sprites.RemoveAt(spriteIndex);
	}
}

