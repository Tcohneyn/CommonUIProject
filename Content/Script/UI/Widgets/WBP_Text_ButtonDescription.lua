--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_Text_ButtonDescription_C
local M = UnLua.Class()

function M:Construct()
   self.CommonTextBlock_ButtonDescription:SetText(nil)
end

function M:OnInitialized()
       -- 获取子系统实例
    local UISubsystem = UE.UFrontendUISubsystem.Get(self)
    UISubsystem.OnButtonDescriptionTextUpdated:Add(self, M.OnButtonDescriptionTextUpdated)
end
function M:OnButtonDescriptionTextUpdated(_BroadcastingButton,DescriptionText)
    self.CommonTextBlock_ButtonDescription:SetText(DescriptionText)
end
return M
