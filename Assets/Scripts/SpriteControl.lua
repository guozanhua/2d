function OnAfterSceneLoaded(self)
  Debug:PrintLine("The entity's new property value is")
	self:SetState("rollRight")
end

function OnThink(self)
  Debug:PrintLine("The entity's new property value is")	
end
