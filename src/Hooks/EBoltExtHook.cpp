#include <exception>
#include <Windows.h>

#include <Extension.h>
#include <Utilities/Macro.h>
#include <Extension/EBoltExt.h>
#include <Common/Components/Component.h>
#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>

#include <Ext/EBoltStatus.h>

#include <EBolt.h>

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

DEFINE_HOOK(0x4C24E4, Ebolt_Draw_Disable1, 0x8)
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

DEFINE_HOOK(0x4C25FD, Ebolt_Draw_Disable2, 0xA)
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

DEFINE_HOOK(0x4C26EE, Ebolt_Draw_Disable3, 0x8)
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
