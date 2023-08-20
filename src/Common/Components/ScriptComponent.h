#pragma once

#include "Component.h"


#include <Utilities/Container.h>

template<typename TBase>
class ScriptComponent : public Component
{
public:
	ScriptComponent(Extension<TBase>* ext)
	{
		ExtData = ext;
	}

	Extension<TBase>* ExtData;

	TBase* GetOwner()
	{
		return ExtData->OwnerObject();
	}
	__declspec(property(get = GetOwner)) TBase* _Owner;

};

class TechnoScript : public ScriptComponent<TechnoClass>
{
public:
	TechnoScript(Extension<TechnoClass>* ext) : ScriptComponent(ext) { }
};
