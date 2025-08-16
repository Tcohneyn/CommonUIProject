--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_CUW_PrimaryLayout_C
local M = UnLua.Class()

function M:OnInitialized()
   self:RegisterWidgetStack(self.InStackTag1,self.WidgetStack_Frontend)
   self:RegisterWidgetStack(self.InStackTag2,self.WidgetStack_GameHud)
   self:RegisterWidgetStack(self.InStackTag3,self.WidgetStack_GameMenu)
   self:RegisterWidgetStack(self.InStackTag4,self.WidgetStack_Modal)
end


return M
