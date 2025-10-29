// Tcohneyn All Rights Reserved


#include "Widgets/Options/Widget_OptionsScreen.h"
#include "Input/CommonUIInputTypes.h"
#include "ICommonInputModule.h"
#include "Widgets/Options/OptionsDataRegistry.h"
#include "Widgets/Components/FrontendTabListWidgetBase.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "FrontendDebugHelper.h"
#include "FrontendSettings/FrontendGameUserSettings.h"
#include "Subsystems/FrontendUISubsystem.h"
#include "Widgets/Components/FrontendCommonButtonBase.h"
#include "Widgets/Components/FrontendCommonListView.h"
#include "Widgets/Options/Widget_OptionsDetailsView.h"
#include "Widgets/Options/ListEntries/Widget_ListEntry_Base.h"

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

	CommonListView_OptionsList->OnItemIsHoveredChanged().AddUObject(this,&ThisClass::OnListViewItemHovered);
	CommonListView_OptionsList->OnItemSelectionChanged().AddUObject(this,&ThisClass::OnListViewItemSelected);
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

void UWidget_OptionsScreen::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
    //立即应用并保存游戏用户设置
	UFrontendGameUserSettings::Get()->ApplySettings(true);
}

// 此函数是UWidget类的虚函数，用于返回此控件所期望的焦点目标
// 当该控件被要求获取焦点时，系统会调用此函数来确定具体的焦点接收对象
UWidget* UWidget_OptionsScreen::NativeGetDesiredFocusTarget() const
{
	// 尝试从CommonListView_OptionsList列表中获取当前选中的列表项对象
	if (UObject* SelectedObject = CommonListView_OptionsList->GetSelectedItem())
	{
		// 如果成功获取到选中的列表项对象，则进一步获取该列表项对应的可视化控件（Widget）
		if (UUserWidget* SelectedEntryWidget = CommonListView_OptionsList->GetEntryWidgetFromItem(SelectedObject))
		{
			// 返回选中的列表项控件作为期望的焦点目标
			// 这确保了焦点能精准地落在用户当前选中的具体选项上
			return SelectedEntryWidget;
		}
	}
    
	// 如果无法获取有效的选中列表项控件，则回退到父类的默认逻辑
	// 父类可能会返回自身或其他默认的焦点目标
	return Super::NativeGetDesiredFocusTarget();
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

// 重置功能的核心入口函数，当用户触发重置操作（如点击重置按钮或快捷键）时调用
void UWidget_OptionsScreen::OnResetBoundActionTriggered()
{
    // 安全检查：如果可重置数据数组为空，直接返回，避免无意义的操作
    if (ResettableDataArray.IsEmpty())
    {
        return;
    }

    // 获取当前选中的标签页按钮，用于在确认对话框中显示具体的重置范围
    UCommonButtonBase* SelectedTabButton = TabListWidget_OptionsTabs->GetTabButtonBaseByID(TabListWidget_OptionsTabs->GetActiveTab());

    // 获取当前选中标签页的显示名称，用于在确认对话框中明确告知用户将重置哪个标签页下的设置
    const FString SelectedTabButtonName = CastChecked<UFrontendCommonButtonBase>(SelectedTabButton)->GetButtonDisplayText().ToString();

    // 推送一个模态确认对话框到屏幕，防止用户误操作
    UFrontendUISubsystem::Get(this)->PushConfirmScreenToModalStackAynsc(
        EConfirmScreenType::YesNo, // 对话框类型为"是/否"选择
        FText::FromString(TEXT("Reset")), // 对话框标题
        FText::FromString(TEXT("Are you sure you want to reset all the settings under the ") + SelectedTabButtonName + TEXT(" tab?")), // 具体提示信息
        [this](EConfirmScreenButtonType ClickedButtonType) // Lambda表达式定义用户点击对话框按钮后的回调逻辑
        {
            // 如果用户点击的不是"确认"按钮（如点击"否"或关闭对话框），则直接返回，不执行重置
            if (ClickedButtonType != EConfirmScreenButtonType::Confirmed)
            {
                return;
            }

            // 设置重置状态标志，防止在重置过程中其他逻辑干扰
            bIsResettingData = true;
            // 初始化重置失败标志，用于跟踪是否有数据项重置失败
            bool bHasDataFailedToReset = false;

            // 遍历所有可重置的数据项
            for (UListDataObject_Base* DataToReset : ResettableDataArray)
            {
                // 安全检查：跳过空指针，确保代码健壮性
                if (!DataToReset)
                {
                    continue;
                }

                // 尝试重置当前数据项到默认值
                if (DataToReset->TryResetBackToDefaultValue())
                {
                    // 重置成功，打印调试信息（可选）
                    Debug::Print(DataToReset->GetDataDisplayName().ToString() + TEXT(" was reset"));
                }
                else
                {
                    // 重置失败，设置失败标志并打印警告信息
                    bHasDataFailedToReset = true;
                    Debug::Print(DataToReset->GetDataDisplayName().ToString() + TEXT(" failed to reset"));
                }
            }

            // 检查是否所有数据项都成功重置
            if (!bHasDataFailedToReset)
            {
                // 如果全部成功，清空可重置数据数组（因为所有项已恢复默认值，不再需要重置）
                ResettableDataArray.Empty();
                // 移除重置操作的事件绑定（因为已无可重置项，重置功能应暂时禁用）
                RemoveActionBinding(ResetActionHandle);
            }

            // 重置操作完成，清除进行中标志
            bIsResettingData = false;
        }
    );
}

void UWidget_OptionsScreen::OnBackBoundActionTriggered()
{
	// Back 按钮回调
	// 调用父类提供的函数，关闭/退出当前 Widget 界面
	DeactivateWidget();
}

void UWidget_OptionsScreen::OnOptionsTabSelected(FName TabId)
{
	//Debug::Print(TEXT("New Tab Selected. Tab ID: ") + TabId.ToString());
	////清空详情视图信息
	DetailsView_ListEntryInfo->ClearDetailsViewInfo();
	
	// 通过数据注册表根据标签页ID获取对应的选项数据项列表
	TArray<UListDataObject_Base*> FoundListSourceItems = GetOrCreateDataRegistry()->GetListSourceItemsBySelectedTabID(TabId);

	// 将获取到的数据项列表设置到列表视图控件中
	CommonListView_OptionsList->SetListItems(FoundListSourceItems);
	// 请求列表视图刷新显示，以反映新的数据内容
	CommonListView_OptionsList->RequestRefresh();

	// 检查列表视图中是否有数据项（非空检查）
	if (CommonListView_OptionsList->GetNumItems() != 0)
	{
		// 导航到列表的第一个项（确保该项在视图中可见）
		CommonListView_OptionsList->NavigateToIndex(0);
		// 将列表的第一个项设置为选中状态
		CommonListView_OptionsList->SetSelectedIndex(0);
	}
	// 清空当前记录的可重置数据项数组，准备根据新的数据源列表重新构建
	ResettableDataArray.Empty();

	// 遍历传入的所有数据源项（FoundListSourceItems）
	for (UListDataObject_Base* FoundListSourceItem : FoundListSourceItems)
	{
		// 安全检查：跳过空指针，避免程序崩溃
		if (!FoundListSourceItem)
		{
			continue;
		}

		// 检查当前对象（this）是否已经绑定了该数据项的OnListDataModified事件
		// 如果没有绑定，则进行绑定。这确保了当数据项被修改时，当前UI能通过OnListViewListDataModified函数得到通知
		if (!FoundListSourceItem->OnListDataModified.IsBoundToObject(this))
		{
			FoundListSourceItem->OnListDataModified.AddUObject(this, &ThisClass::OnListViewListDataModified);
		}

		// 检查该数据项当前是否满足重置回默认值的条件
		if (FoundListSourceItem->CanResetBackToDefaultValue())
		{
			// 如果满足条件，则将该数据项添加到可重置数组中（AddUnique防止重复添加）
			ResettableDataArray.AddUnique(FoundListSourceItem);
		}
	}

	// 遍历并筛选完成后，检查可重置数组是否为空
	if (ResettableDataArray.IsEmpty())
	{
		// 如果没有可重置的项，则移除之前可能存在的重置操作绑定（如按钮事件、快捷键）
		// 这可以防止用户在不该操作的时候触发重置功能
		RemoveActionBinding(ResetActionHandle);
	}
	else
	{
		// 如果存在至少一个可重置的项，检查重置操作是否尚未绑定
		if (!GetActionBindings().Contains(ResetActionHandle))
		{
			// 如果未绑定，则添加操作绑定，使重置功能可用
			AddActionBinding(ResetActionHandle);
		}
	}
}

void UWidget_OptionsScreen::OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered)
{
	if (!InHoveredItem)
	{
		return;
	}

	UWidget_ListEntry_Base* HoveredEntryWidget = CommonListView_OptionsList->GetEntryWidgetFromItem<UWidget_ListEntry_Base>(InHoveredItem);
	check(HoveredEntryWidget);

	HoveredEntryWidget->NativeOnListEntryWidgetHovered(bWasHovered);

	if (bWasHovered)
	{
		DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
			CastChecked<UListDataObject_Base>(InHoveredItem),
			TryGetEntryWidgetClassName(InHoveredItem)
		);
	}
	else
	{
		if (UListDataObject_Base* SelectedItem = CommonListView_OptionsList->GetSelectedItem<UListDataObject_Base>())
		{
			DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
				SelectedItem,
				TryGetEntryWidgetClassName(SelectedItem)
			);
		}
	}
}

void UWidget_OptionsScreen::OnListViewItemSelected(UObject* InSelectedItem)
{
	if (!InSelectedItem)
	{
		return;
	}
	
	DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
			CastChecked<UListDataObject_Base>(InSelectedItem),
			TryGetEntryWidgetClassName(InSelectedItem)
		);
	// const FString DebugString = 
	// CastChecked<UListDataObject_Base>(InSelectedItem)->GetDataDisplayName().ToString() +
	// TEXT(" was selected");
	//
	// Debug::Print(DebugString);
}

FString UWidget_OptionsScreen::TryGetEntryWidgetClassName(UObject* InOwningListItem) const
{
	if (UUserWidget* FoundEntryWidget = CommonListView_OptionsList->GetEntryWidgetFromItem(InOwningListItem))
	{
		return FoundEntryWidget->GetClass()->GetName();
	}

	return TEXT("Entry Widget Not Valid");
}

// 当列表中的数据对象被修改时调用的回调函数
void UWidget_OptionsScreen::OnListViewListDataModified(UListDataObject_Base* ModifiedData,
	EOptionsListDataModifyReason ModifyReason)
{
	// 安全检查：确保被修改的数据对象有效
	if (!ModifiedData || bIsResettingData)
	{
		return; // 如果数据对象无效，直接返回，避免后续操作出错
	}

	// 检查被修改的数据对象是否能够重置回默认值
	if (ModifiedData->CanResetBackToDefaultValue())
	{
		// 如果能够重置，将该数据对象添加到可重置数据数组中（避免重复添加）
		ResettableDataArray.AddUnique(ModifiedData);

		// 检查是否已经绑定了重置操作，如果没有则添加绑定
		if (!GetActionBindings().Contains(ResetActionHandle))
		{
			AddActionBinding(ResetActionHandle); // 添加重置功能的操作绑定（如键盘快捷键或手柄按钮）
		}
	}
	else
	{
		// 如果该数据对象不能重置，则从可重置数据数组中移除（如果存在）
		if (ResettableDataArray.Contains(ModifiedData))
		{
			ResettableDataArray.Remove(ModifiedData);
		}
	}

	// 检查可重置数据数组是否为空（即当前是否有任何可重置的选项）
	if (ResettableDataArray.IsEmpty())
	{
		// 如果没有可重置的选项，移除重置操作绑定，防止用户执行无意义的操作
		RemoveActionBinding(ResetActionHandle);
	}
}
