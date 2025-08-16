// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FrontendPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class COMMONUIPROJECT_API AFrontendPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void OnPossess(APawn* aPawn) override;
};
