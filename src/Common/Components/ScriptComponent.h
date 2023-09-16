#pragma once

#include "Component.h"
#include "GameObject.h"
#include "Scriptable.h"

#include <Utilities/Container.h>

#include <Extension/AnimExt.h>
#include <Extension/BulletExt.h>
#include <Extension/TechnoExt.h>
#include <Extension/SuperWeaponExt.h>

#include <Ext/Helper.h>

#include <YRPP.h>
#include <SpecificStructures.h>

template <typename TBase>
class ScriptComponent : public Component
{
public:
	ScriptComponent(Extension<TBase>* ext)
	{
		this->ExtData = ext;
#ifdef DEBUG
		char t_this[1024];
		sprintf_s(t_this, "%p", this);
		this->thisId = { t_this };

		// thisName = this->Name;

		this->extId = ext->thisId;
		this->extName = ext->thisName;

		this->baseId = ext->baseId;
		this->baseName = ext->baseName;
#endif // DEBUG
	}

	Extension<TBase>* ExtData;

	TBase* GetOwner()
	{
		return ExtData->OwnerObject();
	}
	__declspec(property(get = GetOwner)) TBase* _owner;

	virtual GameObject* GetGameObject() = 0;
	__declspec(property(get = GetGameObject)) GameObject* _gameObject;
};

class TechnoScript : public ScriptComponent<TechnoClass>, public ITechnoScript
{
public:
	TechnoScript(TechnoExt::ExtData* ext) : ScriptComponent(ext) {}

	virtual GameObject* GetGameObject() override
	{
		return ((TechnoExt::ExtData*)ExtData)->_GameObject;
	}

};

class BulletScript : public ScriptComponent<BulletClass>, public IBulletScript
{
public:
	BulletScript(BulletExt::ExtData* ext) : ScriptComponent(ext) {}

	virtual GameObject* GetGameObject() override
	{
		return ((BulletExt::ExtData*)ExtData)->_GameObject;
	}

};

class AnimScript : public ScriptComponent<AnimClass>, public IAnimScript
{
public:
	AnimScript(AnimExt::ExtData* ext) : ScriptComponent(ext) {}

	virtual GameObject* GetGameObject() override
	{
		return ((AnimExt::ExtData*)ExtData)->_GameObject;
	}

};

class SuperWeaponScript : public ScriptComponent<SuperClass>, public ISuperScript
{
public:
	SuperWeaponScript(SuperWeaponExt::ExtData* ext) : ScriptComponent(ext) {}

	virtual GameObject* GetGameObject() override
	{
		return ((SuperWeaponExt::ExtData*)ExtData)->_GameObject;
	}

};
