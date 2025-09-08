--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_TabList_Options_C
local M = UnLua.Class()

-- function M:PreConstruct(IsDesignTime)
--     if (IsDesignTime) then
--         self:DebugCreateTabsEditorOnly()
--         print("PreConstruct in design time")
--     end
-- end

-- function M:DebugCreateTabsEditorOnly()
--     if self.DebugEditorPreviewTabCount > 0 and self.TabButtonEntryWidgetClass:IsValid() then
--         self.HorizontalBox_TabHolder:ClearChildren()
--         for i = 1, self.DebugEditorPreviewTabCount do
--             self:AddTabsToHorizontalBoxContainer()
--         end
--     end
-- end

-- function M:AddTabsToHorizontalBoxContainer()
--     if (self.TabButtonEntryWidgetClass:IsValid()) then
--         local MyHUD = UE.UWidgetBlueprintLibrary.Create(self, self.TabButtonEntryWidgetClass,self.GetOwningPlayer(),self.Style)
--         self.HorizontalBox_TabHolder:AddChild(MyHUD)
--         self:SetPadding(self.TabButtonPadding)
--     end
-- end


return M
