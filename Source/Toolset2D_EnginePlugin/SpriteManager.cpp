//=======
//
// Author: Joel Van Eenwyk
// Purpose: Maintains a list of sprites in the scene and calls their
//          render function appropriately
// TODO:
//   * Make accessible through LUA
//
//=======
//         _____           _____
//     ,ad8PPPP88b,     ,d88PPPP8ba,
//    d8P"      "Y8b, ,d8P"      "Y8b
//   dP'           "8a8"           `Yd
//   8(              "              )8
//   I8                             8I
//    Yb,                         ,dP
//     "8a,                     ,a8"
//       "8a,                 ,a8"
//         "Yba             adP"
//           `Y8a         a8P'
//             `88,     ,88'
//               "8b   d8"  
//                "8b d8"   
//                 `888'
//                   "
//

#include "Toolset2D_EnginePluginPCH.h"

#include "SpriteManager.hpp"
#include "SpriteEntity.hpp"

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>
#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>

extern "C" int luaopen_Toolset2D(lua_State *);

void SpriteManager::OneTimeInit()
{
	FORCE_LINKDYNCLASS(Sprite);

	Vision::Callbacks.OnRenderHook += this;
	IVScriptManager::OnRegisterScriptFunctions += this;
	IVScriptManager::OnScriptProxyCreation += this;
}

void SpriteManager::OneTimeDeInit()
{
	Vision::Callbacks.OnRenderHook -= this;
	IVScriptManager::OnRegisterScriptFunctions -= this;
	IVScriptManager::OnScriptProxyCreation -= this;

	VASSERT(m_sprites.GetSize() == 0);
}

void SpriteManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
	if (pData->m_pSender == &Vision::Callbacks.OnRenderHook)
	{
		VisRenderHookDataObject_cl *pRenderHookData = (VisRenderHookDataObject_cl *)pData;
		if ( pRenderHookData->m_iEntryConst == VRH_POST_OCCLUSION_TESTS)
		{
			Render();
		}
	}
	else if (pData->m_pSender == &IVScriptManager::OnRegisterScriptFunctions)
	{
		RegisterLua();
	}
	else if (pData->m_pSender == &IVScriptManager::OnScriptProxyCreation)
	{
		VScriptCreateStackProxyObject * pScriptData = (VScriptCreateStackProxyObject *)pData;

		// process data only if no other callback did that
		if (!pScriptData->m_bProcessed)
		{
			int iRetParams = 0;

			if (pScriptData->m_pInstance->IsOfType(V_RUNTIME_CLASS(Sprite)))
			{
				iRetParams = LUA_CallStaticFunction(
					pScriptData->m_pLuaState, // our lua state
					"Toolset2D", // the name of the module
					"Sprite", // the name of the class
					"Cast", // the name of the function
					"E>E", // the function's signature
					pScriptData->m_pInstance //the input parameters (out instance to cast)
					);
			}

			// Could we handle the object?
			if (iRetParams > 0)
			{
				// the cast failed if the result is a nil value
				if ( lua_isnil(pScriptData->m_pLuaState, -1) )
				{
					// in case of a nil value we drop the params from the lua stack
					lua_pop(pScriptData->m_pLuaState, iRetParams);
				}
				else
				{
					// avoid further processing
					pScriptData->m_bProcessed = true;
				}
			}
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

void SpriteManager::RegisterLua()
{
	IVScriptManager* pSM = Vision::GetScriptManager();
	if (pSM != NULL)
	{
		lua_State* pLuaState = static_cast<VScriptResourceManager*>(pSM)->GetMasterState();
		if (pLuaState)
		{
			luaopen_Toolset2D(pLuaState);
		}
		else
		{
			Vision::Error.Warning("Unable to create Lua Sprite Module, lua_State is NULL!");
		}
	}
}