-- new script file
-- Depth (Z-order) of the missile sprites
depthLayer = -1

-- Move variables
moveSpeed = 50
isJumping = false;
JumpHeight = 0;

kJumpHeight = 150
kJumpSpeed = 5

kEnemySpeed = 480

function OnAfterSceneLoaded(self)
	self.playerInputMap = Input:CreateMap("InputMap")
	self.playerInputMap:MapTrigger("KeyUp", "KEYBOARD", "CT_KB_SPACE")       -- jump

	--Get viewport size
	local w, h = Screen:GetViewportSize()

	G.w = w
	G.h = h

	--  Get reference to the paltform
	self.platform = Game:GetEntity("platform")
	self.platform:SetCenterPosition(Vision.hkvVec3(w* 0.5, h - self.platform:GetHeight() * 0.5, depthLayer) )
	self.platform:SetWidth(w)

	-- Calculate the starting position based on the viewport size and the height of the paltform
	self:SetCenterPosition(Vision.hkvVec3(w* 0.5, h - self.platform:GetHeight() - self:GetHeight() / 2, depthLayer) )
	self.baseVec = self:GetPosition()

	self.start = self:GetPosition()
	self.position = 0

	self.enemy = Game:GetEntity("enemy")
	self.enemies = {}

	CreateEnemy(self)
end

function CreateEnemy(self)
	local enemyData = {}

	enemyData.position = Vision.hkvVec3(
		G.w + self:GetWidth(),
		G.h - self.platform:GetHeight() - self:GetHeight() / 2,
		depthLayer)

	enemyData.sprite = Toolset2D:CreateSprite(
		Vision.hkvVec3(0, 0, 0),
		"Textures/impossible_enemy.png",
		self.enemy:GetProperty("XmlDataFilename"))
	
	enemyData.sprite:SetKey("enemy")
	enemyData.sprite:SetScaling(self.enemy:GetScaling())
	enemyData.sprite:SetCollision(true)
	enemyData.sprite:SetCenterPosition(enemyData.position)

	table.insert(self.enemies, enemyData)
end

function OnSpriteCollision(self, sprite)
  if self:GetKey() == "player" then
    sprite:SetVisible(false) --Debug:PrintLine("Killed you!")
    --self:remove()
		return
	end
end

function IsTriggered(self, key)
	return (self.playerInputMap:GetTrigger(key) > 0)
end

function OnThink(self)

  local dt = Timer:GetTimeDiff()
  self.position = self.position + dt * 360

  -- player jump
  if(isJumping ~= true) then
    if IsTriggered(self, "KeyUp") and isJumping == false then
      isJumping = true;
      theta = 0
	  end 
  end

	for i,v in ipairs(self.enemies) do
		v.position.x = v.position.x - dt * kEnemySpeed
		v.sprite:SetCenterPosition(v.position)
		
		if v.position.x < -v.sprite:GetWidth() then
			v.position.x = G.w + v.sprite:GetWidth()
			v.sprite:SetVisible(true)
		end
	end

  if (isJumping == true) then 
    if(theta < math.pi) then
      theta = theta + dt * kJumpSpeed
      if theta > math.pi then
        theta = math.pi
      end
      
      height = kJumpHeight * math.sin( theta );
      local offset = Vision.hkvVec3(0, height, 0)
      self:SetPosition(self.baseVec - offset)
	  
	  local rotation = theta / math.pi
      self:SetOrientation( Vision.hkvVec3(0, 0, rotation * 90) )
      Debug:PrintLine(height)
    else
      -- Super hacky but after jump small delta remains leaving player slighly off the ground
      self:SetPosition(self.baseVec)
      self:SetOrientation( Vision.hkvVec3(0, 0, 0) )
      isJumping = false;
    end
  end 
end
