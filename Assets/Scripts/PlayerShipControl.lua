--[[
Author: Joel Van Eenwyk, Ryan Monday
Purpose: Controls the sprite
--]]

Debug:Enable(true)
Debug:SetupLines(20, 1)

function math.clamp(n, low, high)
    return math.min(math.max(n, low), high)
end

function FireWeapon(self)
    if self.fireDelay <= 0 then
      local missilePosition = self:GetPosition()
      local offset = Vision.hkvVec3(98, 65, 0)
      local offset1 = offset + Vision.hkvVec3(80, 0, 0)
      local offset2 = offset + Vision.hkvVec3(-80, 0, 0)
      
      -- Make the missiles appear above everything else
      missilePosition.z = 7
      
      local missile = Game:CreateEntity(missilePosition + offset1, "Sprite", "", "Missile")
      missile:UpdateProperty("TextureFilename", "Textures/missile.png")      
      table.insert(self.missiles, missile)
      
      local missile2 = Game:CreateEntity(missilePosition + offset2, "Sprite", "", "Missile")
      missile2:UpdateProperty("TextureFilename", "Textures/missile.png")      
      table.insert(self.missiles, missile2)
      
      self.fireDelay = 0.1
    else
      self.fireDelay = self.fireDelay - Timer:GetTimeDiff()
    end
end

function UpdateMissiles(self)
    local dt = Timer:GetTimeDiff()
    for index, missile in ipairs(self.missiles) do
      local pos =  missile:GetPosition()
      pos.y = pos.y - dt * 500
      missile:SetPosition(pos)
    end
end

function OnAfterSceneLoaded(self)
    local kDeadzone = {deadzone = 0.1}
 
    self.playerInputMap = Input:CreateMap("InputMap")
 
    self.w, self.h = Screen:GetViewportSize()
    Debug:PrintLine("width: " .. self.w .. " height: " .. self.h)
  
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
    self.playerInputMap:MapTrigger("touchFire", {(self.w*.5), (self.h*.5), self.w, self.h}, "CT_TOUCH_ANY")

    -- Calculate the starting position of the ship
    self.x = self.w * 0.5
    self.y = self.h * 0.8
    self.roll = 0
    self:SetPosition(self.x, self.y, 0) 
    
    self.missiles = {}
    self.fireDelay = 0
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
    
    UpdateMissiles(self)
end