--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_ListEntry_KeyRemap_C
local M = UnLua.Class()

function M:BP_OnToggleEntryWidgetHighlightState(bShouldHighlight)
    if bShouldHighlight then
        self.CommonText_SettingDisplayName:SetStyle(self.StyleHighlight) -- 高亮时应用高亮样式
    else
        self.CommonText_SettingDisplayName:SetStyle(self.StyleDefault)   -- 取消高亮时应用默认样式
    end
end

return M
