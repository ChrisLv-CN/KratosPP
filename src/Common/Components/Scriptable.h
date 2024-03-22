#pragma once

#include <YRPP.h>
#include <SpecificStructures.h>

class IAbstractScript
{
public:
	virtual void OnInit() {};
	virtual void OnUnInit() {};
};

class IAnimScript : public IAbstractScript
{
public:
	virtual void OnLoop() {};
	virtual void OnDone() {};
	virtual void OnNext(AnimTypeClass* next) {};
};

class ISuperScript : public IAbstractScript
{
public:
	virtual void OnLaunch(CellStruct cell, bool isPlayer) {};
};

class IObjectScript : public IAbstractScript
{
public:
	virtual void OnPut(CoordStruct* pCoords, DirType faceDir) {};
	virtual void OnRemove() {};
	virtual void OnReceiveDamage(args_ReceiveDamage* args) {};
	virtual void OnReceiveDamageReal(int* pRealDamage, WarheadTypeClass* pWH, TechnoClass* pAttacker, HouseClass* pAttacingHouse) {};
	virtual void OnReceiveDamageEnd(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, TechnoClass* pAttacker, HouseClass* pAttackingHouse) {};
	virtual void OnReceiveDamageDestroy() {};
};

class ITechnoScript : public IObjectScript
{
public:
	virtual void OnTemporalUpdate(TemporalClass* pTemporal) {};
	virtual void OnTemporalEliminate(TemporalClass* pTemporal) {};

	virtual void OnRegisterDestruction(TechnoClass* pKiller, int cost, bool& skip) {};

	virtual void CanFire(AbstractClass* pTarget, WeaponTypeClass* pWeapon, bool& ceaseFire) {};
	virtual void OnFire(AbstractClass* pTarget, int weaponIdx) {};

	virtual void DrawHealthBar(int barLength, Point2D* pPos, RectangleStruct* pBound, bool isBuilding) {};
	virtual void OnSelect(bool& selectable) {};

	virtual void OnGuardCommand() {};
	virtual void OnStopCommand() {};

	virtual void OnRocketExplosion() {};
	virtual void OnTransform() {};
};

class IBulletScript : public IObjectScript
{
public:
	virtual void OnDetonate(CoordStruct* pCoords, bool& skip) {};
};
