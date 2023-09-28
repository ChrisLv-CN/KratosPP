#pragma once

#include <YRPP.h>
#include <SpecificStructures.h>

#include "Component.h"
#include "GameObject.h"
#include "Scriptable.h"

#include <Utilities/Container.h>

#include <Extension/AnimExt.h>
#include <Extension/BulletExt.h>
#include <Extension/TechnoExt.h>
#include <Extension/SuperWeaponExt.h>

#include <Ext/Helper.h>

template <typename TBase>
class ScriptComponent : public Component
{
public:
	ScriptComponent(Extension<TBase>* ext)
	{
		this->extData = ext;
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

	Extension<TBase>* extData;

	TBase* GetOwner()
	{
		return extData->OwnerObject();
	}
	__declspec(property(get = GetOwner)) TBase* _owner;

	virtual GameObject* GetGameObject() = 0;
	__declspec(property(get = GetGameObject)) GameObject* _gameObject;

	Randomizer GetRandom()
	{
		return ScenarioClass::Instance->Random;
	}
	__declspec(property(get = GetRandom)) Randomizer _random;
};

template <typename TBase, typename TExt>
class ObjectScript : public ScriptComponent<TBase>, public ITechnoScript, public IBulletScript
{
public:
	ObjectScript(GOExtension<TBase, TExt>::ExtData* ext) : ScriptComponent<TBase>(ext) {}

	virtual GameObject* GetGameObject() override
	{
		return ((typename GOExtension<TBase, TExt>::ExtData*)this->extData)->_GameObject;
	}

	__declspec(property(get = GetOwner)) TBase* pObject;
};

class TechnoScript : public ScriptComponent<TechnoClass>, public ITechnoScript
{
public:
	TechnoScript(TechnoExt::ExtData* ext) : ScriptComponent(ext) {}

	virtual GameObject* GetGameObject() override
	{
		return ((TechnoExt::ExtData*)extData)->_GameObject;
	}

	__declspec(property(get = GetOwner)) TechnoClass* pTechno;
};

class TransformScript : public TechnoScript
{
public:
	TransformScript(TechnoExt::ExtData* ext) : TechnoScript(ext)
	{
		EventSystems::Logic.AddHandler(Events::TypeChangeEvent, this, &TransformScript::Transform);
	}

	virtual void Awake() override
	{
		if (!OnAwake())
		{
			_gameObject->RemoveComponent(this);
			return;
		}
	}

	virtual bool OnAwake() { return true; }

	virtual void Destroy() override
	{
		EventSystems::Logic.RemoveHandler(Events::TypeChangeEvent, this, &TransformScript::Transform);
	}

	void Transform(EventSystem* sender, Event e, void* args)
	{
		this->OnTransform((TypeChangeEventArgs*)args);
	}

	virtual void OnTransform(TypeChangeEventArgs* args) = 0;
};

class BulletScript : public ScriptComponent<BulletClass>, public IBulletScript
{
public:
	BulletScript(BulletExt::ExtData* ext) : ScriptComponent(ext) {}

	virtual GameObject* GetGameObject() override
	{
		return ((BulletExt::ExtData*)extData)->_GameObject;
	}

	__declspec(property(get = GetOwner)) BulletClass* pBullet;
};

class AnimScript : public ScriptComponent<AnimClass>, public IAnimScript
{
public:
	AnimScript(AnimExt::ExtData* ext) : ScriptComponent(ext) {}

	virtual GameObject* GetGameObject() override
	{
		return ((AnimExt::ExtData*)extData)->_GameObject;
	}

	__declspec(property(get = GetOwner)) AnimClass* pAnim;
};

class SuperWeaponScript : public ScriptComponent<SuperClass>, public ISuperScript
{
public:
	SuperWeaponScript(SuperWeaponExt::ExtData* ext) : ScriptComponent(ext) {}

	virtual GameObject* GetGameObject() override
	{
		return ((SuperWeaponExt::ExtData*)extData)->_GameObject;
	}

	__declspec(property(get = GetOwner)) SuperClass* pSuper;
};
