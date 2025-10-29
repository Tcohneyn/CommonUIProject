// Tcohneyn All Rights Reserved


#include "FrontendFunctionLibrary.h"
#include "FrontendSettings/FrontendDeveloperSettings.h"

TSoftClassPtr<UWidget_ActivatableBase> UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(UPARAM(meta = (Categories = "Frontend.Widget")) FGameplayTag InWidgetTag)
{	
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();

	checkf(FrontendDeveloperSettings->FrontendWidgetMap.Contains(InWidgetTag),TEXT("Could not find the corresponding widget under the tag %s"),*InWidgetTag.ToString());

	return FrontendDeveloperSettings->FrontendWidgetMap.FindRef(InWidgetTag);
}

// 函数功能：通过游戏标签获取选项界面对应的软引用纹理
// 参数：InImageTag - 用于标识特定图片的游戏标签（如"UI.Options.Graphic"）
// 返回值：TSoftObjectPtr<UTexture2D> - 一个指向UTexture2D的软引用指针，可安全地延迟加载纹理资源
TSoftObjectPtr<UTexture2D> UFrontendFunctionLibrary::GetOptionsSoftImageByTag(FGameplayTag InImageTag)
{
	// 获取前端开发者设置的默认实例（单例模式）
	// 这些设置通常在编辑器中配置并存储在配置文件中
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();
    
	// 安全检查：验证传入的标签是否在映射表中存在
	// 如果不存在，在开发阶段会触发断言错误，提示缺失的标签名称
	// 这有助于在开发早期发现配置错误，避免运行时崩溃 [6,7](@ref)
	checkf(FrontendDeveloperSettings->OptionsScreenSoftImageMap.Contains(InImageTag),
		   TEXT("Could not find an image associated with tag %s"), 
		   *InImageTag.ToString());
    
	// 使用FindRef方法从映射表中查找并返回对应的软引用纹理
	// FindRef在键不存在时会返回空值而非崩溃，与checkf配合使用既安全又严格
	return FrontendDeveloperSettings->OptionsScreenSoftImageMap.FindRef(InImageTag);
}