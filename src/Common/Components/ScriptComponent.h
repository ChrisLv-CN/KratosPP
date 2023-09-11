#pragma once

#include "Component.h"
#include "GameObject.h"

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

class AbstractScript
{
public:
	virtual void OnInit() {};
	virtual void OnUnInit() {};
};

class ObjectScript : public AbstractScript
{
public:
	virtual void OnPut(CoordStruct* location, DirType faceDir) {};
	virtual void OnRemove() {};
	virtual void OnReceiveDamage(args_ReceiveDamage* args) {};
	virtual void OnReceiveDamageEnd(int* realDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse) {};
	virtual void OnReceiveDamageDestroy() {};
};

class TechnoScript : public ScriptComponent<TechnoClass>, public ObjectScript
{
public:
	TechnoScript(TechnoExt::ExtData* ext) : ScriptComponent(ext) {}

	virtual GameObject* GetGameObject() override
	{
		return ((TechnoExt::ExtData*)ExtData)->_GameObject;
	}

	virtual void OnTemporalUpdate(TemporalClass* pTemporal) {};
	virtual void OnTemporalEliminate(TemporalClass* pTemporal) {};

	virtual void OnRegisterDestruction(TechnoClass* pKiller, int cost, bool& skip) {};

	virtual void CanFire(AbstractClass* pTarget, WeaponTypeClass* pWeapon, bool& ceaseFire) {};
	virtual void OnFire(AbstractClass* pTarget, int weaponIdx) {};

	virtual void DrawHealthBar(int barLength, Point2D* pPos, RectangleStruct* pBound, bool isBuilding) {};
	virtual void OnSelect(bool& selectable) {};

	virtual void OnGuardCommand() {};
	virtual void OnStopCommand() {};
};

class BulletScript : public ScriptComponent<BulletClass>, public ObjectScript
{
public:
	BulletScript(BulletExt::ExtData* ext) : ScriptComponent(ext) {}

	virtual GameObject* GetGameObject() override
	{
		return ((BulletExt::ExtData*)ExtData)->_GameObject;
	}

	virtual void OnDetonate(CoordStruct* pCoords, bool& skip) {};
};

class AnimScript : public ScriptComponent<AnimClass>, public AbstractScript
{
public:
	AnimScript(AnimExt::ExtData* ext) : ScriptComponent(ext) {}

	virtual GameObject* GetGameObject() override
	{
		return ((AnimExt::ExtData*)ExtData)->_GameObject;
	}

	virtual void OnLoop() {};
	virtual void OnDone() {};
	virtual void OnNext(AnimTypeClass* next) {};
};

class SuperWeaponScript : public ScriptComponent<SuperClass>, public AbstractScript
{
public:
	SuperWeaponScript(SuperWeaponExt::ExtData* ext) : ScriptComponent(ext) {}

	virtual GameObject* GetGameObject() override
	{
		return ((SuperWeaponExt::ExtData*)ExtData)->_GameObject;
	}

	virtual void OnLaunch(CellStruct cell, bool isPlayer) {};
};
