#ifndef SpriteGameAPPLICAION_HPP_INCLUDED
#define SpriteGameAPPLICAION_HPP_INCLUDED

#include <HUD.hpp>

#include "SpriteEntity.hpp"

#ifdef WIN32
class IVRemoteInput;
#endif

class SpriteGameManager : public IVisCallbackHandler_cl
{
public:
	VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData);

	// called when plugin is loaded/unloaded
	SPRITE_GAME_IMPEXP void OneTimeInit();
	SPRITE_GAME_IMPEXP void OneTimeDeInit();

	// switch to play-the-game mode. When not in vForge, this is default
	SPRITE_GAME_IMPEXP void SetPlayTheGame(bool bStatus);

	SPRITE_GAME_IMPEXP void Render();

	// access one global instance of the frame manager
	static SpriteGameManager& GlobalManager() {return g_GameManager;}

private:
	bool m_bPlayingTheGame;
	HUDGUIContextPtr m_spHUD;

	Sprite *m_pPlayerSprite;

#ifdef WIN32
	IVRemoteInput *m_pRemoteInput;
#endif

	static SpriteGameManager g_GameManager;
};

#endif