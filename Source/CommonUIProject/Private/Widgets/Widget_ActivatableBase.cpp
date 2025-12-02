// Tcohneyn All Rights Reserved


#include "Widgets/Widget_ActivatableBase.h"

#include "CommonInputSubsystem.h"
#include "Controllers/FrontendPlayerController.h"

// 获取拥有此控件实例的前端玩家控制器（FrontendPlayerController）
// 此函数采用懒加载（Lazy-loading）和缓存机制，避免重复查找，提升性能
AFrontendPlayerController* UWidget_ActivatableBase::GetOwningFrontendPlayerController()
{
	// 检查缓存的控制器指针是否有效（非空且未被垃圾回收）
	// 这是一种防御性编程，确保每次使用的都是有效对象
	if (!CachedOwningFrontendPC.IsValid())
	{
		// 如果缓存无效，则通过模板函数查找并获取拥有此控件的玩家控制器
		// GetOwningPlayer<> 是UE提供的函数，返回继承自APlayerController的特定子类
		CachedOwningFrontendPC = GetOwningPlayer<AFrontendPlayerController>();
	}
    
	// 三元运算符：如果缓存有效则返回缓存的对象，否则返回空指针
	// 这确保了即使查找失败，函数也不会返回无效指针，避免程序崩溃
	return CachedOwningFrontendPC.IsValid() ? CachedOwningFrontendPC.Get() : nullptr;
}

// 获取通用输入子系统（CommonInputSubsystem），用于处理不同输入设备（如键盘、手柄、触摸屏）的抽象和切换
UCommonInputSubsystem* UWidget_ActivatableBase::GetInputSubsystem()
{
	// 首先获取拥有此控件的玩家控制器
	// 这里直接调用获取，未使用缓存，可能因为此函数调用不频繁，或需要实时获取最新状态
	AFrontendPlayerController* FrontendPC = GetOwningPlayer<AFrontendPlayerController>();
	// 使用check断言确保获取到的控制器有效。在开发版本中，如果FrontendPC为空，此处会触发断言错误，便于早期发现配置问题
	check(FrontendPC);
    
	// 通过玩家控制器获取本地玩家（LocalPlayer）对象
	// 本地玩家代表了当前游戏实例中的一个人类玩家，是输入子系统的持有者
	ULocalPlayer* LocalPlayer = FrontendPC->GetLocalPlayer();
    
	// 通过静态方法Get从本地玩家对象中获取通用输入子系统的实例
	// CommonInputSubsystem 是UE的Enhanced Input系统的一部分，负责管理输入模式和上下文
	UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(LocalPlayer);
    
	// 安全检查：如果获取失败（例如在专用服务器上运行，没有本地玩家），则返回空指针
	if (!CommonInputSubsystem) return nullptr;
    
	// 返回获取到的输入子系统实例
	return CommonInputSubsystem;
}
