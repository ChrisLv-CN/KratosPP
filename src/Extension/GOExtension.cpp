#include "GOExtension.h"

// 不可以在cpp里定义，要在h里声明带定义，否则会报找不到符号

/*
template<typename TBase, typename TExt>
GOExtension<TBase, TExt>::ExtData::ExtData(TBase* OwnerObject) : Extension<TBase>(OwnerObject)
{

}

// =============================
// load / save

template<typename TBase, typename TExt>
template<typename T>
void GOExtension<TBase, TExt>::ExtData::Serialize(T& Stm)
{
	_GameObject.Foreach([](Component* c) { c->Serialize(Stm); });
}


template<typename TBase, typename TExt>
void GOExtension<TBase, TExt>::ExtData::LoadFromStream(PhobosStreamReader& Stm)
{
	Extension<TBase>::LoadFromStream(Stm);
	this->Serialize(Stm);
}

template<typename TBase, typename TExt>
void GOExtension<TBase, TExt>::ExtData::SaveToStream(PhobosStreamWriter& Stm)
{
	Extension<TBase>::SaveToStream(Stm);
	this->Serialize(Stm);
}

// =============================
// container

template<typename TBase, typename TExt>
GOExtension<TBase, TExt>::ExtContainer::ExtContainer() : Container<TExt>(typeid(TExt).name()) { };

template<typename TBase, typename TExt>
GOExtension<TBase, TExt>::ExtContainer::~ExtContainer() = default;
*/