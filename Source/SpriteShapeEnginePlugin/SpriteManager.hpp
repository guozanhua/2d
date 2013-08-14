#ifndef SpriteGameAPPLICAION_HPP_INCLUDED
#define SpriteGameAPPLICAION_HPP_INCLUDED

#include "SpriteEntity.hpp"

class SpriteManager : public IVisCallbackHandler_cl
{
public:
	VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData);

	// called when plugin is loaded/unloaded
	SPRITE_IMPEXP void OneTimeInit();
	SPRITE_IMPEXP void OneTimeDeInit();

	// switch to play-the-game mode. When not in vForge, this is default
	void SetPlayTheGame(bool bStatus);

	void Render();

	// access one global instance of the frame manager
	static SpriteManager& GlobalManager() {return g_SpriteManager;}

private:
	bool m_bPlayingTheGame;

	static SpriteManager g_SpriteManager;
};

#endif