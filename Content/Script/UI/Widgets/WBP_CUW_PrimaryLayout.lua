--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_CUW_PrimaryLayout_C
local M = UnLua.Class()

-- 定义 M 类的 OnInitialized 方法
-- 此方法是一个生命周期函数，当对应的UE控件完成基础初始化后会自动调用
function M:OnInitialized()
   -- 将不同的控件栈（Widget Stack）注册到主布局中，并为其分配特定的游戏标签（Gameplay Tag）
   -- 此操作建立了标签与控件栈的映射关系，便于后续通过标签精确查找和操作特定的界面层级

   -- 注册前端通用界面栈（如主菜单、设置界面等）
   self:RegisterWidgetStack(self.InStackTag1, self.WidgetStack_Frontend)

   -- 注册游戏平视显示器（HUD）栈，用于显示游戏运行时信息（如血量、分数、小地图）
   self:RegisterWidgetStack(self.InStackTag2, self.WidgetStack_GameHud)

   -- 注册游戏菜单栈（如暂停菜单、库存系统等）
   self:RegisterWidgetStack(self.InStackTag3, self.WidgetStack_GameMenu)

   -- 注册模态对话框栈，用于管理必须优先处理的弹窗（如确认对话框、重要提示）
   -- 模态栈通常会获取输入焦点并阻止与下层界面的交互
   self:RegisterWidgetStack(self.InStackTag4, self.WidgetStack_Modal)
end


return M
