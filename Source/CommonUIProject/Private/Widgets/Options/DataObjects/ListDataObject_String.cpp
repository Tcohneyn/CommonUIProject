// Tcohneyn All Rights Reserved


#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "FrontendDebugHelper.h"

void UListDataObject_String::OnDataObjectInitialized()
{
	if (!AvailableOptionsStringArray.IsEmpty())
	{
		CurrentStringValue = AvailableOptionsStringArray[0];
	}

	//从保存的字符串值中读取，并用它来设置 CurrentStringValue
	if (HasDefaultValue())
	{
		CurrentStringValue = GetDefaultValueAsString();
	}
	// 数据动态获取与更新逻辑
	// 检查数据获取器（DataDynamicGetter）是否存在且有效
	if (DataDynamicGetter)
	{
		// 调用获取器获取字符串值，并检查返回值是否非空
		// 此检查避免用空值覆盖已有的有效数据
		if (!DataDynamicGetter->GetValueAsString().IsEmpty())
		{
			// 将获取到的有效字符串值赋给当前字符串变量
			// 完成从数据源到本地状态的安全同步
			CurrentStringValue = DataDynamicGetter->GetValueAsString();
		}
	}
	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentDisplayText = FText::FromString(TEXT("Invalid Option"));
	}
}
void UListDataObject_String::AddDynamicOption(const FString& InStringValue, const FText& InDisplayText)
{
	AvailableOptionsStringArray.Add(InStringValue);
	AvailableOptionsTextArray.Add(InDisplayText);
}
void UListDataObject_String::AdvanceToNextOption()
{
	// 安全检查：如果选项数组或显示文本数组为空，直接返回，避免后续操作出错
	if (AvailableOptionsStringArray.IsEmpty() || AvailableOptionsTextArray.IsEmpty())
	{
		return;
	}

	// 查找当前字符串值在选项数组中的索引位置
	const int32 CurrentDisplayIndex = AvailableOptionsStringArray.IndexOfByKey(CurrentStringValue);
	// 计算下一个选项的索引（当前索引+1）
	const int32 NextIndexToDisplay = CurrentDisplayIndex + 1;

	// 检查计算出的下一个索引是否在数组有效范围内
	const bool bIsNextIndexValid = AvailableOptionsStringArray.IsValidIndex(NextIndexToDisplay);

	// 如果下一个索引有效，则将当前值更新为下一个选项的值
	if (bIsNextIndexValid)
	{
		CurrentStringValue = AvailableOptionsStringArray[NextIndexToDisplay];
	}
	else
	{
		// 如果下一个索引无效（说明当前已是最后一项），则循环回到第一项（索引0）
		CurrentStringValue = AvailableOptionsStringArray[0];
	}

	// 根据新的当前字符串值更新对应的显示文本
	TrySetDisplayTextFromStringValue(CurrentStringValue);
	
	// 检查数据设置器（DataDynamicSetter）是否有效
	if (DataDynamicSetter)
	{
		// 核心操作：将当前的字符串值（CurrentStringValue）设置到底层数据模型中
		DataDynamicSetter->SetValueFromString(CurrentStringValue);

		Debug::Print(TEXT("DataDynamicSetter is used. The latest value from Getter: ") + DataDynamicGetter->GetValueAsString());
		// 关键通知：告知系统该数据对象已被修改，需要更新相关UI
		NotifyListDataModified(this);
	}
}

void UListDataObject_String::BackToPreviousOption()
{
	// 安全检查：如果选项数组或显示文本数组为空，直接返回
	if (AvailableOptionsStringArray.IsEmpty() || AvailableOptionsTextArray.IsEmpty())
	{
		return;
	}

	// 查找当前字符串值在选项数组中的索引位置
	const int32 CurrentDisplayIndex = AvailableOptionsStringArray.IndexOfByKey(CurrentStringValue);
	// 计算上一个选项的索引（当前索引-1）
	const int32 PreviousIndexToDisplay = CurrentDisplayIndex - 1;

	// 检查计算出的上一个索引是否在数组有效范围内
	const bool bIsPreviousIndexValid = AvailableOptionsStringArray.IsValidIndex(PreviousIndexToDisplay);

	// 如果上一个索引有效，则将当前值更新为上一个选项的值
	if (bIsPreviousIndexValid)
	{
		CurrentStringValue = AvailableOptionsStringArray[PreviousIndexToDisplay];
	}
	else
	{
		// 如果上一个索引无效（说明当前已是第一项），则循环跳转到最后一项
		CurrentStringValue = AvailableOptionsStringArray.Last();
	}

	// 根据新的当前字符串值更新对应的显示文本
	TrySetDisplayTextFromStringValue(CurrentStringValue);
	
	if (DataDynamicSetter)
	{
		DataDynamicSetter->SetValueFromString(CurrentStringValue);

		Debug::Print(TEXT("DataDynamicSetter is used. The latest value from Getter: ") + DataDynamicGetter->GetValueAsString());
		NotifyListDataModified(this);
	}
}

// 当通过旋转选择器等UI组件发起值变更时调用此函数
void UListDataObject_String::OnRotatorInitiatedValueChange(const FText& InNewSelectedText)
{
	// 在可用的显示文本数组中查找新选中的文本所对应的索引
	// 使用Lambda表达式进行精确的文本匹配
	const int32 FoundIndex = AvailableOptionsTextArray.IndexOfByPredicate(
		[InNewSelectedText](const FText& AvailableText)->bool
		{
			// 使用EqualTo方法进行精确的文本比较（考虑本地化）
			return AvailableText.EqualTo(InNewSelectedText);
		}
	);

	// 检查是否找到了有效索引，并且该索引在字符串值数组中也有效
	// 这确保了显示文本和内部字符串值能够正确对应
	if (FoundIndex != INDEX_NONE && AvailableOptionsStringArray.IsValidIndex(FoundIndex))
	{
		// 更新当前显示的文本
		CurrentDisplayText = InNewSelectedText;
		// 根据找到的索引，更新对应的内部字符串值
		// 这保持了显示值与实际存储值之间的映射关系
		CurrentStringValue = AvailableOptionsStringArray[FoundIndex];

		// 如果存在数据动态设置器（用于与底层数据系统同步）
		if (DataDynamicSetter)
		{
			// 将新的内部字符串值同步到底层数据存储中
			DataDynamicSetter->SetValueFromString(CurrentStringValue);

			// 通知系统该数据对象已被修改，触发相关的UI更新或持久化逻辑
			NotifyListDataModified(this);
		}
	}
	// 注意：如果没有找到匹配项，函数将静默退出，这可以防止无效输入导致错误
}

// 判断当前数据对象是否可以重置回默认值
bool UListDataObject_String::CanResetBackToDefaultValue() const
{
	// 检查两个条件：1)是否存在默认值 2)当前值是否不等于默认值
	return HasDefaultValue() && CurrentStringValue != GetDefaultValueAsString();
}

// 尝试将数据重置回默认值，返回操作是否成功
bool UListDataObject_String::TryResetBackToDefaultValue()
{
	// 首先检查是否满足重置条件
	if (CanResetBackToDefaultValue())
	{
		// 将当前值设置为默认值
		CurrentStringValue = GetDefaultValueAsString();

		// 更新显示文本以反映新的当前值
		TrySetDisplayTextFromStringValue(CurrentStringValue);

		// 如果存在数据动态设置器，同步更新底层数据存储
		if (DataDynamicSetter)
		{
			// 将新的默认值设置到底层数据存储中
			DataDynamicSetter->SetValueFromString(CurrentStringValue);

			// 通知列表数据已修改，并指明修改原因是"重置到默认值"
			NotifyListDataModified(this, EOptionsListDataModifyReason::ResetToDefault);

			// 重置成功，返回true
			return true;
		}
	}

	// 重置失败，返回false
	return false;
}

// 尝试根据传入的字符串值设置对应的显示文本
bool UListDataObject_String::TrySetDisplayTextFromStringValue(const FString& InStringValue)
{
	// 在可用选项字符串数组中查找传入字符串值的索引位置
	// IndexOfByKey 方法会返回第一个匹配项的索引，如果未找到则返回 INDEX_NONE [6,7](@ref)
	const int32 CurrentFoundIndex = AvailableOptionsStringArray.IndexOfByKey(InStringValue);

	// 检查找到的索引在显示文本数组中是否有效
	// 使用 IsValidIndex 确保索引在 [0, AvailableOptionsTextArray.Num()-1] 范围内 [6,7](@ref)
	if (AvailableOptionsTextArray.IsValidIndex(CurrentFoundIndex))
	{
		// 如果索引有效，从显示文本数组中获取对应的显示文本
		CurrentDisplayText = AvailableOptionsTextArray[CurrentFoundIndex];

		// 设置成功，返回 true
		return true;
	}

	// 如果未找到匹配项或索引无效，返回 false 表示设置失败
	return false;
}

//************ UListDataObject_StringBool ************//

void UListDataObject_StringBool::OverrideTrueDisplayText(const FText& InNewTrueDisplayText)
{
	if (!AvailableOptionsStringArray.Contains(TrueString))
	{
		AddDynamicOption(TrueString,InNewTrueDisplayText);
	}
}

void UListDataObject_StringBool::OverrideFalseDisplayText(const FText& InNewFalseDisplayText)
{
	if (!AvailableOptionsStringArray.Contains(FalseString))
	{
		AddDynamicOption(FalseString,InNewFalseDisplayText);
	}
}

void UListDataObject_StringBool::SetTrueAsDefaultValue()
{
	SetDefaultValueFromString(TrueString);
}

void UListDataObject_StringBool::SetFalseAsDefaultValue()
{
	SetDefaultValueFromString(FalseString);
}

void UListDataObject_StringBool::OnDataObjectInitialized()
{
	TryInitBoolValues();

	Super::OnDataObjectInitialized();
}

void UListDataObject_StringBool::TryInitBoolValues()
{
	if (!AvailableOptionsStringArray.Contains(TrueString))
	{
		AddDynamicOption(TrueString,FText::FromString(TEXT("ON")));
	}

	if (!AvailableOptionsStringArray.Contains(FalseString))
	{
		AddDynamicOption(FalseString,FText::FromString(TEXT("OFF")));
	}
}

//************ UListDataObject_StringBool ************//