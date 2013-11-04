#include "Toolset2D_EnginePluginPCH.h"

#include "HUD.hpp"

void HUDGUIContext::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
	VSimpleRenderState_t state = VGUIManager::DefaultGUIRenderState(VIS_TRANSP_ALPHA);
	VRectanglef client = this->GetClientRect();
	
	hkvVec2 labelpos = client.GetCenter();
	labelpos.y = 40.f;
	PrintTextCentered(&Graphics.Renderer, labelpos, "2D Sprite Game", V_RGBA_WHITE, state, 1.0f);

	const char *szEndText;

#if defined(SUPPORTS_KEYBOARD)
	if (Vision::Editor.IsInEditor())
		szEndText = "Press ESC to return to editor";
	else
		szEndText = "Press ESC to return to windows";
#endif

#if defined(SUPPORTS_KEYBOARD)
	labelpos.y += 30.f;
	PrintTextCentered(&Graphics.Renderer, labelpos, szEndText, V_RGBA_WHITE, state, 0.8f);
#endif //SUPPORTS_KEYBOARD
}

void HUDGUIContext::PrintTextCentered(
	IVRender2DInterface *pRI,
	const hkvVec2 &vCenter,
	const char *szText,
	VColorRef iColor,
	const VSimpleRenderState_t &iState,
	float fScaling)
{
	VRectanglef textdim;
	m_spFont->GetTextDimension(szText,textdim);

	hkvVec2 pos = vCenter - (textdim.GetSize() * 0.5f * fScaling);

	m_spFont->PrintText(pRI, pos, szText, iColor, iState, fScaling);
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