// Tcohneyn All Rights Reserved


#include "Widgets/Options/ListEntries/Widget_ListEntry_String.h"

#include "CommonInputSubsystem.h"
#include "Widgets/Components/FrontendCommonButtonBase.h"
#include "Widgets/Components/FrontendCommonRotator.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"

#include "FrontendDebugHelper.h"
//执行一次性的初始化操作，特别是事件绑定（如按钮点击）和获取对其他组件或对象的引用
void UWidget_ListEntry_String::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	CommonButton_PreviousOption->OnClicked().AddUObject(this,&ThisClass::OnPreviousOptionButtonClicked);
	CommonButton_NextOption->OnClicked().AddUObject(this,&ThisClass::OnNextOptionButtonClicked);

	CommonRotator_AvailableOptions->OnClicked().AddLambda([this](){SelectThisEntryWidget();});
	
	CommonRotator_AvailableOptions->OnRotatedEvent.AddUObject(this,&ThisClass::OnRotatorValueChanged);
}

void UWidget_ListEntry_String::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);
	CachedOwningStringDataObject = CastChecked<UListDataObject_String>(InOwningListDataObject);

	CommonRotator_AvailableOptions->PopulateTextLabels(CachedOwningStringDataObject->GetAvailableOptionsTextArray());
	CommonRotator_AvailableOptions->SetSelectedOptionByText(CachedOwningStringDataObject->GetCurrentDisplayText());
}

void UWidget_ListEntry_String::OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData,
	EOptionsListDataModifyReason ModifyReason)
{
	if (CachedOwningStringDataObject)
	{
		CommonRotator_AvailableOptions->SetSelectedOptionByText(CachedOwningStringDataObject->GetCurrentDisplayText());
	}
}

void UWidget_ListEntry_String::OnToggleEditableState(bool bIsEditable)
{
	Super::OnToggleEditableState(bIsEditable);

	CommonButton_PreviousOption->SetIsEnabled(bIsEditable);
	CommonRotator_AvailableOptions->SetIsEnabled(bIsEditable);
	CommonButton_NextOption->SetIsEnabled(bIsEditable);
}

void UWidget_ListEntry_String::OnPreviousOptionButtonClicked()
{	
	if (CachedOwningStringDataObject)
	{
		CachedOwningStringDataObject->BackToPreviousOption();
	}
	SelectThisEntryWidget();
}

void UWidget_ListEntry_String::OnNextOptionButtonClicked()
{
	if (CachedOwningStringDataObject)
	{
		CachedOwningStringDataObject->AdvanceToNextOption();
	}
	SelectThisEntryWidget();
}

// 当关联的旋转选择器（Rotator）的值发生变化时调用此函数
void UWidget_ListEntry_String::OnRotatorValueChanged(int32 Value, bool bUserInitiated)
{
	// 安全检查：确保该控件关联的底层数据对象有效
	if (!CachedOwningStringDataObject)
	{
		return; // 如果数据对象无效，直接返回以避免程序崩溃
	}

	// 获取通用输入子系统，用于检测当前的输入设备类型（如键盘、鼠标、游戏手柄）
	UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem();

	// 检查输入子系统是否有效，并且确认此次值变更是由用户交互触发的（而非程序代码设置）
	// 这可以防止在数据初始化或程序化更新时触发不必要的逻辑
	if (!CommonInputSubsystem || !bUserInitiated)
	{
		return;
	}

	// 关键判断：检查当前是否使用游戏手柄进行输入
	if (CommonInputSubsystem->GetCurrentInputType() == ECommonInputType::Gamepad)
	{
		// 如果是游戏手柄操作，则通知关联的数据对象：由旋转选择器发起了一次值变更
		// 获取旋转选择器当前选中的显示文本，并将其传递给数据对象进行处理
		CachedOwningStringDataObject->OnRotatorInitiatedValueChange(CommonRotator_AvailableOptions->GetSelectedText());
	}
	// 注意：如果当前是鼠标或键盘输入，这里没有对应的else分支，意味着这些输入方式可能由其他逻辑处理，或者此函数只专注于手柄输入。
}
