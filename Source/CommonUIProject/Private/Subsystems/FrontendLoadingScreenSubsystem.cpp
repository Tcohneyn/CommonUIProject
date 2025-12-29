// Tcohneyn All Rights Reserved


#include "Subsystems/FrontendLoadingScreenSubsystem.h"
#include "PreLoadScreenManager.h"
#include "FrontendDebugHelper.h"
#include "FrontendSettings/FrontEndLoadingScreenSettings.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/FrontendLoadingScreenInterface.h"

UFrontendLoadingScreenSubsystem* UFrontendLoadingScreenSubsystem::Get(const UObject* WorldContextObject)
{
	// 安全检查：确保引擎已初始化
	if (GEngine)
	{
		// 从世界上下文对象中获取有效的世界指针
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);

		// 从游戏实例中获取此子系统的单例实例
		return UGameInstance::GetSubsystem<UFrontendLoadingScreenSubsystem>(World->GetGameInstance());
	}
	return nullptr; // 引擎未就绪时返回空指针
}

bool UFrontendLoadingScreenSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> FoundClasses;
		GetDerivedClasses(GetClass(), FoundClasses);

		return FoundClasses.IsEmpty();
	}

	return false;
}

void UFrontendLoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &ThisClass::OnMapPreLoaded);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnMapPostLoaded);
}

void UFrontendLoadingScreenSubsystem::Deinitialize()
{
	FCoreUObjectDelegates::PreLoadMapWithContext.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
}

UWorld* UFrontendLoadingScreenSubsystem::GetTickableGameObjectWorld() const
{
	if (UGameInstance* OwningGameInstance = GetGameInstance())
	{
		return OwningGameInstance->GetWorld();
	}

	return nullptr;
}

void UFrontendLoadingScreenSubsystem::Tick(float DeltaTime)
{
	TryUpdateLoadingScreen();
}

ETickableTickType UFrontendLoadingScreenSubsystem::GetTickableTickType() const
{
	if (IsTemplate())
	{
		return ETickableTickType::Never;
	}

	return ETickableTickType::Conditional;
}

bool UFrontendLoadingScreenSubsystem::IsTickable() const
{
	return GetGameInstance() && GetGameInstance()->GetGameViewportClient();
}

TStatId UFrontendLoadingScreenSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UFrontendLoadingScreenSubsystem, STATGROUP_Tickables);
}

void UFrontendLoadingScreenSubsystem::OnMapPreLoaded(const FWorldContext& WorldContext, const FString& MapName)
{
	if (WorldContext.OwningGameInstance != GetGameInstance())
	{
		return;
	}

	SetTickableTickType(ETickableTickType::Conditional);

	bIsCurrentlyLoadingMap = true;

	TryUpdateLoadingScreen();
}

void UFrontendLoadingScreenSubsystem::OnMapPostLoaded(UWorld* LoadedWorld)
{
	if (LoadedWorld && LoadedWorld->GetGameInstance() == GetGameInstance())
	{
		bIsCurrentlyLoadingMap = false;
	}
}

void UFrontendLoadingScreenSubsystem::TryUpdateLoadingScreen()
{
	//Check if there's any start up loading screen that's currently active

	if (IsPreLoadScreenActive())
	{
		return;
	}
	//Check if we should show the loading screen
	if (ShouldShowLoadingScreen())
	{
		//Try display the loading screen here
		TryDisplayLoadingScreenIfNone();
		OnLoadingReasonUpdated.Broadcast(CurrentLoadingReason);
	}
	else
	{
		//Try remove the current active loading screen
		TryRemoveLoadingScreen();

		HoldLoadingScreenStartUpTime = -1.f;
		//Notify the loading is complete
		NotifyLoadingScreenVisibilityChanged(false);
		//Disable the ticking
		SetTickableTickType(ETickableTickType::Never);
	}
}

bool UFrontendLoadingScreenSubsystem::IsPreLoadScreenActive() const
{
	if (FPreLoadScreenManager* PreLoadScreenManager = FPreLoadScreenManager::Get())
	{
		return PreLoadScreenManager->HasValidActivePreLoadScreen();
	}

	return false;
}

bool UFrontendLoadingScreenSubsystem::ShouldShowLoadingScreen()
{
	const UFrontEndLoadingScreenSettings* LoadingScreenSettings = GetDefault<UFrontEndLoadingScreenSettings>();

	if (GIsEditor && !LoadingScreenSettings->bShouldShowLoadingScreenInEditor)
	{
		return false;
	}

	// 检查世界中的对象是否需要加载屏幕
	if (CheckTheNeedToShowLoadingScreen())
	{
		GetGameInstance()->GetGameViewportClient()->bDisableWorldRendering = true;

		return true;
	}
	CurrentLoadingReason = TEXT("Waiting for Texture Streaming");
	// 无需显示加载屏幕。允许在此处将世界渲染到我们的视口
	GetGameInstance()->GetGameViewportClient()->bDisableWorldRendering = false;

	const float CurrentTime = FPlatformTime::Seconds();

	if (HoldLoadingScreenStartUpTime < 0.f)
	{
		HoldLoadingScreenStartUpTime = CurrentTime;
	}

	const float ElapsedTime = CurrentTime - HoldLoadingScreenStartUpTime;

	if (ElapsedTime < LoadingScreenSettings->HoldLoadingScreenExtraSeconds)
	{
		return true;
	}

	return false;
}

bool UFrontendLoadingScreenSubsystem::CheckTheNeedToShowLoadingScreen()
{
	if (bIsCurrentlyLoadingMap)
	{
		CurrentLoadingReason = TEXT("Loading Level");

		return true;
	}

	UWorld* OwningWorld = GetGameInstance()->GetWorld();

	if (!OwningWorld)
	{
		CurrentLoadingReason = TEXT("Initializing World");

		return true;
	}

	if (!OwningWorld->HasBegunPlay())
	{
		CurrentLoadingReason = TEXT("World hasn't begun play yet");

		return true;
	}

	if (!OwningWorld->GetFirstPlayerController())
	{
		CurrentLoadingReason = TEXT("Player Controller is not valid yet");

		return true;
	}

	//Check if the game states, player states, or player character, actor component are ready
	return false;
}

void UFrontendLoadingScreenSubsystem::TryDisplayLoadingScreenIfNone()
{
	// 如果已经存在活动的加载屏幕，则提前返回
	if (CachedCreatedLoadingScreenWidget)
	{
		return;
	}

	const UFrontEndLoadingScreenSettings* LoadingScreenSettings = GetDefault<UFrontEndLoadingScreenSettings>();

	TSubclassOf<UUserWidget> LoadedWidgetClass = LoadingScreenSettings->GetLoadingScreenWidgetClassChecked();

	UUserWidget* CreatedWidget = UUserWidget::CreateWidgetInstance(*GetGameInstance(), LoadedWidgetClass, NAME_None);

	check(CreatedWidget);

	CachedCreatedLoadingScreenWidget = CreatedWidget->TakeWidget();

	GetGameInstance()->GetGameViewportClient()->AddViewportWidgetContent(
		CachedCreatedLoadingScreenWidget.ToSharedRef(),
		1000
	);
	NotifyLoadingScreenVisibilityChanged(true);
}

void UFrontendLoadingScreenSubsystem::TryRemoveLoadingScreen()
{
	if (!CachedCreatedLoadingScreenWidget)
	{
		return;
	}

	GetGameInstance()->GetGameViewportClient()->RemoveViewportWidgetContent(
		CachedCreatedLoadingScreenWidget.ToSharedRef());

	CachedCreatedLoadingScreenWidget.Reset();

	//NotifyLoadingScreenVisibilityChanged(false);
}


void UFrontendLoadingScreenSubsystem::NotifyLoadingScreenVisibilityChanged(bool bIsVisible)
{
	for (ULocalPlayer* ExistingLocalPlayer : GetGameInstance()->GetLocalPlayers())
	{
		if (!ExistingLocalPlayer)
		{
			continue;
		}

		if (APlayerController* PC = ExistingLocalPlayer->GetPlayerController(GetGameInstance()->GetWorld()))
		{
			// 查询玩家控制器是否实现了该接口。如果是，则通过接口调用函数来通知加载状态。
			if (PC->Implements<UFrontendLoadingScreenInterface>())
			{
				if (bIsVisible)
				{
					IFrontendLoadingScreenInterface::Execute_OnLoadingScreenActivated(PC);
				}
				else
				{
					IFrontendLoadingScreenInterface::Execute_OnLoadingScreenDeactivated(PC);
				}
			}

			if (APawn* OwningPawn = PC->GetPawn())
			{
				if (OwningPawn->Implements<UFrontendLoadingScreenInterface>())
				{
					if (bIsVisible)
					{
						IFrontendLoadingScreenInterface::Execute_OnLoadingScreenActivated(OwningPawn);
					}
					else
					{
						IFrontendLoadingScreenInterface::Execute_OnLoadingScreenDeactivated(OwningPawn);
					}
				}
			}
			// 查询玩家控制器是否实现了该接口。如果是，则通过接口调用函数来通知加载状态。
		}
	}
}
