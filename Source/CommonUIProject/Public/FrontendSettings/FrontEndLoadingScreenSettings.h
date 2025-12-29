// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "FrontendSettings/FrontendDeveloperSettings.h"
#include "FrontEndLoadingScreenSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game,DefaultConfig)
class COMMONUIPROJECT_API UFrontEndLoadingScreenSettings : public UFrontendDeveloperSettings
{
	GENERATED_BODY()
public:
	TSubclassOf<UUserWidget> GetLoadingScreenWidgetClassChecked() const;

	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen Settings")
	TSoftClassPtr<UUserWidget> SoftLoadingScreenWidgetClass;

	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen Settings")
	float HoldLoadingScreenExtraSeconds = 3.f;

	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen Settings")
	bool bShouldShowLoadingScreenInEditor = false;
};
