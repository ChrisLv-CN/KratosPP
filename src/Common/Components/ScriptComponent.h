#pragma once

#include <YRPP.h>
#include <SpecificStructures.h>

#include "Component.h"
#include "GameObject.h"
#include "Scriptable.h"

#include <Utilities/Container.h>

#include <Extension/AnimExt.h>
#include <Extension/BulletExt.h>
#include <Extension/EBoltExt.h>
#include <Extension/TechnoExt.h>
#include <Extension/SuperWeaponExt.h>

#include <Ext/Helper.h>

/// @brief 所有的脚本都位于GameObject下
class ScriptComponent : public Component
{
public:
	ScriptComponent() : Component() {}

	virtual GameObject* GetGameObject() = 0;
	__declspec(property(get = GetGameObject)) GameObject* _gameObject;
};

#define SCRIPT_COMPONENT(SCRIPT_TYPE, TBASE, TEXT, P_NAME) \
	SCRIPT_TYPE() : ScriptComponent() {} \
	\
	virtual GameObject* GetGameObject() override \
	{ \
	return ((TEXT::ExtData*)extData)->_GameObject; \
	}\
	\
	TBASE* GetOwner() \
	{ \
		return ((TEXT::ExtData*)extData)->OwnerObject(); \
	} \
	__declspec(property(get = GetOwner)) TBASE* P_NAME; \

class ObjectScript : public ScriptComponent, public ITechnoScript, public IBulletScript
{
public:
	ObjectScript() : ScriptComponent() {}

	virtual GameObject* GetGameObject() override
	{
		if (TechnoExt::ExtData* technoExtData = dynamic_cast<TechnoExt::ExtData*>(extData))
		{
			return technoExtData->_GameObject;
		}
		else if (BulletExt::ExtData* bulletExtData = dynamic_cast<BulletExt::ExtData*>(extData))
		{
			return bulletExtData->_GameObject;
		}
		return nullptr;
	}

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

class EBoltScript : public ScriptComponent
{
public:
	SCRIPT_COMPONENT(EBoltScript, EBolt, EBoltExt, pBolt);
};

#define DECLARE_DYNAMIC_SCRIPT(CLASS_NAME, TSCRIPT) \
	CLASS_NAME() : TSCRIPT() \
	{ \
		this->Name = ScriptName; \
	} \
	\
	inline static std::string ScriptName = #CLASS_NAME; \
	static Component* Create(); \

#define OBJECT_SCRIPT(CLASS_NAME) \
	DECLARE_DYNAMIC_SCRIPT(CLASS_NAME, ObjectScript) \

#define TECHNO_SCRIPT(CLASS_NAME) \
	DECLARE_DYNAMIC_SCRIPT(CLASS_NAME, TechnoScript) \

#define BULLET_SCRIPT(CLASS_NAME) \
	DECLARE_DYNAMIC_SCRIPT(CLASS_NAME, BulletScript) \

#define ANIM_SCRIPT(CLASS_NAME) \
	DECLARE_DYNAMIC_SCRIPT(CLASS_NAME, AnimScript) \

#define SUPER_SCRIPT(CLASS_NAME) \
	DECLARE_DYNAMIC_SCRIPT(CLASS_NAME, SuperWeaponScript) \

#define EBOLT_SCRIPT(CLASS_NAME) \
	DECLARE_DYNAMIC_SCRIPT(CLASS_NAME, EBoltScript) \

#define DYNAMIC_SCRIPT_CPP(CLASS_NAME) \
	Component* CLASS_NAME::Create() \
	{ \
		return static_cast<Component*>(new CLASS_NAME()); \
	} \
	\
	static int g_temp_##CLASS_NAME = \
	ComponentFactory::GetInstance().Register(#CLASS_NAME, CLASS_NAME::Create); \

#define OBJECT_SCRIPT_CPP(CLASS_NAME) \
	DYNAMIC_SCRIPT_CPP(CLASS_NAME) \

#define TECHNO_SCRIPT_CPP(CLASS_NAME) \
	DYNAMIC_SCRIPT_CPP(CLASS_NAME) \

#define BULLET_SCRIPT_CPP(CLASS_NAME) \
	DYNAMIC_SCRIPT_CPP(CLASS_NAME) \

#define ANIM_SCRIPT_CPP(CLASS_NAME) \
	DYNAMIC_SCRIPT_CPP(CLASS_NAME) \

#define SUPER_SCRIPT_CPP(CLASS_NAME) \
	DYNAMIC_SCRIPT_CPP(CLASS_NAME) \

#define EBOLT_SCRIPT_CPP(CLASS_NAME) \
	DYNAMIC_SCRIPT_CPP(CLASS_NAME) \
