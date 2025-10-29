// Tcohneyn All Rights Reserved


#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "FrontendSettings/FrontendGameUserSettings.h"


// 选项数据交互辅助类，用于通过属性路径动态获取和设置游戏用户设置中的属性值
FOptionsDataInteractionHelper::FOptionsDataInteractionHelper(const FString& InSetterOrGetterFuncPath)
	: CachedDynamicFunctionPath(InSetterOrGetterFuncPath)  // 初始化缓存属性路径（如"GraphicsSettings.Resolution"）
{
	// 获取游戏用户设置单例实例的弱指针，避免强引用导致的内存管理问题
	CachedWeakGameUserSettings = UFrontendGameUserSettings::Get();
}

// 获取当前属性路径对应的属性值，并将其转换为字符串格式返回
FString FOptionsDataInteractionHelper::GetValueAsString() const
{	
	FString OutStringValue;  // 存储获取到的字符串值
	
	// 使用属性路径帮助类，从游戏用户设置中根据路径获取属性值
	PropertyPathHelpers::GetPropertyValueAsString(
		CachedWeakGameUserSettings.Get(),  // 获取弱指针指向的实际用户设置对象
		CachedDynamicFunctionPath,         // 属性路径（如"GraphicsSettings.Resolution"）
		OutStringValue                    // 输出参数，接收转换后的字符串值
	);

	return OutStringValue;  // 返回获取到的字符串值
}

// 将传入的字符串值转换为对应类型，并设置到属性路径指定的属性中
void FOptionsDataInteractionHelper::SetValueFromString(const FString& InStringValue)
{
	// 使用属性路径帮助类，将字符串值转换并设置到游戏用户设置的指定属性中
	PropertyPathHelpers::SetPropertyValueFromString(
		CachedWeakGameUserSettings.Get(),  // 获取弱指针指向的实际用户设置对象
		CachedDynamicFunctionPath,         // 属性路径（如"GraphicsSettings.Resolution"）
		InStringValue                     // 输入的字符串值，将被转换为属性实际类型
	);
}
