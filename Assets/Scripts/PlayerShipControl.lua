--[[
Author: Joel Van Eenwyk, Ryan Monday
Purpose: Controls the player
--]]

-- Depth (Z-order) of the missile sprites
kMissileLayer = 7

-- Scale the missile sprite down
kMissileScale = 0.2

-- Add a delay (in milliseconds) to when you can fire
kMissilemissileFireTimer = 0.1

-- Velocity of the missile
kMissileVelocity = Vision.hkvVec3(0, -600, 0)

kMissileTexture = "Textures/missile.png"

function FireWeapon(self)
	if self.missileFireTimer <= 0 then
		local default = Vision.hkvVec3(0, 0, 0)
		local layer = 7
		local scale = 0.2
		
		local offset1 = self:GetPoint(171, 97, kMissileLayer)
		local offset2 = self:GetPoint(84, 97, kMissileLayer)
		local velocity = Vision.hkvVec3(0, -600, 0)
		
		local removeFunc = function(entity)
			return entity:GetPosition().y < -entity:GetHeight()
		end
		
		local missileLeft = Game:CreateEntity(default, "Sprite", "", "Missile")
		missileLeft:UpdateProperty("TextureFilename", kMissileTexture)
		missileLeft:SetScaling(scale)
		missileLeft:SetCenterPosition(offset1)
		G.AddSprite(missileLeft, kMissileVelocity, removeFunc)
		
		local missileRight = Game:CreateEntity(default, "Sprite", "", "Missile")
		missileRight:SetScaling(scale)
		missileRight:SetCenterPosition(offset2)	
		missileRight:UpdateProperty("TextureFilename", kMissileTexture)
		G.AddSprite(missileRight, kMissileVelocity, removeFunc)
		
		self.missileFireTimer = kMissilemissileFireTimer
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
	self.playerInputMap:MapTrigger("keyFire", "KEYBOARD", "CT_KB_SPACE")

	-- Create a virtual thumbstick then setup playerInputMap for it
	Input:CreateVirtualThumbStick()
	self.playerInputMap:MapTriggerAxis("TouchLeft", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_LEFT", kDeadzone)
	self.playerInputMap:MapTriggerAxis("TouchRight", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_RIGHT", kDeadzone)
	self.playerInputMap:MapTriggerAxis("TouchUp", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_UP", kDeadzone)
	self.playerInputMap:MapTriggerAxis("TouchDown", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_DOWN", kDeadzone)
	self.playerInputMap:MapTrigger(
		"touchFire",
		{ (G.screenWidth * 0.5), (G.screenHeight * 0.5), G.screenWidth, G.screenHeight },
		"CT_TOUCH_ANY")

	-- Calculate the starting position of the ship
	self.x = G.screenWidth * 0.5
	self.y = G.screenHeight * 0.8
	self.roll = 0
	self:SetPosition(
		Vision.hkvVec3(G.screenWidth * 0.5, G.screenHeight - self:GetHeight() - 10, 0) )
	
	self.missileFireTimer = 0

	math.clamp = function(n, low, high)
		return math.min(math.max(n, low), high)
	end
end

function OnBeforeSceneUnloaded(self)
	Input:DestroyVirtualThumbStick()
	Input:DestroyMap(self.playerInputMap)
end

function OnThink(self)
	-- Constants based off delta time
	local kTimeDifference = Timer:GetTimeDiff()
	local kMoveSpeed = 300 * kTimeDifference
	local kInvMoveSpeed = kMoveSpeed * -1
	local kRollSpeed = 5 * kTimeDifference
	local kRollRecoverSpeed = 4 * kTimeDifference
	
	-- Get keyboard state
	local keyLeft = self.playerInputMap:GetTrigger("KeyLeft")>0
	local keyRight = self.playerInputMap:GetTrigger("KeyRight")>0
	local keyUp = self.playerInputMap:GetTrigger("KeyUp")>0
	local keyDown = self.playerInputMap:GetTrigger("KeyDown")>0
	local keyFire = self.playerInputMap:GetTrigger("keyFire")>0

	-- Get touch control state
	local touchLeft = self.playerInputMap:GetTrigger("TouchLeft")>0
	local touchRight = self.playerInputMap:GetTrigger("TouchRight")>0
	local touchUp = self.playerInputMap:GetTrigger("TouchUp")>0
	local touchDown = self.playerInputMap:GetTrigger("TouchDown")>0
	local touchFire = self.playerInputMap:GetTrigger("touchFire")>0
	
	local hasMovementY = false
	local hasMovementX = false
	
	if keyUp or touchUp then
		self:IncPosition(0, kInvMoveSpeed, 0)
		hasMovementY = true
	end

	if keyDown or touchDown then
		self:IncPosition(0, kMoveSpeed, 0)
		hasMovementY = true
	end
	
	if keyLeft or touchLeft then
		self:IncPosition(kInvMoveSpeed, 0, 0)
		self.roll = self.roll - kRollSpeed
		hasMovementX = true
	end

	if keyRight or touchRight then
		self:IncPosition(kMoveSpeed, 0, 0)
		self.roll = self.roll + kRollSpeed
		hasMovementX = true
	end
	
	self.roll = math.clamp(self.roll, -1, 1)
	
	-- Adjust the roll back to flat over time if we're not moving
	if not hasMovementX then
		local absoluteRoll = math.abs(self.roll)
		if absoluteRoll > 0 and (not hasMovementX) then
			local rollCorrection = (self.roll / absoluteRoll) * kRollRecoverSpeed
			
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
	
	if keyFire or touchFire and touchUp == false then
		FireWeapon(self)
	end
end