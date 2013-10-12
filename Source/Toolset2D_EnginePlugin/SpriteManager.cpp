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

#if defined(WIN32)
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetManager.hpp>
#endif

#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokSync.hpp>

extern "C" int luaopen_Toolset2D(lua_State *);

#if defined(WIN32)
TOOLSET_2D_IMPEXP bool convertToAssetPath(const char* absolutePath, hkStringBuf& out_relativePath)
{
	bool valid = !hkvStringHelper::isEmpty(absolutePath);

	if (valid)
	{
		if (VPathHelper::IsAbsolutePath(absolutePath))
		{
			hkvAssetManager* assetManager = hkvAssetManager::getGlobalInstance();
			hkvCriticalSectionLock lock(assetManager->acquireLock());

			hkvAssetLibrary::RefPtr out_library;
			valid = assetManager->makePathRelative(absolutePath, out_relativePath, out_library);
		}
		else
		{
			out_relativePath = absolutePath;
		}
	}

	return valid;
}
#endif // WIN32

// compare function for qsort
static int CompareSprites(const void *arg1, const void *arg2)
{
	Sprite *pSort1 = *((Sprite **)arg1);
	Sprite *pSort2 = *((Sprite **)arg2);

	float a = pSort1->GetPosition().z;
	float b = pSort2->GetPosition().z;

	int result = 0;

	if (hkvMath::isFloatEqual(a, b))
	{
		result = static_cast<int>(pSort2->GetUniqueID() - pSort1->GetUniqueID());
	}
	else
	{
		result = (b > a) ? -1 : 1;
	}

	return result;
}

void SpriteManager::OneTimeInit()
{
	FORCE_LINKDYNCLASS(Sprite);

	VISION_PLUGIN_ENSURE_LOADED(vHavok);
	VISION_HAVOK_SYNC_ALL_STATICS();

	Vision::Callbacks.OnRenderHook += this;
	Vision::Callbacks.OnUpdateSceneFinished += this;
	IVScriptManager::OnRegisterScriptFunctions += this;
	IVScriptManager::OnScriptProxyCreation += this;
}

void SpriteManager::OneTimeDeInit()
{
	VISION_HAVOK_UNSYNC_ALL_STATICS();

	Vision::Callbacks.OnRenderHook -= this;
	Vision::Callbacks.OnUpdateSceneFinished -= this;
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
	if (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneFinished)
	{
		Update();
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

	// Sort all the sprites by their Z order
	qsort(m_sprites.GetData(), m_sprites.GetSize(), sizeof(Sprite*), CompareSprites);

	// Now render all the things
	for (int spriteIndex = 0; spriteIndex < m_sprites.GetSize(); spriteIndex++)
	{
		if ((m_sprites[spriteIndex]->GetVisibleBitmask() & VIS_ENTITY_VISIBLE) != FALSE)
		{
			m_sprites[spriteIndex]->Render(pRender, state);
		}
	}

	Vision::RenderLoopHelper.EndOverlayRendering();
}

void SpriteManager::Update()
{
	// Check to see if there are any overlaps and report it
	for (int spriteIndex = 0; spriteIndex < m_sprites.GetSize(); spriteIndex++)
	{
		Sprite *sprite = m_sprites[spriteIndex];

		if ((sprite->GetVisibleBitmask() & VIS_ENTITY_VISIBLE) != FALSE)
		{
			sprite->Update();

			if (!sprite->IsFullscreenMode())
			{
				for (int otherSpriteIndex = spriteIndex + 1; otherSpriteIndex < m_sprites.GetSize(); otherSpriteIndex++)
				{
					Sprite *otherSprite = m_sprites[otherSpriteIndex];
					if (!otherSprite->IsFullscreenMode() && sprite->IsOverlapping(otherSprite))
					{
						sprite->OnCollision(otherSprite);
						otherSprite->OnCollision(sprite);
					}
				}
			}
		}
	}
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