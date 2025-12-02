// Tcohneyn All Rights Reserved


#include "Widgets/Options/OptionsDataRegistry.h"

#include "FrontendFunctionLibrary.h"
#include "FrontendGameplayTags.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "FrontendSettings/FrontendGameUserSettings.h"
#include "Widgets/Options/DataObjects/ListDataObject_Scalar.h"

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

	TArray<UListDataObject_Base*> AllChildListItems;

	for (UListDataObject_Base* ChildListData : FoundTabCollection->GetAllChildListData())
	{
		if (!ChildListData)
		{
			continue;
		}

		AllChildListItems.Add(ChildListData);

		if (ChildListData->HasAnyChildListData())
		{
			FindChildListDataRecursively(ChildListData,AllChildListItems);
		}
	}

	return AllChildListItems;
}

void UOptionsDataRegistry::FindChildListDataRecursively(UListDataObject_Base* InParentData,
	TArray<UListDataObject_Base*>& OutFoundChildListData) const
{
	// 1. 检查父数据对象和子数据
	if (!InParentData || !InParentData->HasAnyChildListData())
	{
		// 如果 InParentData 为空，或者它没有子列表数据，则直接返回，终止递归
		return;
	}

	// 2. 遍历当前父数据对象的所有直接子列表数据
	for (UListDataObject_Base* SubChildListData : InParentData->GetAllChildListData())
	{
		// 3. 检查当前子列表数据是否有效
		if (!SubChildListData)
		{
			// 如果子列表数据为空，则跳过当前循环，处理下一个子项
			continue;
		}

		// 4. 将当前子列表数据添加到输出数组中
		OutFoundChildListData.Add(SubChildListData);

		// 5. 递归调用
		if (SubChildListData->HasAnyChildListData())
		{
			// 如果当前子列表数据自身还有子列表数据，则以它为新的父数据对象进行递归调用
			// 这一步是实现“深度优先”遍历的关键
			FindChildListDataRecursively(SubChildListData,OutFoundChildListData);
		}
	}
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
	//音量分类
	{
		UListDataObject_Collection* VolumeCategoryCollection = NewObject<UListDataObject_Collection>();
		VolumeCategoryCollection->SetDataID(FName("VolumeCategoryCollection"));
		VolumeCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Volume")));

		AudioTabCollection->AddChildListData(VolumeCategoryCollection);

		//Overall Volume
		{
			UListDataObject_Scalar* OverallVolume = NewObject<UListDataObject_Scalar>();
			OverallVolume->SetDataID(FName("OverallVolume"));
			OverallVolume->SetDataDisplayName(FText::FromString(TEXT("Overall Volume")));
			OverallVolume->SetDescriptionRichText(FText::FromString(TEXT("This is description for Overall Volume")));
			// 设置显示值的范围：滑块在UI上显示为0%到100%
			OverallVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
    
			// 设置实际输出值的范围：内部处理时映射到0.0到2.0的范围
			OverallVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));
    
			// 设置滑块步长：每次调整变化0.01（1%）
			OverallVolume->SetSliderStepSize(0.01f);
    
			// 设置默认值：1.0（100%音量）
			OverallVolume->SetDefaultValueFromString(LexToString(1.f));
    
			// 设置显示格式为百分比（如显示50%而不是0.5）
			OverallVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
    
			// 设置数字格式化选项：不显示小数位（显示为50%而不是50.0%）
			OverallVolume->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());
    
			OverallVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetOverallVolume));
			OverallVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetOverallVolume));
			OverallVolume->SetShouldApplySettingsImmediately(true);
		
			VolumeCategoryCollection->AddChildListData(OverallVolume);
		}
		//Music Volume
		{
			UListDataObject_Scalar* MusicVolume = NewObject<UListDataObject_Scalar>();
			MusicVolume->SetDataID(FName("MusicVolume"));
			MusicVolume->SetDataDisplayName(FText::FromString(TEXT("Music Volume")));
			MusicVolume->SetDescriptionRichText(FText::FromString(TEXT("This is description for Music Volume")));
			MusicVolume->SetDisplayValueRange(TRange<float>(0.f,1.f));
			MusicVolume->SetOutputValueRange(TRange<float>(0.f,2.f));
			MusicVolume->SetSliderStepSize(0.01f);
			MusicVolume->SetDefaultValueFromString(LexToString(1.f));
			MusicVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
			MusicVolume->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());  //No Decimal: 50%  //One Decimal: 50.5%
			MusicVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetMusicVolume));
			MusicVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetMusicVolume));
			MusicVolume->SetShouldApplySettingsImmediately(true);

			VolumeCategoryCollection->AddChildListData(MusicVolume);
		}
		//Sound FX Volume
		{
			UListDataObject_Scalar* SoundFXVolume = NewObject<UListDataObject_Scalar>();
			SoundFXVolume->SetDataID(FName("SoundFXVolume"));
			SoundFXVolume->SetDataDisplayName(FText::FromString(TEXT("Sound Effects Volume")));
			SoundFXVolume->SetDescriptionRichText(FText::FromString(TEXT("This is description for Sound Effects Volume")));
			SoundFXVolume->SetDisplayValueRange(TRange<float>(0.f,1.f));
			SoundFXVolume->SetOutputValueRange(TRange<float>(0.f,2.f));
			SoundFXVolume->SetSliderStepSize(0.01f);
			SoundFXVolume->SetDefaultValueFromString(LexToString(1.f));
			SoundFXVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
			SoundFXVolume->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());  //No Decimal: 50%  //One Decimal: 50.5%
			SoundFXVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetSoundFXVolume));
			SoundFXVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetSoundFXVolume));
			SoundFXVolume->SetShouldApplySettingsImmediately(true);

			VolumeCategoryCollection->AddChildListData(SoundFXVolume);
		}
		//声音目录
		{
			UListDataObject_Collection* SoundCategoryCollection = NewObject<UListDataObject_Collection>();
			SoundCategoryCollection->SetDataID(FName("SoundCategoryCollection"));
			SoundCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Sound")));
			AudioTabCollection->AddChildListData(SoundCategoryCollection);

			//允许背景音乐
			{
				UListDataObject_StringBool* AllowBackgroundAudio = NewObject<UListDataObject_StringBool>();
				AllowBackgroundAudio->SetDataID(FName("AllowBackgroundAudio"));
				AllowBackgroundAudio->SetDataDisplayName(FText::FromString(TEXT("Allow Background Audio")));
				AllowBackgroundAudio->OverrideTrueDisplayText(FText::FromString(TEXT("Enabled")));
				AllowBackgroundAudio->OverrideFalseDisplayText(FText::FromString(TEXT("Disabled")));
				AllowBackgroundAudio->SetFalseAsDefaultValue();
				AllowBackgroundAudio->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetAllowBackgroundAudio));
				AllowBackgroundAudio->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetAllowBackgroundAudio));
				AllowBackgroundAudio->SetShouldApplySettingsImmediately(true);

				SoundCategoryCollection->AddChildListData(AllowBackgroundAudio);
			}
			//启用HDR音频
			{
				UListDataObject_StringBool* UseHDRAudioMode = NewObject<UListDataObject_StringBool>();
				UseHDRAudioMode->SetDataID(FName("UseHDRAudioMode"));
				UseHDRAudioMode->SetDataDisplayName(FText::FromString(TEXT("Use HDR Audio Mode")));
				UseHDRAudioMode->OverrideTrueDisplayText(FText::FromString(TEXT("Enabled")));
				UseHDRAudioMode->OverrideFalseDisplayText(FText::FromString(TEXT("Disabled")));
				UseHDRAudioMode->SetFalseAsDefaultValue();
				UseHDRAudioMode->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetUseHDRAudioMode));
				UseHDRAudioMode->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetUseHDRAudioMode));
				UseHDRAudioMode->SetShouldApplySettingsImmediately(true);

				SoundCategoryCollection->AddChildListData(UseHDRAudioMode);
			}
		}
	}
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
