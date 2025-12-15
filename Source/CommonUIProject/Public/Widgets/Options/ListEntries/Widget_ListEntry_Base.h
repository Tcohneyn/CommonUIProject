// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "FrontendTypes/FrontendEnumTypes.h"
#include "Widget_ListEntry_Base.generated.h"

class UListDataObject_Base;
class UCommonTextBlock;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class COMMONUIPROJECT_API UWidget_ListEntry_Base : public UCommonUserWidget,public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On List Entry Widget Hovered"))
	void BP_OnListEntryWidgetHovered(bool bWasHovered,bool bIsEntryWidgetStillSelected);
	void NativeOnListEntryWidgetHovered(bool bWasHovered);
protected:
	//子控件蓝图应重写此函数，以使游戏手柄交互功能正常工作。
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Get Widget To Focus For Gamepad"))
	UWidget* BP_GetWidgetToFocusForGamepad() const;
	
	//~ Begin IUserObjectListEntry Interface
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnEntryReleased() override;
	//~ End IUserObjectListEntry Interface

	//~ Begin UUserWidget Interface
	virtual FReply NativeOnFocusReceived( const FGeometry& InGeometry, const FFocusEvent& InFocusEvent ) override;
	//~ End UUserWidget Interface
	
	//子类应重写此函数以处理所需的初始化工作。预期会调用父类方法（Super）。
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject);

	//子类应重写此函数，以便在数据对象被修改后更新UI值。无需调用父类方法（Super
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData,EOptionsListDataModifyReason ModifyReason);

	virtual void OnOwningDependencyDataObjectModified(UListDataObject_Base* OwningModifiedDependencyData,EOptionsListDataModifyReason ModifyReason);
	
	// 子类应重写此方法，以更改其拥有的小部件的可编辑状态。期望调用父类方法（Super）
	virtual void OnToggleEditableState(bool bIsEditable);
	
	void SelectThisEntryWidget();
private:
	//***** Bound Widgets ***** //
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional,AllowPrivateAccess = "true"))
	UCommonTextBlock* CommonText_SettingDisplayName;
	//***** Bound Widgets ***** //

	UPROPERTY(Transient)
	UListDataObject_Base* CachedOwningDataObject;
};
