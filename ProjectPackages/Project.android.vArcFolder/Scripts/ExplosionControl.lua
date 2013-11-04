--[[
Author: Joel Van Eenwyk
Purpose: Any logic on the explosion effect goes here. This script
         is applied to every explosion.
--]]

function OnSpriteStateEnd(self)
	G.RemoveSpriteDeferred(self)
end
