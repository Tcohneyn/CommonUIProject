// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "FrontendDeveloperSettings.generated.h"

class UWidget_ActivatableBase;
/**
 * 前端UI开发者设置类。
 * 此类的实例是一个数据资产，其属性可以在UE编辑器的项目设置（Project Settings）窗口中直接配置。
 * 它作为前端UI资源的中央仓库，使用GameplayTag进行映射，实现数据驱动的资源管理。
 */
UCLASS(Config = Game, defaultconfig,meta =(DisplayName = "Frontend UI Settings"))
class COMMONUIPROJECT_API UFrontendDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	/**
	 * 前端控件映射表。
	 * 将GameplayTag关联到可激活控件类的软引用（Soft Reference）。
	 * - 用途：用于在运行时根据标签动态、异步地加载和实例化UI控件（如各种菜单、弹窗）。
	 * - 配置位置：在编辑器项目设置的 "Frontend UI Settings" 分类下的 "Widget Reference" 分组中。
	 * - 示例：Tag "Frontend.Widget.MainMenu" 可以映射到软引用 "/Game/UI/Widgets/MainMenu.MainMenu_C"。
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Widget Reference", meta = (ForceInlineRow, Categories = "Frontend.Widget"))
	TMap<FGameplayTag, TSoftClassPtr<UWidget_ActivatableBase>> FrontendWidgetMap;

	/**
	 * 选项界面图片资源映射表。
	 * 将GameplayTag关联到纹理资源（UTexture2D）的软引用。
	 * - 用途：用于在运行时根据标签动态加载选项菜单、设置界面等所需的图标和图片。
	 * - 配置位置：在编辑器项目设置的 "Frontend UI Settings" 分类下的 "Options Image Reference" 分组中。
	 * - 示例：Tag "Frontend.Image.Graphic.Quality" 可以映射到一张代表画质等级的图标纹理。
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Options Image Reference", meta = (ForceInlineRow, Categories = "Frontend.Image"))
	TMap<FGameplayTag, TSoftObjectPtr<UTexture2D>> OptionsScreenSoftImageMap;
};
