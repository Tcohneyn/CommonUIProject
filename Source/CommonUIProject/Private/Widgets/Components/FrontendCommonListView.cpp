// Tcohneyn All Rights Reserved


#include "Widgets/Components/FrontendCommonListView.h"
#include "Editor/WidgetCompilerLog.h"
#include "Widgets/Options/DataAsset_DataListEntryMapping.h"
#include "Widgets/Options/ListEntries/Widget_ListEntry_Base.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"

/**
 * 核心方法：为列表中的每个数据项生成对应的可视化条目控件（Widget）。
 * 此方法重写了父类的逻辑，实现了基于数据驱动的方式动态选择正确的UI控件来显示列表项。
 * 
 * @param Item 列表中的数据项，必须是 UListDataObject_Base 或其子类。
 * @param DesiredEntryClass 父类逻辑中期望使用的条目控件类（在此自定义实现中可能被覆盖）。
 * @param OwnerTable 底层Slate表格控件的引用，用于管理条目的生命周期和布局。
 * @return 返回一个完全初始化的用户控件实例，该控件将显示在列表中。
 *
 * 工作机制：
 * 1. 在编辑器设计时（Design Time），直接使用父类逻辑，便于在UMG编辑器中预览。
 * 2. 在游戏运行时（Run Time），通过一个专用的数据映射资产（DataListEntryMapping）来查询，
 *    根据传入的数据项类型，找到最匹配的、预先配置好的条目控件类。
 * 3. 这种方法实现了数据与显示的完全解耦，允许设计师通过配置而非代码来改变不同数据类型的显示样式。[4,9](@ref)
 */
UUserWidget& UFrontendCommonListView::OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable)
{
    // 判断当前是否处于编辑器设计阶段（例如在UMG界面中拖拽布局）
    if (IsDesignTime())
    {
        // 在设计时，使用父类的默认生成逻辑，确保在编辑器中可以看到直观的预览效果
        return Super::OnGenerateEntryWidgetInternal(Item, DesiredEntryClass, OwnerTable);
    }

    // 在运行时，执行自定义的条目生成逻辑
    // 1. 将数据项转换为基类（确保类型安全）
    // 2. 通过映射数据资产，根据数据项的具体类型查找对应的条目控件类
    if (TSubclassOf<UWidget_ListEntry_Base> FoundWidgetClass = DataListEntryMapping->FindEntryWidgetClassByDataObject(CastChecked<UListDataObject_Base>(Item)))
    {
        // 使用找到的特定控件类来生成最终的列表条目
        // GenerateTypedEntry 模板函数会处理控件实例的创建和初始化
        return GenerateTypedEntry<UWidget_ListEntry_Base>(FoundWidgetClass,OwnerTable);
    }
    else
    {
        return Super::OnGenerateEntryWidgetInternal(Item,DesiredEntryClass,OwnerTable);
    }
}

bool UFrontendCommonListView::OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem)
{
    return !FirstSelectedItem->IsA<UListDataObject_Collection>();
}

/**
 * 编辑器编译时验证函数。
 * 此函数在编译蓝图或用户控件时被调用，用于检查资源设置是否存在明显错误，防止将错误配置打包到游戏中。
 * 它类似于静态代码分析，在项目编译阶段就能捕获一些常见的设置疏忽。[10](@ref)
 */
#if WITH_EDITOR    
void UFrontendCommonListView::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
    // 首先调用父类的验证逻辑，确保基础检查得以执行
    Super::ValidateCompiledDefaults(CompileLog);

    // 检查关键的 DataListEntryMapping 数据资产是否被正确赋值
    if (!DataListEntryMapping)
    {
        // 如果映射资产为空，记录一个编译错误
        // 这个错误会显示在Unreal编辑器的编译日志中，提醒开发者必须为此属性分配一个有效的数据资产
        CompileLog.Error(FText::FromString(
            TEXT("The variable DataListEntryMapping has no valid data asset assigned ") +
            GetClass()->GetName() +
            TEXT(" needs a valid data asset to function properly")
        ));
    }
}
#endif