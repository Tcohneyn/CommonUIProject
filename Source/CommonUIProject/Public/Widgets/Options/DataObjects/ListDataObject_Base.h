// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "FrontendTypes/FrontendEnumTypes.h"
#include "UObject/NoExportTypes.h"
#include "ListDataObject_Base.generated.h"

#define LIST_DATA_ACCESSOR(DataType,PropertyName) \
FORCEINLINE DataType Get##PropertyName() const { return PropertyName;} \
void Set##PropertyName(DataType In##PropertyName) { PropertyName = In##PropertyName;}
/**
 * 
 */
UCLASS(Abstract)
class COMMONUIPROJECT_API UListDataObject_Base : public UObject
{
	GENERATED_BODY()
public:
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnListDataModifiedDelegate,UListDataObject_Base*,EOptionsListDataModifyReason)
    FOnListDataModifiedDelegate OnListDataModified;
	LIST_DATA_ACCESSOR(FName,DataID)
	LIST_DATA_ACCESSOR(FText,DataDisplayName)
	LIST_DATA_ACCESSOR(FText,DescriptionRichText)
	LIST_DATA_ACCESSOR(FText,DisabledRichText)
	LIST_DATA_ACCESSOR(TSoftObjectPtr<UTexture2D>,SoftDescriptionImage)
	LIST_DATA_ACCESSOR(UListDataObject_Base*,ParentData)

	void InitDataObject();
	//类中此方法为空。子类 ListDataObject_Collection 应重写它。该函数应返回标签页的所有子级数据
	virtual TArray<UListDataObject_Base*> GetAllChildListData() const { return TArray<UListDataObject_Base*>();}
	virtual bool HasAnyChildListData() const { return false;}

	void SetShouldApplySettingsImmediately(bool bShouldApplyRightAway) { bShouldApplyChangeImmediatly = bShouldApplyRightAway;}

	//子类应重写这些方法，以提供重置数据的实现。
	virtual bool HasDefaultValue() const { return false;}
	virtual bool CanResetBackToDefaultValue() const { return false;}
	virtual bool TryResetBackToDefaultValue() { return false;}
protected:
	//基类中此方法为空。子类应重写此方法，以实现所需的特定初始化逻辑。
	virtual void OnDataObjectInitialized();

	virtual void NotifyListDataModified(UListDataObject_Base* ModifiedData,EOptionsListDataModifyReason ModifyReason = EOptionsListDataModifyReason::DirectlyModified);
private:
	FName DataID;
	FText DataDisplayName;
	FText DescriptionRichText;
	FText DisabledRichText;
	TSoftObjectPtr<UTexture2D> SoftDescriptionImage;

	UPROPERTY(Transient)
	UListDataObject_Base* ParentData;

	bool bShouldApplyChangeImmediatly = false;
};
