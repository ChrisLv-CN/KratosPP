#pragma once

#include "Component.h"
#include "GameObject.h"

#include <Utilities/Container.h>
#include <Ext/Helper.h>

#include <YRPP.h>
#include <SpecificStructures.h>

template <typename TBase>
class ScriptComponent : public Component
{
public:
	ScriptComponent(Extension<TBase> *ext)
	{
		this->ExtData = ext;
	}

	Extension<TBase> *ExtData;

	TBase *GetOwner()
	{
		return ExtData->OwnerObject();
	}
	__declspec(property(get = GetOwner)) TBase *_owner;
};

class AbstractScript
{
public:
	virtual void OnInit(){};
	virtual void OnUnInit(){};
};

class ObjectScript : public AbstractScript
{
public:
	virtual void OnPut(CoordStruct *location, DirType faceDir){};
	virtual void OnRemove(){};
	virtual void OnReceiveDamage(args_ReceiveDamage *args){};
	virtual void OnReceiveDamageEnd(int *realDamage, WarheadTypeClass *pWH, DamageState damageState, ObjectClass *pAttacker, HouseClass *pAttackingHouse){};
	virtual void OnReceiveDamageDestroy(){};
};

class TechnoScript : public ScriptComponent<TechnoClass>, public ObjectScript
{
public:
	TechnoScript(Extension<TechnoClass> *ext) : ScriptComponent(ext) {}

	GameObject *GetGameObject();
	__declspec(property(get = GetGameObject)) GameObject *_gameObject;

	virtual void OnTemporalUpdate(TemporalClass *pTemporal){};
	virtual void OnTemporalEliminate(TemporalClass *pTemporal){};

	virtual void OnRegisterDestruction(TechnoClass *pKiller, int cost, bool &skip){};

	virtual void CanFire(AbstractClass *pTarget, WeaponTypeClass *pWeapon, bool &ceaseFire){};
	virtual void OnFire(AbstractClass *pTarget, int weaponIdx){};

	virtual void DrawHealthBar(int barLength, Point2D *pPos, RectangleStruct *pBound, bool isBuilding){};
	virtual void OnSelect(bool &selectable){};

	virtual void OnGuardCommand(){};
	virtual void OnStopCommand(){};
};

class BulletScript : public ScriptComponent<BulletClass>, public ObjectScript
{
public:
	BulletScript(Extension<BulletClass> *ext) : ScriptComponent(ext) {}

	GameObject *GetGameObject();
	__declspec(property(get = GetGameObject)) GameObject *_gameObject;

	virtual void OnDetonate(CoordStruct *pCoords, bool &skip){};
};

class AnimScript : public ScriptComponent<AnimClass>, public AbstractScript
{
public:
	AnimScript(Extension<AnimClass> *ext) : ScriptComponent(ext) {}

	GameObject *GetGameObject();
	__declspec(property(get = GetGameObject)) GameObject *_gameObject;

	virtual void OnLoop(){};
	virtual void OnDone(){};
	virtual void OnNext(AnimTypeClass *next){};
};

class SuperWeaponScript : public ScriptComponent<SuperClass>, public AbstractScript
{
public:
	SuperWeaponScript(Extension<SuperClass> *ext) : ScriptComponent(ext) {}

	GameObject *GetGameObject();
	__declspec(property(get = GetGameObject)) GameObject *_gameObject;

	virtual void OnLaunch(CellStruct cell, bool isPlayer){};
};
