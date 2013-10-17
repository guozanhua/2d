--[[
Author: Joel Van Eenwyk, Ryan Monday
Purpose: Controls the sprite
--]]

function OnSpriteStateEnd(self)
	G.RemoveSpriteDeferred(self)
end
