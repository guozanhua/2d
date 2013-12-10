function OnExpose(self)
	self.DepthLayer = -1

	self.MoveSpeed = 50
	self.IsJumping = false;

	self.JumpHeight = 150
	self.JumpSpeed = 5

	self.EnemySpeed = 480
end

function OnAfterSceneLoaded(self)
	self.playerInputMap = Input:CreateMap("InputMap")
	self.playerInputMap:MapTrigger("KeyUp", "KEYBOARD", "CT_KB_SPACE")

	local w, h = Screen:GetViewportSize()

	G.w = w
	G.h = h

	--  Get reference to the paltform
	self.platform = Game:GetEntity("platform")
	self.platform:SetCenterPosition(
		Vision.hkvVec3(
			w * 0.5,
			h - self.platform:GetHeight() * 0.5,
			self.DepthLayer) )
	self.platform:SetWidth(w)

	-- Calculate the starting position based on the viewport size and
	-- the height of the paltform
	self:SetCenterPosition(
		Vision.hkvVec3(
			w * 0.5,
			h - self.platform:GetHeight() - self:GetHeight() / 2,
			self.DepthLayer) )
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
		self.DepthLayer)
	enemyData.sprite = self.enemy:Clone( enemyData.position )

	table.insert(self.enemies, enemyData)
end

function OnSpriteCollision(self, sprite)
	if self:GetKey() == "player" then
		sprite:SetVisible(false)
	end
end

function IsTriggered(self, key)
	return (self.playerInputMap:GetTrigger(key) > 0)
end

function OnThink(self)
	local dt = Timer:GetTimeDiff()
	self.position = self.position + dt * 360

	if not self.IsJumping then
		if IsTriggered(self, "KeyUp") and self.IsJumping == false then
			self.IsJumping = true;
			theta = 0
		end 
	end

	for i,v in ipairs(self.enemies) do
		v.position.x = v.position.x - dt * self.EnemySpeed
		v.sprite:SetCenterPosition(v.position)
		
		if v.position.x < -v.sprite:GetWidth() then
			v.position.x = G.w + v.sprite:GetWidth()
			v.sprite:SetVisible(true)
		end
	end

	if self.IsJumping then 
		if theta < math.pi then
			theta = theta + dt * self.JumpSpeed

			if theta > math.pi then
				theta = math.pi
			end

			height = self.JumpHeight * math.sin( theta );
			local offset = Vision.hkvVec3(0, height, 0)
			self:SetPosition(self.baseVec - offset)

			local rotation = theta / math.pi
			self:SetOrientation( Vision.hkvVec3(0, 0, rotation * 90) )
			Debug:PrintLine(height)
		else
			self:SetPosition(self.baseVec)
			self:SetOrientation( Vision.hkvVec3(0, 0, 0) )
			self.IsJumping = false
		end
	end 
end
