// Tcohneyn All Rights Reserved


#include "Subsystems/FrontendUISubsystem.h"
#include "Engine/AssetManager.h"
#include "Widgets/Widget_PrimaryLayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "FrontendDebugHelper.h"
#include "FrontendFunctionLibrary.h"
#include "FrontendGameplayTags.h"
#include "Widgets/Widget_ConfirmScreen.h"

// 获取前端UI子系统的全局实例，便于在游戏任何地方访问
UFrontendUISubsystem* UFrontendUISubsystem::Get(const UObject* WorldContextObject)
{
	// 安全检查：确保引擎已初始化
	if (GEngine)
	{
		// 从世界上下文对象中获取有效的世界指针
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
        
		// 从游戏实例中获取此子系统的单例实例
		return UGameInstance::GetSubsystem<UFrontendUISubsystem>(World->GetGameInstance());
	}
	return nullptr; // 引擎未就绪时返回空指针
}

// 重写此函数以控制子系统何时被创建，避免不必要的实例化
bool UFrontendUISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// 检查：确保不在专用服务器上运行（专用服务器不需要UI）
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		// 检查是否存在此类的派生类，避免基类与派生类同时实例化
		TArray<UClass*> FoundClasses;
		GetDerivedClasses(GetClass(), FoundClasses);
        
		// 仅当没有派生类时才创建此基类实例，确保单一实例原则
		return FoundClasses.IsEmpty();
	}
    
	// 专用服务器上不创建UI子系统
	return false;
}

// 注册已创建的主布局控件，为后续UI操作建立根容器
void UFrontendUISubsystem::RegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InCreatedWidget)
{
	// 安全检查：确保传入的控件有效
	check(InCreatedWidget);
    
	// 存储主布局控件的引用，作为所有UI操作的根容器
	CreatedPrimaryLayout = InCreatedWidget;
    
	// 调试输出（已注释）
	//Debug::Print(TEXT("Primary Layout Widget Stored"));
}
// 主要的异步UI推送方法：将软引用的控件类异步加载并添加到指定控件栈[1](@ref)
void UFrontendUISubsystem::PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag,
                                                      TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,
                                                      TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)>
                                                      AysncPushStateCallback)
{
	// 安全检查：确保软引用有效
	check(!InSoftWidgetClass.IsNull());
	// 使用资源管理器开始异步加载控件类资源[1](@ref)
	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		InSoftWidgetClass.ToSoftObjectPath(), // 将软引用转换为可加载的路径
		FStreamableDelegate::CreateLambda(   // 加载完成后的回调Lambda
			[InSoftWidgetClass,this,InWidgetStackTag,AysncPushStateCallback]()
			{
				// 资源加载完成后，获取实际的控件类
				UClass* LoadedWidgetClass = InSoftWidgetClass.Get();
				// 验证加载的类和主布局是否有效
				check(LoadedWidgetClass && CreatedPrimaryLayout);
				// 根据标签查找对应的控件栈容器[1](@ref)
				UCommonActivatableWidgetContainerBase* FoundWidgetStack = CreatedPrimaryLayout->FindWidgetStackByTag(
					InWidgetStackTag);
				// 创建控件实例并添加到栈中，同时设置创建回调
				UWidget_ActivatableBase* CreatedWidget = FoundWidgetStack->AddWidget<UWidget_ActivatableBase>(
					LoadedWidgetClass, // 已加载的控件类
					[AysncPushStateCallback](UWidget_ActivatableBase& CreatedWidgetInstance)
					{
						// 此回调在控件实例化后、添加到栈前触发，用于早期初始化
						AysncPushStateCallback(EAsyncPushWidgetState::OnCreatedBeforePush, &CreatedWidgetInstance);
					}
				);
				// 控件已成功添加到栈后触发完成回调
				AysncPushStateCallback(EAsyncPushWidgetState::AfterPush, CreatedWidget);
			}
		)
	);
}

void UFrontendUISubsystem::PushConfirmScreenToModalStackAynsc(EConfirmScreenType InScreenType,
                                                              const FText& InScreenTitle, const FText& InScreenMsg,
                                                              TFunction<void(EConfirmScreenButtonType)>
                                                              ButtonClickedCallback)
{
	UConfirmScreenInfoObject* CreatedInfoObject = nullptr;
	switch (InScreenType)
	{
	case EConfirmScreenType::Ok:
		CreatedInfoObject = UConfirmScreenInfoObject::CreateOKScreen(InScreenTitle, InScreenMsg);
		break;
	case EConfirmScreenType::YesNo:
		CreatedInfoObject = UConfirmScreenInfoObject::CreateYesNoScreen(InScreenTitle, InScreenMsg);
		break;
	case EConfirmScreenType::OKCancel:
		CreatedInfoObject = UConfirmScreenInfoObject::CreateOkCancelScreen(InScreenTitle, InScreenMsg);
		break;
	case EConfirmScreenType::Unkown:
		break;
	default:
		break;
	}
	check(CreatedInfoObject);
	PushSoftWidgetToStackAsync(
		FrontendGameplayTags::Frontend_WidgetStack_Modal,
		UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(FrontendGameplayTags::Frontend_Widget_ConfirmScreen),
		[CreatedInfoObject,ButtonClickedCallback]
	(EAsyncPushWidgetState InPushState, UWidget_ActivatableBase* PushedWidget)
		{
			if (InPushState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				UWidget_ConfirmScreen* CreatedConfirmScreen = CastChecked<UWidget_ConfirmScreen>(PushedWidget);
				CreatedConfirmScreen->InitConfirmScreen(CreatedInfoObject, ButtonClickedCallback);
			}
		});
}
