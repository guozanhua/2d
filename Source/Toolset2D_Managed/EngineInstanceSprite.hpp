#pragma once

using namespace System;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace CSharpFramework::Scene;

class Sprite;

namespace Toolset2D_Managed
{
	/// <summary>
	/// EngineInstanceSprite : class that manages the native Sprite entity class
	/// </summary>
	public ref class EngineInstanceSprite : public IEngineShapeInstance
	{
	public:
		EngineInstanceSprite();

		VOVERRIDE void DisposeObject() override;
		VOVERRIDE IntPtr GetNativeObject() override
		{
			return System::IntPtr((void *)m_pSprite);
		}

		VOVERRIDE void SetVisible(bool bStatus) override;
		VOVERRIDE void SetObjectKey(String ^key) override;
		VOVERRIDE void SetPosition(float x,float y,float z) override;
		VOVERRIDE void SetOrientation(float yaw,float pitch,float roll) override;
		VOVERRIDE bool GetOrientation(Vector3F %orientation) override;
		VOVERRIDE void SetScaling(float x, float y, float z) override;
		VOVERRIDE bool GetLocalBoundingBox(BoundingBox^ %bbox) override;
		VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult^ %result) override;
		VOVERRIDE bool OnExport(SceneExportInfo ^info) override;
		VOVERRIDE void SetUniqueID(unsigned __int64 iID) override;

		VOVERRIDE bool CanAttachComponent(ShapeComponent ^component, String ^%sError) override;
		VOVERRIDE void OnAttachComponent(ShapeComponent ^component) override;

		// special functions
		void RenderShape(VisionViewBase ^view, CSharpFramework::Shapes::ShapeRenderMode mode);

		void SetSpriteSheetData(String ^pFileName, String ^pXml);

		array<String^>^ GetStateNames();

		String^ GetCurrentState();
		void SetCurrentState(String^ state);

		int GetCurrentFrame();
		void SetCurrentFrame(int frame);

		float GetScrollX();
		float GetScrollY();
		void SetScroll(float x, float y);

		float GetWidth();
		float GetHeight();

		void SetWidth(float width);
		void SetHeight(float height);

		void SetFullscreenMode(bool enabled);
		bool IsFullscreenMode();

		void SetPlayOnce(bool enabled);
		bool IsPlayOnce();

		void SetCollision(bool enabled);
		bool IsColliding();

	private:		
		// pointer to native engine object
		Sprite *m_pSprite;
	};
}
