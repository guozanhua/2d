--[[
Author: Joel Van Eenwyk, Ryan Monday
Purpose: Controls the sprite
--]]

function OnSpriteCollision(self, sprite)
	Debug:PrintLine("Collision2!")
	self:Remove()
end
