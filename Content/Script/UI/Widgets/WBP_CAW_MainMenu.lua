--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_CAW_MainMenu_C
local M = UnLua.Class()

-- 生命周期：Construct 在 UMG Widget 初始化时调用
function M:Construct()
    if self.Button_Quit then
        self.Button_Quit.OnButtonBaseClicked:Add(self, M.OnButtonClicked_Quit)
    else
        print("[Lua] Quit button is nil!")
    end

    if self.Button_Story then
        self.Button_Story.OnButtonBaseClicked:Add(self, M.OnButtonClicked_Story)
    else
        print("[Lua] Story button is nil!")
    end
end

-- Quit 按钮
function M:OnButtonClicked_Quit()
    local Task = UE.UAsyncAction_PushConfirmScreen.PushConfirmScreen(
        self,
        self.ScreenType,
        "Exit",
        "Are you sure you want to exit?"
    )

    if Task then
        Task.OnButtonClicked:Add(self, M.OnConfirmScreenClicked)
        Task:Activate()
    else
        print("[Lua] Failed to create confirm screen task")
    end
end

-- 确认框回调
function M:OnConfirmScreenClicked(ButtonType)
-- 获取本地玩家的 CommonInputSubsystem 实例
local InputSubsystem = self:GetInputSubsystem()
    if ButtonType == UE.EConfirmScreenButtonType.Confirmed then
        if (InputSubsystem:GetCurrentInputType() == UE.ECommonInputType.Gamepad) then
            InputSubsystem:SetCurrentInputType( UE.ECommonInputType.Keyboard)
            UE.UKismetSystemLibrary.QuitGame(self, nil, UE.EQuitPreference.Quit, false)
        else
            print("[Lua] Keyboard confirmed exit")
            UE.UKismetSystemLibrary.QuitGame(self, nil, UE.EQuitPreference.Quit, false)
        end
    elseif ButtonType == UE.EConfirmScreenButtonType.Cancelled then
        print("[Lua] Quit canceled by user")
    else
        print("[Lua] Other confirm dialog result: " .. tostring(ButtonType))
    end
end

-- Story 按钮
function M:OnButtonClicked_Story()
    print("[Lua] Story button clicked")

    local OwningPC = self:GetOwningFrontendPlayerController()
    print("[Lua] OwningPC = " .. tostring(OwningPC))

    local WidgetClass = UE.UFrontendFunctionLibrary.GetFrontendSoftWidgetClassByTag(self.InWidgetTag)
    print("[Lua] WidgetClass = " .. tostring(WidgetClass))

    if WidgetClass then
        local Task = UE.UAsyncAction_PushSoftWidget.PushSoftWidget(
            self,
            OwningPC,
            WidgetClass,
            self.InWidgetStackTag,
            true
        )
        if Task then
            print("[Lua] Soft widget task created")
            Task.OnWidgetCreatedBeforePush:Add(self, M.OnWidgetPushed)
            Task:Activate()
        else
            print("[Lua] Failed to create push soft widget task")
        end
    else
        print("[Lua] Failed to load WidgetClass for Story")
    end
end

-- Widget 推入回调
function M:OnWidgetPushed(Widget)
    print("[Lua] Widget pushed: " .. tostring(Widget))
end

return M
