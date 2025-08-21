--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--
--require("LuaPanda").start("127.0.0.1",8818)
---@type BP_FrontendController_C
local M = UnLua.Class()

function M:ReceivePossess(aPawn)
    local UMG_C = UE.UClass.Load("/Game/UI/Widgets/WBP_CUW_PrimaryLayout.WBP_CUW_PrimaryLayout_C")
    local MyHUD = UE.UWidgetBlueprintLibrary.Create(self, UMG_C, self)
    MyHUD:AddToViewport(0)
    -- 获取子系统实例
    local UISubsystem = UE.UFrontendUISubsystem.Get(self)
    if UISubsystem then
        -- 调用注册方法
        UISubsystem:RegisterCreatedPrimaryLayoutWidget(MyHUD)
        local WidgetClass = UE.UFrontendFunctionLibrary.GetFrontendSoftWidgetClassByTag(self.InWidgetTag)
        --local WidgetClass = UE.FSoftObjectPtr(UE.UClass.Load("/Game/UI/Widgets/WBP_CAW_PressAnyKey.WBP_CAW_PressAnyKey_C"))
        local WidgetStackTag = self.InWidgetStackTag
        if WidgetClass then
            local Task = UE.UAsyncAction_PushSoftWidget.PushSoftWidget(self, self, WidgetClass,
                WidgetStackTag,
                true)
            Task.OnWidgetCreatedBeforePush:Add(self, M.OnEventReceived)
            --Task.OnWidgetCreatedBeforePush:Add(self, M.OnEventReceived)
            Task:Activate()
        else
            print("Failed to load WidgetClass")
        end
    end
end

function M:OnEventReceived(PushedWidget)
    print("Event received: " .. PushedWidget:GetName())
    -- 处理事件逻辑
end
return M
