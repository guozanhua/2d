--[[
Author: Joel Van Eenwyk, Ryan Monday
Purpose: Controls the sprite
--]]

-- Depth (Z-order) of the missile sprites
kMissileLayer = 7

-- Scale the missile sprite down
kMissileScale = 0.2

-- Add a delay (in milliseconds) to when you can fire
kMissileFireTimer = 0.8

-- Velocity of the missile
kMissileVelocity = Vision.hkvVec3(0, 600, 0)

kMissileTexture = "Textures/missile_evil.png"

kRollSpeed = 5
kRollRecoverSpeed = 4
kDecisionTime = 1
kDecisionTimeMin = 0.8
kSpeedMin = 5
kSpeedMax = 10

function OnSpriteCollision(self, sprite)
	if self:GetKey() == "Enemy" and sprite:GetKey() == "Enemy" then
		return
	end

	-- Create an explosion effect if neither have been removed yet
	if (not G.IsSpriteRemoved(self)) and (not G.IsSpriteRemoved(sprite)) then
		local texture = "Textures/SpriteSheets/Explosion_v1.png"
		local data = "Textures/SpriteSheets/Explosion_v1.xml"
		
		if Util:GetRandFloat() > 0.5 then
			texture = "Textures/SpriteSheets/Explosion_v2.png"
			data = "Textures/SpriteSheets/Explosion_v2.xml"
		end
		
		local position = sprite:GetCenterPosition()
		position.z = self:GetPosition().z + 200

		local explosion = Toolset2D:CreateSprite( position, texture, data )
				
		explosion:SetScaling(Util:GetRandFloat(0.4) + 0.6)
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

function FireWeapon(self)
	if self.missileFireTimer <= 0 then
		local default = Vision.hkvVec3(0, 0, 0)		
		local offset1 = self:GetPoint(191, 100, kMissileLayer)
		local offset2 = self:GetPoint(64, 100, kMissileLayer)
		
		local removeFunc = function(entity)
			return entity:GetPosition().y < -entity:GetHeight()
		end
		
		local missileLeft = Toolset2D:CreateSprite(default, kMissileTexture)
		missileLeft:SetScaling(kMissileScale)
		missileLeft:SetCenterPosition(offset1)
		missileLeft:SetCollision(false)
		G.AddSprite(missileLeft, kMissileVelocity, removeFunc)
		
		local missileRight = Toolset2D:CreateSprite(default, kMissileTexture)
		missileRight:SetScaling(kMissileScale)
		missileRight:SetCenterPosition(offset2)	
		missileRight:SetCollision(false)
		G.AddSprite(missileRight, kMissileVelocity, removeFunc)
		
		self.missileFireTimer = kMissileFireTimer
	else
		self.missileFireTimer = self.missileFireTimer - Timer:GetTimeDiff()
	end
end

function OnThink(self)
	local dt = Timer:GetTimeDiff()
	
	local rollSpeed = kRollSpeed * dt
	local rollRecoverSpeed = kRollRecoverSpeed * dt
	
	if self.decisionTime < 0 then
		if Util:GetRandFloat() < 0.5 then
			self.direction = 0
		else
			self.direction = Util:GetRandFloat(2) - 1.0f
			self.speed = kSpeedMin + Util:GetRandFloat(kSpeedMax - kSpeedMin)
		end

		self.decisionTime = kDecisionTimeMin + Util:GetRandFloat() * kDecisionTime
	else
		self.decisionTime = self.decisionTime - dt
	end
	
	local x = self:GetPosition().x
	if x < self:GetWidth() or x > G.screenWidth - self:GetWidth() then
		self.direction = 0
	end

	local absoluteRoll = math.abs(self.roll)
	
	if math.abs(self.direction) > 0 then
		local d = self.direction / math.abs(self.direction)
		self:IncPosition(-d * self.speed, 0, 0)
		self.roll = self.roll + d * rollSpeed
	elseif absoluteRoll > 0 then
		local rollCorrection = (self.roll / absoluteRoll) * rollRecoverSpeed
		
		-- Make sure we don't over-correct as that would cause wobbling
		if math.abs(rollCorrection) > absoluteRoll then
			self.roll = 0
		else
			self.roll = self.roll - rollCorrection	
		end
	end

	self.roll = math.clamp(self.roll, -1, 1)

	if self.roll < 0 then
		self:SetState("rollLeft")
	else
		self:SetState("rollRight")
	end
		
	self:SetFramePercent(math.abs(self.roll))
	
	FireWeapon(self)
end

function OnCreate(self)	
	self.roll = 0
	self.velocity = 0
	self.decisionTime = kDecisionTimeMin + Util:GetRandFloat() * kDecisionTime
	self.direction = 0
	self.missileFireTimer = 0
end
