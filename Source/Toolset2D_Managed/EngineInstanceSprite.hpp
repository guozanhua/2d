#pragma once

using namespace System;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace CSharpFramework::Scene;

#include "Toolset2D_EnginePlugin/SpriteEntity.hpp"

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

		// overridden IEngineShapeInstance functions
		VOVERRIDE void SetVisible(bool bStatus) override {m_bIsVisible=bStatus;}
		VOVERRIDE void SetPosition(float x,float y,float z) override;
		VOVERRIDE void SetOrientation(float yaw,float pitch,float roll) override;
		VOVERRIDE void SetScaling(float /*x*/,float /*y*/, float /*z*/) override {}
		VOVERRIDE bool GetLocalBoundingBox(BoundingBox^ %bbox) override;
		VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult^ %result) override;
		VOVERRIDE bool OnExport(SceneExportInfo ^info) override;

		VOVERRIDE bool CanAttachComponent(ShapeComponent ^component, String ^%sError) override;
		VOVERRIDE void OnAttachComponent(ShapeComponent ^component) override;

		// special functions
		void RenderShape(VisionViewBase ^view, CSharpFramework::Shapes::ShapeRenderMode mode);

		void SetShoeBoxData(String ^pFileName, String ^pXml);

	private:
		bool m_bIsVisible;
		
		// pointer to native engine object
		Sprite *m_pSprite;
	};
}
