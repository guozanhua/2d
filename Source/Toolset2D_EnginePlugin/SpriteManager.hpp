#ifndef SPRITE_MANAGER_HPP_INCLUDED
#define SPRITE_MANAGER_HPP_INCLUDED

class Sprite;

/// \brief Returns true if the given path is relative to one of the asset libraries (a.k.a. data directories).
TOOLSET_2D_IMPEXP bool convertToAssetPath(const char* absolutePath, hkStringBuf& out_relativePath);

class SpriteManager : public IVisCallbackHandler_cl
{
public:
	VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData);

	// Called when plugin is loaded/unloaded
	TOOLSET_2D_IMPEXP void OneTimeInit();
	TOOLSET_2D_IMPEXP void OneTimeDeInit();
	
	TOOLSET_2D_IMPEXP void AddSprite(Sprite *sprite);
	TOOLSET_2D_IMPEXP void RemoveSprite(Sprite *sprite);

	TOOLSET_2D_IMPEXP void Render();

	// Access one global instance of the frame manager
	static SpriteManager& GlobalManager()
	{
		static SpriteManager spriteManager;
		return spriteManager;
	}

	// Register our LUA library with the script manager
	static void RegisterLua();

private:
	VArray<Sprite*> m_sprites;
};

#endif // SPRITE_MANAGER_HPP_INCLUDED