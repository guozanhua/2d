#include "Toolset2D_ManagedPCH.h"

#include "EngineInstanceSprite.hpp"
#include "Toolset2D_EnginePlugin/SpriteEntity.hpp"
#include "Toolset2D_EnginePlugin/Toolset2dManager.hpp"

using namespace ManagedFramework;
#using <mscorlib.dll>

namespace Toolset2D_Managed
{
	EngineInstanceSprite::EngineInstanceSprite() : IEngineShapeInstance()
	{
		Sprite *sprite = (Sprite *)Vision::Game.CreateEntity("Sprite", hkvVec3(0, 0, 0));
		Debug::Assert(sprite != nullptr, "Could not create Sprite entity!");

		m_pEntityWP = new VWeakPtr<VisBaseEntity_cl>(sprite->GetWeakReference());
	}

	void EngineInstanceSprite::DisposeObject()
	{
		Sprite *pEntity = GetSpriteEntity();
		V_SAFE_DISPOSEOBJECT(pEntity);
	}

	void EngineInstanceSprite::SetVisible(bool bStatus)
	{
		if (GetSpriteEntity())
		{
			GetSpriteEntity()->SetVisibleBitmask(bStatus ? VIS_ENTITY_VISIBLE : VIS_ENTITY_INVISIBLE);
		}
	}

	void EngineInstanceSprite::SetObjectKey(String ^key)
	{
		if (!GetSpriteEntity())
			return;

		VString sKey;
		ConversionUtils::StringToVString(key, sKey);

		if (sKey.IsEmpty())
			GetSpriteEntity()->SetObjectKey(NULL);
		else
			GetSpriteEntity()->SetObjectKey(sKey);

		// inform owner object and its components, that the object key has changed
		GetSpriteEntity()->SendMsg(GetSpriteEntity(), VIS_MSG_EDITOR_PROPERTYCHANGED, (INT_PTR)"ObjectKey", 0);
	}

	void EngineInstanceSprite::SetPosition(float x, float y, float z)
	{
		if (GetSpriteEntity())
		{
			GetSpriteEntity()->SetPosition(x, y, z);
		}
	}

	void EngineInstanceSprite::SetOrientation(float yaw, float pitch, float roll)
	{
		if (GetSpriteEntity())
		{
			GetSpriteEntity()->SetOrientation(yaw, pitch, roll);
		}
	}

	bool EngineInstanceSprite::GetOrientation(Vector3F %orientation)
	{
		if (GetSpriteEntity())
		{
			const hkvVec3 &o = GetSpriteEntity()->GetOrientation();
			orientation.X = o.x;
			orientation.Y = o.y;
			orientation.Z = o.z;
		}
		return (GetSpriteEntity() != NULL);
	}

	void EngineInstanceSprite::SetScaling(float x, float y, float z)
	{
		if (GetSpriteEntity())
		{
			GetSpriteEntity()->SetScaling( hkvVec3(x, y, z) );
		}
	}

	void EngineInstanceSprite::RenderShape(VisionViewBase^ /*view*/, CSharpFramework::Shapes::ShapeRenderMode mode)
	{
		if (GetSpriteEntity() != NULL)
		{
			IVRenderInterface* pRI = Vision::Contexts.GetCurrentContext()->GetRenderInterface();
			VColorRef color;
			bool render = false;
			float width = 1.f;

			// render this shape as a small bounding box. Use a slightly larger bounding box if it is selected
			switch (mode)
			{
			case ShapeRenderMode::Normal:
				if (GetSpriteEntity()->GetVisibleBitmask() & VIS_ENTITY_VISIBLE)
				{
					color = VColorRef(255, 0, 0, 80);
					render = true;
				}
				break;

			case ShapeRenderMode::Selected:
				color = VColorRef(255, 160, 0, 140);
				render = true;
				break;
			}

			if (render && !GetSpriteEntity()->IsFullscreenMode())
			{
				// Make sure the vertices are updated before rendering
				GetSpriteEntity()->Update();

				const int edges[8] = {
					0, 1,
					1, 3,
					3, 2,
					2, 0};

				const hkvVec2 *vertices = GetSpriteEntity()->GetVertices();

				for (int edgeIndex = 0; edgeIndex < 4; edgeIndex++)
				{
					const int p1 = edges[edgeIndex * 2 + 0];
					const int p2 = edges[edgeIndex * 2 + 1];
					Vision::Game.DrawSingleLine2D(
						vertices[p1].x, vertices[p1].y,
						vertices[p2].x, vertices[p2].y,
						color, width);
				}

				// render the convex hull
				const SpriteCell *cell = GetSpriteEntity()->GetCurrentCell();
				if (cell != NULL && cell->vertexIndices.getSize() > 0)
				{
					const float scaleX = GetSpriteEntity()->GetScaling().x;
					const float scaleY = GetSpriteEntity()->GetScaling().y;
					const float offsetX = GetSpriteEntity()->GetCenterPosition().x;
					const float offsetY = GetSpriteEntity()->GetCenterPosition().y;

					const int* vi = &cell->vertexIndices[0];
					for(int i = 0;i < cell->verticesPerFace.getSize();++i)
					{
						const int count = cell->verticesPerFace[i];

						for(int j = 0; j < count; ++j)
						{
							const hkVector4 &e1 = cell->vertexPositions[ vi[j] ];
							const hkVector4 &e2 = cell->vertexPositions[ vi[(j + 1) % count] ];

							Vision::Game.DrawSingleLine2D(
								e1(0) * scaleX + offsetX, e1(1) * scaleY + offsetY,
								e2(0) * scaleX + offsetX, e2(1) * scaleY + offsetY,
								VColorRef(0, 255, 0, 80), width);
						}

						vi += count;
					}
				}

				bool drawOriginalOutlineSize = false;
				if (drawOriginalOutlineSize)
				{
					hkvVec2 position = GetSpriteEntity()->GetPosition().getAsVec2();
					hkvVec2 topLeft = position;
					hkvVec2 topRight = topLeft + hkvVec2(GetSpriteEntity()->GetOriginalCellWidth(), 0);
					hkvVec2 bottomLeft = topLeft + hkvVec2(0, GetSpriteEntity()->GetOriginalCellHeight());
					hkvVec2 bottomRight = bottomLeft + hkvVec2(GetSpriteEntity()->GetOriginalCellWidth(), 0);
					const hkvVec2 border[8] = {
						topLeft, topRight,
						topRight, bottomRight,
						bottomRight, bottomLeft,
						bottomLeft, topLeft};

						for (int borderEdgeIndex = 0; borderEdgeIndex < 4; borderEdgeIndex++)
						{
							const hkvVec2 p1 = border[borderEdgeIndex * 2 + 0];
							const hkvVec2 p2 = border[borderEdgeIndex * 2 + 1];
							Vision::Game.DrawSingleLine2D(
								p1.x, p1.y,
								p2.x, p2.y,
								VColorRef(255, 255, 0, 30), width);
						}
				}
			}
		}
	}

	bool EngineInstanceSprite::GetLocalBoundingBox(BoundingBox^ %bbox)
	{
		bool result = false;

		// create a small bounding box for picking
		if (GetSpriteEntity())
		{
			hkvAlignedBBox bbox = GetSpriteEntity()->GetBBox();
			result = true;
		}

		return result;
	}

	void EngineInstanceSprite::TraceShape(Shape3D^ ownerShape, Vector3F rayStart, Vector3F rayEnd, ShapeTraceResult^% result)
	{
		float x = 0.f;
		float y = 0.f;
		const hkvVec3 start(rayStart.X, rayStart.Y, rayStart.Z);

		if ( GetSpriteEntity() && Vision::Contexts.GetMainRenderContext()->Project2D(start, x, y) )
		{
			hkvAlignedBBox bbox = GetSpriteEntity()->GetBBox();
			const hkvVec3 point(x, y, 0.0f);
			if ( bbox.contains(point) )
			{
				float distance = (GetSpriteEntity()->GetCenterPosition() - point).getLength();

				if (GetSpriteEntity()->IsFullscreenMode())
				{
					// push the distance way back if we're in fullscreen mode
					distance += 1000.f;
				}

				if (!result->bHit || distance < result->fHitDistance)
				{
					result->hitShape = ownerShape;
					result->fHitDistance = distance;
					result->bHit = true;
				}
			}
		}
	}
	
	bool EngineInstanceSprite::OnExport(SceneExportInfo ^info)
	{
		// do the export: Get the export binary archive and serialize into it.
		// Requires the native node class to support serialization (derive from VisTypedEngineObject_cl and 
		// implement Serialize function)
		if (GetSpriteEntity())
		{
			VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
			ar.WriteObject(GetSpriteEntity());
		}
		return true;
	}

	void EngineInstanceSprite::SetUniqueID(unsigned __int64 iID)
	{
		if (GetSpriteEntity())
		{
			GetSpriteEntity()->SetUniqueID(iID);
		}
	}

	bool EngineInstanceSprite::CanAttachComponent(ShapeComponent ^component, String ^%sError)
	{
		// For now, just allow script components
		return (component->PrefabIdentifier == "VScriptComponent");
	}

	void EngineInstanceSprite::OnAttachComponent(ShapeComponent ^component)
	{
		if (GetSpriteEntity())
		{
			ConversionUtils::OnAttachComponent(GetSpriteEntity(), component);
		}
	}

	void EngineInstanceSprite::SetSpriteSheetData(String ^pFileName, String ^pXml)
	{
		if (GetSpriteEntity() == NULL)
		{
			return;
		}

		VString sFileName;
		ConversionUtils::StringToVString(pFileName, sFileName);

		hkStringBuf filename;
		bool validFilename = convertToAssetPath(sFileName, filename);

		if (validFilename)
		{
			VString sXml;
			ConversionUtils::StringToVString(pXml, sXml);

			hkStringBuf xml;
			bool validXml = convertToAssetPath(sXml, xml);
			if (!validXml)
			{
				xml = "";
			}

			GetSpriteEntity()->SetSpriteSheetData(filename, xml);
		}
	}

	array<String^>^ EngineInstanceSprite::GetStateNames()
	{
		if (GetSpriteEntity() != NULL)
		{
			const VArray<VString> stateNames = GetSpriteEntity()->GetStateNames();

			array<String^>^ names = gcnew array<String^>(stateNames.GetSize());
			for (int i = 0; i < stateNames.GetSize(); i++)
			{
				names[i] = ConversionUtils::VStringToString(stateNames[i]);
			}

			return names;
		}

		return gcnew array<String^>(0);
	}

	String^ EngineInstanceSprite::GetCurrentState()
	{
		String^ stateName = "";

		if (GetSpriteEntity() != NULL)
		{
			String^ stateName = "NONE";
			const SpriteState *state = GetSpriteEntity()->GetCurrentState();
			if (state != NULL)
			{
				stateName = ConversionUtils::VStringToString(state->name);
			}
		}

		return stateName;
	}

	void EngineInstanceSprite::SetCurrentState(String^ state)
	{
		if (GetSpriteEntity() != NULL)
		{
			VString stateName;
			ConversionUtils::StringToVString(state, stateName);
			GetSpriteEntity()->SetState(stateName);
		}
	}

	int EngineInstanceSprite::GetCurrentFrame()
	{
		int frame = -1;

		if (GetSpriteEntity() != NULL)
		{
			frame = GetSpriteEntity()->GetCurrentFrame();
		}

		return frame;
	}

	void EngineInstanceSprite::SetCurrentFrame(int frame)
	{
		if (GetSpriteEntity() != NULL)
		{
			GetSpriteEntity()->SetCurrentFrame(frame);
		}
	}

	float EngineInstanceSprite::GetScrollX()
	{
		float scrollX = 0.0f;
		if (GetSpriteEntity() != NULL)
		{
			scrollX = GetSpriteEntity()->GetScrollSpeed().x;
		}
		return scrollX;
	}

	float EngineInstanceSprite::GetScrollY()
	{
		float scrollY = 0.0f;
		if (GetSpriteEntity() != NULL)
		{
			scrollY = GetSpriteEntity()->GetScrollSpeed().y;
		}
		return scrollY;
	}

	void EngineInstanceSprite::SetScroll(float x, float y)
	{
		if (GetSpriteEntity() != NULL)
		{
			GetSpriteEntity()->SetScrollSpeed(hkvVec2(x, y));
		}
	}

	float EngineInstanceSprite::GetWidth()
	{
		float width = 0.f;
		if (GetSpriteEntity() != NULL)
		{
			width = GetSpriteEntity()->GetWidth();
		}
		return width;
	}

	float EngineInstanceSprite::GetHeight()
	{
		float height = 0.f;
		if (GetSpriteEntity() != NULL)
		{
			height = GetSpriteEntity()->GetHeight();
		}
		return height;
	}

	void EngineInstanceSprite::SetWidth(float width)
	{
		if (GetSpriteEntity() != NULL)
		{
			return GetSpriteEntity()->SetWidth(width);
		}
	}

	void EngineInstanceSprite::SetHeight(float height)
	{
		if (GetSpriteEntity() != NULL)
		{
			return GetSpriteEntity()->SetHeight(height);
		}
	}

	void EngineInstanceSprite::SetFullscreenMode(bool enabled)
	{
		if (GetSpriteEntity() != NULL)
		{
			GetSpriteEntity()->SetFullscreenMode(enabled);
		}
	}

	bool EngineInstanceSprite::IsFullscreenMode()
	{
		bool fullscreen = false;
		if (GetSpriteEntity() != NULL)
		{
			fullscreen = GetSpriteEntity()->IsFullscreenMode();
		}
		return fullscreen;
	}

	void EngineInstanceSprite::SetPlayOnce(bool enabled)
	{
		if (GetSpriteEntity() != NULL)
		{
			GetSpriteEntity()->SetPlayOnce(enabled);
		}
	}

	bool EngineInstanceSprite::IsPlayOnce()
	{
		bool playOnce = false;
		if (GetSpriteEntity() != NULL)
		{
			playOnce =  GetSpriteEntity()->IsPlayOnce();
		}
		return playOnce;
	}

	void EngineInstanceSprite::SetCollision(bool enabled)
	{
		if (GetSpriteEntity() != NULL)
		{
			GetSpriteEntity()->SetCollision(enabled);
		}
	}

	bool EngineInstanceSprite::IsColliding()
	{
		bool colliding = false;
		if (GetSpriteEntity() != NULL)
		{
			colliding = GetSpriteEntity()->IsColliding();
		}
		return colliding;
	}
}