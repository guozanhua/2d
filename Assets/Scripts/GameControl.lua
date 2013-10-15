--[[
Author: Joel Van Eenwyk, Ryan Monday
Purpose: Controls the enemy and does other management
--]]

kEnemyScale = 0.4

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
	Debug:Enable(true)
	Debug:SetupLines(20, 1)

	G.screenWidth, G.screenHeight = Screen:GetViewportSize()
	Debug:PrintLine("Width: " .. G.screenWidth .. ", Height: " .. G.screenHeight)
	
	G.sprites = {}
	G.AddSprite = AddSprite
	G.RemoveSpriteDeferred = RemoveSpriteDeferred

	math.clamp = function(n, low, high)
		return math.min(math.max(n, low), high)
	end

	-- Hide the global enemy entity
	self.enemy = Game:GetEntity("Enemy")
	self.enemy:SetVisible(false)
	self.enemySpawnTimer = 0
end

function UpdateSprites(self)
	local dt = Timer:GetTimeDiff()
	local toDelete = {}

	for _, sprite in ipairs(G.sprites) do		
		if sprite.remove or sprite.removeFunc(sprite.entity) then
			table.insert(toDelete, sprite)
		else
			local position = sprite.entity:GetPosition() + (sprite.velocity * dt)
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
	self.enemySpawnTimer = self.enemySpawnTimer + kTimeDifference
	
	if self.enemySpawnTimer > 0.8 then
		local enemy = Game:CreateEntity(Vision.hkvVec3(10, 23, 0), "Sprite", "", "Enemy")
		
		enemy:SetScaling(kEnemyScale)
		enemy:SetOrientation(0, 0, 180)

		local default = Vision.hkvVec3(
			enemy:GetWidth() + Util:GetRandInt(G.screenWidth - enemy:GetWidth() * 2),
			-enemy:GetHeight(),
			0)
		enemy:SetCenterPosition(default)

		enemy:UpdateProperty("TextureFilename", "Textures/SpriteSheets/EnemyShip.png")	
		enemy:UpdateProperty("XmlDataFilename", "Textures/SpriteSheets/EnemyShip.xml")
		
		enemy:AddComponentOfType("VScriptComponent", "EnemyControlScript")
		enemy.EnemyControlScript:SetProperty("ScriptFile", "Scripts/EnemyControl.lua")
		enemy.EnemyControlScript:SetOwner(enemy)
		
		local removeFunc = function(entity)
			return entity:GetPosition().y > (G.screenHeight + entity:GetHeight())
		end

		G.AddSprite(enemy, Vision.hkvVec3(0, 100, 0), removeFunc)
		
		self.enemySpawnTimer = 0
	end
	
	UpdateSprites(self)
end
