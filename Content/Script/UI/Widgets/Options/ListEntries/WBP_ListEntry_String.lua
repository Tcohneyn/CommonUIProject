--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_ListEntry_String_C
local M = UnLua.Class()

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


function M:BP_OnToggleEntryWidgetHighlightState(bShouldHighlight)
    if bShouldHighlight then
        self.CommonText_SettingDisplayName:SetStyle(self.StyleHighlight) -- 高亮时应用高亮样式
        self.CommonRotator_AvailableOptions.MyText:SetStyle(self.StyleHighlight) 
    else
        self.CommonText_SettingDisplayName:SetStyle(self.StyleDefault) -- 取消高亮时应用默认样式
        self.CommonRotator_AvailableOptions.MyText:SetStyle(self.StyleDefault) 
    end
    self.CommonButton_NextOption:Cast(UE.UWBP_Button_ListEntry_Image_C):ToggleButtonImageHighlight(bShouldHighlight)
    self.CommonButton_PreviousOption:Cast(UE.UWBP_Button_ListEntry_Image_C):ToggleButtonImageHighlight(bShouldHighlight)
end
return M
