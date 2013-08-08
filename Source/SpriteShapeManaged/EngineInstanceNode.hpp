
#pragma once

using namespace System;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace CSharpFramework::Scene;

#include "SpriteShapeEnginePlugin/VNode.hpp"

namespace NodeManaged
{
  /// <summary>
  /// EngineInstanceNode : class that manages the native class
  /// </summary>
  public ref class EngineInstanceNode : public IEngineShapeInstance
  {
  public:     

    EngineInstanceNode();
    VOVERRIDE void DisposeObject() override;
    VOVERRIDE IntPtr GetNativeObject() override {return System::IntPtr((void *)m_pNode);}

    // overridden IEngineShapeInstance functions
    VOVERRIDE void SetVisible(bool bStatus) override {m_bIsVisible=bStatus;}
    VOVERRIDE void SetPosition(float x,float y,float z) override;
    VOVERRIDE void SetOrientation(float yaw,float pitch,float roll) override;
    VOVERRIDE void SetScaling(float /*x*/,float /*y*/, float /*z*/) override {}
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox^ %bbox) override;
    VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult^ %result) override;
    VOVERRIDE bool OnExport(SceneExportInfo ^info) override;

    // special functions
    void RenderShape(VisionViewBase ^view, CSharpFramework::Shapes::ShapeRenderMode mode);

    void LinkNode(EngineInstanceNode ^pOther);
    void UnLinkNode(EngineInstanceNode ^pOther);
  private:
    bool m_bIsVisible;
    VNode_cl *m_pNode; // pointer to native engine object
  };
}
