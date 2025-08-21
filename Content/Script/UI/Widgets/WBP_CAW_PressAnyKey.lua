--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--
require("LuaPanda").start("127.0.0.1",8818)
---@type WBP_CAW_PressAnyKey_C
local M = UnLua.Class()

function M:Construct()
    -- 设置动画循环播放
    self:PlayAnimation(self.Pulsing,0.0,0,self.PlayMode,0.25,false)
end

function M:PushMainMenu()
    local Controller = self:GetOwningFrontendPlayerController()
    local WidgetStackTag = self.InWidgetStackTag
    local WidgetClass = UE.UFrontendFunctionLibrary.GetFrontendSoftWidgetClassByTag(self.InWidgetTag)
    local Task = UE.UAsyncAction_PushSoftWidget.PushSoftWidget(self, Controller, WidgetClass,
        WidgetStackTag,
        true)
    Task:Activate()
end

function M:OnMouseButtonDown(_MyGeometry,_MouseEvent)
   self:PushMainMenu()
   return UE.UWidgetBlueprintLibrary.Handled()
end
function M:OnKeyDown(_MyGeometry, _InKeyEvent)
   self:PushMainMenu()
   return UE.UWidgetBlueprintLibrary.Handled()
end
return M
