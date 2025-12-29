// Tcohneyn All Rights Reserved


#include "FrontendSettings/FrontEndLoadingScreenSettings.h"
#include "Blueprint/UserWidget.h"

TSubclassOf<UUserWidget> UFrontEndLoadingScreenSettings::GetLoadingScreenWidgetClassChecked() const
{
	checkf(!SoftLoadingScreenWidgetClass.IsNull(),TEXT("Forgot to assign a valid widget blueprint in the project settings as loading screen"));

	TSubclassOf<UUserWidget> LoadedLoadingScreenWidget = SoftLoadingScreenWidgetClass.LoadSynchronous();

	return LoadedLoadingScreenWidget;
}
