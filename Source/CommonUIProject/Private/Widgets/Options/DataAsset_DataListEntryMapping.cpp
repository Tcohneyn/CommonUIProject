// Tcohneyn All Rights Reserved


#include "Widgets/Options/DataAsset_DataListEntryMapping.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"

// 根据输入的数据对象，查找并返回其对应的列表条目控件类
TSubclassOf<UWidget_ListEntry_Base> UDataAsset_DataListEntryMapping::FindEntryWidgetClassByDataObject(UListDataObject_Base* InDataObject) const
{	
	// 安全检查：确保传入的数据对象有效
	check(InDataObject);

	// 遍历数据对象的类继承链（从最具体的子类到最通用的父类）
	for (UClass* DataObjectClass = InDataObject->GetClass(); DataObjectClass; DataObjectClass = DataObjectClass->GetSuperClass())
	{
		// 将当前遍历到的类转换为TSubclassOf<UListDataObject_Base>类型，确保类型安全
		if (TSubclassOf<UListDataObject_Base> ConvertedDataObjectClass = TSubclassOf<UListDataObject_Base>(DataObjectClass))
		{
			// 检查映射表中是否存在该数据对象类对应的条目控件类
			if (DataObjectListEntryMap.Contains(ConvertedDataObjectClass))
			{
				// 找到匹配项，返回对应的控件类
				return DataObjectListEntryMap.FindRef(ConvertedDataObjectClass);
			}
		}
	}

	// 如果遍历完整個继承链都没有找到匹配项，返回空控件类（默认处理）
	return TSubclassOf<UWidget_ListEntry_Base>();
}