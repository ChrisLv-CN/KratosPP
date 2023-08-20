#include "TechnoTypeExt.h"

#include <TechnoTypeClass.h>
#include <StringTable.h>

#include <Utilities/GeneralUtils.h>

TechnoTypeExt::ExtContainer TechnoTypeExt::ExtMap;

void TechnoTypeExt::ExtData::Initialize()
{

}

// =============================
// load / save

void TechnoTypeExt::ExtData::LoadFromINIFile(CCINIClass* const pINI)
{
	auto pThis = this->OwnerObject();
	const char* pSection = pThis->ID;

	if (!pINI->GetSection(pSection))
		return;

	INI_EX exINI(pINI);
	
	//TODO read ini
}

template <typename T>
void TechnoTypeExt::ExtData::Serialize(T& Stm)
{
	Stm;
}

void TechnoTypeExt::ExtData::LoadFromStream(PhobosStreamReader& Stm)
{
	Extension<TechnoTypeClass>::LoadFromStream(Stm);
	this->Serialize(Stm);
}

void TechnoTypeExt::ExtData::SaveToStream(PhobosStreamWriter& Stm)
{
	Extension<TechnoTypeClass>::SaveToStream(Stm);
	this->Serialize(Stm);
}

// =============================
// container

TechnoTypeExt::ExtContainer::ExtContainer() : Container("TechnoTypeClass") { }
TechnoTypeExt::ExtContainer::~ExtContainer() = default;
