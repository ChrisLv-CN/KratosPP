#include <exception>
#include <Windows.h>

#include <Extension.h>
#include <Utilities/Macro.h>

#include <Extension/VoxelAnimTypeExt.h>


DEFINE_HOOK(0x74AEB0, VoxelAnimTypeClass_CTOR, 0xB)
{
	if (!Common::IsLoadGame)
	{
		GET(VoxelAnimTypeClass *, pItem, ESI);

		VoxelAnimTypeExt::ExtMap.TryAllocate(pItem);
	}
	return 0;
}

DEFINE_HOOK(0x74BA31, VoxelAnimTypeClass_DTOR, 0x5)
{
	GET(VoxelAnimTypeClass *, pItem, ECX);

	VoxelAnimTypeExt::ExtMap.Remove(pItem);
	return 0;
}

DEFINE_HOOK_AGAIN(0x74B810, VoxelAnimTypeClass_SaveLoad_Prefix, 0x5)
DEFINE_HOOK(0x74B8D0, VoxelAnimTypeClass_SaveLoad_Prefix, 0x8)
{
	GET_STACK(VoxelAnimTypeClass *, pItem, 0x4);
	GET_STACK(IStream *, pStm, 0x8);

	VoxelAnimTypeExt::ExtMap.PrepareStream(pItem, pStm);

	return 0;
}

DEFINE_HOOK(0x74B8C2, VoxelAnimTypeClass_Load_Suffix, 0x5)
{
	VoxelAnimTypeExt::ExtMap.LoadStatic();
	return 0;
}

DEFINE_HOOK(0x74B8EA, VoxelAnimTypeClass_Save_Suffix, 0x5)
{
	VoxelAnimTypeExt::ExtMap.SaveStatic();
	return 0;
}

DEFINE_HOOK_AGAIN(0x74B607, VoxelAnimTypeClass_LoadFromINI, 0x5)
DEFINE_HOOK_AGAIN(0x74B561, VoxelAnimTypeClass_LoadFromINI, 0x5)
DEFINE_HOOK_AGAIN(0x74B54A, VoxelAnimTypeClass_LoadFromINI, 0x5)
DEFINE_HOOK_AGAIN(0x74B51B, VoxelAnimTypeClass_LoadFromINI, 0x5)
DEFINE_HOOK(0x74B4F0, VoxelAnimTypeClass_LoadFromINI, 0x5)
{
	GET(VoxelAnimTypeClass *, pItem, ESI);
	GET_STACK(CCINIClass *, pINI, 0x4);

	VoxelAnimTypeExt::ExtMap.LoadFromINI(pItem, pINI);
	return 0;
}
