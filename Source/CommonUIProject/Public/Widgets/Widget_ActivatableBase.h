// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Widget_ActivatableBase.generated.h"

class AFrontendPlayerController;
/**
 * 
 */
UCLASS(Abstract, BlueprintType,meta=(DisableNativeTick))
class COMMONUIPROJECT_API UWidget_ActivatableBase : public UCommonActivatableWidget
{
	GENERATED_BODY()
protected:
	UFUNCTION(BlueprintPure)
	AFrontendPlayerController* GetOwningFrontendPlayerController();

	UFUNCTION(BlueprintPure)
	UCommonInputSubsystem* GetInputSubsystem();
private:
	TWeakObjectPtr<class AFrontendPlayerController> CachedOwningFrontendPC;
};
