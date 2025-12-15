// Tcohneyn All Rights Reserved


#include "Controllers/FrontendPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "FrontendSettings/FrontendGameUserSettings.h"
// 当此PlayerController获得对一个Pawn的控制权时调用
void AFrontendPlayerController::OnPossess(APawn* aPawn)
{
	// 首先调用父类（APlayerController）的OnPossess实现，确保引擎的基础逻辑（如设置Pawn、生成默认控制器等）正常执行
	Super::OnPossess(aPawn);
    
	// 定义一个数组，用于存储查找到的摄像机Actor
	TArray<AActor*> FoundCameras;
    
	// 在游戏世界中查找所有具有特定标签（"Default"）的ACameraActor类实例
	// 这常用于在关卡中预先放置并标记好作为默认视角的摄像机
	UGameplayStatics::GetAllActorsOfClassWithTag(this, ACameraActor::StaticClass(), FName("Default"), FoundCameras);
    
	// 检查是否找到了至少一个符合条件的摄像机
	if (FoundCameras.Num() > 0)
	{
		// 将玩家的视角切换至找到的第一个标记为"Default"的摄像机
		// 这通常用于前端场景（如主菜单、角色选择界面）中固定玩家的视角，提供一个预设的观察位置
		SetViewTarget(FoundCameras[0]);
	}
	// 如果没有找到标记为"Default"的摄像机，则不会进行视角切换，可能会保持引擎默认视角或之前设置的视角
	UFrontendGameUserSettings* GameUserSettings = UFrontendGameUserSettings::Get();
	// 检查是否已保存有效的CPU/GPU基准测试结果
	// 如果任意一个结果为-1（表示从未运行过基准测试或结果无效）
	if (GameUserSettings->GetLastCPUBenchmarkResult() == -1.f || GameUserSettings->GetLastGPUBenchmarkResult() == -1.f)
	{
		// 运行硬件性能基准测试
		GameUserSettings->RunHardwareBenchmark();
		// 应用基准测试结果（通常会根据性能自动调整图形设置）
		GameUserSettings->ApplyHardwareBenchmarkResults();
	}
}