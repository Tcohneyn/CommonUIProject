// Tcohneyn All Rights Reserved


#include "Widgets/Components/FrontendCommonRotator.h"
#include "CommonTextBlock.h"

// 根据传入的文本选项设置旋转选择器的选中项
void UFrontendCommonRotator::SetSelectedOptionByText(const FText& InTextOption)
{
	// 在TextLabels数组中查找与传入文本匹配的项的索引
	// 使用IndexOfByPredicate和Lambda表达式进行文本比较
	const int32 FoundIndex = TextLabels.IndexOfByPredicate(
		// Lambda表达式：比较数组中的每个文本项是否与目标文本相同
		[InTextOption](const FText& TextItem)->bool
		{
			// 使用EqualTo方法进行精确的文本比较（考虑本地化）
			return TextItem.EqualTo(InTextOption);
		}
	);

	// 检查是否找到了匹配的索引（INDEX_NONE表示未找到）
	if (FoundIndex != INDEX_NONE)
	{
		// 如果找到匹配项，将旋转选择器设置为对应的索引
		SetSelectedItem(FoundIndex);
	}
	else
	{
		// 如果没有找到匹配项，直接将传入的文本设置为显示文本
		// 这提供了回退机制，确保即使没有精确匹配也能更新显示
		MyText->SetText(InTextOption);
	}
}
