--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type WBP_CAW_StoryScreen_C
local M = UnLua.Class()

function M:Construct()

    if self.Button_NewStory then
        self.Button_NewStory.OnButtonBaseClicked:Add(self, M.OnButtonClicked_NewStory)
    else
        print("[Lua] NewStory button is nil!")
    end
end

function M:OnButtonClicked_NewStory()
    local Task = UE.UAsyncAction_PushConfirmScreen.PushConfirmScreen(
        self,
        self.ScreenType,
        "Under Construction",
        "The new story experience is currenly under construction"
    )
    Task:Activate()
end

function M:BP_OnActivated()
    print("[Lua] Story Screen Activated")
    self.Button_Continue:SetIsInteractionEnabled(false)
    self.Button_NewGamePlus:SetIsInteractionEnabled(false)
    self.Button_Continue:SetIsFocusable( false)
    self.Button_NewGamePlus:SetIsFocusable( false)
end
return M
