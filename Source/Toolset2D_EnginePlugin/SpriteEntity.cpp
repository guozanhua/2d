//=======
//
// Author: Joel Van Eenwyk
// Purpose: Handle sprite setup and rendering
// TODO:
//   * Add shader support
//   * Add 3D mode more it just generates a plane
//   * Add alpha test option
//   * Add blending mode options (additive, blend, etc)
//
//=======
//
//       .-""""""-.
//     .'          '.
//    /   O    -=-   \
//   :                :
//   |                | 
//   : ',          ,' :
//    \  '-......-'  /
//     '.          .'
//       '-......-'
//

#include "Toolset2D_EnginePluginPCH.h"

#include "SpriteEntity.hpp"
#include "SpriteManager.hpp"

#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>
#include <Vision/Runtime/Base/ThirdParty/tinyXML/tinyxml.h>

#define CURRENT_SPRITE_VERSION 2

#define PROP_TEXTURE_FILENAME TextureFilename
#define PROP_TEXTURE_SCALE TextureScale

V_IMPLEMENT_SERIAL(Sprite, VisBaseEntity_cl, 0, &gToolset2D_EngineModule);

// Called by the engine when entity is created. Not when it is de-serialized!
void Sprite::InitFunction()
{
	VisBaseEntity_cl::InitFunction();
	SetObjectKey(NULL);

	Clear();
	CommonInit();
}

// called by the engine when entity is destroyed
void Sprite::DeInitFunction()
{
	VisBaseEntity_cl::DeInitFunction();
	CommonDeInit();
}

// called by our InitFunction and our de-serialization code
void Sprite::CommonInit()
{
	SpriteManager::GlobalManager().AddSprite(this);
	SetShoeBoxData(m_spriteSheetFilename, m_xmlDataFilename);
}

void Sprite::CommonDeInit()
{ 
	SpriteManager::GlobalManager().RemoveSprite(this);	
	Clear();
}

void Sprite::Clear()
{
	m_loaded = false;

	Fullscreen = false;
	ScrollSpeed.setZero();

	m_currentState = -1;
	m_currentFrame = -1;
	m_frameTime = 0.f;
	m_paused = false;
	m_scrollOffset.setZero();

	ClearTextures();
}

void Sprite::ClearTextures()
{
	// Remove any old cells, states, or textures
	m_cells.RemoveAll();
	m_states.RemoveAll();
	m_stateNameToIndex.Reset();
	m_spTextureAnimation = NULL;
	V_SAFE_RELEASE(m_spSpriteSheetTexture);
}

bool Sprite::UpdateTextures()
{
	ClearTextures();

	bool success = false;

	m_spSpriteSheetTexture = Vision::TextureManager.Load2DTexture(m_spriteSheetFilename);
	if (m_spSpriteSheetTexture)
	{
		// Go ahead and add a reference to this texture
		m_spSpriteSheetTexture->AddRef();

		// TODO: Perhaps it's a bit weird to have a spritesheet that also has animations on it--all of the cells
		//       would have to be in the same places for each sheet, which is odd. Probably assert when there is
		//       XML document AND a texture animation.
		m_spTextureAnimation = Vision::TextureManager.GetAnimationInstance(m_spSpriteSheetTexture);

		TiXmlDocument xmlDocument;

		// If we successfully load the XML, then there is data we can parse about the sprites in the sheet
		if ( xmlDocument.LoadFile(m_xmlDataFilename, Vision::File.GetManager()) )
		{
			const char *szActionNode = "SubTexture";

			for (TiXmlElement *pNode = xmlDocument.RootElement()->FirstChildElement(szActionNode);
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

				const int newCellIndex = m_cells.Append(SpriteCell());
				SpriteCell *currentCell = &m_cells[newCellIndex];
				currentCell->name = name;
				currentCell->offset.x = static_cast<float>(x);
				currentCell->offset.y = static_cast<float>(y);
				currentCell->pivot.x = static_cast<float>(ox);
				currentCell->pivot.y = static_cast<float>(oy);
				currentCell->width = width;
				currentCell->height = height;
				currentCell->originalWidth = originalWidth;
				currentCell->originalHeight = originalHeight;

				const char *result = strrchr(name, '_');
				int index = -1;
				if (result != NULL)
					index = result - name;

				SpriteState *state = NULL;
				int stateIndex = -1;
				if (index == -1)
				{
					const char *extension = strrchr(name, '.');
					int extensionIndex = -1;
					if (extension != NULL)
						extensionIndex = extension - name;
					VString stateName = VString(name, extensionIndex);

					stateIndex = m_states.Append(SpriteState());
					state = &m_states[stateIndex];
					state->name = stateName;
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
		// No XML describing the sprite sheet, but we do have a sprite texture
		else if (m_spSpriteSheetTexture != NULL)
		{
			int stateIndex = m_states.Append(SpriteState());
			SpriteState *state = &m_states[stateIndex];
			state->name = m_spriteSheetFilename;
			state->framerate = 30.0f;
			state->cells.Add(0);
			m_stateNameToIndex.Set(state->name, stateIndex);

			const int newCellIndex = m_cells.Append(SpriteCell());
			SpriteCell *currentCell = &m_cells[newCellIndex];

			char buffer[FS_MAX_PATH];
			VFileHelper::GetFilenameNoExt(buffer, m_spriteSheetFilename);
			currentCell->name = buffer;
			currentCell->offset.x = 0.f;
			currentCell->offset.y = 0.f;
			currentCell->pivot.x = 0.f;
			currentCell->pivot.y = 0.f;
			currentCell->width = currentCell->originalWidth = m_spSpriteSheetTexture->GetTextureWidth();
			currentCell->height = currentCell->originalHeight = m_spSpriteSheetTexture->GetTextureHeight();

			m_currentState = m_currentFrame = 0;

			success = true;
		}

		if (m_states.GetSize() > 0)
		{
			m_currentState = m_currentFrame = 0;
		}
	}

	return success;
}

const hkvVec2 *Sprite::GetVertices() const
{
	return m_vertices;
}

bool Sprite::SetShoeBoxData(const char *spriteSheetFilename, const char *xmlFilename)
{
	bool success = false;

	if (m_spriteSheetFilename == spriteSheetFilename &&
		m_xmlDataFilename == xmlFilename &&
		m_loaded)
	{
		success = true;
	}
	else
	{
		m_spriteSheetFilename = spriteSheetFilename;
		m_xmlDataFilename = xmlFilename;

		success = UpdateTextures();
		if (success)
		{
			m_loaded = true;
		}
	}

	return success;
}

const VArray<VString> Sprite::GetStateNames() const
{
	VArray<VString> names;

	for (int i = 0; i < m_states.GetSize(); i++)
	{
		names.Add(m_states[i].name);
	}

	return names;
}

const SpriteState *Sprite::GetCurrentState() const
{
	const SpriteState *state = NULL;
	if (m_currentState != -1)
	{
		state = &m_states[m_currentState];
	}
	return state;
}

int Sprite::GetCurrentFrame() const
{
	return m_currentFrame;
}

void Sprite::SetCurrentFrame(int currentFrame)
{
	if (m_currentState != -1)
	{
		m_paused = true;

		const SpriteState *s = &m_states[m_currentState];
		const int numCells = s->cells.GetSize();		
		m_currentFrame = hkvMath::clamp(currentFrame, 0, numCells - 1);
	}
}

void Sprite::ThinkFunction()
{
	if (m_currentState != -1 && !m_paused)
	{
		const float dt = Vision::GetTimer()->GetTimeDifference();

		m_frameTime += dt;
		m_scrollOffset += ScrollSpeed * dt;

		if (!hkvMath::isFloatEqual(ScrollSpeed.x, 0.0f))
		{
			if (ScrollSpeed.x > 0 && m_scrollOffset.x > 1.0f)
			{
				m_scrollOffset.x -= 1.0f;
			}
			else if (ScrollSpeed.x < 0 && m_scrollOffset.x < 0.0f)
			{
				m_scrollOffset.x += 1.0f;
			}
		}

		if (!hkvMath::isFloatEqual(ScrollSpeed.y, 0.0f))
		{
			if (ScrollSpeed.y > 0 && m_scrollOffset.y > 1.0f)
			{
				m_scrollOffset.y -= 1.0f;
			}
			else if (ScrollSpeed.y < 0 && m_scrollOffset.y < 0.0f)
			{
				m_scrollOffset.y += 1.0f;
			}
		}

		const SpriteState *state = &m_states[m_currentState];
		const float inverseFramerate = 1.0f / state->framerate;
		if (m_frameTime >= inverseFramerate)
		{
			const int numCells = state->cells.GetSize();
			m_currentFrame = (m_currentFrame + 1) % numCells;
			m_frameTime -= dt;
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
		m_frameTime = 0.f;
	}
	return (index != -1);
}

void Sprite::SetFramePercent(float percent)
{
	if (m_currentState != -1)
	{
		m_paused = true;

		const SpriteState *s = &m_states[m_currentState];
		const int numCells = s->cells.GetSize();		
		m_currentFrame = static_cast<int>( hkvMath::clamp(percent, 0.f, 1.f) * static_cast<float>(numCells - 1) );
	}
}

void Sprite::Play()
{
	m_paused = false;
}

void Sprite::Pause()
{
	m_paused = true;
}

void Sprite::SetScrollSpeed(hkvVec2 scrollSpeed)
{
	ScrollSpeed = scrollSpeed;
}

const hkvVec2 &Sprite::GetScrollSpeed() const
{
	return ScrollSpeed;
}

void Sprite::SetFullscreenMode(bool enabled)
{
	Fullscreen = enabled;
}

bool Sprite::IsFullscreenMode() const
{
	return Fullscreen;
}

void Sprite::SetCenterPosition(const hkvVec3 &position)
{
	float width = 0.f;
	float height = 0.f;
	VTextureObject *texture = GetTexture();
	const hkvVec3 &scale = GetScaling();

	if (m_currentState != -1)
	{
		const SpriteState *spriteState = &m_states[m_currentState];
		const SpriteCell *cell = &m_cells[spriteState->cells[m_currentFrame]];
		width = static_cast<float>(cell->originalWidth);
		height = static_cast<float>(cell->originalHeight);
	}
	else if (texture != NULL)
	{
		width = static_cast<float>(texture->GetTextureWidth());
		height = static_cast<float>(texture->GetTextureHeight());
	}

	width *= scale.x;
	height *= scale.y;

	SetPosition(position.x - width / 2.f, position.y - height / 2.f, position.z);
}

hkvVec3 Sprite::GetPoint(float x, float y) const
{
	hkvVec2 point(x, y);

	if (m_currentState != -1)
	{
		const hkvVec2 &scale = GetScaling().getAsVec2();

		const SpriteState *spriteState = &m_states[m_currentState];
		const SpriteCell *cell = &m_cells[spriteState->cells[m_currentFrame]];
		const float width = static_cast<float>(cell->originalWidth);
		const float height = static_cast<float>(cell->originalHeight);

		hkvVec2 offset(width / 2.0f, height / 2.0f);

		point -= offset;
		point = point.compMul(scale);

		// rotate the corners
		hkvMat3 rotation;
		rotation.setRotationMatrixZ(m_vOrientation.z);
		point = (rotation * point.getAsVec3(0.0f)).getAsVec2();

		// offset it back to the corner and final position
		point += GetPosition().getAsVec2() + offset.compMul(scale);
	}

	return point.getAsVec3(0.f);
}

void Sprite::OnVariableValueChanged(VisVariable_cl *pVar, const char *value)
{
	if ( !strcmp(pVar->name, "TextureFilename") )
	{
		if (value &&
			value[0] &&
			m_spriteSheetFilename != value)
		{
			m_spriteSheetFilename = value;
			UpdateTextures();
		}
		else
		{
			ClearTextures();
		}
	}
	else if ( !strcmp(pVar->name, "XmlDataFilename") )
	{
		if (value &&
			value[0] &&
			m_xmlDataFilename != value)
		{
			m_xmlDataFilename = value;
			UpdateTextures();
		}
		else
		{
			ClearTextures();
		}
	}
}

BOOL Sprite::AddComponent(IVObjectComponent *pComponent)
{
	BOOL success = VisBaseEntity_cl::AddComponent(pComponent);
	return success;
}

void Sprite::DebugRender(IVRenderInterface *pRenderer, float fSize, VColorRef iColor, bool bRenderConnections) const
{
	//VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
	//hkvAlignedBBox bbox;
	//hkvVec3 vRad(fSize, fSize, fSize);
	//const hkvVec3 worldPosition = GetPosition();
	//bbox.m_vMin = worldPosition - vRad;
	//bbox.m_vMax = worldPosition + vRad;
	//pRenderer->RenderAABox(bbox, iColor, state);
}

VTextureObject *Sprite::GetTexture() const
{
	VTextureObject *texture = m_spSpriteSheetTexture;	
	if (m_spTextureAnimation)
	{
		texture = m_spTextureAnimation->GetCurrentFrame();
	}
	return texture;
}

void Sprite::Update()
{
	const VTextureObject *texture = GetTexture();

	float width = 5.0f;
	float height = 5.0f;

	if (texture != NULL)
	{
		width = static_cast<float>(texture->GetTextureWidth());
		height = static_cast<float>(texture->GetTextureHeight());
	}

	hkvVec3 worldPosition = GetPosition();	
	hkvVec2 topLeft;
	hkvVec2 bottomRight;
	hkvVec2 uvTopLeft(0, 0);
	hkvVec2 uvBottomRight(1, 1);

	if (m_currentState != -1)
	{
		const SpriteState *spriteState = &m_states[m_currentState];
		const SpriteCell *cell = &m_cells[spriteState->cells[m_currentFrame]];

		float w = (float)cell->width / width;
		float h = (float)cell->height / height;
		float x = cell->offset.x / width;
		float y = cell->offset.y / height;

		uvTopLeft.x = x + m_scrollOffset.x;
		uvTopLeft.y = y + m_scrollOffset.y;
		uvBottomRight.x = uvTopLeft.x + w;
		uvBottomRight.y = uvTopLeft.y + h;

		if (IsFullscreenMode())
		{
			topLeft.x = 0;
			topLeft.y = 0;

			{
				int x, y, w, h;
				Vision::Contexts.GetMainRenderContext()->GetViewport(x, y, w, h);

				// Stretch it out to make sure it fits the screen
				if (w - width > h - height)
				{
					bottomRight.x = static_cast<float>(w);
					bottomRight.y = static_cast<float>(w * height) / width;
				}
				else
				{
					bottomRight.y = static_cast<float>(h);
					bottomRight.x = static_cast<float>(width * h) / height;
				}
			}

			uvTopLeft.x += worldPosition.x / width;
			uvBottomRight.x += worldPosition.x / width;
			uvTopLeft.y += worldPosition.y / height;
			uvBottomRight.y += worldPosition.y / height;

			m_vertices[VERTEX_TOP_LEFT] = topLeft;
			m_vertices[VERTEX_TOP_RIGHT] = hkvVec2(bottomRight.x, topLeft.y);
			m_vertices[VERTEX_BOTTOM_LEFT] = hkvVec2(topLeft.x, bottomRight.y);
			m_vertices[VERTEX_BOTTOM_RIGHT] = bottomRight;
		}
		else
		{
			const hkvVec3 &scale = GetScaling();
			topLeft.x += cell->pivot.x;
			topLeft.y += cell->pivot.y;
			bottomRight.x = topLeft.x + cell->width * scale.x;
			bottomRight.y = topLeft.y + cell->height * scale.y;

			const float ww = bottomRight.x - topLeft.x;
			const float hh = bottomRight.y - topLeft.y;
			hkvVec2 offset(ww / 2.0f, hh / 2.0f);

			// offset it so that we rotate around the center of the shape
			topLeft -= offset;
			bottomRight -= offset;

			hkvVec2 tr = topLeft;
			tr.x += ww;

			hkvVec2 bl = bottomRight;
			bl.x -= ww;

			// rotate the corners
			hkvMat3 rotation;
			rotation.setRotationMatrixZ(m_vOrientation.z);
			topLeft = (rotation * topLeft.getAsVec3(0.0f)).getAsVec2();
			bottomRight = (rotation * bottomRight.getAsVec3(0.0f)).getAsVec2();
			tr = (rotation * tr.getAsVec3(0.0f)).getAsVec2();
			bl = (rotation * bl.getAsVec3(0.0f)).getAsVec2();

			// offset it back to the corner and final position
			topLeft += worldPosition.getAsVec2() + offset;
			bottomRight += worldPosition.getAsVec2() + offset;
			tr += worldPosition.getAsVec2() + offset;
			bl += worldPosition.getAsVec2() + offset;

			// save off the final position
			m_vertices[VERTEX_TOP_LEFT] = topLeft;
			m_vertices[VERTEX_TOP_RIGHT] = tr;
			m_vertices[VERTEX_BOTTOM_LEFT] = bl;
			m_vertices[VERTEX_BOTTOM_RIGHT] = bottomRight;
		}
	}
	else
	{
		bottomRight.x = topLeft.x + width;
		bottomRight.y = topLeft.y + height;

		m_vertices[VERTEX_TOP_LEFT] = topLeft;
		m_vertices[VERTEX_TOP_RIGHT] = hkvVec2(bottomRight.x, topLeft.y);
		m_vertices[VERTEX_BOTTOM_LEFT] = hkvVec2(topLeft.x, bottomRight.y);
		m_vertices[VERTEX_BOTTOM_RIGHT] = bottomRight;
	}

	m_texCoords[VERTEX_TOP_LEFT] = uvTopLeft;
	m_texCoords[VERTEX_TOP_RIGHT] = hkvVec2(uvBottomRight.x, uvTopLeft.y);
	m_texCoords[VERTEX_BOTTOM_LEFT] = hkvVec2(uvTopLeft.x, uvBottomRight.y);
	m_texCoords[VERTEX_BOTTOM_RIGHT] = uvBottomRight;
}

void Sprite::Render(IVRender2DInterface *pRender, VSimpleRenderState_t& state)
{
	Overlay2DVertex_t vertices[6];

	vertices[0].Set(m_vertices[VERTEX_TOP_LEFT].x, m_vertices[VERTEX_TOP_LEFT].y, m_texCoords[VERTEX_TOP_LEFT].x, m_texCoords[VERTEX_TOP_LEFT].y);
	vertices[1].Set(m_vertices[VERTEX_BOTTOM_LEFT].x, m_vertices[VERTEX_BOTTOM_LEFT].y, m_texCoords[VERTEX_BOTTOM_LEFT].x, m_texCoords[VERTEX_BOTTOM_LEFT].y);
	vertices[2].Set(m_vertices[VERTEX_TOP_RIGHT].x, m_vertices[VERTEX_TOP_RIGHT].y, m_texCoords[VERTEX_TOP_RIGHT].x, m_texCoords[VERTEX_TOP_RIGHT].y);

	vertices[3].Set(m_vertices[VERTEX_TOP_RIGHT].x, m_vertices[VERTEX_TOP_RIGHT].y, m_texCoords[VERTEX_TOP_RIGHT].x, m_texCoords[VERTEX_TOP_RIGHT].y);
	vertices[4].Set(m_vertices[VERTEX_BOTTOM_LEFT].x, m_vertices[VERTEX_BOTTOM_LEFT].y, m_texCoords[VERTEX_BOTTOM_LEFT].x, m_texCoords[VERTEX_BOTTOM_LEFT].y);
	vertices[5].Set(m_vertices[VERTEX_BOTTOM_RIGHT].x, m_vertices[VERTEX_BOTTOM_RIGHT].y, m_texCoords[VERTEX_BOTTOM_RIGHT].x, m_texCoords[VERTEX_BOTTOM_RIGHT].y);

	pRender->Draw2DBuffer(6, vertices, GetTexture(), state);
}

void Sprite::OnCollision(Sprite *other)
{
	this->TriggerScriptEvent("OnSpriteCollision", "*o", other);
}

bool Sprite::GetIntersection(const hkvVec2 &p1, const hkvVec2 &p2, const hkvVec2 &p3, const hkvVec2 &p4, hkvVec2 *result) const
{
	bool intersecting = true;

	// Store the values for fast access and easy
	// equations-to-code conversion
	float x1 = p1.x, x2 = p2.x, x3 = p3.x, x4 = p4.x;
	float y1 = p1.y, y2 = p2.y, y3 = p3.y, y4 = p4.y;

	float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

	// If d is zero, there is no intersection
	if (hkvMath::isZero(d))
	{
		intersecting = false;
	}
	else
	{
		// Get the x and y
		float pre = (x1 * y2 - y1 * x2), post = (x3 * y4 - y3 * x4);
		float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
		float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;

		// Check if the x and y coordinates are within both lines
		if ( x < min(x1, x2) || x > max(x1, x2) ||
			 x < min(x3, x4) || x > max(x3, x4) )
		{
			intersecting = false;
		}
		else if ( y < min(y1, y2) || y > max(y1, y2) ||
			      y < min(y3, y4) || y > max(y3, y4) )
		{
			intersecting = false;
		}

		// Return the point of intersection4
		if (intersecting && result != NULL)
		{
			result->x = x;
			result->y = y;
		}
	}

	return intersecting;
}

bool Sprite::IsOverlapping(Sprite *other) const
{
	const hkvVec2 *otherVertices = other->GetVertices();
	bool inside = true;

	int edges[8] = {0, 1,
					1, 3,
					3, 2,
					2, 0};

	// Loop through each vertex and see if any of them are inside all
	// of the edges
	for (int j = 0; j < 4; j++)
	{
		// Test each edge
		for (int i = 0; i < 4; i++)
		{
			int p1 = edges[i * 2 + 0];
			int p2 = edges[i * 2 + 1];

			hkvVec3 e = (m_vertices[p2] - m_vertices[p1]).getAsVec3(0.f);
			hkvVec3 l = (otherVertices[j] - m_vertices[p1]).getAsVec3(0.f);
			e.normalize();
			l.normalize();
			hkvVec3 up = e.cross(l);

			if (up.z < 0)
			{
				inside = false;
				break;
			}
		}

		if (inside)
		{
			break;
		}
	}

	return inside;
}

void Sprite::Serialize(VArchive &ar)
{
	VisBaseEntity_cl::Serialize(ar);

	if (ar.IsLoading())
	{
		Clear();

		char spriteVersion;
		ar >> spriteVersion;
		VASSERT(spriteVersion <= CURRENT_SPRITE_VERSION);

		char spriteSheetBuffer[FS_MAX_PATH + 1];
		ar.ReadStringBinary(spriteSheetBuffer, FS_MAX_PATH);
		m_spriteSheetFilename = spriteSheetBuffer;

		char xmlFilenameBuffer[FS_MAX_PATH + 1];
		ar.ReadStringBinary(xmlFilenameBuffer, FS_MAX_PATH);
		m_xmlDataFilename = xmlFilenameBuffer;

		ar >> TextureScale;
		ar >> ScrollSpeed.x;
		ar >> ScrollSpeed.y;
		ar >> Fullscreen;
	} 
	else
	{
		ar << (char)CURRENT_SPRITE_VERSION;

		ar.WriteStringBinary(m_spriteSheetFilename);
		ar.WriteStringBinary(m_xmlDataFilename);

		ar << TextureScale;
		ar << ScrollSpeed.x;
		ar << ScrollSpeed.y;
		ar << Fullscreen;
	}
}

void Sprite::OnSerialized(VArchive &ar)
{
	VisBaseEntity_cl::OnSerialized(ar);

	CommonInit();
}

START_VAR_TABLE(Sprite, VisBaseEntity_cl, "Sprite", 0, "")
	DEFINE_VAR_STRING_CALLBACK(Sprite, TextureFilename, "Sprite sheet", "white.dds", DISPLAY_HINT_TEXTUREFILE, NULL);
	DEFINE_VAR_STRING_CALLBACK(Sprite, XmlDataFilename, "Xml Data", "", DISPLAY_HINT_CUSTOMFILE, NULL);
	DEFINE_VAR_FLOAT_AND_NAME(Sprite, PROP_TEXTURE_SCALE, "Scale", "Scale of the sprite sheet (% of pixels)", "0", 0, "Clamp(0, 1)");
END_VAR_TABLE