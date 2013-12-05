--[[
Author: Joel Van Eenwyk, Ryan Monday
Purpose: Controls the player
--]]

-- Depth (Z-order) of the missile sprites
kMissileLayer = 7

-- Scale the missile sprite down
kMissileScale = 0.2

-- Add a delay (in milliseconds) to when you can fire
kMissileFireTimer = 0.1

-- Velocity of the missile
kMissileVelocity = Vision.hkvVec3(0, -600, 0)

kMissileTexture = "Textures/missile.png"

kMoveSpeed = 300
kRollSpeed = 5
kRollRecoverSpeed = 4

function FireWeapon(self)
	if self.missileFireTimer <= 0 then
		local default = Vision.hkvVec3(0, 0, 0)		
		local offset1 = self:GetPoint(215, 106, kMissileLayer)
		local offset2 = self:GetPoint(40, 106, kMissileLayer)
		
		local removeFunc = function(entity)
			return entity:GetPosition().y < -entity:GetHeight()
		end
		
		local missileLeft = Toolset2D:CreateSprite(default, kMissileTexture)
		missileLeft:SetScaling(kMissileScale)
		missileLeft:SetCenterPosition(offset1)
		G.AddSprite(missileLeft, kMissileVelocity, removeFunc)
		
		local missileRight = Toolset2D:CreateSprite(default, kMissileTexture)
		missileRight:SetScaling(kMissileScale)
		missileRight:SetCenterPosition(offset2)	
		G.AddSprite(missileRight, kMissileVelocity, removeFunc)
		
		self.missileFireTimer = kMissileFireTimer
	else
		self.missileFireTimer = self.missileFireTimer - Timer:GetTimeDiff()
	end
end

function OnAfterSceneLoaded(self)
	local kDeadzone = {deadzone = 0.1}
 
	self.playerInputMap = Input:CreateMap("InputMap")
 
	-- Setup the WASD keyboard playerInputMap
	self.playerInputMap:MapTrigger("KeyLeft", "KEYBOARD", "CT_KB_A")
	self.playerInputMap:MapTrigger("KeyRight", "KEYBOARD", "CT_KB_D")
	self.playerInputMap:MapTrigger("KeyUp", "KEYBOARD", "CT_KB_W")
	self.playerInputMap:MapTrigger("KeyDown", "KEYBOARD", "CT_KB_S")
	self.playerInputMap:MapTrigger("KeyFire", "KEYBOARD", "CT_KB_SPACE")

	-- Create a virtual thumbstick then setup playerInputMap for it
	if Application:GetPlatformName() ~= "WIN32DX9" and
	   Application:GetPlatformName() ~= "WIN32DX11" then	
		Input:CreateVirtualThumbStick()
		self.playerInputMap:MapTriggerAxis("TouchLeft", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_LEFT", kDeadzone)
		self.playerInputMap:MapTriggerAxis("TouchRight", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_RIGHT", kDeadzone)
		self.playerInputMap:MapTriggerAxis("TouchUp", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_UP", kDeadzone)
		self.playerInputMap:MapTriggerAxis("TouchDown", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_DOWN", kDeadzone)
		self.playerInputMap:MapTrigger(
			"TouchFire",
			{ (G.screenWidth * 0.5), (G.screenHeight * 0.5), G.screenWidth, G.screenHeight },
			"CT_TOUCH_ANY")
	end

	-- Calculate the starting position of the ship
	self:SetCenterPosition(
		Vision.hkvVec3(G.screenWidth * 0.5, G.screenHeight - self:GetHeight() - 10, 0) )
	
	self.roll = 0
	self.missileFireTimer = 0
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
	local moveSpeed = kMoveSpeed * dt
	local invMoveSpeed = moveSpeed * -1
	local rollSpeed = kRollSpeed * dt
	local rollRecoverSpeed = kRollRecoverSpeed * dt

	local hasMovementY = false
	local hasMovementX = false
	
	if IsTriggered(self, "KeyUp") or IsTriggered(self, "TouchUp") then
		self:IncPosition(0, invMoveSpeed, 0)
		hasMovementY = true
	end

	if IsTriggered(self, "KeyDown") or IsTriggered(self, "TouchDown") then
		self:IncPosition(0, moveSpeed, 0)
		hasMovementY = true
	end
	
	if IsTriggered(self, "KeyLeft") or IsTriggered(self, "TouchLeft") then
		self:IncPosition(invMoveSpeed, 0, 0)
		self.roll = self.roll - rollSpeed
		hasMovementX = true
	end

	if IsTriggered(self, "KeyRight") or IsTriggered(self, "TouchRight") then
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
	
	if (IsTriggered(self, "KeyFire") or IsTriggered(self, "TouchFire")) and
	   not IsTriggered(self, "TouchUp") then
		FireWeapon(self)
	end
end
