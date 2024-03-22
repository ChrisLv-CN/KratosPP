#pragma once

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>

#include <Extension/GOExtension.h>

class AttachEffectTypeData;
class AttachEffect;
class BlackHoleState;

template <typename TExt, typename TScript, typename TBase>
bool TryGetScript(TBase* p, TScript*& pComponent)
{
	if (p != nullptr)
	{
		if (auto* ext = TExt::ExtMap.Find(p))
		{
			pComponent = ext->GetScript<TScript>();
			return pComponent != nullptr;
		}
	}
	return false;
}

template <typename TExt, typename TScript, typename TBase>
TScript* GetScript(TBase* p)
{
	TScript* pComponent = nullptr;
	TryGetScript<TExt>(p, pComponent);
	return pComponent;
}

template <typename TExt, typename TScript, typename TBase>
TScript* FindOrAttachScript(TBase* p)
{
	TScript* pComponent = nullptr;
	if (p != nullptr)
	{
		if (auto* ext = TExt::ExtMap.Find(p))
		{
			pComponent = ext->FindOrAttach<TScript>();
		}
	}
	return pComponent;
}

template <typename TExt, typename TStatus, typename TBase>
bool TryGetStatus(TBase* p, TStatus*& status)
{
	if (p != nullptr)
	{
		if (auto* ext = TExt::ExtMap.Find(p))
		{
			status = ext->GetExtStatus<TStatus>();
			return status != nullptr;
		}
	}
	return false;
}

template <typename TExt, typename TStatus, typename TBase>
TStatus* GetStatus(TBase* p)
{
	TStatus* status = nullptr;
	TryGetStatus<TExt>(p, status);
	return status;
}

template <typename TExt, typename TBase>
bool TryGetAEManager(TBase* p, AttachEffect*& aeManager)
{
	return TryGetScript<TExt>(p, aeManager);
}

template <typename TExt, typename TBase>
AttachEffect* GetAEManager(TBase* p)
{
	return GetScript<TExt, AttachEffect>(p);
}

BlackHoleState* GetBlackHoleState(ObjectClass* pObject);
bool TryGetBlackHoleState(ObjectClass* pObject, BlackHoleState*& state);

template <typename TypeExt, typename TBase>
bool TryGetAEData(TBase* pWH, AttachEffectTypeData*& data)
{
	if (pWH != nullptr)
	{
		if (auto* typeExt = TypeExt::ExtMap.Find(pWH))
		{
			data = typeExt->pTypeAEData;
			if (!data)
			{
				data = INI::GetConfig<AttachEffectTypeData>(INI::Rules, pWH->ID)->Data;
				typeExt->pTypeAEData = data;
			}
			return data != nullptr;
		}
	}
	return false;
}

template <typename TypeExt, typename TBase>
AttachEffectTypeData* GetAEData(TBase* p)
{
	AttachEffectTypeData* data = nullptr;
	TryGetAEData<TypeExt>(p, data);
	return data;
}

template <typename TypeExt, typename TypeData, typename TBase>
bool TryGetTypeData(TBase* p, TypeData*& data)
{
	if (p != nullptr)
	{
		if (auto* typeExt = TypeExt::ExtMap.Find(p))
		{
			INIConfigReader<TypeData>* pTypeData = static_cast<INIConfigReader<TypeData>*>(typeExt->pTypeData);
			if (!pTypeData)
			{
				pTypeData = INI::GetConfig<TypeData>(INI::Rules, p->ID);
				typeExt->pTypeData = pTypeData;
			}
			data = pTypeData->Data;
			return data != nullptr;
		}
	}
	return false;
}

template <typename TypeExt, typename TypeData, typename TBase>
TypeData* GetTypeData(TBase* p)
{
	TypeData* data = nullptr;
	TryGetTypeData<TypeExt>(p, data);
	return data;
}

