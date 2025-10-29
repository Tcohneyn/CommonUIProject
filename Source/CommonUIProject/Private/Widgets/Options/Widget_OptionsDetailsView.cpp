// Tcohneyn All Rights Reserved


#include "Widgets/Options/Widget_OptionsDetailsView.h"
#include "CommonTextBlock.h"
#include "CommonLazyImage.h"
#include "CommonRichTextBlock.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"
//更新详情视图信息
void UWidget_OptionsDetailsView::UpdateDetailsViewInfo(UListDataObject_Base* InDataObject, const FString& InEntryWidgetClassName)
{
	// 安全检查：确保传入的数据对象有效，避免操作空指针
	if (!InDataObject)
	{
		return;
	}

	// 设置标题：直接使用数据对象的显示名称
	CommonTextBlock_Title->SetText(InDataObject->GetDataDisplayName());

	// 设置描述图片（懒加载）：检查软引用是否有效，有效则设置纹理并显示图片控件
	if (!InDataObject->GetSoftDescriptionImage().IsNull())
	{
		CommonLazyImage_DescriptionImage->SetBrushFromLazyTexture(InDataObject->GetSoftDescriptionImage());
		CommonLazyImage_DescriptionImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		CommonLazyImage_DescriptionImage->SetVisibility(ESlateVisibility::Collapsed);
	}
	// 设置描述文本：使用富文本格式，支持样式和布局
	CommonRichText_Description->SetText(InDataObject->GetDescriptionRichText());

	// 构建并设置动态详情信息：格式化字符串显示数据对象类和其对应的界面控件类
	const FString DynamicDetails = FString::Printf(
		TEXT("Data Object Class: <Bold>%s</>\n\nEntry Widget Class:<Bold>%s</>"),
		*InDataObject->GetClass()->GetName(),      // 数据对象的C++类名
		*InEntryWidgetClassName                    // 关联的界面控件蓝图类名
	);
	CommonRichText_DynamicDetails->SetText(FText::FromString(DynamicDetails));

	// 设置禁用原因提示：当选项被禁用时，显示原因（例如"需要先完成前置任务"）
	CommonRichText_DisabledReason->SetText(InDataObject->GetDisabledRichText());
}
//清空详情视图信息
void UWidget_OptionsDetailsView::ClearDetailsViewInfo()
{
	CommonTextBlock_Title->SetText(FText::GetEmpty()); // 清空标题文本
	CommonLazyImage_DescriptionImage->SetVisibility(ESlateVisibility::Collapsed); // 隐藏图片
	CommonRichText_Description->SetText(FText::GetEmpty()); // 清空描述
	CommonRichText_DynamicDetails->SetText(FText::GetEmpty()); // 清空动态详情
	CommonRichText_DisabledReason->SetText(FText::GetEmpty()); // 清空禁用原因
}

void UWidget_OptionsDetailsView::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ClearDetailsViewInfo();
}
