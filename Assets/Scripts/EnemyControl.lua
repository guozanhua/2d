--[[
Author: Joel Van Eenwyk, Ryan Monday
Purpose: Controls the sprite
--]]

function OnSpriteCollision(self, sprite)
	-- Create an explosion effect if neither have been removed yet
	if (not G.IsSpriteRemoved(self)) and (not G.IsSpriteRemoved(sprite)) then
		local explosion = Game:CreateEntity(
			Vision.hkvVec3(0, 0, G.GetNumSprites() + 5),
			"Sprite",
			"",
			"Explosion")
		
		if Util:GetRandFloat() > 0.5 then
			explosion:UpdateProperty("TextureFilename", "Textures/SpriteSheets/Explosion_v1.png")
			explosion:UpdateProperty("XmlDataFilename", "Textures/SpriteSheets/Explosion_v1.xml")
		else
			explosion:UpdateProperty("TextureFilename", "Textures/SpriteSheets/Explosion_v2.png")
			explosion:UpdateProperty("XmlDataFilename", "Textures/SpriteSheets/Explosion_v2.xml")
		end
		
		explosion:SetScaling(Util:GetRandFloat(0.2) + 0.8)
		explosion:SetCenterPosition(sprite:GetCenterPosition())
		explosion:SetPlayOnce(true)
		explosion:SetCollision(false)

		explosion:AddComponentOfType("VScriptComponent", "ExplosionControlScript")
		explosion.ExplosionControlScript.OnSpriteStateEnd = function(self)
			self:Remove()
		end	
		explosion.ExplosionControlScript:SetOwner(explosion)

		G.AddSprite(explosion, Vision.hkvVec3(0, 0, 0), nil) 
	end

	G.RemoveSpriteDeferred(sprite)
	G.RemoveSpriteDeferred(self)
end
