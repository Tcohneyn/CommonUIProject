// Tcohneyn All Rights Reserved


#include "AsyncActions/AsyncAction_PushSoftWidget.h"
#include "Subsystems/FrontendUISubsystem.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "FrontendDebugHelper.h"
// 静态工厂函数：创建并返回一个异步推送控件的操作节点实例。此函数暴露为蓝图可调用节点。
UAsyncAction_PushSoftWidget* UAsyncAction_PushSoftWidget::PushSoftWidget(
	const UObject* WorldContextObject,      // 世界上下文对象，用于获取当前游戏世界和游戏实例
	APlayerController* OwningPlayerController, // 将要拥有新控件的玩家控制器
	TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass, // 要加载的控件类的软引用
	FGameplayTag InWidgetStackTag,          // 目标UI堆栈的标签，用于定位存放控件的容器
	bool bFocusOnNewlyPushedWidget)         // 推送完成后是否自动将焦点设置到新控件
{
	// 安全检查：确保传入的软引用有效，避免后续操作空资源
	checkf(!InSoftWidgetClass.IsNull(), TEXT("PushSoftWidgetToStack was passed a null soft widget class "));

	// 检查引擎全局对象是否有效
	if (GEngine)
	{
		// 通过世界上下文对象安全地获取当前的世界（World）对象
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			// 创建异步操作节点的新实例
			UAsyncAction_PushSoftWidget* Node = NewObject<UAsyncAction_PushSoftWidget>();
            
			// 缓存参数到节点实例中，供后续的 Activate 阶段使用
			Node->CachedOwningWorld = World;
			Node->CachedOwningPC = OwningPlayerController;
			Node->CachedSoftWidgetClass = InSoftWidgetClass;
			Node->CachedWidgetStackTag = InWidgetStackTag;
			Node->bCachedFocusOnNewlyPushedWidget = bFocusOnNewlyPushedWidget;

			// 关键步骤：将节点注册到游戏实例中，使其生命周期与游戏实例绑定
			// 这确保了在异步加载过程中，节点不会被垃圾回收器意外销毁[1](@ref)
			Node->RegisterWithGameInstance(World);
            
			// 调试输出（已注释）
			//Debug::Print("PushSoftWidget");
            
			return Node; // 返回配置好的节点实例
		}
	}
	return nullptr; // 如果获取世界失败，返回空指针
}

// 异步操作的激活函数，在蓝图执行流到达此节点时自动调用
void UAsyncAction_PushSoftWidget::Activate()
{
	// 通过缓存的World获取前端UI子系统实例，该子系统是管理UI堆栈的中心枢纽[1](@ref)
	UFrontendUISubsystem* FrontendUISubsystem = UFrontendUISubsystem::Get(CachedOwningWorld.Get());

	// 调用子系统的异步推送方法，并传入一个Lambda表达式作为状态回调函数
	FrontendUISubsystem->PushSoftWidgetToStackAsync(
		CachedWidgetStackTag,
		CachedSoftWidgetClass,
		[this](EAsyncPushWidgetState InPushState, UWidget_ActivatableBase* PushedWidget) {
			// 根据异步推送的不同状态，执行相应的逻辑
			switch (InPushState)
			{
			case EAsyncPushWidgetState::OnCreatedBeforePush:
				// 状态1：控件实例已创建，但尚未添加到堆栈
				// 设置控件的所有者玩家控制器，确保其能正确接收输入事件
				PushedWidget->SetOwningPlayer(CachedOwningPC.Get());
				// 广播"控件创建前"事件，蓝图可以在此引脚绑定初始化逻辑
				OnWidgetCreatedBeforePush.Broadcast(PushedWidget);
				break;

			case EAsyncPushWidgetState::AfterPush:
				// 状态2：控件已成功添加到堆栈，整个推送流程完成
				// 广播"推送完成后"事件，蓝图可以在此引脚绑定后续逻辑
				AfterPush.Broadcast(PushedWidget);
                
				// 如果设置了自动对焦，则尝试将焦点设置到新控件的期望目标上
				if (bCachedFocusOnNewlyPushedWidget) {
					if (UWidget* WidgetToFocus = PushedWidget->GetDesiredFocusTarget()) {
						WidgetToFocus->SetFocus();
					}
				}
				// 关键步骤：标记此异步操作已完成，允许系统销毁此节点实例
				SetReadyToDestroy();
				break;
			default:
				break;
			}
		}
	);
}
