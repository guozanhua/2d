-- new script file

function OnThink(self)
	self.timer = (self.timer + Timer:GetTimeDiff()) % (2 * math.pi)
	self:SetPosition( Vision.hkvVec3(100, math.sin(self.timer), 0) )
	
	Debug:Enable(true)
	Debug:PrintLine(Toolset2D:GetNumSprites())
end

function OnAfterSceneLoaded(self)
	self.timer = 0
end
