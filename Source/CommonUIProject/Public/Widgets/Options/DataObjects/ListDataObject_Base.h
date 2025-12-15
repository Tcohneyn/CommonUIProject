// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "FrontendTypes/FrontendEnumTypes.h"
#include "FrontendTypes/FrontendStructTypes.h"
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
	FOnListDataModifiedDelegate OnDependencyDataModified;
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

	// 此方法由 OptionsDataRegister 调用，用于为已构建的列表数据对象添加编辑条件
	void AddEditCondition(const FOptionsDataEditConditionDescriptor& InEditCondition);

	// 从 OptionsDataRegistry 调用，用于添加依赖数据
	void AddEditDependencyData(UListDataObject_Base* InDependencyData);
	
	bool IsDataCurrentlyEditable();
protected:
	//基类中此方法为空。子类应重写此方法，以实现所需的特定初始化逻辑。
	virtual void OnDataObjectInitialized();

	virtual void NotifyListDataModified(UListDataObject_Base* ModifiedData,EOptionsListDataModifyReason ModifyReason = EOptionsListDataModifyReason::DirectlyModified);
	// 子类应重写此方法，以便允许将该值设置为强制字符串值
	virtual bool CanSetToForcedStringValue(const FString& InForcedValue) const { return false;}

	// 子类应重写此方法，以指定如何将当前值设置为强制值
	virtual void OnSetToForcedStringValue(const FString& InForcedValue) {}
	// 此函数将在依赖数据值更改时被调用。子类可重写此函数以处理所需的自定义逻辑。预期会调用父类实现（Super）。
	virtual void OnEditDependencyDataModified(UListDataObject_Base* ModifiedDependencyData,EOptionsListDataModifyReason ModifyReason);
private:
	FName DataID;
	FText DataDisplayName;
	FText DescriptionRichText;
	FText DisabledRichText;
	TSoftObjectPtr<UTexture2D> SoftDescriptionImage;

	UPROPERTY(Transient)
	UListDataObject_Base* ParentData;

	bool bShouldApplyChangeImmediatly = false;

	UPROPERTY(Transient)
	TArray<FOptionsDataEditConditionDescriptor> EditConditionDescArray;
};
