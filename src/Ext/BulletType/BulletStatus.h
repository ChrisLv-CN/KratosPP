#pragma once

#include <string>
#include <format>
#include <vector>
#include <map>

#include <BulletClass.h>
#include <TechnoClass.h>

#include <Utilities/Macro.h>
#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include <Common/INI/INIConfig.h>

#include <Ext/Helper/CastEx.h>

#include <Ext/ObjectType/State/BounceData.h>
#include <Ext/ObjectType/State/DestroySelfState.h>
#include <Ext/ObjectType/State/GiftBoxState.h>
#include <Ext/ObjectType/State/PaintballState.h>
#include "BulletStatusData.h"
#include "Status/ProximityData.h"
#include "Trajectory/TrajectoryData.h"

/// @brief base compoment, save the Techno status
class BulletStatus : public BulletScript
{
public:
	BULLET_SCRIPT(BulletStatus);

	BulletType GetBulletType();

	bool IsArcing();
	bool IsMissile();
	bool IsRocket();
	bool IsBomb();

	void TakeDamage(int damage = 0, bool eliminate = true, bool harmless = false, bool checkInterceptable = false);

	void TakeDamage(BulletDamage damageData, bool checkInterceptable = false);

	void ResetTarget(AbstractClass* pNewTarget, CoordStruct targetPos);

	/**
	 *@brief 重新计算Arcing抛射体的出膛向量，精确命中目标位置
	 *
	 * @param speedMultiple 速度倍率，主要用于弹跳
	 * @param force 强制重新计算
	 */
	void ResetArcingVelocity(float speedMultiple = 1.0f, bool force = false);

	void DrawVXL_Paintball(REGISTERS* R);

	void OnTechnoDelete(EventSystem* sender, Event e, void* args);

	virtual void Awake() override;
	virtual void Destroy() override;

	virtual void OnPut(CoordStruct* pLocation, DirType dir) override;

	virtual void OnUpdate() override;

	virtual void OnUpdateEnd() override;

	virtual void OnDetonate(CoordStruct* pCoords, bool& skip) override;

	// 状态机
	State<BounceData> BounceState{};
	DestroySelfState DestroySelfState{};
	GiftBoxState GiftBoxState{};
	PaintballState PaintballState{};

	TechnoClass* pSource = nullptr;
	HouseClass* pSourceHouse = nullptr;

	ObjectClass* pFakeTarget = nullptr;

	BulletLife life = {};
	BulletDamage damage = {};

	// 碰触地面会炸
	bool SubjectToGround = false;
	// 正在被黑洞吸引
	bool CaptureByBlackHole = false;

	bool SpeedChanged = false; // 改变抛射体的速度
	bool LocationLocked = false; // 锁定抛射体的位置

	virtual void OwnerIsRelease(void* ptr) override
	{
		pSource = nullptr;
	};

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->BounceState)
			.Process(this->DestroySelfState)
			.Process(this->GiftBoxState)
			.Process(this->PaintballState)

			.Process(this->pSource)
			.Process(this->pSourceHouse)
			.Process(this->pFakeTarget)
			.Process(this->life)
			.Process(this->damage)

			.Process(this->SubjectToGround)

			.Process(this->SpeedChanged)
			.Process(this->LocationLocked)

			.Process(this->_initFlag)
			// 弹道控制
			.Process(this->_arcingTrajectoryInitFlag)
			// 弹跳
			.Process(this->_bounceData)
			.Process(this->_isBounceSplit)
			.Process(this->_bounceIndex)
			.Process(this->_bounceTargetPos)
			.Process(this->_bounceSpeedMultiple)
			// 直线
			.Process(this->straightBullet)
			.Process(this->_resetTargetFlag)
			// 碰撞引信
			.Process(this->proximity)
			.Process(this->_activeProximity)
			.Process(this->_proximityRange)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		Component::Load(stream, registerForChange);
		EventSystems::Logic.AddHandler(Events::TechnoDeleteEvent, this, &BulletStatus::OnTechnoDelete);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		Component::Save(stream);
		return const_cast<BulletStatus*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	/**
	 *@brief 获取一个移动方向上的随机倍率
	 *
	 */
	void ShakeVelocity();
	void ActiveProximity();

	// 礼物盒
	bool IsOnMark_GiftBox();
	void ReleaseGift(std::vector<std::string> gifts, GiftBoxData data);

	// 反抛射体
	void CanAffectAndDamageBullet(BulletClass* pTarget, WarheadTypeClass* pWH);

	// 生成弹跳子抛射体
	void SetBounceData(BounceData bounceData);
	bool SpawnSplitCannon();

	void InitState_Trajectory_Missile();
	void InitState_Trajectory_Straight();

	void InitState_BlackHole();
	void InitState_Bounce();
	void InitState_DestroySelf();
	void InitState_ECM();
	void InitState_GiftBox();
	void InitState_Paintball();
	void InitState_Proximity();

	void OnUpdate_Trajectory_Arcing();
	void OnUpdate_Trajectory_Bounce();
	void OnUpdate_Trajectory_Straight();
	void OnUpdate_Trajectory_Decroy();

	void OnUpdate_DestroySelf();

	void OnUpdate_BlackHole();
	void OnUpdate_ECM();
	void OnUpdate_GiftBox();
	void OnUpdate_RecalculateStatus();
	void OnUpdate_SelfLaunchOrPumpAction();

	void OnUpdateEnd_BlackHole(CoordStruct& sourcePos);
	void OnUpdateEnd_Proximity(CoordStruct& sourcePos);

	bool OnDetonate_AntiBullet(CoordStruct* pCoords);
	bool OnDetonate_Bounce(CoordStruct* pCoords);
	bool OnDetonate_GiftBox(CoordStruct* pCoords);
	bool OnDetonate_SelfLaunch(CoordStruct* pCoords);

	// 抛射体类型
	BulletType _bulletType = BulletType::UNKNOWN;
	// 弹道配置
	TrajectoryData* _trajectoryData = nullptr;
	TrajectoryData* GetTrajectoryData();
	__declspec(property(get = GetTrajectoryData)) TrajectoryData* trajectoryData;

	bool _initFlag = false;

	bool _targetToAircraftFlag = false;

	// 弹道控制
	bool _arcingTrajectoryInitFlag = false;
	bool _missileShakeVelocityFlag = false;

	// 弹跳
	BounceData _bounceData{};
	bool _isBounceSplit = false; // 是弹跳抛射体分裂的子抛射体
	int _bounceIndex = 0; // 第几号子抛射体
	CoordStruct _bounceTargetPos = CoordStruct::Empty;
	float _bounceSpeedMultiple = 1.0f;

	// 直线
	StraightBullet straightBullet{};
	bool _resetTargetFlag = false;

	// 碰撞引信配置
	ProximityData* _proximityData = nullptr;
	ProximityData* GetProximityData();
	// 碰撞引信
	Proximity proximity{};
	bool _activeProximity = false;
	// 近炸引信
	int _proximityRange = -1;
};
