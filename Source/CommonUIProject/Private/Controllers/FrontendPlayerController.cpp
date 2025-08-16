// Tcohneyn All Rights Reserved


#include "Controllers/FrontendPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
void AFrontendPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	TArray<AActor*> FoundCameras;
	UGameplayStatics::GetAllActorsOfClassWithTag(this,ACameraActor::StaticClass(),FName("Default"),FoundCameras);
	if (FoundCameras.Num() > 0)
	{
		SetViewTarget(FoundCameras[0]);
	}
}