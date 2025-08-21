// Tcohneyn All Rights Reserved


#include "Widgets/Widget_ActivatableBase.h"

#include "CommonInputSubsystem.h"
#include "Controllers/FrontendPlayerController.h"

AFrontendPlayerController* UWidget_ActivatableBase::GetOwningFrontendPlayerController()
{
	if (!CachedOwningFrontendPC.IsValid())
	{
		CachedOwningFrontendPC = GetOwningPlayer<AFrontendPlayerController>();
	}
	return CachedOwningFrontendPC.IsValid()? CachedOwningFrontendPC.Get() : nullptr;
}

UCommonInputSubsystem* UWidget_ActivatableBase::GetInputSubsystem()
{
	AFrontendPlayerController* FrontendPC = GetOwningPlayer<AFrontendPlayerController>();
	check(FrontendPC);
	ULocalPlayer* LocalPlayer = FrontendPC->GetLocalPlayer();
	UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(LocalPlayer);
	if (!CommonInputSubsystem) return nullptr;
	return CommonInputSubsystem;
}
