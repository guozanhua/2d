--[[
Author: Joel Van Eenwyk, Ryan Monday
Purpose: Controls the sprite
--]]

function OnSpriteCollision(self, sprite)
	if self:GetKey() == "Enemy" and sprite:GetKey() == "Enemy" then
		return
	end

	-- Create an explosion effect if neither have been removed yet
	if (not G.IsSpriteRemoved(self)) and (not G.IsSpriteRemoved(sprite)) then
		local explosion = Game:CreateEntity(
			Vision.hkvVec3(0, 0, 0),
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
		
		explosion:SetScaling(Util:GetRandFloat(0.4) + 0.6)
		
		local position = sprite:GetCenterPosition()
		position.z = self:GetPosition().z + 200
		
		explosion:SetCenterPosition(position)
		explosion:SetPlayOnce(true)
		explosion:SetCollision(false)

		explosion:AddComponentOfType("VScriptComponent", "ExplosionControlScript")
		explosion.ExplosionControlScript:SetProperty("ScriptFile", "Scripts/ExplosionControl.lua")
		explosion.ExplosionControlScript:SetOwner(explosion)

		G.AddSprite(explosion, Vision.hkvVec3(0, 0, 0), nil) 
	end

	G.RemoveSpriteDeferred(sprite)
	G.RemoveSpriteDeferred(self)
end

function OnThink(self)
	-- Constants based off delta time
	local kTimeDifference = Timer:GetTimeDiff()
	
	-- Move the enemy ships around
end
