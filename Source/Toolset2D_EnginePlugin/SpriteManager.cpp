//=======
//
// Author: Joel Van Eenwyk
// Purpose: Maintains a list of sprites in the scene and calls their
//          render function appropriately
//
//=======

#include "Toolset2D_EnginePluginPCH.h"

#include "SpriteManager.hpp"
#include "SpriteEntity.hpp"

#if defined(WIN32)
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetManager.hpp>
#endif

#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VLuaHelpers.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokSync.hpp>

#include "Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/Toolset2D_Module_wrapper.hpp"

// global function referenced
extern int luaopen_Toolset2D(lua_State *);

// compare two sprites for depth sorting
static int compareSprites(const void *sprite1, const void *sprite2);

#if defined(WIN32)
TOOLSET_2D_IMPEXP bool convertToAssetPath(const char* absolutePath, hkStringBuf& out_relativePath);
#endif

void SpriteManager::OneTimeInit()
{
	FORCE_LINKDYNCLASS(Sprite);

	VISION_PLUGIN_ENSURE_LOADED(vHavok);
	VISION_HAVOK_SYNC_ALL_STATICS();

	Vision::Callbacks.OnRenderHook += this;
	Vision::Callbacks.OnUpdateSceneFinished += this;
	Vision::Callbacks.OnEditorModeChanged += this;
	Vision::Callbacks.OnAfterSceneUnloaded += this;
	Vision::Callbacks.OnWorldDeInit += this;
	IVScriptManager::OnRegisterScriptFunctions += this;
	IVScriptManager::OnScriptProxyCreation += this;
}

void SpriteManager::OneTimeDeInit()
{
	VISION_HAVOK_UNSYNC_ALL_STATICS();

	Vision::Callbacks.OnRenderHook -= this;
	Vision::Callbacks.OnUpdateSceneFinished -= this;
	Vision::Callbacks.OnAfterSceneUnloaded -= this;
	Vision::Callbacks.OnEditorModeChanged -= this;
	Vision::Callbacks.OnWorldDeInit -= this;
	IVScriptManager::OnRegisterScriptFunctions -= this;
	IVScriptManager::OnScriptProxyCreation -= this;

	if (m_spHUD)
	{
		m_spHUD->SetActivate(false);
		m_spHUD = NULL;
	}
}

void SpriteManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
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
	if (pData->m_pSender == &Vision::Callbacks.OnAfterSceneUnloaded)
	{
		VASSERT(m_sprites.GetSize() == 0);

		for (int spriteDataIndex = 0; spriteDataIndex < m_spriteData.GetSize(); spriteDataIndex++)
		{
			SpriteData *spriteData = m_spriteData[spriteDataIndex];

			spriteData->cells.RemoveAll();
			spriteData->states.RemoveAll();
			spriteData->stateNameToIndex.Reset();

			V_SAFE_RELEASE(spriteData->spTextureAnimation);
			V_SAFE_RELEASE(spriteData->spriteSheetTexture);

			delete spriteData;
			m_spriteData[spriteDataIndex] = NULL;
		}

		m_spriteData.RemoveAll();
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
	
	VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_ALWAYSVISIBLE | RENDERSTATEFLAG_FILTERING | RENDERSTATEFLAG_DOUBLESIDED);

	// Sort all the sprites by their Z order
	qsort(m_sprites.GetData(), m_sprites.GetSize(), sizeof(Sprite*), compareSprites);

	// Now render all the things
	for (int spriteIndex = 0; spriteIndex < m_sprites.GetSize(); spriteIndex++)
	{
		m_sprites[spriteIndex]->Render(pRender, state);
	}

	// Render a little quad to represent that 2D is being used
	hkvVec2	tl(10, 10);
	hkvVec2	br(60, 60);
	pRender->DrawSolidQuad(tl, br, VColorRef(0, 255, 0, 128), state);

	Vision::RenderLoopHelper.EndOverlayRendering();
}

void SpriteManager::Update()
{
	// Update all the sprites first so we're sure their vertices are up to date
	for (int spriteIndex = 0; spriteIndex < m_sprites.GetSize(); spriteIndex++)
	{
		Sprite *sprite = m_sprites[spriteIndex];
		sprite->Update();
	}

	// Check to see if there are any overlaps and report it
	for (int spriteIndex = 0; spriteIndex < m_sprites.GetSize(); spriteIndex++)
	{
		Sprite *sprite = m_sprites[spriteIndex];

		// Only worry about sprites that have collision enabled
		if (sprite->IsColliding())
		{
			// Check this sprite against 
			for (int otherSpriteIndex = spriteIndex + 1; otherSpriteIndex < m_sprites.GetSize(); otherSpriteIndex++)
			{
				Sprite *otherSprite = m_sprites[otherSpriteIndex];
				if (otherSprite->IsColliding() &&
					(sprite->IsOverlapping(otherSprite) || otherSprite->IsOverlapping(sprite)))
				{
					sprite->OnCollision(otherSprite);
					otherSprite->OnCollision(sprite);
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

int SpriteManager::GetNumSprites()
{
	return m_sprites.GetLength();
}

const SpriteData *SpriteManager::GetSpriteData(const VString &spriteSheetFilename, const VString &xmlDataFilename)
{
	for (int spriteDataIndex = 0; spriteDataIndex < m_spriteData.GetSize(); spriteDataIndex++)
	{
		const SpriteData *data = m_spriteData[spriteDataIndex];
		if (data->spriteSheetFilename == spriteSheetFilename &&
			data->xmlDataFilename == xmlDataFilename)
		{
			return data;
		}
	}

	SpriteData *spriteData = NULL;

	VTextureObject *spSpriteSheetTexture = Vision::TextureManager.Load2DTexture(spriteSheetFilename);
	if (spSpriteSheetTexture)
	{
		spriteData = new SpriteData();
		m_spriteData.Append(spriteData);

		spriteData->sourceWidth = 0.f;
		spriteData->sourceHeight = 0.f;

		spriteData->spriteSheetFilename = spriteSheetFilename;
		spriteData->xmlDataFilename = xmlDataFilename;

		// Go ahead and add a reference to this texture
		spriteData->spriteSheetTexture = spSpriteSheetTexture;
		spriteData->spriteSheetTexture->AddRef();

		// TODO: Perhaps it's a bit weird to have a spritesheet that also has animations on it--all of the cells
		//       would have to be in the same places for each sheet, which is odd. Probably assert when there is
		//       XML document AND a texture animation.
		spriteData->spTextureAnimation = Vision::TextureManager.GetAnimationInstance(spriteData->spriteSheetTexture);
		if (spriteData->spTextureAnimation)
		{
			spriteData->spTextureAnimation->AddRef();
		}

		TiXmlDocument xmlDocument;

		// If we successfully load the XML, then there is data we can parse about the sprites in the sheet
		if ( xmlDocument.LoadFile(xmlDataFilename, Vision::File.GetManager()) )
		{
			const char *szActionNode = "SubTexture";
			TiXmlElement *rootElement = xmlDocument.RootElement();

			rootElement->QueryFloatAttribute("width", &spriteData->sourceWidth);
			rootElement->QueryFloatAttribute("height", &spriteData->sourceHeight);

			for (TiXmlElement *pNode = rootElement->FirstChildElement(szActionNode);
				pNode != NULL;
				pNode = pNode->NextSiblingElement(szActionNode) )
			{
				const char *name = pNode->Attribute("name");

				int x;
				int y;
				int width;
				int height;
				int originalWidth;
				int originalHeight;
				int ox;
				int oy;

				pNode->QueryIntAttribute("x", &x);
				pNode->QueryIntAttribute("y", &y);
				pNode->QueryIntAttribute("width", &width);
				pNode->QueryIntAttribute("height", &height);
				pNode->QueryIntAttribute("original_width", &originalWidth);
				pNode->QueryIntAttribute("original_height", &originalHeight);
				pNode->QueryIntAttribute("ox", &ox);
				pNode->QueryIntAttribute("oy", &oy);

				const int newCellIndex = spriteData->cells.Append(SpriteCell());
				SpriteCell *currentCell = &spriteData->cells[newCellIndex];
				currentCell->name = name;
				currentCell->offset.x = static_cast<float>(x);
				currentCell->offset.y = static_cast<float>(y);
				currentCell->pivot.x = static_cast<float>(ox);
				currentCell->pivot.y = static_cast<float>(oy);
				currentCell->width = static_cast<float>(width);
				currentCell->height = static_cast<float>(height);
				currentCell->originalWidth = static_cast<float>(originalWidth);
				currentCell->originalHeight = static_cast<float>(originalHeight);

				const char *result = strrchr(name, '_');
				int index = -1;
				if (result != NULL)
				{
					index = result - name;
				}

				SpriteState *state = NULL;
				int stateIndex = -1;
				if (index == -1)
				{
					const char *extension = strrchr(name, '.');
					int extensionIndex = -1;
					if (extension != NULL)
						extensionIndex = extension - name;
					VString stateName = VString(name, extensionIndex);

					stateIndex = spriteData->states.Append(SpriteState());
					state = &spriteData->states[stateIndex];
					state->name = stateName;
					state->framerate = 30.0f;
					spriteData->stateNameToIndex.Set(state->name, stateIndex);
				}
				else
				{
					VString s = name;
					VString stateName = VString(name, index);
					VString last = VString(&name[index + 1], strlen(name) - index);
					VString number = VString(last.GetChar(), last.Find("."));
					currentCell->index = atoi(number);

					for (int i = 0; i < spriteData->states.GetSize(); i++)
					{
						if (spriteData->states[i].name == stateName)
						{
							stateIndex = i;
							state = &spriteData->states[i];
							break;
						}
					}

					if (stateIndex == -1)
					{
						stateIndex = spriteData->states.Append(SpriteState());
						state = &spriteData->states[stateIndex];
						state->name = stateName;
						state->framerate = 10.f;
						spriteData->stateNameToIndex.Set(state->name, stateIndex);
					}
				}

				state->cells.Append(newCellIndex);
			}
		}
		// No XML describing the sprite sheet, but we do have a sprite texture, so create a default
		// state and cell for it
		else if (spriteData->spriteSheetTexture != NULL)
		{
			int stateIndex = spriteData->states.Append(SpriteState());
			SpriteState *state = &spriteData->states[stateIndex];
			state->name = spriteData->spriteSheetFilename;
			state->framerate = 30.0f;
			state->cells.Add(0);
			spriteData->stateNameToIndex.Set(state->name, stateIndex);

			const int newCellIndex = spriteData->cells.Append(SpriteCell());
			SpriteCell *currentCell = &spriteData->cells[newCellIndex];

			char buffer[FS_MAX_PATH];
			VFileHelper::GetFilenameNoExt(buffer, spriteData->spriteSheetFilename);
			currentCell->name = buffer;
			currentCell->offset.x = 0.f;
			currentCell->offset.y = 0.f;
			currentCell->pivot.x = 0.f;
			currentCell->pivot.y = 0.f;

			float textureWidth = static_cast<float>(spriteData->spriteSheetTexture->GetTextureWidth());
			float textureHeight = static_cast<float>(spriteData->spriteSheetTexture->GetTextureHeight());

			spriteData->sourceWidth = currentCell->width = currentCell->originalWidth = textureWidth;
			spriteData->sourceHeight = currentCell->height = currentCell->originalHeight = textureHeight;
		}
	}

	return spriteData;
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

			VSWIG_PUSH_PROXY(pLuaState, SpriteManager, &SpriteManager::GlobalManager());
			lua_setglobal(pLuaState, "SpriteManager");
		}
		else
		{
			Vision::Error.Warning("Unable to create Lua Sprite Module, lua_State is NULL!");
		}
	}
}

// switch to play-the-game mode. Do some initialization such as HUD display
void SpriteManager::SetPlayTheGame(bool bStatus)
{
	if (m_bPlayingTheGame != bStatus)
	{
		m_bPlayingTheGame = bStatus;
		if (m_bPlayingTheGame)
		{
			m_spHUD = new HUDGUIContext(NULL);
			m_spHUD->SetActivate(true);
		}
		else
		{
			// Deactivate the HUD
			if (m_spHUD)
			{
				m_spHUD->SetActivate(false);
				m_spHUD = NULL;
			}
		}
	}
}

//----- functions

static int compareSprites(const void *arg1, const void *arg2)
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
