#include <exception>
#include <Windows.h>

#include <Extension.h>
#include <Utilities/Macro.h>

#include <Extension/SuperWeaponTypeExt.h>


DEFINE_HOOK(0x6CE6F6, SuperWeaponTypeClass_CTOR, 0x5)
{
	if (!Common::IsLoadGame)
	{
		GET(SuperWeaponTypeClass *, pItem, EAX);

		SuperWeaponTypeExt::ExtMap.TryAllocate(pItem);
	}
	return 0;
}

DEFINE_HOOK(0x6CEFE0, SuperWeaponTypeClass_SDDTOR, 0x8)
{
	GET(SuperWeaponTypeClass *, pItem, ECX);

	SuperWeaponTypeExt::ExtMap.Remove(pItem);
	return 0;
}

DEFINE_HOOK_AGAIN(0x6CE8D0, SuperWeaponTypeClass_SaveLoad_Prefix, 0x8)
DEFINE_HOOK(0x6CE800, SuperWeaponTypeClass_SaveLoad_Prefix, 0xA)
{
	GET_STACK(SuperWeaponTypeClass *, pItem, 0x4);
	GET_STACK(IStream *, pStm, 0x8);

	SuperWeaponTypeExt::ExtMap.PrepareStream(pItem, pStm);

	return 0;
}

DEFINE_HOOK(0x6CE8BE, SuperWeaponTypeClass_Load_Suffix, 0x7)
{
	SuperWeaponTypeExt::ExtMap.LoadStatic();
	return 0;
}

DEFINE_HOOK(0x6CE8EA, SuperWeaponTypeClass_Save_Suffix, 0x3)
{
	SuperWeaponTypeExt::ExtMap.SaveStatic();
	return 0;
}

DEFINE_HOOK_AGAIN(0x6CEE50, SuperWeaponTypeClass_LoadFromINI, 0xA)
DEFINE_HOOK(0x6CEE43, SuperWeaponTypeClass_LoadFromINI, 0xA)
{
	GET(SuperWeaponTypeClass *, pItem, EBP);
	GET_STACK(CCINIClass *, pINI, 0x3FC);

	SuperWeaponTypeExt::ExtMap.LoadFromINI(pItem, pINI);
	return 0;
}
