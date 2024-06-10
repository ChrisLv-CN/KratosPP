#include <exception>
#include <Windows.h>

#include <Extension.h>
#include <Utilities/Macro.h>

#include <Ext/Helper/MathEx.h>

#include <Extension/WarheadTypeExt.h>


DEFINE_HOOK(0x75D1A9, WarheadTypeClass_CTOR, 0x7)
{
	if (!Common::IsLoadGame)
	{
		GET(WarheadTypeClass*, pItem, EBP);

		WarheadTypeExt::ExtMap.TryAllocate(pItem);
	}
	return 0;
}

DEFINE_HOOK(0x75E5C8, WarheadTypeClass_SDDTOR, 0x6)
{
	GET(WarheadTypeClass*, pItem, ESI);

	WarheadTypeExt::ExtMap.Remove(pItem);

	return 0;
}

DEFINE_HOOK_AGAIN(0x75E2C0, WarheadTypeClass_SaveLoad_Prefix, 0x5)
DEFINE_HOOK(0x75E0C0, WarheadTypeClass_SaveLoad_Prefix, 0x8)
{
	GET_STACK(WarheadTypeClass*, pItem, 0x4);
	GET_STACK(IStream*, pStm, 0x8);

	WarheadTypeExt::ExtMap.PrepareStream(pItem, pStm);

	return 0;
}

DEFINE_HOOK(0x75E2AE, WarheadTypeClass_Load_Suffix, 0x7)
{
	WarheadTypeExt::ExtMap.LoadStatic();

	return 0;
}

DEFINE_HOOK(0x75E39C, WarheadTypeClass_Save_Suffix, 0x5)
{
	WarheadTypeExt::ExtMap.SaveStatic();

	return 0;
}

DEFINE_HOOK_AGAIN(0x75DEAF, WarheadTypeClass_LoadFromINI, 0x5)
DEFINE_HOOK(0x75DEA0, WarheadTypeClass_LoadFromINI, 0x5)
{
	GET(WarheadTypeClass*, pItem, ESI);
	GET_STACK(CCINIClass*, pINI, 0x150);

	WarheadTypeExt::ExtMap.LoadFromINI(pItem, pINI);
	WarheadTypeExt::ReadAresArmorTypes();

	return 0;
}

DEFINE_HOOK(0x48A551, WarheadTypeClass_AnimList_SplashList, 0x6)
{
	GET(WarheadTypeClass* const, pThis, ESI);
	GET(int, nDamage, ECX);

	if (WarheadTypeExt::TypeData* typeData = GetTypeData<WarheadTypeExt, WarheadTypeExt::TypeData>(pThis))
	{
		size_t size = typeData->SplashList.size();
		if (size > 0)
		{
			AnimTypeClass* pAnimType = nullptr;
			int idx = typeData->SplashListRandom ? Random::RandomRanged(0, size - 1) : std::min(size * 35 - 1, (size_t)nDamage) / 35;
			std::string animType = typeData->SplashList[idx];
			if (IsNotNone(animType))
			{
				pAnimType = AnimTypeClass::Find(animType.c_str());
			}
			R->EAX(pAnimType);
			return 0x48A5AD;
		}
	}
	return 0;
}


