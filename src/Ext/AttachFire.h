#pragma once

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <type_traits>

#include <GeneralStructures.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>
#include <Ext/Helper.h>
#include <Ext/AttachFire/DelayFire.h>
#include <Ext/AttachFire/SimulateBurst.h>
#include <Ext/BulletType/TrajectoryData.h>
#include <Extension/TechnoExt.h>
#include <Extension/BulletExt.h>
#include <Extension/WarheadTypeExt.h>
#include <Extension/WeaponTypeExt.h>

template <typename TBase, typename TExt>
class AttachFire : public ObjectScript<TBase, TExt>
{
public:
	AttachFire(GOExtension<TBase, TExt>::ExtData* ext) : ObjectScript<TBase, TExt>(ext)
	{
		this->Name = typeid(this).name();
		EventSystems::General.AddHandler(Events::PointerExpireEvent, this, &AttachFire::PointerExpired);
	}

	virtual void Destroy() override
	{
		EventSystems::General.RemoveHandler(Events::PointerExpireEvent, this, &AttachFire::PointerExpired);
	}

	void PointerExpired(EventSystem* sender, Event e, void* args)
	{
		auto const& argsArray = reinterpret_cast<void**>(args);
		AbstractClass* pInvalid = (AbstractClass*)argsArray[0];
		// bool removed = argsArray[1];
		for (DelayFire& delayFire : _delayFires)
		{
			if (delayFire.pTarget == pInvalid)
			{
				delayFire.pTarget = nullptr;
				delayFire.Invalid = true;
			}
		}
		for (SimulateBurst& burst : _simulateBurst)
		{
			if (burst.pTarget == pInvalid)
			{
				burst.pTarget = nullptr;
				burst.Invalid = true;
			}
		}
	}

	/**
	 *@brief 发射任务结束，清除所有未发射任务
	 *
	 */
	void FireMissionDone()
	{
		_delayFires.clear();
		_simulateBurst.clear();
	}

	/**
	 *@brief 发射单位自身的武器
	 *
	 * @param weaponIdx 武器序号
	 * @param pTarget 目标
	 * @param delay 延时
	 * @param count 次数
	 */
	void FireOwnWeapon(int weaponIdx, AbstractClass* pTarget, int delay = 0, int count = 1)
	{
		DelayFire delayFire{ weaponIdx, pTarget, delay, count };
		_delayFires.emplace_back(delayFire);
	}

	/**
	 *@brief 发射自定义武器
	 *
	 * @param pAttacker 攻击者
	 * @param pTarget 目标
	 * @param pAttackingHouse 攻击阵营
	 * @param pWeapon 武器指针
	 * @param weaponTypeData 武器通用配置
	 * @param flh FLH
	 * @param isOnBody 按发射者计算FLH
	 * @param isOnTarget 按目标计算FLH
	 * @param callback 抛射体发射后回调
	 * @return true
	 * @return false
	 */
	bool FireCustomWeapon(TechnoClass* pAttacker, AbstractClass* pTarget, HouseClass* pAttackingHouse,
		WeaponTypeClass* pWeapon, WeaponTypeExt::TypeData weaponTypeData,
		CoordStruct flh, bool isOnBody = false, bool isOnTarget = false, FireBulletToTarget callback = nullptr)
	{
		bool isFire = false;
		// 不允许朝这个目标发射
		if (!weaponTypeData.CanFireToTarget(pTarget, dynamic_cast<ObjectClass*>(this->pObject), pAttacker, pAttackingHouse, pWeapon))
		{
			return isFire;
		}
		int burst = pWeapon->Burst;
		int minRange = pWeapon->MinimumRange;
		int maxRange = pWeapon->Range;
		// 检查抛射体是否具有AA
		if (pTarget->IsInAir())
		{
			if (weaponTypeData.CheckAA && !pWeapon->Projectile->AA)
			{
				// 抛射体没有AA，终止发射
				return isFire;
			}
			if (std::is_same<TBase, TechnoClass>::value)
			{
				maxRange += dynamic_cast<TechnoClass*>(this->pObject)->GetTechnoType()->AirRangeBonus;
			}
		}
		// 检查射程
		if (!weaponTypeData.CheckRange || InRange(pTarget, pWeapon, minRange, maxRange))
		{
			// 可以发射武器
			if (burst > 1 && weaponTypeData.SimulateBurst)
			{
				// burst 发射
				int flipY = 1;
				BulletTypeClass* pBulletType = pWeapon->Projectile;
				if (pBulletType)
				{
					// 翻转抛射体的速度，左右对调
					TrajectoryData* trajectoryData = INI::GetConfig<TrajectoryData>(INI::Rules, pBulletType->ID)->Data;
					if (trajectoryData->ReverseVelocity)
					{
						flipY = -1;
					}
				}
				// 模拟burst发射武器
				SimulateBurst newBurst{ pTarget, pAttacker, pAttackingHouse, pWeapon, weaponTypeData, minRange, maxRange, flh, isOnBody, isOnTarget, burst, flipY, callback };
				SimulateBurstFire(newBurst);
				// 入队
				_simulateBurst.emplace_back(newBurst);
				isFire = true;
			}
			else
			{
				// 直接发射武器
				DirStruct facingDir{};
				CoordStruct sourcePos;
				CoordStruct targetPos = pTarget->GetCoords();
				if (isOnTarget)
				{
					CoordStruct location = this->pObject->GetCoords(); // 射手的位置
					sourcePos = GetSourcePosOnTarget(location, targetPos, flh, facingDir);
				}
				else
				{
					sourcePos = GetSourcePos(flh, facingDir);
				}
				// 扇形攻击
				RadialFire radialFire{ facingDir, burst, weaponTypeData.RadialAngle };
				BulletVelocity bulletVelocity = GetBulletVelocity(sourcePos, targetPos);
				for (int i = 0; i < burst; i++)
				{
					if (weaponTypeData.RadialFire)
					{
						bulletVelocity = radialFire.GetBulletVelocity(i, weaponTypeData.RadialZ);
					}
					// 发射武器，全射出去
					BulletClass* pBullet = FireBulletTo(dynamic_cast<ObjectClass*>(this->pObject), pAttacker, pTarget, pAttackingHouse, pWeapon, sourcePos, targetPos, bulletVelocity);
					// 记录下子机发射器的开火坐标
					if (pWeapon->Spawner)
					{
						SpawnerBurstFLH[i] = sourcePos;
					}
					if (callback != nullptr)
					{
						callback(i, burst, pBullet, pTarget);
					}
				}
			}
		}
			return isFire;
	}

	/**
	 *@brief 发射自定义武器
	 *
	 * @param pAttacker 攻击者
	 * @param pTarget 目标
	 * @param pAttackingHouse 攻击阵营
	 * @param weaponId 武器ID
	 * @param flh FLH
	 * @param isOnBody 按发射者计算FLH
	 * @param isOnTarget 按目标计算FLH
	 * @param callback 抛射体发射后回调
	 * @return true
	 * @return false
	 */
	bool FireCustomWeapon(TechnoClass* pAttacker, AbstractClass* pTarget, HouseClass* pAttackingHouse,
		std::string weaponId,
		CoordStruct flh, bool isOnBody = false, bool isOnTarget = false, FireBulletToTarget callback = nullptr)
	{
		bool isFire = false;
		WeaponTypeClass* pWeapon = WeaponTypeClass::Find(weaponId.c_str());
		if (pWeapon)
		{
			WeaponTypeExt::TypeData weaponTypeData = *GetTypeData<WeaponTypeExt, WeaponTypeExt::TypeData>(pWeapon);
			isFire = FireCustomWeapon(pAttacker, pTarget, pAttackingHouse, pWeapon, weaponTypeData, flh, isOnBody, isOnTarget, callback);
		}
		return isFire;
	}

	virtual void OnUpdate() override
	{
		if (IsDeadOrInvisible(this->pObject))
		{
			FireMissionDone();
			return;
		}
		// 发射自身武器
		if (std::is_same_v<TBase, TechnoClass>)
		{
			TechnoClass* pShooter = dynamic_cast<TechnoClass*>(this->pObject);
			int size = _delayFires.size();
			for (int i = 0; i < size; i++)
			{
				auto it = _delayFires.begin();
				DelayFire delayFire = *it;
				_delayFires.erase(it);
				if (!delayFire.Invalid)
				{
					if (delayFire.TimesUp())
					{
						// 发射武器
						if (delayFire.FireOwnWeapon)
						{
							pShooter->Fire_IgnoreType(delayFire.pTarget, delayFire.WeaponIndex);
						}
						else
						{
							if (!FireCustomWeapon(pShooter, delayFire.pTarget, pShooter->Owner, delayFire.pWeapon, delayFire.WeaponTypeData, delayFire.FLH))
							{
								delayFire.Done();
							}
						}
						delayFire.RecuceOnce();
					}
				}
				if (delayFire.NotDone())
				{
					_delayFires.emplace_back(delayFire);
				}
			}
		}
		else
		{
			_delayFires.clear();
		}
		// 模拟Burst发射
		int size = _simulateBurst.size();
		for (int i = 0; i < size; i++)
		{
			auto it = _simulateBurst.begin();
			SimulateBurst burst = *it;
			_simulateBurst.erase(it);
			if (!burst.Invalid)
			{
				// 检查余弹
				if (burst.Index < burst.Burst)
				{
					// 检查延迟
					if (burst.CanFire())
					{
						AbstractClass* pTarget = burst.pTarget; // 武器的目标
						WeaponTypeClass* pWeapon = burst.pWeapon;

						TechnoClass* pTargetTechno = nullptr;
						// 检查目标幸存和射程
						if (pWeapon && pTarget
							&& (!CastToTechno(pTarget, pTargetTechno) || (!IsDeadOrInvisible(pTargetTechno) && !pTargetTechno->IsFallingDown)) // 如果目标是单位，检查存活情况
							&& (!burst.WeaponTypeData.CheckRange || InRange(pTarget, burst)) // 射程之内
							&& (!burst.WeaponTypeData.CheckAA || !pTarget->IsInAir() || pWeapon->Projectile->AA) // 检查AA
							)
						{
							// 发射
							SimulateBurstFire(burst);
						}
						else
						{
							// 武器失效，任务取消
							continue;
						}
					}
					// 归队
					_simulateBurst.emplace_back(burst);
				}
			}
		}
	}

	// 发射过的子机发射器的开火坐标
	std::map<int, CoordStruct> SpawnerBurstFLH{};

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->_delayFires)
			.Process(this->_simulateBurst)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		Component::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		Component::Save(stream);
		return const_cast<AttachFire*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	CoordStruct GetSourcePos(CoordStruct flh, DirStruct& facingDir, bool isOnTurret = true, int flipY = 1)
	{
		CoordStruct sourcePos = this->pObject->GetCoords();
		if (std::is_same_v<TBase, TechnoClass>)
		{
			TechnoClass* pTechno = dynamic_cast<TechnoClass*>(this->pObject);
			sourcePos = GetFLHAbsoluteCoords(pTechno, flh, isOnTurret, flipY);
			facingDir = pTechno->GetRealFacing().Current();
		}
		else if (std::is_same_v<TBase, BulletClass>)
		{
			BulletClass* pBullet = dynamic_cast<BulletClass*>(this->pObject);
			facingDir = Facing(pBullet, sourcePos);
			CoordStruct tempFLH = flh;
			tempFLH.Y *= flipY;
			sourcePos = GetFLHAbsoluteCoords(sourcePos, tempFLH, facingDir);
		}
		return sourcePos;
	}

	CoordStruct GetSourcePosOnTarget(CoordStruct sourcePos, CoordStruct targetPos, CoordStruct flh, DirStruct& facingDir, int flipY = 1)
	{
		facingDir = Point2Dir(sourcePos, targetPos);
		CoordStruct tempFLH = flh;
		tempFLH.Y *= flipY;
		return GetFLHAbsoluteCoords(targetPos, tempFLH, facingDir);
	}

	bool InRange(AbstractClass* pTarget, WeaponTypeClass* pWeapon, int minRange, int maxRange)
	{
		CoordStruct location = this->pObject->GetCoords();
		switch (this->pObject->WhatAmI())
		{
		case AbstractType::Building:
		case AbstractType::Infantry:
		case AbstractType::Unit:
		case AbstractType::Aircraft:
			return dynamic_cast<TechnoClass*>(this->pObject)->InRange(&location, pTarget, pWeapon);
		default:
			CoordStruct targetPos = pTarget->GetCoords();
			double distance = targetPos.DistanceFrom(location);
			if (isnan(distance))
			{
				distance = 0;
			}
			return distance <= pWeapon->Range && distance >= minRange;
		}
	}
	bool InRange(AbstractClass* pTarget, SimulateBurst burst)
	{
		return InRange(pTarget, burst.pWeapon, burst.MinRange, burst.MaxRange);
	}

	void SimulateBurstFire(SimulateBurst& burst)
	{
		// burst模式3，双发
		if (burst.WeaponTypeData.SimulateBurstMode == 3)
		{
			SimulateBurst b2 = burst;
			b2.FlipY *= -1;
			SimulateBurstFireOnce(b2);
		}
		SimulateBurstFireOnce(burst);
	}

	/**
	 *@brief 发射一次武器
	 *
	 * @param burst
	 */
	void SimulateBurstFireOnce(SimulateBurst& burst)
	{
		DirStruct facingDir{};
		CoordStruct sourcePos = CoordStruct::Empty;
		CoordStruct targetPos = burst.pTarget->GetCoords();
		if (burst.IsOnTarget)
		{
			CoordStruct location = this->pObject->GetCoords(); // 射手的位置
			sourcePos = GetSourcePosOnTarget(location, targetPos, burst.FLH, facingDir, burst.FlipY);
		}
		else
		{
			sourcePos = GetSourcePos(burst.FLH, facingDir, !burst.IsOnBody, burst.FlipY);
		}

		BulletVelocity bulletVelocity = BulletVelocity::Empty;
		// 扇形攻击
		if (burst.WeaponTypeData.RadialFire)
		{
			RadialFire radialFire{ facingDir, burst.Burst, burst.WeaponTypeData.RadialAngle };
			bulletVelocity = radialFire.GetBulletVelocity(burst.Index, burst.WeaponTypeData.RadialZ);
		}
		else
		{
			bulletVelocity = GetBulletVelocity(sourcePos, targetPos);
		}
		// 发射武器
		WeaponTypeClass* pWeapon = burst.pWeapon;
		BulletClass* pBullet = FireBulletTo(dynamic_cast<ObjectClass*>(this->pObject), burst.pAttacker, burst.pTarget, burst.pAttackingHouse, pWeapon, sourcePos, targetPos, bulletVelocity);
		// 记录下子机发射器的开火坐标
		if (pWeapon->Spawner)
		{
			SpawnerBurstFLH[burst.Index] = sourcePos;
		}
		if (burst.Callback != nullptr)
		{
			burst.Callback(burst.Index, burst.Burst, pBullet, burst.pTarget);
		}
		burst.CountOne();
	}

	// 发射自身武器的待发射队列
	std::vector<DelayFire> _delayFires{};
	// Burst模式下剩余待发射队列
	std::vector<SimulateBurst> _simulateBurst{};
};
