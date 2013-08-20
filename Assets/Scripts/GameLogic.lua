Debug:Enable(true)
Debug:SetupLines(20,1)

function OnAfterSceneLoaded(self)
  self.controls = Input:CreateMap("InputMap")
  self.w, self.h = Screen:GetViewportSize()
  Debug:PrintLine("width: " .. self.w .. " height: " .. self.h)
  
  self.MainShip = Game:GetEntity("MainShip")
  self.StarField = Game:GetEffect("StarField")
 
  
  --setting up the keyboard controls
  self.controls:MapTrigger("KeyLeft", "KEYBOARD", "CT_KB_A")
  self.controls:MapTrigger("KeyRight", "KEYBOARD", "CT_KB_D")
  self.controls:MapTrigger("KeyUp", "KEYBOARD", "CT_KB_W")
  self.controls:MapTrigger("KeyDown", "KEYBOARD", "CT_KB_S")
  self.controls:MapTrigger("KeyFire", "KEYBOARD", "CT_KB_SPACE")
  
  --Create a virtual thumbstick then setup controls for it

  Input:CreateVirtualThumbStick()
  self.controls:MapTriggerAxis("TouchLeft", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_LEFT", {deadzone = 0.1})
  self.controls:MapTriggerAxis("TouchRight", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_RIGHT", {deadzone = 0.1})
  self.controls:MapTriggerAxis("TouchUp", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_UP", {deadzone = 0.1})
  self.controls:MapTriggerAxis("TouchDown", "VirtualThumbStick", "CT_PAD_LEFT_THUMB_STICK_DOWN", {deadzone = 0.1})
  self.controls:MapTrigger("TouchFire", {(self.w*.5), (self.h*.5), self.w, self.h}, "CT_TOUCH_ANY")

  --Setup a simple static Camera
  local cam = Game:GetCamera()
  cam:AttachToEntity(self, Vision.hkvVec3(0,0,100))
  cam:LookAt(self.MainShip:GetPosition()) 
  
  --Setup Starting Ship position
  self.StartPosition = Screen:Project3D((self.w*.5), (self.h*.8), 100)
  self.MainShip:SetPosition(self.StartPosition) 
  self.StarField:SetPosition(Screen:Project3D((self.w*.5), 0, 400))
  self.StarField:IncPosition(0,-400,0)
end

function OnThink(self)
  -- Tweak Values
  local MoveSpeed = 3
  
  --Get Ship poistion in relative screen space
  self.ShipPos2D_x, self.ShipPos2D_y = Screen:Project2D(self.MainShip:GetPosition())

  --Get Keyboard Controls
  local KeyGoLeft = self.controls:GetTrigger("KeyLeft")>0
  local KeyGoRight = self.controls:GetTrigger("KeyRight")>0
  local KeyGoUp = self.controls:GetTrigger("KeyUp")>0
  local KeyGoDown = self.controls:GetTrigger("KeyDown")>0
  local KeyFire = self.controls:GetTrigger("KeyFire")>0
  
  --Get Touch Controls
  local TouchGoLeft = self.controls:GetTrigger("TouchLeft")>0
  local TouchGoRight = self.controls:GetTrigger("TouchRight")>0
  local TouchGoUp = self.controls:GetTrigger("TouchUp")>0
  local TouchGoDown = self.controls:GetTrigger("TouchDown")>0
  local TouchFire = self.controls:GetTrigger("TouchFire")>0

  if KeyGoLeft or TouchGoLeft then
    if self.ShipPos2D_x > (self.w*.1) then
      self.MainShip:IncPosition(MoveSpeed,0,0)
    end
  end
  
  if KeyGoRight or TouchGoRight then
    if self.ShipPos2D_x < (self.w*.9) then
      self.MainShip:IncPosition((MoveSpeed*-1),0,0)
    end
  end
  
  if KeyGoUp or TouchGoUp then
    if self.ShipPos2D_y > (self.h*.1) then
      self.MainShip:IncPosition(0,(MoveSpeed*-1),0)
    end
  end
  
  if KeyGoDown or TouchGoDown then
    if self.ShipPos2D_y < (self.h*.9) then
      self.MainShip:IncPosition(0,MoveSpeed,0)
    end
  end
  
  if KeyFire or TouchFire and TouchGoUp == false then
    Debug:PrintLine("Shot FIRED!")
  end

 
end