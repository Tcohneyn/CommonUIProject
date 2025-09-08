// Tcohneyn All Rights Reserved


#include "Widgets/Options/Widget_OptionsScreen.h"
#include "Input/CommonUIInputTypes.h"
#include "ICommonInputModule.h"
#include "Widgets/Options/OptionsDataRegistry.h"
#include "Widgets/Components/FrontendTabListWidgetBase.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "FrontendDebugHelper.h"

void UWidget_OptionsScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	// 1. 绑定 Reset 按钮
	if (!ResetAction.IsNull())  // 确认 ResetAction 是否已在蓝图/数据表中配置
	{
		// 注册 UI Action Binding，保存返回的 Handle 以便之后移除或管理
		ResetActionHandle = RegisterUIActionBinding(
			FBindUIActionArgs(     // 构造绑定参数
				ResetAction,       // 输入映射（来自 FDataTableRowHandle）
				true,              // 是否显示在 Action Bar（UI 底部操作栏）
				FSimpleDelegate::CreateUObject(this, &ThisClass::OnResetBoundActionTriggered)
				// 回调函数，当触发 ResetAction 时执行
			)
		);
	}
	// 2. 绑定 Back 按钮
	RegisterUIActionBinding(
		FBindUIActionArgs(
			ICommonInputModule::GetSettings().GetDefaultBackAction(),
			// 使用 CommonInputModule 提供的默认返回动作（全局设置）
			true,   // 显示在 Action Bar
			FSimpleDelegate::CreateUObject(this,&ThisClass::OnBackBoundActionTriggered)
			// 回调函数，当触发 BackAction 时执行
		)
	);
	TabListWidget_OptionsTabs->OnTabSelected.AddUniqueDynamic(this,&ThisClass::OnOptionsTabSelected);
}

// Widget 激活函数，每次 Widget 被显示时调用
void UWidget_OptionsScreen::NativeOnActivated()
{
	Super::NativeOnActivated(); // 调用父类实现

	// 遍历注册的选项 Tab 集合
	for (UListDataObject_Collection* TabCollection : GetOrCreateDataRegistry()->GetRegisteredOptionsTabCollections())
	{
		if (!TabCollection) // 如果集合无效，跳过
		{
			continue;
		}

		const FName TabID = TabCollection->GetDataID(); // 获取 Tab 的唯一 ID

		// 如果 Tab 已经存在于 UI 中，则跳过
		if (TabListWidget_OptionsTabs->GetTabButtonBaseByID(TabID) != nullptr)
		{
			continue;
		}

		// 注册 Tab 到 TabListWidget
		TabListWidget_OptionsTabs->RequestRegisterTab(TabID, TabCollection->GetDataDisplayName());
	}
}

// 获取或创建选项数据注册器
UOptionsDataRegistry* UWidget_OptionsScreen::GetOrCreateDataRegistry()
{
	if (!CreatedOwningDataRegistry) // 如果还未创建
	{
		CreatedOwningDataRegistry = NewObject<UOptionsDataRegistry>(); // 创建新对象
		CreatedOwningDataRegistry->InitOptionsDataRegistry(GetOwningLocalPlayer()); 
		// 初始化，绑定当前 LocalPlayer
	}

	// 校验指针是否有效，如果无效则报错
	checkf(CreatedOwningDataRegistry, TEXT("Data registry for options screen is not valid"));

	return CreatedOwningDataRegistry; // 返回注册器指针
}

void UWidget_OptionsScreen::OnResetBoundActionTriggered()
{
	// Reset 按钮回调
	// 打印调试信息到屏幕和日志（通过 Debug Helper 实现）
	Debug::Print(TEXT("Reset bound action triggered"));
}

void UWidget_OptionsScreen::OnBackBoundActionTriggered()
{
	// Back 按钮回调
	// 调用父类提供的函数，关闭/退出当前 Widget 界面
	DeactivateWidget();
}

void UWidget_OptionsScreen::OnOptionsTabSelected(FName TabId)
{
	Debug::Print(TEXT("New Tab Selected. Tab ID: ") + TabId.ToString());
}