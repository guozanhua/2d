#include "SpriteGamePCH.h"

#include <HUD.hpp>
#include <SpriteGameApplication.hpp>

// overridden OnPaint function. Rather than rendering all open dialogs, we do all the rendering on our own.
// Alternatively, all HUD controls (health status bar etc.) could be implemented as dialog controls
void HUDGUIContext::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  //if (!SpriteGameManager::GlobalManager().GetPlayer())
  //  return;
  
  VSimpleRenderState_t state = VGUIManager::DefaultGUIRenderState(VIS_TRANSP_ALPHA);
  VRectanglef client = Graphics.GUIContext.GetClientRect();

  // render health bar
  float fHealth = 0.2f;
  if (fHealth<0.f) 
    fHealth=0.f;
  VColorRef iColor = V_RGBA_GREEN;
  if (fHealth<30.f)
    iColor = V_RGBA_RED;
  else if (fHealth<60.f)
    iColor = V_RGBA_YELLOW;
  VRectanglef healthbar(10.f,30.f,210.f,50.f);
  hkvVec2 border(2.f,2.f);
  hkvVec2 barsize(healthbar.GetSizeX()*fHealth*0.01f,healthbar.GetSizeY());

  Graphics.Renderer.DrawSolidQuad(healthbar.m_vMin-border,healthbar.m_vMax+border, V_RGBA_BLACK, state);
  Graphics.Renderer.DrawSolidQuad(healthbar.m_vMin,healthbar.m_vMin+barsize, iColor, state);

  hkvVec2 labelpos = healthbar.m_vMin+hkvVec2(5.f,-26.f);
  m_spFont->PrintText(&Graphics.Renderer,labelpos,"Health :",V_RGBA_WHITE,state,1.f);

  // display remaining item counts
  int iCount = SpriteGameManager::GlobalManager().GetRemainingItems();
  char szBuffer[64];
  sprintf(szBuffer,"%i",iCount);
  labelpos.x = client.m_vMax.x-150.f;
  labelpos.y = 30.f;
  m_spFont->PrintText(&Graphics.Renderer,labelpos,"Remaining :",V_RGBA_WHITE,state,1.f);
  labelpos.x += 110.f;
  m_spFont->PrintText(&Graphics.Renderer,labelpos,szBuffer,V_RGBA_WHITE,state,1.f);

  // GAME OVER ?
  const char *szEndText;
#if defined (_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
  szEndText = "Press Y to return to scene viewer dialog";
#elif defined(_VISION_PS3)
  szEndText = "Press TRIANGLE to return to scene viewer dialog";
#elif defined(SUPPORTS_KEYBOARD)
  if (Vision::Editor.IsInEditor())
    szEndText = "Press ESC to return to editor";
  else
    szEndText = "Press ESC to return to windows";
#endif

  //if (playerData.IsDead())
  if (0)
  {
    hkvVec2 vPos = client.GetCenter();
    vPos.y-=40.f;
    PrintTextCentered(&Graphics.Renderer,vPos,"Game Over",V_RGBA_WHITE,state,1.f);
#if defined(SUPPORTS_KEYBOARD)
    vPos.y+=40.f;
    PrintTextCentered(&Graphics.Renderer,vPos,szEndText,V_RGBA_WHITE,state,1.f);
#endif //SUPPORTS_KEYBOARD
  } 
  else if (iCount==0)
  {
    hkvVec2 vPos = client.GetCenter();
    vPos.y-=40.f;
    PrintTextCentered(&Graphics.Renderer,vPos,"Congratulations - all items found",V_RGBA_WHITE,state,1.f);
#if defined(SUPPORTS_KEYBOARD)
    vPos.y+=40.f;
    PrintTextCentered(&Graphics.Renderer,vPos,szEndText,V_RGBA_WHITE,state,1.f);
#endif //SUPPORTS_KEYBOARD
  }
}


void HUDGUIContext::PrintTextCentered(IVRender2DInterface *pRI, const hkvVec2 &vCenter, const char *szText, VColorRef iColor, const VSimpleRenderState_t &iState, float fScaling)
{
  VRectanglef textdim;
  m_spFont->GetTextDimension(szText,textdim);
  hkvVec2 pos = vCenter - textdim.GetSize()*0.5f*fScaling;
  m_spFont->PrintText(pRI,pos,szText,iColor,iState,fScaling);
}


void HUDGUIContext::SetActivate(bool bStatus)
{
  VGUIMainContext::SetActivate(bStatus);
  if (bStatus)
  {
    m_spFont = GetManager()->LoadFont("Fonts\\Arial_22.fnt");
    VASSERT(m_spFont);
  } else
  {
    m_spFont = NULL;
  }

}

