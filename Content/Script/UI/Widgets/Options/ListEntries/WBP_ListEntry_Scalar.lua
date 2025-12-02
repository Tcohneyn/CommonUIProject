--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_ListEntry_Scalar_C
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
        self.CommonNumeric_SettingValue:SetStyle(self.StyleHighlight)
        self.AnalogSlider_SettingSlider:SetSliderBarColor(self.SliderHighlightColor)
        self.AnalogSlider_SettingSlider:SetSliderHandleColor(self.SliderHighlightColor) 
    else
        self.CommonText_SettingDisplayName:SetStyle(self.StyleDefault) -- 取消高亮时应用默认样式
        self.CommonNumeric_SettingValue:SetStyle(self.StyleDefault)
        self.AnalogSlider_SettingSlider:SetSliderBarColor(self.SliderDefaultColor)
        self.AnalogSlider_SettingSlider:SetSliderHandleColor(self.SliderDefaultColor) 
    end

end

return M
