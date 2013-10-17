--[[
Author: Joel Van Eenwyk, Ryan Monday
Purpose: Controls the enemy and does other management
--]]

kEnemyScale = 0.6
kEnemySpawnTimeRangeStart = 0.7
kEnemySpawnTimeRangeEnd = 1.2

--[[ Below is used for game script --]]

function AddSprite(sprite, velocity, removeFunc)
	local spriteEntry = {}

	spriteEntry.entity = sprite
	spriteEntry.removeFunc = removeFunc
	spriteEntry.remove = false
	spriteEntry.velocity = velocity
	
	table.insert(G.sprites, spriteEntry)
end

function GetNumSprites()
	return table.getn(G.sprites)
end

function RemoveSpriteDeferred(sprite)
	for _, spriteEntry in ipairs(G.sprites) do		
		if spriteEntry.entity == sprite then
			spriteEntry.remove = true
			break
		end
	end
end

function RemoveAllSprites()
	for _, spriteEntry in ipairs(G.sprites) do
		spriteEntry.entity:Remove()
	end
	G.sprites = {}
end

function IsSpriteRemoved(sprite)
	local removed = true
	for _, spriteEntry in ipairs(G.sprites) do		
		if spriteEntry.entity == sprite then
			removed = spriteEntry.remove
			break
		end
	end
	return removed
end

function UpdateSprites()
	local dt = Timer:GetTimeDiff()
	local toDelete = {}

	for _, sprite in ipairs(G.sprites) do		
		if sprite.remove or
		   (sprite.removeFunc ~= nil and sprite.removeFunc(sprite.entity)) then
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

function InitializeScene()
	Debug:Enable(true)
	Debug:SetupLines(20, 1)

	G.screenWidth, G.screenHeight = Screen:GetViewportSize()
	Debug:PrintLine("Width: " .. G.screenWidth .. ", Height: " .. G.screenHeight)
	
	G.sprites = {}
	G.AddSprite = AddSprite
	G.RemoveSpriteDeferred = RemoveSpriteDeferred
	G.IsSpriteRemoved = IsSpriteRemoved
	G.GetNumSprites = GetNumSprites
	G.RemoveAllSprites = RemoveAllSprites

	math.clamp = function(n, low, high)
		return math.min(math.max(n, low), high)
	end
end

--[[ Below is used for the entity update --]]

function UpdateSpawnTimer(self)
	self.enemySpawnTimer = Util:GetRandFloat() * (kEnemySpawnTimeRangeEnd - kEnemySpawnTimeRangeStart)
	self.enemySpawnTimer = self.enemySpawnTimer + kEnemySpawnTimeRangeStart
end

function OnAfterSceneLoaded(self)
	if self == nil then
		InitializeScene()
	else
		UpdateSpawnTimer(self)
	end
end

function OnBeforeSceneUnloaded()
	G.RemoveAllSprites()	
end

function OnThink(self)
	local kTimeDifference = Timer:GetTimeDiff()
	
	if self.enemySpawnTimer <= 0 then
		local enemy = Game:CreateEntity(
			Vision.hkvVec3(0, 0, 0),
			"Sprite",
			"",
			"Enemy")
		
		enemy:SetScaling(kEnemyScale)
		enemy:SetOrientation(0, 0, 180)

		enemy:UpdateProperty("TextureFilename", "Textures/SpriteSheets/EnemyShipV2.png")	
		enemy:UpdateProperty("XmlDataFilename", "Textures/SpriteSheets/EnemyShipV2.xml")
		
		local default = Vision.hkvVec3(
			enemy:GetWidth() / 2.0 + Util:GetRandInt(G.screenWidth - enemy:GetWidth()),
			-enemy:GetHeight(),
			G.GetNumSprites() + 10)
		enemy:SetCenterPosition(default)
		
		enemy:AddComponentOfType("VScriptComponent", "EnemyControlScript")
		enemy.EnemyControlScript:SetProperty("ScriptFile", "Scripts/EnemyControl.lua")
		enemy.EnemyControlScript:SetOwner(enemy)
		
		local removeFunc = function(entity)
			return entity:GetPosition().y > (G.screenHeight + entity:GetHeight())
		end

		local speed = 100.0f + Util:GetRandFloat(200.0f)
		G.AddSprite(enemy, Vision.hkvVec3(0, speed, 0), removeFunc)
		
		UpdateSpawnTimer(self)
	else	
		self.enemySpawnTimer = self.enemySpawnTimer - kTimeDifference
	end
	
	UpdateSprites()
end
