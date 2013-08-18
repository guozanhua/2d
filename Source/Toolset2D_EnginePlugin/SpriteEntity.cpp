#include "Toolset2D_EnginePluginPCH.h"

#include "SpriteEntity.hpp"
#include "SpriteManager.hpp"

#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>
#include <Vision/Runtime/Base/ThirdParty/tinyXML/tinyxml.h>

V_IMPLEMENT_SERIAL(Sprite, VisBaseEntity_cl, 0, &gToolset2D_EngineModule);

#define PROP_TEXTURE_FILENAME TextureFilename

Sprite::Sprite() :
VisBaseEntity_cl(),
m_currentFrame(-1),
m_currentState(-1),
m_spSpriteSheetTexture(NULL),
m_spriteMeshBuffer(NULL),
m_staticMesh(NULL),
m_staticMeshInstance(NULL)
{
	CommonInit();
}

Sprite::Sprite(const char *spriteSheetFilename, const char *shoeBoxXml) :
VisBaseEntity_cl(),
	m_spSpriteSheetTexture(NULL),
	m_spriteMeshBuffer(NULL),
	m_staticMesh(NULL),
	m_staticMeshInstance(NULL)
{
	CommonInit();
	LoadShoeBox(spriteSheetFilename, shoeBoxXml);
}

Sprite::~Sprite()
{
	SpriteManager::GlobalManager().RemoveSprite(this);
}

// Called by the engine when entity is created. Not when it is de-serialized!
void Sprite::InitFunction()
{
	CommonInit();
}

// called by the engine when entity is destroyed
void Sprite::DeInitFunction()
{
	SpriteManager::GlobalManager().RemoveSprite(this);

	// TODO: Shouldn't this be removed?
	//m_spriteMeshBuffer->Remove;
	m_spriteMeshBuffer = NULL;
}

bool Sprite::LoadShoeBox(const char *spriteSheetFilename, const char *xmlFilename)
{
	TiXmlDocument doc;
	bool success = false;

	m_spSpriteSheetTexture = Vision::TextureManager.Load2DTexture(spriteSheetFilename);

	if ( doc.LoadFile(xmlFilename, Vision::File.GetManager()) )
	{
		const char *szActionNode = "SubTexture";
		for (TiXmlElement *pNode = doc.RootElement()->FirstChildElement(szActionNode);
			 pNode != NULL;
			 pNode = pNode->NextSiblingElement(szActionNode) )
		{
			SpriteCell cell;

			const char *name = pNode->Attribute("name");

			int x;
			int y;
			int width;
			int height;

			pNode->QueryIntAttribute("x", &x);
			pNode->QueryIntAttribute("y", &y);
			pNode->QueryIntAttribute("width", &width);
			pNode->QueryIntAttribute("height", &height);

			cell.name = name;
			cell.offset.x = static_cast<float>(x);
			cell.offset.y = static_cast<float>(y);
			cell.width = width;
			cell.height = height;

			const int newCellIndex = m_cells.Append(cell);
			SpriteCell *currentCell = &m_cells[newCellIndex];

			const char *result = strrchr(name, '_');
			int index = -1;
			if (result != NULL)
				index = result - name;

			SpriteState *state = NULL;
			int stateIndex = -1;
			if (index == -1)
			{
				stateIndex = m_states.Append(SpriteState());
				state = &m_states[stateIndex];
				state->name = name;
				state->framerate = 30.0f;
				m_stateNameToIndex.Set(state->name, stateIndex);
			}
			else
			{
				VString s = name;
				VString stateName = VString(name, index);
				VString last = VString(&name[index + 1], strlen(name) - index);
				VString number = VString(last.GetChar(), last.Find("."));
				currentCell->index = atoi(number);

				for (int i = 0; i < m_states.GetSize(); i++)
				{
					if (m_states[i].name == stateName)
					{
						stateIndex = i;
						state = &m_states[i];
						break;
					}
				}

				if (stateIndex == -1)
				{
					stateIndex = m_states.Append(SpriteState());
					state = &m_states[stateIndex];
					state->name = stateName;
					state->framerate = 10.f;
					m_stateNameToIndex.Set(state->name, stateIndex);
				}

			}

			state->cells.Append(newCellIndex);
		}

		success = true;
	}

	return success;
}

// called by our InitFunction and our de-serialization code
void Sprite::CommonInit()
{ 
	m_pCustomTraceBBox = NULL;
	SpriteManager::GlobalManager().AddSprite(this);

	if (Vision::Editor.IsPlaying())
	{
		VDynamicMesh* pMesh = NULL;

		{
			VDynamicMeshBuilder b(4, 2, 0, 1);
			const hkvVec3 n;
			const hkvVec3 t;
			const hkvVec2 uv;
			VColorRef color;
			b.AddVertex( hkvVec3(0, 30, 0), n, t, uv, color );
			b.AddVertex( hkvVec3(0, 30, 30), n, t, uv, color );
			b.AddVertex( hkvVec3(0, 0, 30), n, t, uv, color );
			b.AddVertex( hkvVec3(0, 0, 0), n, t, uv, color );
			b.AddTriangle(0, 1, 2);
			b.AddTriangle(2, 1, 0);
			pMesh = b.Finalize();
		}

		if (pMesh != NULL)
		{
			pMesh->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
		}
	}
}

void Sprite::ThinkFunction()
{
	// TODO: Update the current cell being rendered
	if (m_currentState != -1)
	{
		m_lastTime += Vision::GetTimer()->GetTimeDifference();
		const SpriteState *s = &m_states[m_currentState];
		const float dt = 1.0f / s->framerate;
		if (m_lastTime >= dt)
		{
			const int numCells = s->cells.GetSize();
			m_currentFrame = (m_currentFrame + 1) % numCells;
			m_lastTime -= dt;
		}
	}
}

bool Sprite::SetState(const char *state)
{
	int index = m_stateNameToIndex.Find(state);
	if (index != -1)
	{
		m_currentState = index;
		m_currentFrame = 0;
		m_lastTime = 0.f;
	}
	return (index != -1);
}

void Sprite::OnVariableValueChanged(VisVariable_cl *pVar, const char *value)
{
	if ( !strcmp(pVar->name, V_QUOTE(PROP_TEXTURE_FILENAME)) )
	{
		if (value && value[0])
		{
			m_spSpriteSheetTexture = Vision::TextureManager.Load2DTexture(value);
		}
		else
		{
			m_spSpriteSheetTexture = NULL;
		}
	}
}

// render the node (and optionally the connections)
void Sprite::DebugRender(IVRenderInterface *pRenderer, float fSize, VColorRef iColor, bool bRenderConnections) const
{
	VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
	hkvAlignedBBox bbox;
	hkvVec3 vRad(fSize,fSize,fSize);
	const hkvVec3 pos = GetPosition();
	bbox.m_vMin = pos - vRad;
	bbox.m_vMax = pos + vRad;
	pRenderer->RenderAABox(bbox,iColor,state);
}

void Sprite::Render(IVRender2DInterface *pRender, VSimpleRenderState_t& state)
{
	hkvVec2 tl;
	hkvVec2 br;

	hkvVec3 pos = GetPosition();
	tl.x = pos.x;
	tl.y = pos.y;

	if (m_currentState != -1)
	{
		hkvVec2 topLeft;
		hkvVec2 bottomRight;

		float width = static_cast<float>(m_spSpriteSheetTexture->GetTextureWidth());
		float height = static_cast<float>(m_spSpriteSheetTexture->GetTextureHeight());

		const SpriteState *spriteState = &m_states[m_currentState];
		const SpriteCell *cell = &m_cells[spriteState->cells[m_currentFrame]];
		br.x = tl.x + cell->width;
		br.y = tl.y + cell->height;

		float w = (float)cell->width / width;
		float h = (float)cell->height / height;
		float x = cell->offset.x / width;
		float y = cell->offset.y / height;

		topLeft.x = x;
		topLeft.y = y;
		bottomRight.x = x + w;
		bottomRight.y = y + h;

		pRender->DrawTexturedQuad( tl, br, m_spSpriteSheetTexture, topLeft, bottomRight, V_RGBA_WHITE, state );
	}
	else
	{
		br.x = 150;
		br.y = 150;
		pRender->DrawTexturedQuad( tl, br, m_spSpriteSheetTexture, hkvVec2(0, 0), hkvVec2(1, 1), V_RGBA_WHITE, state );
	}
}

void Sprite::Serialize( VArchive &ar )
{
	VisBaseEntity_cl::Serialize(ar);

	// TODO: Do we need to serialize the filename or is that handled automatically?

	if (ar.IsLoading())
	{
		//m_vCenterPos.SerializeAsVisVector (ar);
	} 
	else
	{
		//m_vCenterPos.SerializeAsVisVector (ar);
	}
}

void Sprite::OnSerialized(VArchive &ar)
{
	VisBaseEntity_cl::OnSerialized(ar);

	// call this after VisBaseEntity_cl::OnSerialized(ar) because in that function components are attached
	CommonInit();
}

// vartable; provide the following members to vForge. Although vPhysXEntity is the base class we use VisBaseEntity_cl here to hide physX properties
START_VAR_TABLE(Sprite, VisBaseEntity_cl, "Sprite", 0, "")
	DEFINE_VAR_STRING_CALLBACK(Sprite, PROP_TEXTURE_FILENAME, "Sprite sheet", "white.dds", DISPLAY_HINT_TEXTUREFILE, NULL);
END_VAR_TABLE