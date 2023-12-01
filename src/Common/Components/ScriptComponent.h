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

class ScriptComponent : public Component
{
public:
	ScriptComponent(IExtData* ext)
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

	IExtData* extData;

	GameObject* GetGameObject()
	{
		return extData->GetGameObject();
	}

	__declspec(property(get = GetGameObject)) GameObject* _gameObject;
};

#define SCRIPT_COMPONENT(SCRIPT_TYPE, TBASE, TEXT, P_NAME) \
	SCRIPT_TYPE(TEXT::ExtData* ext) : ScriptComponent(ext) {} \
	\
	TBASE* GetOwner() \
	{ \
		return ((TEXT::ExtData*)extData)->OwnerObject(); \
	} \
	__declspec(property(get = GetOwner)) TBASE* P_NAME; \

class ObjectScript : public ScriptComponent, public ITechnoScript, public IBulletScript
{
public:
	ObjectScript(IExtData* ext) : ScriptComponent(ext) {}

	TechnoClass* GetTechno()
	{
		if (TechnoExt::ExtData* technoExtData = dynamic_cast<TechnoExt::ExtData*>(extData))
		{
			return technoExtData->OwnerObject();
		}
		return nullptr;
	}

	__declspec(property(get = GetTechno)) TechnoClass* pTechno;

	BulletClass* GetBullet()
	{
		if (BulletExt::ExtData* bulletExtData = dynamic_cast<BulletExt::ExtData*>(extData))
		{
			return bulletExtData->OwnerObject();
		}
		return nullptr;
	}

	__declspec(property(get = GetBullet)) BulletClass* pBullet;

	ObjectClass* GetOwner()
	{
		ObjectClass* pObject = pTechno;
		if (!pTechno)
		{
			pObject = pBullet;
		}
		if (!pObject)
		{
			Debug::Log("Warning: ObjectScript \"%s\" got a unknown ExtData!\n", Name.c_str());
		}
		return pObject;
	}

	__declspec(property(get = GetOwner)) ObjectClass* pObject;
};

class TechnoScript : public ScriptComponent, public ITechnoScript
{
public:
	SCRIPT_COMPONENT(TechnoScript, TechnoClass, TechnoExt, pTechno);
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

class BulletScript : public ScriptComponent, public IBulletScript
{
public:
	SCRIPT_COMPONENT(BulletScript, BulletClass, BulletExt, pBullet);
};

class AnimScript : public ScriptComponent, public IAnimScript
{
public:
	SCRIPT_COMPONENT(AnimScript, AnimClass, AnimExt, pAnim);
};

class SuperWeaponScript : public ScriptComponent, public ISuperScript
{
public:
	SCRIPT_COMPONENT(SuperWeaponScript, SuperClass, SuperWeaponExt, pSuper);
};

class ScriptFactory
{
public:
	static ScriptFactory& GetInstance()
	{
		static ScriptFactory instance;
		return instance;
	}

	using ScriptCreator = std::function<Component* (IExtData*)>;

	int Register(const std::string& name, ScriptCreator creator)
	{
		_creatorMap.insert(make_pair(name, creator));
		Debug::Log("Registration script \"%s\".\n", name.c_str());
		return 0;
	}

	Component* Create(const std::string& name, IExtData* extData)
	{
		auto it = _creatorMap.find(name);
		if (it != _creatorMap.end())
		{
			Component* c = it->second(extData);
			c->Name = name;
			return c;
		}
		return nullptr;
	}

private:
	ScriptFactory() {};
	~ScriptFactory() {};

	ScriptFactory(const ScriptFactory&) = delete;

	std::map<std::string, ScriptCreator> _creatorMap{};
};

#define DECLARE_DYNAMIC_SCRIPT(CLASS_NAME, TEXTDATA, TSCRIPT) \
	CLASS_NAME(TEXTDATA* ext) : TSCRIPT(ext) \
	{ \
		this->Name = #CLASS_NAME; \
	} \
	\
	static Component* Create(IExtData* extData); \

#define OBJECT_SCRIPT(CLASS_NAME) \
	DECLARE_DYNAMIC_SCRIPT(CLASS_NAME, IExtData, ObjectScript) \

#define TECHNO_SCRIPT(CLASS_NAME) \
	DECLARE_DYNAMIC_SCRIPT(CLASS_NAME, TechnoExt::ExtData, TechnoScript) \

#define TRANSFORM_SCRIPT(CLASS_NAME) \
	DECLARE_DYNAMIC_SCRIPT(CLASS_NAME, TechnoExt::ExtData, TransformScript) \

#define BULLET_SCRIPT(CLASS_NAME) \
	DECLARE_DYNAMIC_SCRIPT(CLASS_NAME, BulletExt::ExtData, BulletScript) \

#define DYNAMIC_SCRIPT_CPP(CLASS_NAME, EXTDATA) \
	Component* CLASS_NAME::Create(IExtData* extData) \
	{ \
		return static_cast<Component*>(new CLASS_NAME(static_cast<EXTDATA*>(extData))); \
	} \
	\
	static int g_temp_##CLASS_NAME = \
	ScriptFactory::GetInstance().Register(#CLASS_NAME, CLASS_NAME::Create); \

#define OBJECT_SCRIPT_CPP(CLASS_NAME) \
	DYNAMIC_SCRIPT_CPP(CLASS_NAME, IExtData) \

#define TECHNO_SCRIPT_CPP(CLASS_NAME) \
	DYNAMIC_SCRIPT_CPP(CLASS_NAME, TechnoExt::ExtData) \

#define BULLET_SCRIPT_CPP(CLASS_NAME) \
	DYNAMIC_SCRIPT_CPP(CLASS_NAME, BulletExt::ExtData) \

template<typename T>
T* CREATE_SCRIPT(const std::string& name, IExtData* extData)
{
	return dynamic_cast<T*>(ScriptFactory::GetInstance().Create(name, extData));
}

