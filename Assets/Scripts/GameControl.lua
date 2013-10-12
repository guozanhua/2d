--[[
Author: Joel Van Eenwyk, Ryan Monday
Purpose: Controls the enemy and does other management
--]]

Debug:Enable(true)
Debug:SetupLines(20, 1)

function OnAfterSceneLoaded(self) 
	self.enemy = Game:GetEntity("Enemy")
	self.enemy:SetVisible(false)
	self.time = 0
	self.enemies = {}
end

function UpdateEnemies(self)
	local dt = Timer:GetTimeDiff()
	local toDelete = {}

	for key, enemy in ipairs(self.enemies) do
		local pos =	enemy:GetPosition()
		pos.y = pos.y + (dt * 100)

		if pos.y >500 then
			table.insert(toDelete, enemy)
		else
			enemy:SetPosition(pos)
		end
	end

	for _, enemyToDelete in ipairs(toDelete) do
		for index, enemy in ipairs(self.enemies) do
			if enemy == enemyToDelete then
				enemy:Remove()
				table.remove(self.enemies, index)
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
		local default = Vision.hkvVec3(156, 24, 0)
		
		enemy:SetScaling(0.3)
		enemy:SetCenterPosition(default)
		enemy:SetOrientation(0, 0, 180)
		enemy:UpdateProperty("TextureFilename", "Textures/SpriteSheets/EnemyShip.png")	
		enemy:UpdateProperty("XmlDataFilename", "Textures/SpriteSheets/EnemyShip.xml")
		enemy:AddComponentOfType("VScriptComponent", "Script")
		enemy.Script:UpdateProperty("ScriptFile", "Scripts\EnemyControl.lua")
		table.insert(self.enemies, enemy)
		
		self.time = 0
	end
	
	UpdateEnemies(self)
end
