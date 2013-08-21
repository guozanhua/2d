--[[
Author: Joel Van Eenwyk, Ryan Monday
Purpose: Controls the sprite
--]]

Debug:Enable(true)
Debug:SetupLines(20, 1)

function math.clamp(n, low, high)
    return math.min(math.max(n, low), high)
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
    self.playerInputMap:MapTrigger("KeyFire", "KEYBOARD", "CT_KB_SPACE")

    -- Create a virtual thumbstick then setup playerInputMap for it
    Input:CreateVirtualThumbStick()
    self.playerInputMap:MapTriggerAxis("TouchLeft", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_LEFT", kDeadzone)
    self.playerInputMap:MapTriggerAxis("TouchRight", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_RIGHT", kDeadzone)
    self.playerInputMap:MapTriggerAxis("TouchUp", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_UP", kDeadzone)
    self.playerInputMap:MapTriggerAxis("TouchDown", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_DOWN", kDeadzone)
    self.playerInputMap:MapTrigger("TouchFire", {(self.w*.5), (self.h*.5), self.w, self.h}, "CT_TOUCH_ANY")

    -- Calculate the starting position of the ship
    self.x = self.w * 0.5
    self.y = self.h * 0.8
    self.roll = 0
    self:SetPosition(self.x, self.y, 0) 
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
    local kRollRecoverSpeed = 40 * kTimeDifference
    
    local isMoving = false

    -- Get keyboard state
    local KeyGoLeft = self.playerInputMap:GetTrigger("KeyLeft")>0
    local KeyGoRight = self.playerInputMap:GetTrigger("KeyRight")>0
    local KeyGoUp = self.playerInputMap:GetTrigger("KeyUp")>0
    local KeyGoDown = self.playerInputMap:GetTrigger("KeyDown")>0
    local KeyFire = self.playerInputMap:GetTrigger("KeyFire")>0

    -- Get touch control state
    local TouchGoLeft = self.playerInputMap:GetTrigger("TouchLeft")>0
    local TouchGoRight = self.playerInputMap:GetTrigger("TouchRight")>0
    local TouchGoUp = self.playerInputMap:GetTrigger("TouchUp")>0
    local TouchGoDown = self.playerInputMap:GetTrigger("TouchDown")>0
    local TouchFire = self.playerInputMap:GetTrigger("TouchFire")>0
    
    if KeyGoUp or TouchGoUp then
        self:IncPosition(0, kInvMoveSpeed, 0)
    end

    if KeyGoDown or TouchGoDown then
        self:IncPosition(0, kMoveSpeed, 0)
    end

    if KeyFire or TouchFire and TouchGoUp == false then
        Debug:PrintLine("Shot FIRED!")
    end 
    
    if KeyGoLeft or TouchGoLeft then
        self:IncPosition(kInvMoveSpeed, 0, 0)
        self.roll = self.roll - kRollSpeed
        isMoving = true
    end

    if KeyGoRight or TouchGoRight then
        self:IncPosition(kMoveSpeed, 0, 0)
        self.roll = self.roll + kRollSpeed
        isMoving = true
    end
    
    self.roll = math.clamp(self.roll, -1, 1)
    local absoluteRoll = math.abs(self.roll)
    
    -- Adjust the roll back to flat over time if we're not moving
    if not isMoving then
        if absoluteRoll > 0 and (not isMoving) then
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
 
    self:SetFramePercent(absoluteRoll)
end