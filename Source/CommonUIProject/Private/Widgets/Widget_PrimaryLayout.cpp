// Tcohneyn All Rights Reserved


#include "Widgets/Widget_PrimaryLayout.h"
#include "FrontendDebugHelper.h"

/**
 * 根据给定的游戏标签查找并返回对应的可激活控件堆栈。
 * 此函数用于在运行时快速定位特定的界面堆栈（如设置菜单、库存界面等），以便进行导航或状态管理。
 * @param InTag - 用于标识控件堆栈的游戏标签（例如 "Frontend.WidgetStack.Settings"）
 * @return 返回与标签关联的 UCommonActivatableWidgetContainerBase 指针。如果标签未注册，会触发断言错误（仅在开发版本中）。
 * @note 使用 checkf 宏确保标签已注册，避免返回空指针导致运行时错误 [6](@ref)
 */
UCommonActivatableWidgetContainerBase* UWidget_PrimaryLayout::FindWidgetStackByTag(const FGameplayTag& InTag) const
{
	checkf(RegisteredWidgetStackMap.Contains(InTag),TEXT("Can not find the widget stack by the tag %s"),*InTag.ToString());

	return RegisteredWidgetStackMap.FindRef(InTag);
}

/**
 * 将一个新的可激活控件堆栈注册到管理器中，并将其与特定标签关联。
 * 此函数通常在界面初始化时调用，用于建立标签与控件堆栈的映射关系，支持后续的动态导航。
 * @param InStackTag - 用于标识控件堆栈的游戏标签（需属于 "Frontend.WidgetStack" 分类）
 * @param InStack - 要注册的可激活控件堆栈对象指针
 * @note 仅在运行时（非设计时）执行注册操作，且避免重复注册同一标签 [6](@ref)
 */
void UWidget_PrimaryLayout::RegisterWidgetStack(UPARAM(meta = (Categories = "Frontend.WidgetStack"))FGameplayTag InStackTag, UCommonActivatableWidgetContainerBase* InStack)
{
	if (!IsDesignTime())
	{
		if (!RegisteredWidgetStackMap.Contains(InStackTag))
		{
			RegisteredWidgetStackMap.Add(InStackTag, InStack);
			//Debug::Print(TEXT("Widget Stack Registered under the tag")+InStackTag.ToString());
		}
	}
}
