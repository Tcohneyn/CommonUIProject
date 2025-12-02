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

-- 重写UE的ReceivePossess事件函数，当此PlayerController获得对一个Pawn的控制权时自动调用
function M:ReceivePossess(aPawn)
    -- 1. 同步加载并创建主布局UI控件
    -- 使用UE的反射系统，根据路径加载UI控件蓝图类（WBP_CUW_PrimaryLayout）
    local UMG_C = UE.UClass.Load("/Game/UI/Widgets/WBP_CUW_PrimaryLayout.WBP_CUW_PrimaryLayout_C")
    -- 利用加载到的控件类，创建出一个实际的控件实例。`self`作为创建者（Owning Player），`self`也作为外部对象（Outer）
    local MyHUD = UE.UWidgetBlueprintLibrary.Create(self, UMG_C, self)
    -- 将创建好的控件实例添加到游戏视口（0层），使其对玩家可见
    MyHUD:AddToViewport(0)

    -- 2. 获取前端UI子系统并注册主布局控件
    -- 获取前端UI子系统的全局实例，它是管理所有前端UI的中心枢纽
    local UISubsystem = UE.UFrontendUISubsystem.Get(self)
    -- 安全检查，确保子系统获取成功
    if UISubsystem then
        -- 将刚刚创建的主布局控件注册到子系统中，使得子系统可以管理和操作这个UI根容器
        UISubsystem:RegisterCreatedPrimaryLayoutWidget(MyHUD)

        -- 3. 异步推送初始界面到指定的UI堆栈
        -- 通过一个自定义的函数（或数据资产）根据标签（Tag）获取要推送的控件类的软引用（Soft Reference）
        -- 软引用允许异步加载资源，避免主线程卡顿，特别适合大型UI资源
        local WidgetClass = UE.UFrontendFunctionLibrary.GetFrontendSoftWidgetClassByTag(self.InWidgetTag)
        -- 定义目标UI堆栈的标签，决定这个控件将被推送到哪个层级（如模态对话框层、HUD层等）
        local WidgetStackTag = self.InWidgetStackTag

        -- 检查控件类是否成功获取
        if WidgetClass then
            -- 创建并配置一个异步推送任务
            -- 调用异步操作的工厂函数，传入必要的参数：世界上下文、玩家控制器、控件类软引用、目标堆栈标签、是否自动聚焦
            local Task = UE.UAsyncAction_PushSoftWidget.PushSoftWidget(self, self, WidgetClass, WidgetStackTag, true)
            -- 为异步任务绑定回调事件：当控件实例已创建但尚未推入堆栈时，会触发OnEventReceived函数
            -- 这通常用于在控件显示前进行一些自定义的初始化操作
            Task.OnWidgetCreatedBeforePush:Add(self, M.OnEventReceived)
            -- 激活异步任务，开始执行资源加载和推送流程
            Task:Activate()
        else
            -- 如果控件类加载失败，打印错误信息（在实际项目中应使用更健壮的错误处理机制）
            print("Failed to load WidgetClass")
        end
    end
end

-- 异步推送任务的自定义回调函数
-- 当异步操作完成控件创建（即将推送前）时被调用
function M:OnEventReceived(PushedWidget)
    -- 打印日志，确认事件被触发并获取被推送控件的名称，用于调试
    print("Event received: " .. PushedWidget:GetName())
    -- 这里可以添加更多自定义逻辑，例如：
    -- 为控件设置初始数据、绑定子控件的事件、播放入场动画等
end
return M
