#ifndef SPRITE_MANAGER_HPP_INCLUDED
#define SPRITE_MANAGER_HPP_INCLUDED

class Sprite;

class SpriteManager : public IVisCallbackHandler_cl
{
public:
	SpriteManager();

	VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  static void Ensure2dToolsetLuaRegistration();

	// called when plugin is loaded/unloaded
	TOOLSET_2D_IMPEXP void OneTimeInit();
	TOOLSET_2D_IMPEXP void OneTimeDeInit();

	TOOLSET_2D_IMPEXP void AddSprite(Sprite *sprite);
	TOOLSET_2D_IMPEXP void RemoveSprite(Sprite *sprite);

	TOOLSET_2D_IMPEXP void Render();

	// access one global instance of the frame manager
	static SpriteManager& GlobalManager() { return g_SpriteManager; }

private:
	VArray<Sprite*> m_sprites;

	static SpriteManager g_SpriteManager;
};

#endif // SPRITE_MANAGER_HPP_INCLUDED