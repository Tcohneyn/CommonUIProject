// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OptionsDataRegistry.generated.h"
class UListDataObject_Base;
class UListDataObject_Collection;
/**
 * 
 */
UCLASS()
class COMMONUIPROJECT_API UOptionsDataRegistry : public UObject
{
	GENERATED_BODY()
public:
	//在创建 UOptionsDataRegistry 类型的对象后，由选项屏幕立即调用。
	void InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer);

	const TArray<UListDataObject_Collection*>& GetRegisteredOptionsTabCollections() const { return RegisteredOptionsTabCollections;}

	TArray<UListDataObject_Base*> GetListSourceItemsBySelectedTabID(const FName& InSelectedTabID) const;
private:
	void FindChildListDataRecursively(UListDataObject_Base* InParentData,TArray<UListDataObject_Base*>& OutFoundChildListData) const;
	
	void InitGameplayCollectionTab();
	void InitAudioCollectionTab();
	void InitVideoCollectionTab();
	void InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer);

	UPROPERTY(Transient) //属性是临时的，这意味着它不会被保存或加载。以这种方式标记的属性在加载时将被清零
	TArray<UListDataObject_Collection*> RegisteredOptionsTabCollections;
};
