// Tcohneyn All Rights Reserved


#include "Widgets/Options/OptionsDataRegistry.h"

#include "FrontendFunctionLibrary.h"
#include "FrontendGameplayTags.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "FrontendSettings/FrontendGameUserSettings.h"

#define MAKE_OPTIONS_DATA_CONTROL(SetterOrGetterFuncName) \
MakeShared<FOptionsDataInteractionHelper>(GET_FUNCTION_NAME_STRING_CHECKED(UFrontendGameUserSettings,SetterOrGetterFuncName))
/**
 * 选项数据注册表的主要初始化入口。
 * 该函数在注册表创建时被调用（例如由游戏实例或玩家控制器），负责初始化所有选项标签页的数据集合。
 * @param InOwningLocalPlayer - 拥有此注册表的本地玩家对象，可用于后续的上下文相关操作[2](@ref)
 */
void UOptionsDataRegistry::InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer)
{
	InitGameplayCollectionTab();
	InitAudioCollectionTab();
	InitVideoCollectionTab();
	InitControlCollectionTab();
}

// 根据选中的标签页ID，获取该标签页下的所有子数据项列表
TArray<UListDataObject_Base*> UOptionsDataRegistry::GetListSourceItemsBySelectedTabID(const FName& InSelectedTabID) const
{
	// 使用FindByPredicate算法在已注册的标签页集合中查找匹配的标签页
	// FindByPredicate会遍历数组，对每个元素执行给定的Lambda表达式，返回第一个匹配元素的指针
	UListDataObject_Collection* const* FoundTabCollectionPtr = RegisteredOptionsTabCollections.FindByPredicate(
		// Lambda表达式：判断当前标签页的ID是否与目标ID匹配
		[InSelectedTabID](UListDataObject_Collection* AvailableTabCollection)->bool//显式指定这个匿名函数的返回类型为bool
		{
			// 比较当前可用标签页的DataID是否等于传入的选中标签页ID
			return AvailableTabCollection->GetDataID() == InSelectedTabID;
		}
	);

	// 检查是否找到了有效的标签页，如果未找到则触发断言错误（在开发阶段立即暴露问题）
	// checkf宏在调试模式下会触发断言，在发布模式下会被移除
	checkf(FoundTabCollectionPtr, TEXT("在注册的选项标签页集合中未找到ID为 %s 的标签页"), *InSelectedTabID.ToString());

	// 解引用指针，获取实际的标签页集合对象
	UListDataObject_Collection* FoundTabCollection = *FoundTabCollectionPtr;

	// 返回该标签页集合中的所有子数据项
	return FoundTabCollection->GetAllChildListData();
}

/**
 * 初始化"游戏玩法"（Gameplay）标签页的数据集合。
 * 此函数创建了一个集合（Collection）作为标签页容器，并向其中添加具体的选项项（例如"游戏难度"）。
 * 在虚幻引擎的资产管理中，这种层级结构常用于组织UI数据[2](@ref)
 */
void UOptionsDataRegistry::InitGameplayCollectionTab()
{
	// 创建一个新的集合数据对象，用于代表整个"游戏玩法"标签页
	UListDataObject_Collection* GameplayTabCollection = NewObject<UListDataObject_Collection>();
	// 设置该集合的唯一标识符，通常用于后续的查找和引用
	GameplayTabCollection->SetDataID(FName("GameplayTabCollection"));
	// 设置显示给用户的标签页名称
	GameplayTabCollection->SetDataDisplayName(FText::FromString(TEXT("Gameplay")));

	// //这是构建数据交互助手（data interactor helper）的完整代码
	// TSharedPtr<FOptionsDataInteractionHelper> ConstructedHelper = 
	// MakeShared<FOptionsDataInteractionHelper>(
	// 	GET_FUNCTION_NAME_STRING_CHECKED(UFrontendGameUserSettings,GetCurrentGameDifficulty)
	// );
	
	// 区域注释：开始定义"游戏难度"选项项
	//Game Difficulty
	{
		// 创建一个字符串类型的数据对象，用于表示一个具体的选项（如下拉菜单或选择器）
		UListDataObject_String* GameDifficulty = NewObject<UListDataObject_String>();
		GameDifficulty->SetDataID(FName("GameDifficulty")); // 设置选项ID
		GameDifficulty->SetDataDisplayName(FText::FromString("Difficulty")); // 设置选项显示名

		GameDifficulty->SetDescriptionRichText(FText::FromString(TEXT("Adjusts the difficulty of the game experience.\n\n<Bold>Easy:</> Focuses on the story experience. Provides the most relaxing combat.\n\n<Bold>Normal:</> Offers slightly harder combat experience\n\n<Bold>Hard:</> Offers a much more challenging combat experience\n\n<Bold>Vert Hard:</> Provides the most challenging combat experience. Not recommended for first play through.")));
		GameDifficulty->AddDynamicOption(TEXT("Easy"),FText::FromString(TEXT("Easy")));
		GameDifficulty->AddDynamicOption(TEXT("Normal"),FText::FromString(TEXT("Normal")));
		GameDifficulty->AddDynamicOption(TEXT("Hard"),FText::FromString(TEXT("Hard")));
		GameDifficulty->AddDynamicOption(TEXT("Very Hard"),FText::FromString(TEXT("Very Hard")));
		GameDifficulty->SetDefaultValueFromString(TEXT("Normal"));
		GameDifficulty->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetCurrentGameDifficulty));
		GameDifficulty->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetCurrentGameDifficulty));
		GameDifficulty->SetShouldApplySettingsImmediately(true);
		// 将"游戏难度"选项添加到"游戏玩法"标签页集合中
		GameplayTabCollection->AddChildListData(GameDifficulty);
	}

	// 区域注释：开始定义一个测试选项项
	//Test Item
	{
		// 创建另一个测试用的选项项
		UListDataObject_String* TestItem = NewObject<UListDataObject_String>();
		TestItem->SetDataID(FName("TestItem"));
		TestItem->SetDataDisplayName(FText::FromString(TEXT("Test Image Item")));	
		TestItem->SetSoftDescriptionImage(UFrontendFunctionLibrary::GetOptionsSoftImageByTag(FrontendGameplayTags::Frontend_Image_TestImage));
		TestItem->SetDescriptionRichText(FText::FromString(TEXT("The image to display can be specified in the project settings. It can be anything the developer assigned in there")));
		// 将测试项添加到集合中
		GameplayTabCollection->AddChildListData(TestItem);
	}

	// 将完全初始化好的"游戏玩法"标签页集合添加到注册表的全局列表中保存
	RegisteredOptionsTabCollections.Add(GameplayTabCollection);
}
/**
 * 初始化"音频"（Audio）标签页的数据集合。
 * 当前实现仅创建了一个空的标签页容器，后续可在此添加具体的音频选项（如主音量、音效音量等）。
 */
void UOptionsDataRegistry::InitAudioCollectionTab()
{
	UListDataObject_Collection* AudioTabCollection = NewObject<UListDataObject_Collection>();
	AudioTabCollection->SetDataID(FName("AudioTabCollection"));
	AudioTabCollection->SetDataDisplayName(FText::FromString(TEXT("Audio")));

	RegisteredOptionsTabCollections.Add(AudioTabCollection);
}
/**
 * 初始化"视频"（Video）标签页的数据集合。
 * 当前实现仅创建了一个空的标签页容器，后续可在此添加视频选项（如分辨率、画质设置等）。
 */
void UOptionsDataRegistry::InitVideoCollectionTab()
{
	UListDataObject_Collection* VideoTabCollection = NewObject<UListDataObject_Collection>();
	VideoTabCollection->SetDataID(FName("VideoTabCollection"));
	VideoTabCollection->SetDataDisplayName(FText::FromString(TEXT("Video")));

	RegisteredOptionsTabCollections.Add(VideoTabCollection);
}
/**
 * 初始化"控制"（Control）标签页的数据集合。
 * 当前实现仅创建了一个空的标签页容器，后续可在此添加控制选项（如按键绑定、鼠标灵敏度等）。
 */
void UOptionsDataRegistry::InitControlCollectionTab()
{
	UListDataObject_Collection* ControlTabCollection = NewObject<UListDataObject_Collection>();
	ControlTabCollection->SetDataID(FName("ControlTabCollection"));
	ControlTabCollection->SetDataDisplayName(FText::FromString(TEXT("Control")));

	RegisteredOptionsTabCollections.Add(ControlTabCollection);
}
