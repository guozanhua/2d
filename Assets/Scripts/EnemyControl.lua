--[[
Author: Joel Van Eenwyk, Ryan Monday
Purpose: Controls the sprite
--]]

function OnSpriteCollision(self, sprite)
	G.RemoveSpriteDeferred(sprite)
	G.RemoveSpriteDeferred(self)
end
