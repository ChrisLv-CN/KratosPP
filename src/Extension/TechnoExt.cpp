#include "TechnoExt.h"

#include <Utilities/AresFunctions.h>
#include <Utilities/EnumFunctions.h>

void TechnoExt::AddGlobalScripts(std::list<ScriptComponent*>* globalScripts, TechnoClass* OwnerObject)
{
	LaserTrail* _LaserTrail = new LaserTrail(OwnerObject);
	globalScripts->push_back(_LaserTrail);
}

/*
template<> TechnoExt::ExtData::ExtData(TechnoClass* OwnerObject) : Extension(OwnerObject)
{

}
template<> TechnoExt::ExtData::~ExtData()
{
	// auto const pTypeExt = this->TypeExtData;
	
}

template<> void TechnoExt::ExtData::InvalidatePointer(void* ptr, bool bRemoved)
{

}

template<> void TechnoExt::ExtData::LoadFromStream(PhobosStreamReader& Stm)
{

}

template<> void TechnoExt::ExtData::SaveToStream(PhobosStreamWriter& Stm)
{

}
*/
// =============================
// container

//template<> TechnoExt::ExtContainer::ExtContainer() : Container("TechnoClass") { }
//template<> TechnoExt::ExtContainer::~ExtContainer() = default;

TechnoExt::ExtContainer TechnoExt::ExtMap;
