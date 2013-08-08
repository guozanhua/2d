#ifndef HUD_HPP_INCLUDED
#define HUD_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>

class HUDGUIContext;
typedef VSmartPtr<HUDGUIContext> HUDGUIContextPtr;

///////////////////////////////////////////////////////////////////////////////////////
// class HUDGUIContext : This GUI context is used to render our HUD in the game
///////////////////////////////////////////////////////////////////////////////////////
class HUDGUIContext : public VGUIMainContext
{
public:
  // overridden IVGUIContext functions
  HUDGUIContext(VGUIManager *pManager) : VGUIMainContext(pManager) {}
  VOVERRIDE void GetMouseDelta(float &fDeltaX, float &fDeltaY) {fDeltaX=fDeltaY=0.f;}
  VOVERRIDE int GetButtonMask(VGUIUserInfo_t &user) {return 0;}
  VOVERRIDE void OnTickFunction(float fTimeDelta) {}

  VOVERRIDE void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState);
  VOVERRIDE void SetActivate(bool bStatus);

protected:
  void PrintTextCentered(IVRender2DInterface *pRI, const hkvVec2 &vCenter, const char *szText, VColorRef iColor, const VSimpleRenderState_t &iState, float fScaling);
  VisFontPtr m_spFont;
};


#endif


