#pragma once

#include <YRPP.h>
#include <GeneralDefinitions.h>
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

#include <Ext/Helper/Status.h>

#include <Ext/BulletType/Trajectory/TrajectoryData.h>

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
	return ((TEXT::ExtData*)_extData)->_GameObject; \
	}\
	\
	TBASE* GetOwner() \
	{ \
		return ((TEXT::ExtData*)_extData)->OwnerObject(); \
	} \
	__declspec(property(get = GetOwner)) TBASE* P_NAME; \

class ObjectScript : public ScriptComponent, public ITechnoScript, public IBulletScript
{
public:
	ObjectScript() : ScriptComponent() {}

	virtual GameObject* GetGameObject() override
	{
		if (TechnoExt::ExtData* technoExtData = dynamic_cast<TechnoExt::ExtData*>(_extData))
		{
			return technoExtData->_GameObject;
		}
		else if (BulletExt::ExtData* bulletExtData = dynamic_cast<BulletExt::ExtData*>(_extData))
		{
			return bulletExtData->_GameObject;
		}
		return nullptr;
	}

	TechnoClass* GetTechno()
	{
		if (TechnoExt::ExtData* technoExtData = dynamic_cast<TechnoExt::ExtData*>(_extData))
		{
			return technoExtData->OwnerObject();
		}
		return nullptr;
	}

	__declspec(property(get = GetTechno)) TechnoClass* pTechno;

	BulletClass* GetBullet()
	{
		if (BulletExt::ExtData* bulletExtData = dynamic_cast<BulletExt::ExtData*>(_extData))
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

	AbstractType GetAbsType()
	{
		if (_absType == AbstractType::None)
		{
			_absType = pObject->WhatAmI();
		}
		return _absType;
	}

	bool IsBullet()
	{
		return pBullet != nullptr;
	}

	bool IsBuilding()
	{
		return pTechno && GetAbsType() == AbstractType::Building;
	}

	bool IsInfantry()
	{
		return pTechno && GetAbsType() == AbstractType::Infantry;
	}

	bool IsUnit()
	{
		return pTechno && GetAbsType() == AbstractType::Unit;
	}

	bool IsAircraft()
	{
		return pTechno && GetAbsType() == AbstractType::Aircraft;
	}

	bool IsFoot()
	{
		return !IsBullet() && !IsBuilding();
	}

protected:
	AbstractType _absType = AbstractType::None;
};

class TechnoScript : public ScriptComponent, public ITechnoScript
{
public:
	SCRIPT_COMPONENT(TechnoScript, TechnoClass, TechnoExt, pTechno);

	AbstractType GetAbsType()
	{
		if (_absType == AbstractType::None)
		{
			_absType = pTechno->WhatAmI();
		}
		return _absType;
	}

	bool IsBuilding()
	{
		return GetAbsType() == AbstractType::Building;
	}
	bool IsInfantry()
	{
		return GetAbsType() == AbstractType::Infantry;
	}
	bool IsUnit()
	{
		return GetAbsType() == AbstractType::Unit;
	}
	bool IsAircraft()
	{
		return GetAbsType() == AbstractType::Aircraft;
	}

	LocoType GetThisLocoType()
	{
		if (!IsBuilding())
		{
			if (_locoType == LocoType::None)
			{
				_locoType = GetLocoType(pTechno);
			}
		}
		return _locoType;
	}

	bool IsFly()
	{
		return GetThisLocoType() == LocoType::Fly;
	}
	bool IsJumpjet()
	{
		return GetThisLocoType() == LocoType::Jumpjet;
	}
	bool IsShip()
	{
		return GetThisLocoType() == LocoType::Ship;
	}

	bool IsRocket()
	{
		return IsAircraft() && GetThisLocoType() == LocoType::Rocket;
	}

protected:
	// 单位类型
	AbstractType _absType = AbstractType::None;
	LocoType _locoType = LocoType::None;
};

class BulletScript : public ScriptComponent, public IBulletScript
{
public:
	SCRIPT_COMPONENT(BulletScript, BulletClass, BulletExt, pBullet);

	BulletType GetBulletType()
	{
		if (_bulletType == BulletType::UNKNOWN)
		{
			_bulletType = WhatAmI(pBullet);
			if (_bulletType != BulletType::ROCKET && trajectoryData->IsStraight())
			{
				_bulletType = BulletType::ROCKET;
			}
		}
		return _bulletType;
	}

	bool IsArcing()
	{
		return GetBulletType() == BulletType::ARCING;
	}
	bool IsMissile()
	{
		return GetBulletType() == BulletType::MISSILE;
	}
	bool IsRocket()
	{
		return GetBulletType() == BulletType::ROCKET;
	}
	bool IsBomb()
	{
		return GetBulletType() == BulletType::BOMB;
	}

protected:
	// 抛射体类型
	BulletType _bulletType = BulletType::UNKNOWN;
	// 弹道配置
	TrajectoryData* _trajectoryData = nullptr;
	TrajectoryData* GetTrajectoryData()
	{
		if (!_trajectoryData)
		{
			_trajectoryData = INI::GetConfig<TrajectoryData>(INI::Rules, pBullet->GetType()->ID)->Data;
		}
		return _trajectoryData;
	}
	__declspec(property(get = GetTrajectoryData)) TrajectoryData* trajectoryData;
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

#define DECLARE_DYNAMIC_SCRIPT(CLASS_NAME, ...) \
	CLASS_NAME() : __VA_ARGS__() \
	{ \
		this->Name = ScriptName; \
	} \
	\
	inline static std::string ScriptName = #CLASS_NAME; \
	static Component* Create() \
	{ \
		return static_cast<Component*>(new CLASS_NAME()); \
	} \
	\
	inline static int g_temp_##CLASS_NAME = \
	ComponentFactory::GetInstance().Register(#CLASS_NAME, CLASS_NAME::Create); \

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
