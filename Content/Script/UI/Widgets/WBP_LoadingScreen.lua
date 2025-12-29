--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_LoadingScreen_C
local M = UnLua.Class()

function M:Construct()
   local LoadingScreenSubsystem = UE.UFrontendLoadingScreenSubsystem.Get(self)
   LoadingScreenSubsystem.OnLoadingReasonUpdated:Add(self, M.OnLoadingRegionUpdated)
end
function M:Destruct()
   local LoadingScreenSubsystem = UE.UFrontendLoadingScreenSubsystem.Get(self)
   LoadingScreenSubsystem.OnLoadingReasonUpdated:Remove(self, M.OnLoadingRegionUpdated)
end
function M:OnLoadingRegionUpdated(CurrentLoadingReason)
   self.CommonTextBlock_LoadingRegion:SetText(CurrentLoadingReason)
end


return M