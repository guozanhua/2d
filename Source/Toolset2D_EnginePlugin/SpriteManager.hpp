#ifndef SpriteGameAPPLICAION_HPP_INCLUDED
#define SpriteGameAPPLICAION_HPP_INCLUDED

class Sprite;

class SpriteManager : public IVisCallbackHandler_cl
{
public:
	VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData);

	// called when plugin is loaded/unloaded
	SPRITE_IMPEXP void OneTimeInit();
	SPRITE_IMPEXP void OneTimeDeInit();

	void AddSprite(Sprite *sprite);
	void RemoveSprite(Sprite *sprite);

	void Render();

	// access one global instance of the frame manager
	static SpriteManager& GlobalManager() {return g_SpriteManager;}

private:
	VArray<Sprite*> m_sprites;

	static SpriteManager g_SpriteManager;
};

#endif