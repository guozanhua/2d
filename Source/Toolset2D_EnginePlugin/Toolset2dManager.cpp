//=======
//
// Author: Joel Van Eenwyk
// Purpose: Maintains a list of sprites in the scene and calls their
//          render function appropriately
//
//=======

#include "Toolset2D_EnginePluginPCH.h"

#include "Toolset2dManager.hpp"
#include "SpriteEntity.hpp"
#include "Camera2dEntity.hpp"

#if defined(WIN32)
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetManager.hpp>
#endif

#include <Vision/Runtime/Base/Graphics/VColor.hpp>
#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VLuaHelpers.hpp>

#if USE_HAVOK_PHYSICS_2D
#include <Common/Base/hkBase.h>
#include <Common/Base/Config/hkProductFeatures.h>
#include <Common/Serialize/Version/hkVersionPatchManager.h>
#include <Common/Compat/hkHavokVersions.h>
#include <Common/Base/Config/hkOptionalComponent.h>

#include <Physics2012/Internal/BroadPhase/TreeBroadPhase/hkpTreeBroadPhase.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokVisualDebugger.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokSync.hpp>
#include <Common/Internal/GeometryProcessing/ConvexHull/hkgpConvexHull.h>
#endif // USE_HAVOK_PHYSICS_2D

// global function referenced
extern "C" int luaopen_Toolset2dModule(lua_State *);

// compare two sprites for depth sorting
static int compareSprites(const void *sprite1, const void *sprite2);

#if defined(WIN32)
TOOLSET_2D_IMPEXP bool convertToAssetPath(const char* absolutePath, hkStringBuf& out_relativePath);
#endif

static const hkUint32 HK_VISUAL_DEBUGGER_WORLD_2D_PORT = 25003;

SpriteCell::SpriteCell()
{
	width = originalWidth = 0.f;
	height = originalHeight = 0.f;
	index = 0;

#if USE_HAVOK_PHYSICS_2D
	shape = NULL;
	shape3d = NULL;
#endif // USE_HAVOK_PHYSICS_2D
}

SpriteData::SpriteData()
{
	spriteSheetTexture = NULL;
	textureAnimation = NULL;
}

SpriteData::~SpriteData()
{
	Cleanup();
}

void SpriteData::Cleanup()
{
	for (int cellIndex = 0; cellIndex < cells.GetSize(); cellIndex++)
	{
		SpriteCell *cell = &cells[cellIndex];

#if USE_HAVOK_PHYSICS_2D
		if (cell->shape != NULL)
		{
			VASSERT(cell->shape->getReferenceCount() == 1);
			cell->shape->removeReference();
			cell->shape = NULL;
		}

		if (cell->shape3d != NULL)
		{
			VASSERT(cell->shape3d->getReferenceCount() == 1);
			cell->shape3d->removeReference();
			cell->shape3d = NULL;
		}
#endif // USE_HAVOK_PHYSICS_2D
	}

	cells.RemoveAll();
	states.RemoveAll();
	stateNameToIndex.Reset();

	V_SAFE_RELEASE(textureAnimation);
	V_SAFE_RELEASE(spriteSheetTexture);
}

bool SpriteData::GenerateConvexHull()
{
	if ( spriteSheetTexture == NULL || !spriteSheetTexture->HasDeviceHandle() )
	{
		return false;
	}

	bool success = false;

#if defined(WIN32) && USE_HAVOK_PHYSICS_2D
	const int bytesPerPixel = spriteSheetTexture->GetBitsPerPixel(spriteSheetTexture->GetTextureFormat()) / 8;
	IDirect3DTexture9 *pTexture2D = (IDirect3DTexture9 *)spriteSheetTexture->GetD3DInterface();
	const int iMipLevel = 0;
	const int iLockFlags = D3DLOCK_READONLY;

	D3DSURFACE_DESC surfaceDesc;
	HRESULT result = pTexture2D->GetLevelDesc(iMipLevel, &surfaceDesc);

	if (result != S_OK)
	{
		return false;
	}

	D3DLOCKED_RECT destRect;
	RECT lockRect = { 0, 0, spriteSheetTexture->GetTextureWidth(), spriteSheetTexture->GetTextureHeight() };
	result = pTexture2D->LockRect(iMipLevel, &destRect, NULL, spriteSheetTexture->GetD3D9LockFlags(iLockFlags));

	if (result == S_OK)
	{
		const unsigned char thresholdMin = 10;

		unsigned char *pDest = (unsigned char *)destRect.pBits;

		for (int stateIndex = 0; stateIndex < cells.GetLength(); stateIndex++)
		{
			SpriteCell &cell = cells[stateIndex];
			const int width = static_cast<int>(cell.width);
			const int height = static_cast<int>(cell.height);

			int *mins = new int[height];
			int *maxs = new int[height];
			memset(mins, -1, sizeof(int) * height);
			memset(maxs, -1, sizeof(int) * height);

			const int startX = static_cast<int>(cell.offset.x);
			const int endX = startX + width;
			const int startY = static_cast<int>(cell.offset.y);
			const int endY = startY + height;

			for (int y = startY; y < endY; y++)
			{
				bool findMin = true;
				const int positionY = y - startY;

				for (int x = startX; x < endX; x++)
				{
					unsigned char *pixelRaw = &pDest[y * destRect.Pitch + x * bytesPerPixel];
					unsigned int alpha = 0;
					
					// if there's an alpha channel, just worry about that
					if (bytesPerPixel == 4)
					{
						const unsigned int *pixel = reinterpret_cast<unsigned int*>(pixelRaw);
						alpha = ((*pixel) & 0xff000000) >> 24;
					}
					else
					{
						for (int offset = 0; offset < bytesPerPixel; offset++)
						{
							alpha += pixelRaw[offset];
						}
					}

					const int positionX = x - startX;
					const bool withinThreshold = (alpha >= thresholdMin);
					if (findMin && withinThreshold)
					{
						mins[positionY] = positionX;
						findMin = false;
					}
					else if (!findMin && !withinThreshold)
					{
						maxs[positionY] = positionX;
						break;
					}
				}

				if (maxs[positionY] == -1)
				{
					maxs[positionY] = width;
				}
			}

			const float hwidth = static_cast<float>(width) / 2.0f;
			const float hheight = static_cast<float>(height) / 2.0f;
			hkArray<hkVector4> vertices;
			for (int y = 0; y < height; y++)
			{
				if (mins[y] != -1)
				{
					hkReal xx1 = static_cast<hkReal>(mins[y]) - hwidth;
					hkReal xx2 = static_cast<hkReal>(maxs[y]) - hwidth;
					hkReal hh = static_cast<hkReal>(y) - hheight;

					vertices.pushBack( hkVector4(xx1, hh, 0, 0) );
					vertices.pushBack( hkVector4(xx2, hh, 0, 0) );

					if (y == height - 1)
					{
						hkReal hh2 = static_cast<hkReal>(y + 1) - hheight;
						vertices.pushBack( hkVector4(xx1, hh2, 0, 0) );
						vertices.pushBack( hkVector4(xx2, hh2, 0, 0) );
					}
				}
			}

			delete [] mins;
			delete [] maxs;

			const bool sortInputs = true;
			const bool simplify = false;

			hkgpConvexHull::BuildConfig	config;
			config.m_allowLowerDimensions = true;
			config.m_buildIndices = true;
			config.m_sortInputs = sortInputs;

			hkVector4 projectionPlane(0, 0, 1);

			hkgpConvexHull convexHull;

			if ( convexHull.build(vertices, config) != -1 )
			{
				hkgpConvexHull *result = &convexHull;	

				result->generateIndexedFaces(hkgpConvexHull::INTERNAL_VERTICES, cell.verticesPerFace, cell.vertexIndices, true);					
				result->fetchPositions(hkgpConvexHull::INTERNAL_VERTICES, cell.vertexPositions);

				hkpConvexVerticesShape::BuildConfig config;
				config.m_convexRadius = 0.0f;
				config.m_shrinkByConvexRadius = false;
				config.m_useOptimizedShrinking = false;

				cell.shape = new hkpConvexVerticesShape(cell.vertexPositions, config);

				const hkReal depth = 100.0f;

				// Generate a 3d
				hkArray<hkVector4> vertexPositions3d;
				for (int vertexIndex = 0; vertexIndex < cell.vertexPositions.getSize(); vertexIndex++)
				{
					const hkVector4 position = cell.vertexPositions[vertexIndex];
					vertexPositions3d.pushBack( hkVector4(position(0), position(1), 0.0f, 0.0f) );
				}

				// Get the bounding box of the shape and use the max extent as the depth of the 3d shape
				hkAabb aabb;
				cell.shape->getAabb( hkTransform::getIdentity(), 0.f, aabb );
				const hkReal depthCenter = depth * 1.3f;
				hkVector4 center;
				aabb.getCenter(center);
				vertexPositions3d.pushBack( hkVector4(center(0), center(1), -depthCenter , 0.f) );
				vertexPositions3d.pushBack( hkVector4(center(0), center(1), depthCenter , 0.f) );

				config.m_convexRadius = 0.05f;
				config.m_shrinkByConvexRadius = false;
				cell.shape3d = new hkpConvexVerticesShape(vertexPositions3d, config);

				success = true;
			}
		}

		HRESULT unlockResult = pTexture2D->UnlockRect(iMipLevel);
		VASSERT(unlockResult == S_OK);
	}
#endif // WIN32

	return success;
}

void Toolset2dManager::OneTimeInit()
{
	m_camera = NULL;
	m_gameMode = MODE_STOPPED;

	FORCE_LINKDYNCLASS(Sprite);
	FORCE_LINKDYNCLASS(Camera2D);

	Vision::Callbacks.OnRenderHook += this;
	Vision::Callbacks.OnUpdateSceneFinished += this;
	Vision::Callbacks.OnEditorModeChanged += this;
	Vision::Callbacks.OnAfterSceneUnloaded += this;
	Vision::Callbacks.OnWorldDeInit += this;

	IVScriptManager::OnRegisterScriptFunctions += this;
	IVScriptManager::OnScriptProxyCreation += this;

#if USE_HAVOK_PHYSICS_2D
	m_world = NULL;
	m_physicsModule = NULL;
	m_pContext = NULL;

	FORCE_LINKDYNCLASS(vHavokRigidBody);

	VISION_PLUGIN_ENSURE_LOADED(vHavok);

	vHavokPhysicsModule::OnBeforeInitializePhysics += this;
	vHavokPhysicsModule::OnAfterDeInitializePhysics += this;
	vHavokPhysicsModule::OnAfterWorldCreated += this;
	vHavokPhysicsModule::OnBeforeDeInitializePhysics += this;

	vHavokVisualDebugger::OnCreatingContexts += this;
	vHavokVisualDebugger::OnAddingDefaultViewers += this;
#endif // USE_HAVOK_PHYSICS_2D
}

void Toolset2dManager::OneTimeDeInit()
{
	Vision::Callbacks.OnRenderHook -= this;
	Vision::Callbacks.OnUpdateSceneFinished -= this;
	Vision::Callbacks.OnAfterSceneUnloaded -= this;
	Vision::Callbacks.OnEditorModeChanged -= this;
	Vision::Callbacks.OnWorldDeInit -= this;

	IVScriptManager::OnRegisterScriptFunctions -= this;
	IVScriptManager::OnScriptProxyCreation -= this;

#if USE_HAVOK_PHYSICS_2D
	vHavokPhysicsModule::OnBeforeInitializePhysics -= this;
	vHavokPhysicsModule::OnAfterDeInitializePhysics -= this;
	vHavokPhysicsModule::OnAfterWorldCreated -= this;
	vHavokPhysicsModule::OnBeforeDeInitializePhysics -= this;
	vHavokVisualDebugger::OnCreatingContexts -= this;
	vHavokVisualDebugger::OnAddingDefaultViewers -= this;
#endif // USE_HAVOK_PHYSICS_2D
}

void Toolset2dManager::InitializeHavokPhysics()
{
#if USE_HAVOK_PHYSICS_2D
	{
		hkpWorldCinfo worldInfo;

		worldInfo.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_4ITERS_MEDIUM);
		worldInfo.m_gravity = hkVector4(0.0f, 9.81f, 0.0f);
		worldInfo.m_broadPhaseType = hkpWorldCinfo::BROADPHASE_TYPE_TREE;

		// just fix the entity if the object falls off too far
		worldInfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_DO_NOTHING;

		// You must specify the size of the broad phase - objects should not be simulated outside this region
		worldInfo.setBroadPhaseWorldSize(1000.0f);
		m_world = new hkpWorld(worldInfo);

		if (m_pContext)
		{
			m_pContext->addWorld(m_world);
		}
	}

	// Register all collision agents, even though only box - box will be used in this particular example.
	// It's important to register collision agents before adding any entities to the world.
	{
		hkpAgentRegisterUtil::registerAllAgents( m_world->getCollisionDispatcher() );
	}
#endif // USE_HAVOK_PHYSICS_2D
}

void Toolset2dManager::UnintializeHavokPhysics()
{
#if USE_HAVOK_PHYSICS_2D
	V_SAFE_DELETE(m_world);
	VISION_HAVOK_UNSYNC_ALL_STATICS();

	if (m_pContext != NULL)
	{
		m_pContext->removeReference();
		m_pContext = NULL;
	}
#endif
}

void Toolset2dManager::RemoveSpriteData()
{
	VASSERT(m_sprites.GetSize() == 0);

	for (int spriteDataIndex = 0; spriteDataIndex < m_spriteData.GetSize(); spriteDataIndex++)
	{
		SpriteData *spriteData = m_spriteData[spriteDataIndex];
		delete spriteData;
		m_spriteData[spriteDataIndex] = NULL;
	}

	m_spriteData.RemoveAll();
}

void Toolset2dManager::Step( float dt )
{
#if USE_HAVOK_PHYSICS_2D
	if (m_world)
	{
		m_world->stepDeltaTime(dt);
	}
#endif
}

bool Toolset2dManager::InSimulationMode() const
{
	return (m_gameMode == MODE_PLAY_THE_GAME || m_gameMode == MODE_RUN_IN_EDITOR);
}

bool Toolset2dManager::CreateLuaCast(VScriptCreateStackProxyObject *scriptData, const char *typeName, VType *type)
{
	int iRetParams = 0;

	if (scriptData->m_pInstance->IsOfType(type))
	{
		iRetParams = LUA_CallStaticFunction(
			scriptData->m_pLuaState, // our lua state
			"Toolset2dModule", // the name of the module
			typeName, // the name of the class
			"Cast", // the name of the function
			"E>E", // the function's signature
			scriptData->m_pInstance //the input parameters (out instance to cast)
			);
	}

	// Could we handle the object?
	if (iRetParams > 0)
	{
		// the cast failed if the result is a nil value
		if ( lua_isnil(scriptData->m_pLuaState, -1) )
		{
			// in case of a nil value we drop the params from the lua stack
			lua_pop(scriptData->m_pLuaState, iRetParams);
		}
		else
		{
			// avoid further processing
			scriptData->m_bProcessed = true;
		}
	}

	return true;
}

void Toolset2dManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
	//-- Scene load events

	if (pData->m_pSender == &Vision::Callbacks.OnAfterSceneLoaded)
	{
		// initialize play-the-game only in this vForge mode (or outside vForge)
		if ( Vision::Editor.IsPlayingTheGame() )
		{
			m_gameMode = MODE_PLAY_THE_GAME;
		}
	}
	else if (pData->m_pSender == &Vision::Callbacks.OnWorldDeInit)
	{
		m_gameMode = MODE_STOPPED;
		RemoveSpriteData();
	}
	else if (pData->m_pSender == &Vision::Callbacks.OnAfterSceneUnloaded)
	{
		RemoveSpriteData();
	}

	//-- Runtime events

	if (pData->m_pSender == &Vision::Callbacks.OnEditorModeChanged)
	{
		VisEditorModeChangedDataObject_cl *pEditorModeChangedData = (VisEditorModeChangedDataObject_cl *)pData;

		// when vForge switches back from EDITORMODE_PLAYING_IN_GAME, turn off our play the game mode
		if ( pEditorModeChangedData->m_eNewMode == VisEditorManager_cl::EDITORMODE_PLAYING_IN_GAME )
		{
			m_gameMode = MODE_PLAY_THE_GAME;
		}
		else if ( pEditorModeChangedData->m_eNewMode == VisEditorManager_cl::EDITORMODE_PLAYING_IN_EDITOR )
		{
			m_gameMode = MODE_RUN_IN_EDITOR;
		}
		else
		{
			m_gameMode = MODE_STOPPED;
		}
	}
	else if (pData->m_pSender == &Vision::Callbacks.OnRenderHook)
	{
		VisRenderHookDataObject_cl *pRenderHookData = (VisRenderHookDataObject_cl *)pData;
		if ( pRenderHookData->m_iEntryConst == VRH_POST_TRANSPARENT_PASS_GEOMETRY)
		{
			Render();
		}
	}
	else if (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneFinished)
	{
		Update( Vision::GetTimer()->GetTimeDifference() );
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
			CreateLuaCast(pScriptData, "Sprite", V_RUNTIME_CLASS(Sprite));
			CreateLuaCast(pScriptData, "Camera2D", V_RUNTIME_CLASS(Camera2D));
		}
	}
#if USE_HAVOK_PHYSICS_2D
	if (pData->m_pSender == &vHavokPhysicsModule::OnBeforeInitializePhysics)
	{
		vHavokPhysicsModuleCallbackData *pHavokData = (vHavokPhysicsModuleCallbackData*)pData;

		// #jve #note : This shouldn't be necessary, but we add this here to make sure that the broadphase
		//              is accessible.
		hkProductFeatures::initialize();

		VISION_HAVOK_SYNC_STATICS();
		VISION_HAVOK_SYNC_PER_THREAD_STATICS( pHavokData->GetHavokModule() );

		hkDefaultClassNameRegistry& dcnReg	= hkDefaultClassNameRegistry::getInstance();
		hkTypeInfoRegistry&			tyReg	= hkTypeInfoRegistry::getInstance();
		hkVtableClassRegistry&		vtcReg	= hkVtableClassRegistry::getInstance();
	
		dcnReg.registerList(hkBuiltinTypeRegistry::StaticLinkedClasses);
		tyReg.registerList(hkBuiltinTypeRegistry::StaticLinkedTypeInfos);
		vtcReg.registerList(hkBuiltinTypeRegistry::StaticLinkedTypeInfos, hkBuiltinTypeRegistry::StaticLinkedClasses);

		m_physicsModule = pHavokData->GetHavokModule();
		VASSERT(m_physicsModule != NULL);
		if (m_physicsModule)
		{
			m_physicsModule->AddStepper(this);
		}
	}
	else if (pData->m_pSender == &vHavokPhysicsModule::OnAfterDeInitializePhysics)
	{
		vHavokPhysicsModuleCallbackData *pHavokData = (vHavokPhysicsModuleCallbackData*)pData;
		pHavokData->GetHavokModule()->RemoveStepper(this);

		VISION_HAVOK_UNSYNC_STATICS();
		VISION_HAVOK_UNSYNC_PER_THREAD_STATICS( pHavokData->GetHavokModule() );
	}
	else if (pData->m_pSender == &vHavokPhysicsModule::OnAfterWorldCreated)
	{
		InitializeHavokPhysics();
	}
	else if (pData->m_pSender == &vHavokPhysicsModule::OnBeforeDeInitializePhysics)
	{
		UnintializeHavokPhysics();
	}
	else if (pData->m_pSender == &vHavokVisualDebugger::OnCreatingContexts)
	{
		//hkpPhysicsContext::registerAllPhysicsProcesses();
		vHavokVisualDebuggerCallbackData_cl *pEventData = (vHavokVisualDebuggerCallbackData_cl *)pData;
		for( int contextIt = 0; contextIt < pEventData->m_contexts->getSize(); contextIt++ )
		{
			hkProcessContext* currentContext = (*(pEventData->m_contexts))[contextIt];
			if ( hkString::strCmp(currentContext->getType(), HKP_PHYSICS_CONTEXT_TYPE_STRING)==0)
			{
				hkpPhysicsContext* physicsContext = static_cast<hkpPhysicsContext*>(currentContext);
				physicsContext->addWorld(m_world);
			}
		}
	}
	else if (pData->m_pSender == &vHavokVisualDebugger::OnAddingDefaultViewers)
	{
		// 		vHavokVisualDebuggerCallbackData_cl *pEventData = (vHavokVisualDebuggerCallbackData_cl *)pData;
		// 		if( pEventData->m_pVisualDebugger != HK_NULL )
		// 		{
		// 			//hkpPhysicsContext::addDefaultViewers( pEventData->m_pVisualDebugger );
		// 		}

		vHavokVisualDebuggerCallbackData_cl *pEventData = (vHavokVisualDebuggerCallbackData_cl *)pData;
		for( int contextIt = 0; contextIt < pEventData->m_contexts->getSize(); contextIt++ )
		{
			hkProcessContext* currentContext = (*(pEventData->m_contexts))[contextIt];
			const char* theName = currentContext->getType();
			if ( hkString::strCmp(theName, HKP_PHYSICS_CONTEXT_TYPE_STRING)==0)
			{
				m_pContext = static_cast<hkpPhysicsContext*>(currentContext);
				m_pContext->addReference();
			}
		}
	}
#endif // USE_HAVOK_PHYSICS_2D
}

void Toolset2dManager::Render()
{
	IVRender2DInterface *pRender = Vision::RenderLoopHelper.BeginOverlayRendering();	

	pRender->SetDepth(512.f);
	pRender->SetScissorRect(NULL);
	
	VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_ALWAYSVISIBLE | RENDERSTATEFLAG_FILTERING | RENDERSTATEFLAG_DOUBLESIDED);

	// Sort all the sprites by their Z order
	qsort(m_sprites.GetData(), m_sprites.GetSize(), sizeof(VWeakPtr<VisBaseEntity_cl> *), compareSprites);

	if (m_camera != NULL)
	{
		const hkvVec4 *transform = m_camera->GetTransform();
		pRender->SetTransformation(transform);
	}

	// Now render all the things
	for (int spriteIndex = 0; spriteIndex < m_sprites.GetSize(); spriteIndex++)
	{
		Sprite *sprite = static_cast<Sprite*>( m_sprites[spriteIndex]->GetPtr() );
		if (sprite)
		{
			sprite->Render(pRender, state);
		}
	}

	Vision::RenderLoopHelper.EndOverlayRendering();
}

void Toolset2dManager::Update(float deltaTime)
{
	hkvAlignedBBox *viewportBoundingBox = NULL;
	hkvAlignedBBox viewport;

	if ( Vision::IsInitialized() && Vision::Contexts.GetMainRenderContext() )
	{
		int x, y, w, h;
		Vision::Contexts.GetMainRenderContext()->GetViewport(x, y, w, h);

		viewport.setZero();
		viewport.expandToInclude( hkvVec3(static_cast<float>(x), static_cast<float>(y), 0) );
		viewport.expandToInclude( hkvVec3(static_cast<float>(x + w), static_cast<float>(y + h), 0) );

		viewportBoundingBox = &viewport;
	}

	int spriteIndex = 0;

	// Remove all dead sprites and update vertices first before checking collision
	while (spriteIndex < m_sprites.GetSize())
	{
		Sprite *sprite = static_cast<Sprite*>( m_sprites[spriteIndex]->GetPtr() );
		if (sprite == NULL)
		{
			V_SAFE_DELETE( m_sprites[spriteIndex] );
			m_sprites.RemoveAt(spriteIndex);
		}
		else
		{
			// Update all the sprites first so we're sure their vertices are up to date
			sprite->Update(viewportBoundingBox);

			spriteIndex++;
		}
	}
	
	// Check to see if there are any overlaps and report it
	for (spriteIndex = 0; spriteIndex < m_sprites.GetSize(); spriteIndex++)
	{
		Sprite *sprite = static_cast<Sprite*>( m_sprites[spriteIndex]->GetPtr() );

		// Only worry about sprites that have collision enabled
		if (sprite->IsColliding())
		{
			// Check this sprite against 
			for (int otherSpriteIndex = spriteIndex + 1; otherSpriteIndex < m_sprites.GetSize(); otherSpriteIndex++)
			{
				Sprite *otherSprite = static_cast<Sprite*>( m_sprites[otherSpriteIndex]->GetPtr() );
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

void Toolset2dManager::AddSprite(Sprite *sprite)
{
	if (FindSprite(sprite) == -1)
	{
		m_sprites.Append( new VWeakPtr<VisBaseEntity_cl>(sprite->GetWeakReference()) );
	}
}

int Toolset2dManager::FindSprite(Sprite *sprite)
{
	int resultIndex = -1;
	for (int spriteIndex = 0; spriteIndex < m_sprites.GetSize(); spriteIndex++)
	{
		if (m_sprites[spriteIndex]->GetPtr() == sprite)
		{
			resultIndex = spriteIndex;
			break;
		}
	}
	return resultIndex;
}

void Toolset2dManager::RemoveSprite(Sprite *sprite)
{
	int index = FindSprite(sprite);
	if (index != -1)
	{
		V_SAFE_DELETE( m_sprites[index] );
		m_sprites.RemoveAt(index);
	}
}

int Toolset2dManager::GetNumSprites()
{
	return m_sprites.GetLength();
}

const SpriteData *Toolset2dManager::GetSpriteData(const VString &spriteSheetFilename, const VString &xmlDataFilename)
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
	if (spSpriteSheetTexture && spSpriteSheetTexture->GetTextureType() == VTextureLoader::Texture2D)
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

		// #todo #jve : Perhaps it's a bit weird to have a spritesheet that also has animations on it--all of the cells
		//              would have to be in the same places for each sheet, which is odd. Probably assert when there is
		//              XML document AND a texture animation.
		spriteData->textureAnimation = Vision::TextureManager.GetAnimationInstance(spriteData->spriteSheetTexture);
		if (spriteData->textureAnimation)
		{
			spriteData->textureAnimation->AddRef();
		}

		TiXmlDocument xmlDocument;

		// If we successfully load the XML, then there is data we can parse about the sprites in the sheet
		if ( xmlDocument.LoadFile(xmlDataFilename) )
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
		else
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

		spriteData->GenerateConvexHull();
	}

	return spriteData;
}

void Toolset2dManager::RegisterLua()
{
	IVScriptManager* pSM = Vision::GetScriptManager();
	if (pSM != NULL)
	{
		lua_State* pLuaState = static_cast<VScriptResourceManager*>(pSM)->GetMasterState();
		if (pLuaState)
		{
			luaopen_Toolset2dModule(pLuaState);

			int iRetParams = LUA_CallStaticFunction(pLuaState, "Toolset2dModule", "Toolset2dManager", "Cast", "v>v", Toolset2dManager::Instance());
			if (iRetParams==1)
			{
				if(lua_isnil(pLuaState, -1))
				{
					lua_pop(pLuaState, iRetParams);
				}
				else
				{
					lua_setglobal(pLuaState, "Toolset2D");
				}
			}
		}
		else
		{
			Vision::Error.AddReportEntry(VIS_REPORTENTRY_WARNING, "Toolset2D", "Unable to create Lua Sprite Module, lua_State is NULL!", "");
		}
	}
}

Sprite *Toolset2dManager::CreateSprite(const hkvVec3 &position, const char *spriteSheetFilename, const char *xmlDataFilename)
{
	Sprite *sprite = (Sprite *)Vision::Game.CreateEntity( "Sprite", hkvVec3::ZeroVector() );
	if (sprite != NULL)
	{
		sprite->SetSpriteSheetData(spriteSheetFilename, xmlDataFilename);
		sprite->SetCenterPosition(position);
	}
	return sprite;
}

void Toolset2dManager::SetCamera(Camera2D *camera)
{
	m_camera = camera;
}

Camera2D *Toolset2dManager::GetCamera()
{
	return m_camera;
}

#if USE_HAVOK_PHYSICS_2D
hkpWorld *Toolset2dManager::GetPhysicsWorld()
{
	return m_world;
}
#endif // USE_HAVOK_PHYSICS_2D

//----- functions

static int compareSprites(const void *arg1, const void *arg2)
{
	VWeakPtr<VisBaseEntity_cl> *pSort1 = *((VWeakPtr<VisBaseEntity_cl> **)arg1);
	VWeakPtr<VisBaseEntity_cl> *pSort2 = *((VWeakPtr<VisBaseEntity_cl> **)arg2);

	Sprite *pSprite1 = (Sprite *)pSort1->GetPtr();
	Sprite *pSprite2 = (Sprite *)pSort2->GetPtr();

	int result = 0;

	if (pSprite1 && pSprite2)
	{
		float a = pSprite1->GetPosition().z;
		float b = pSprite2->GetPosition().z;
		
		if (hkvMath::isFloatEqual(a, b))
		{
			__int64 id1 = pSprite1->GetUniqueID();
			__int64 id2 = pSprite2->GetUniqueID();
			result = static_cast<int>(id2 - id1);
		}
		else
		{
			result = (b > a) ? -1 : 1;
		}
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
