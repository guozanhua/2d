--[[
Author: Joel Van Eenwyk, Ryan Monday
Purpose: Controls the enemy and does other management
--]]

Debug:Enable(true)
Debug:SetupLines(20, 1)

function AddSprite(sprite, velocity, removeFunc)
	local spriteEntry = {}

	spriteEntry.entity = sprite
	spriteEntry.removeFunc = removeFunc
	spriteEntry.remove = false
	spriteEntry.velocity = velocity
	
	table.insert(G.sprites, spriteEntry)
end

function RemoveSpriteDeferred(sprite)
	for _, spriteEntry in ipairs(G.sprites) do		
		if spriteEntry.entity == sprite then
			spriteEntry.remove = true
			break
		end
	end
end

function OnAfterSceneLoaded(self) 
	self.enemy = Game:GetEntity("Enemy")
	self.enemy:SetVisible(false)
	self.time = 0
	self.enemies = {}
	
	G.sprites = {}
	G.AddSprite = AddSprite
	G.RemoveSpriteDeferred = RemoveSpriteDeferred
end

function UpdateSprites(self)
	local dt = Timer:GetTimeDiff()
	local toDelete = {}

	for _, sprite in ipairs(G.sprites) do		
		if sprite.remove or sprite.removeFunc(sprite.entity) then
			table.insert(toDelete, sprite)
		else
			local position = sprite.entity:GetPosition()
			position.y = position.y + (dt * sprite.velocity)
			sprite.entity:SetPosition(position)
		end
	end

	for _, spriteToDelete in ipairs(toDelete) do
		for index, sprite in ipairs(G.sprites) do
			if sprite == spriteToDelete then
				sprite.entity:Remove()
				table.remove(G.sprites, index)
				break
			end
		end
	end
end

function OnThink(self)
	local kTimeDifference = Timer:GetTimeDiff()
	self.time = self.time + kTimeDifference
	
	if self.time > 0.8 then
		local enemy = Game:CreateEntity(Vision.hkvVec3(10,23,0), "Sprite", "", "Enemy")
		local default = Vision.hkvVec3(Util:GetRandInt(500), 24, 0)
		
		enemy:SetScaling(0.3)
		enemy:SetCenterPosition(default)
		enemy:SetOrientation(0, 0, 180)
		enemy:UpdateProperty("TextureFilename", "Textures/SpriteSheets/EnemyShip.png")	
		enemy:UpdateProperty("XmlDataFilename", "Textures/SpriteSheets/EnemyShip.xml")
		
		enemy:AddComponentOfType("VScriptComponent", "EnemyControlScript")
		enemy.EnemyControlScript:SetProperty("ScriptFile", "Scripts/EnemyControl.lua")
		enemy.EnemyControlScript:SetOwner(enemy)
		
		local removeFunc = function(entity)
			return entity:GetPosition().y > 500
		end

		G.AddSprite(enemy, 100, removeFunc)
		
		self.time = 0
	end
	
	UpdateSprites(self)
end
