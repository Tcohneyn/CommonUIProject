// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OptionsDataRegistry.generated.h"
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
private:
	void InitGameplayCollectionTab();
	void InitAudioCollectionTab();
	void InitVideoCollectionTab();
	void InitControlCollectionTab();

	UPROPERTY(Transient) //属性是临时的，这意味着它不会被保存或加载。以这种方式标记的属性在加载时将被清零
	TArray<UListDataObject_Collection*> RegisteredOptionsTabCollections;
};
