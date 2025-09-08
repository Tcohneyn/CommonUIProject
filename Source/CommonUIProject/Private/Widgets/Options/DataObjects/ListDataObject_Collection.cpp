// Tcohneyn All Rights Reserved


#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"

// 集合类：用于存放多个子数据对象，并提供父子层级关系管理
void UListDataObject_Collection::AddChildListData(UListDataObject_Base* InChildListData)
{	
	// 1. 通知传入的子数据对象执行初始化逻辑
	InChildListData->InitDataObject();

	// 2. 将当前集合对象(this)设置为子数据对象的父级
	InChildListData->SetParentData(this);

	// 3. 将子数据对象加入当前集合的子对象数组
	ChildListDataArray.Add(InChildListData);
}
// 获取当前集合下的所有子数据对象
TArray<UListDataObject_Base*> UListDataObject_Collection::GetAllChildListData() const
{
	return ChildListDataArray;
}
// 判断当前集合中是否包含任何子数据对象
bool UListDataObject_Collection::HasAnyChildListData() const
{
	return !ChildListDataArray.IsEmpty();
}