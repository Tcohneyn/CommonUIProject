--
-- DESCRIPTION
--
-- @COMPANY 
-- @AUTHOR 
-- @DATE 2025/10/27 15:16:18
--

---@type WBP_ListEntry_String_C
local M = UnLua.Class()
function M:BP_OnItemSelectionChanged(bIsSelected)
    self:ToggleHighlightState(bIsSelected)
end
function M:BP_OnListEntryWidgetHovered(bWasHovered,bIsEntryWidgetStillSelected)
    if bWasHovered then
        self:ToggleHighlightState(true)
    else
        if not bIsEntryWidgetStillSelected then
            self:ToggleHighlightState(false)
        end
    end
end

function M:ToggleHighlightState(bToHighlight)
    if bToHighlight then
        self.CommonText_SettingDisplayName:SetStyle(self.StyleHighlight) -- 高亮时应用高亮样式
        self.CommonRotator_AvailableOptions.MyText:SetStyle(self.StyleHighlight) 
    else
        self.CommonText_SettingDisplayName:SetStyle(self.StyleDefault) -- 取消高亮时应用默认样式
        self.CommonRotator_AvailableOptions.MyText:SetStyle(self.StyleDefault) 
    end
    self.CommonButton_NextOption:Cast(UE.UWBP_Button_ListEntry_Image_C):ToggleButtonImageHighlight(bToHighlight)
    self.CommonButton_PreviousOption:Cast(UE.UWBP_Button_ListEntry_Image_C):ToggleButtonImageHighlight(bToHighlight)
end
return M
