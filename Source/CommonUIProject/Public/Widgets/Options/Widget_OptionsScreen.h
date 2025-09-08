// Tcohneyn All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_OptionsScreen.generated.h"

class UOptionsDataRegistry;
class UFrontendTabListWidgetBase;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class COMMONUIPROJECT_API UWidget_OptionsScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	//~ Begin UUserWidget Interface
    virtual void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	//~ Begin UCommonActivatableWidget Interface
	virtual void NativeOnActivated() override;
	//~ End UCommonActivatableWidget Interface
private:
	UOptionsDataRegistry* GetOrCreateDataRegistry();
	
	void OnResetBoundActionTriggered();
	void OnBackBoundActionTriggered();

	UFUNCTION()
	void OnOptionsTabSelected(FName TabId);
	
	//***** Bound Widgets ***** //
	UPROPERTY(meta = (BindWidget))
	UFrontendTabListWidgetBase* TabListWidget_OptionsTabs;
	//***** Bound Widgets ***** //
	
	//处理选项屏幕中的数据创建。禁止直接访问此变量。
	UPROPERTY(Transient)
	UOptionsDataRegistry* CreatedOwningDataRegistry;
	
	// RowType: 这是一个特殊的元数据说明符，用于指定 FDataTableRowHandle 期望引用的数据表行结构体的类型[2,4](@ref)。
	// 这里的 "/Script/CommonUI.CommonInputActionDataBase" 指明了该句柄应指向一个 Row Struct 为 'CommonInputActionDataBase' 或其子类的 DataTable[4](@ref)。
	// 这通常与 CommonUI 插件配合使用，用于定义和引用通用的输入操作（如确认、取消等），使得UI按钮能根据不同的输入设备（如键盘、手柄）自动显示相应的图标或提示文字[4](@ref)。
	UPROPERTY(EditDefaultsOnly, Category = "Properties",meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAction;

	FUIActionBindingHandle ResetActionHandle;
};
