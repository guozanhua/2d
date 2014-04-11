--[[
Author: Joel Van Eenwyk, Ryan Monday
Purpose: Controls the player
--]]

--== Event handlers

function OnExpose(self)
	self.MissileTexture = "Textures/missile.png"
	self.MissileVelocity = 600
	self.MissileFireTimer = 0.1
	self.MissileScale = 0.2
	self.MoveSpeed = 300
	self.RollSpeed = 5
	self.RollRecoverSpeed = 4
end

function OnAfterSceneLoaded(self)
	local kDeadzone = {deadzone = 0.1}
 
	self.playerInputMap = Input:CreateMap("InputMap")
 
	-- Setup the WASD keyboard playerInputMap
	self.playerInputMap:MapTrigger("Left", "KEYBOARD", "CT_KB_A")
	self.playerInputMap:MapTrigger("Right", "KEYBOARD", "CT_KB_D")
	self.playerInputMap:MapTrigger("Up", "KEYBOARD", "CT_KB_W")
	self.playerInputMap:MapTrigger("Down", "KEYBOARD", "CT_KB_S")
	self.playerInputMap:MapTrigger("Fire", "KEYBOARD", "CT_KB_SPACE")
	self.playerInputMap:MapTrigger("Zoom", "KEYBOARD", "CT_KB_Z")

	-- Create a virtual thumbstick then setup playerInputMap for it
	if Application:GetPlatformName() ~= "WIN32DX9" and
	   Application:GetPlatformName() ~= "WIN32DX11" then	
		Input:CreateVirtualThumbStick()
		self.playerInputMap:MapTriggerAxis("Left", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_LEFT", kDeadzone)
		self.playerInputMap:MapTriggerAxis("Right", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_RIGHT", kDeadzone)
		self.playerInputMap:MapTriggerAxis("Up", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_UP", kDeadzone)
		self.playerInputMap:MapTriggerAxis("Down", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_DOWN", kDeadzone)
		self.playerInputMap:MapTrigger(
			"Fire",
			{ (G.screenWidth * 0.5), (G.screenHeight * 0.5), G.screenWidth, G.screenHeight },
			"CT_TOUCH_ANY")
	end

	-- Calculate the starting position of the ship
	self:SetCenterPosition(
		Vision.hkvVec3(G.screenWidth * 0.5, 
		               G.screenHeight - self:GetHeight() - 10,
					   0) )
	
	self.roll = 0
	self.missileFireTimer = 0
	self.zoom = 0
end

function OnBeforeSceneUnloaded(self)
	Input:DestroyVirtualThumbStick()
	Input:DestroyMap(self.playerInputMap)
end

function IsTriggered(self, key)
	return (self.playerInputMap:GetTrigger(key) > 0)
end

function OnThink(self)
	local dt = Timer:GetTimeDiff()
	local moveSpeed = self.MoveSpeed * dt
	local invMoveSpeed = moveSpeed * -1
	local rollSpeed = self.RollSpeed * dt
	local rollRecoverSpeed = self.RollRecoverSpeed * dt

	local hasMovementY = false
	local hasMovementX = false
	
	if IsTriggered(self, "Up") then
		self:IncPosition(0, invMoveSpeed, 0)
		hasMovementY = true
	end

	if IsTriggered(self, "Down") then
		self:IncPosition(0, moveSpeed, 0)
		hasMovementY = true
	end
	
	if IsTriggered(self, "Left") then
		self:IncPosition(invMoveSpeed, 0, 0)
		self.roll = self.roll - rollSpeed
		hasMovementX = true
	end

	if IsTriggered(self, "Right") then
		self:IncPosition(moveSpeed, 0, 0)
		self.roll = self.roll + rollSpeed
		hasMovementX = true
	end
	
	self.roll = math.clamp(self.roll, -1, 1)
	
	-- Adjust the roll back to flat over time if we're not moving
	if not hasMovementX then
		local absoluteRoll = math.abs(self.roll)
		if absoluteRoll > 0 and (not hasMovementX) then
			local rollCorrection = (self.roll / absoluteRoll) * rollRecoverSpeed
			
			-- Make sure we don't over-correct as that would cause wobbling
			if math.abs(rollCorrection) > absoluteRoll then
				self.roll = 0
			else
				self.roll = self.roll - rollCorrection	
			end
		end
	end
	
	-- Set the current state by which direction we're going
	if self.roll < 0 then
		self:SetState("rollLeft")
	else
		self:SetState("rollRight")
	end
 
	self:SetFramePercent(math.abs(self.roll))
	
	if IsTriggered(self, "Fire") and (not IsTriggered(self, "Up")) then
		FireWeapon(self)
	end
	
	if IsTriggered(self, "Zoom") then
		self.zoom = self.zoom + dt
	else
		self.zoom = self.zoom - dt
	end
	
	if self.zoom > 1 then
		self.zoom = 1
	elseif self.zoom < 0 then
		self.zoom = 0
	end
		
	local camera = Toolset2D:GetCamera()
	if camera ~= nil then
		local z = 1.0 - (self.zoom * 0.3)
		camera:SetTransform( Vision.hkvVec4(z, z, 0.0, 0.0) )
	end
end

--== Utility functions

function FireWeapon(self)
	if self.missileFireTimer <= 0 then
		local layer = self:GetPosition().z + 1
		local offset1 = self:GetPoint(169, 97, layer)
		local offset2 = self:GetPoint(85, 97, layer)
		local missileVelocity = Vision.hkvVec3(0, -self.MissileVelocity, 0)

		local removeFunc = function(entity)
			return entity:GetPosition().y < -entity:GetHeight()
		end
		
		local missileLeft = Toolset2D:CreateSprite(offset1, self.MissileTexture)
		missileLeft:SetScaling(self.MissileScale)
		G.AddSprite(missileLeft, missileVelocity, removeFunc)
		
		local missileRight = Toolset2D:CreateSprite(offset2, self.MissileTexture)
		missileRight:SetScaling(self.MissileScale)
		G.AddSprite(missileRight, missileVelocity, removeFunc)
		
		self.missileFireTimer = self.MissileFireTimer
	else
		self.missileFireTimer = self.missileFireTimer - Timer:GetTimeDiff()
	end
end
