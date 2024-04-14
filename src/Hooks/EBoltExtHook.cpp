#include <exception>
#include <Windows.h>

#include <GeneralStructures.h>
#include <EBolt.h>

#include <Extension.h>
#include <Utilities/Macro.h>

#include <Extension/EBoltExt.h>
#include <Extension/WeaponTypeExt.h>

#include <Ext/Helper/Scripts.h>

#include <Ext/EBoltType/EBoltStatus.h>

// ----------------
// Extension
// ----------------

DEFINE_HOOK(0x4C1E42, EBolt_CTOR, 0x5)
{
	GET(EBolt*, pItem, EAX);
	EBoltExt::ExtMap.TryAllocate(pItem);

	return 0;
}

DEFINE_HOOK(0x4C2951, EBolt_DTOR, 0x5)
{
	GET(EBolt*, pItem, ECX);

	EBoltExt::ExtMap.Remove(pItem);

	return 0;
}

// ----------------
// Feature
// ----------------

DEFINE_HOOK(0x6FD494, TechnoClass_FireEBolt_SetWeaponData, 0x7)
{
	GET(EBolt*, pThis, EAX);
	GET_STACK(WeaponTypeClass*, pWeapon, 0x30 + 0x8);

	if (WeaponTypeExt::TypeData* weaponData = GetTypeData<WeaponTypeExt, WeaponTypeExt::TypeData>(pWeapon))
	{
		if (EBoltStatus* status = GetStatus<EBoltExt, EBoltStatus>(pThis))
		{
			status->ArcCount = weaponData->BoltArcCount;

			status->Color1 = weaponData->BoltColor1;
			status->Color2 = weaponData->BoltColor2;
			status->Color3 = weaponData->BoltColor3;

			status->Disable1 = weaponData->BoltDisable1;
			status->Disable2 = weaponData->BoltDisable2;
			status->Disable3 = weaponData->BoltDisable3;

			status->DisableParticle = weaponData->BoltDisableParticle;
		}
	}
	return 0;
}

DEFINE_HOOK(0x4C28B6, EBolt_Draw_Update, 0x6)
{
	GET(EBolt*, pThis, EAX);

	if (EBoltStatus* status = GetStatus<EBoltExt, EBoltStatus>(pThis))
	{
		status->OnDraw();
	}
	return 0;
}

namespace EBoltDraw
{
	EBolt* pBolt = nullptr;
	int arcCount = 8;
}

DEFINE_HOOK(0x4C20BC, EBolt_Draw_Arcs, 0xB)
{
	enum { DoLoop = 0x4C20C7, Break = 0x4C2400 };

	GET_STACK(EBolt*, pThis, 0x40);
	int arcCount = 8;
	if (EBoltDraw::pBolt == pThis)
	{
		arcCount = EBoltDraw::arcCount;
	}
	else
	{
		if (EBoltStatus* status = GetStatus<EBoltExt, EBoltStatus>(pThis))
		{
			arcCount = status->ArcCount;
		}
		EBoltDraw::pBolt = pThis;
		EBoltDraw::arcCount = arcCount;
	}
	GET_STACK(int, plotIndex, 0x408 - 0x3E0);
	return plotIndex < arcCount ? DoLoop : Break;
}

DEFINE_HOOK(0x4C24BE, EBolt_Draw_Color1, 0x5)
{
	GET_STACK(EBolt*, pThis, 0x40);

	if (EBoltStatus* status = GetStatus<EBoltExt, EBoltStatus>(pThis))
	{
		if (ColorStruct color1 = status->Color1)
		{
			R->EAX((unsigned int)Drawing::RGB_To_Int(color1));
			return 0x4C24E4;
		}
	}
	return 0;
}

DEFINE_HOOK(0x4C25CB, EBolt_Draw_Color2, 0x5)
{
	GET_STACK(EBolt*, pThis, 0x40);

	if (EBoltStatus* status = GetStatus<EBoltExt, EBoltStatus>(pThis))
	{
		if (ColorStruct color2 = status->Color2)
		{
			R->Stack<int>(0x18, Drawing::RGB_To_Int(color2));
			return 0x4C25FD;
		}
	}
	return 0;
}

DEFINE_HOOK(0x4C26C7, EBolt_Draw_Color3, 0x5)
{
	GET_STACK(EBolt*, pThis, 0x40);

	if (EBoltStatus* status = GetStatus<EBoltExt, EBoltStatus>(pThis))
	{
		if (ColorStruct color3 = status->Color3)
		{
			GET(int, ebx, EBX);
			R->EBX(ebx - 2);
			R->EAX((unsigned int)Drawing::RGB_To_Int(color3));
			return 0x4C26EE;
		}
	}
	return 0;
}

DEFINE_HOOK(0x4C24E4, EBolt_Draw_Disable1, 0x8)
{
	GET_STACK(EBolt*, pThis, 0x40);

	if (EBoltStatus* status = GetStatus<EBoltExt, EBoltStatus>(pThis))
	{
		if (status->Disable1)
		{
			return 0x4C2515;
		}
	}
	return 0;
}

DEFINE_HOOK(0x4C25FD, EBolt_Draw_Disable2, 0xA)
{
	GET_STACK(EBolt*, pThis, 0x40);

	if (EBoltStatus* status = GetStatus<EBoltExt, EBoltStatus>(pThis))
	{
		if (status->Disable2)
		{
			return 0x4C262A;
		}
	}
	return 0;
}

DEFINE_HOOK(0x4C26EE, EBolt_Draw_Disable3, 0x8)
{
	GET_STACK(EBolt*, pThis, 0x40);

	if (EBoltStatus* status = GetStatus<EBoltExt, EBoltStatus>(pThis))
	{
		if (status->Disable3)
		{
			return 0x4C2710;
		}
	}
	return 0;
}

// Can not hook in 0x4C2AFF, maybe it was skipped by Ares
DEFINE_HOOK(0x4C2AE7, EBolt_Fire_DisableParticle, 0x6)
{
	GET(EBolt*, pThis, ESI);

	if (EBoltStatus* status = GetStatus<EBoltExt, EBoltStatus>(pThis))
	{
		if (status->DisableParticle)
		{
			EBolt::Array->AddItem(pThis);
			R->EAX(0);
			return 0x4C2B0C;
		}
	}
	return 0;
}
