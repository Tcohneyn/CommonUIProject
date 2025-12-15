// Tcohneyn All Rights Reserved


#include "Widgets/Options/ListEntries/Widget_ListEntry_Base.h"

#include "CommonInputSubsystem.h"
#include "CommonTextBlock.h"
#include "Components/ListView.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"

void UWidget_ListEntry_Base::NativeOnListEntryWidgetHovered(bool bWasHovered)
{
	BP_OnListEntryWidgetHovered(bWasHovered, GetListItem()? IsListItemSelected() : false);
}

void UWidget_ListEntry_Base::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	
	//SetVisibility(ESlateVisibility::Visible);
	
	OnOwningListDataObjectSet(CastChecked<UListDataObject_Base>(ListItemObject));
}

// 当列表条目被释放时由系统自动调用（例如：条目被滚动出屏幕，或列表被刷新/重置）
void UWidget_ListEntry_Base::NativeOnEntryReleased()
{
	// 首先调用父类接口的默认实现，确保基础的清理逻辑得以执行
	IUserObjectListEntry::NativeOnEntryReleased();
    
	// 特别处理悬停状态：将条目的悬停状态显式设置为 false（非悬停）
	// 这确保了当条目被回收进对象池以备重用时，不会残留之前的交互状态[1,6](@ref)
	NativeOnListEntryWidgetHovered(false);
}

// 当该列表项控件接收到焦点时，由引擎自动调用此函数
FReply UWidget_ListEntry_Base::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	// 获取通用输入子系统，用于检测当前的输入设备类型（如键盘、鼠标、手柄、触摸屏）
	UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem();

	// 检查：1. 输入子系统有效；2. 当前输入设备是游戏手柄
	if (CommonInputSubsystem && CommonInputSubsystem->GetCurrentInputType() == ECommonInputType::Gamepad)
	{
		// 调用一个蓝图可实现的函数，获取当使用手柄时，此列表项内具体哪个子控件应该获得焦点
		// 这通常由设计师在子类的Widget Blueprint中重写并指定
		if (UWidget* WidgetToFocus = BP_GetWidgetToFocusForGamepad())
		{	
			// 获取该子控件底层对应的Slate控件（UE的底层UI框架）
			if (TSharedPtr<SWidget> SlateWidgetToFocus = WidgetToFocus->GetCachedWidget())
			{
				// 核心操作：将焦点设置给指定的子控件，并返回"已处理"的响应
				// 这能确保手柄导航时焦点能精确落到列表项内的按钮或其他可交互元素上
				return FReply::Handled().SetUserFocus(SlateWidgetToFocus.ToSharedRef());
			}		
		}
	}

	// 如果不是游戏手柄输入，或者未能成功设置焦点，则交由父类的默认逻辑处理
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}


void UWidget_ListEntry_Base::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	if (CommonText_SettingDisplayName)
	{   //更新 UI 的显示文本
		CommonText_SettingDisplayName->SetText(InOwningListDataObject->GetDataDisplayName());
	}
	if (!InOwningListDataObject->OnListDataModified.IsBoundToObject(this))
	{
		InOwningListDataObject->OnListDataModified.AddUObject(this,&ThisClass::OnOwningListDataObjectModified);
	}
	if (!InOwningListDataObject->OnDependencyDataModified.IsBoundToObject(this))
	{
		InOwningListDataObject->OnDependencyDataModified.AddUObject(this,&ThisClass::OnOwningDependencyDataObjectModified);
	}
	OnToggleEditableState(InOwningListDataObject->IsDataCurrentlyEditable());
	CachedOwningDataObject = InOwningListDataObject;
}

void UWidget_ListEntry_Base::OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData,
	EOptionsListDataModifyReason ModifyReason)
{
}

void UWidget_ListEntry_Base::OnOwningDependencyDataObjectModified(UListDataObject_Base* OwningModifiedDependencyData,
	EOptionsListDataModifyReason ModifyReason)
{
	if (CachedOwningDataObject)
	{
		OnToggleEditableState(CachedOwningDataObject->IsDataCurrentlyEditable());
	}
}

void UWidget_ListEntry_Base::OnToggleEditableState(bool bIsEditable)
{
	if (CommonText_SettingDisplayName)
	{
		CommonText_SettingDisplayName->SetIsEnabled(bIsEditable);
	}
}

void UWidget_ListEntry_Base::SelectThisEntryWidget()
{
	CastChecked<UListView>(GetOwningListView())->SetSelectedItem(GetListItem());
}
