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
#include <Ext/Helper/MathEx.h>

#include <Ext/StateType/State/DestroySelfState.h>
#include <Ext/StateType/State/GiftBoxState.h>
#include <Ext/StateType/State/PaintballState.h>

#include "BulletStatusData.h"
#include "Status/ProximityData.h"
#include "Trajectory/TrajectoryData.h"

class AttachEffect;

/// @brief base compoment, save the Techno status
class BulletStatus : public BulletScript
{
public:
	BULLET_SCRIPT(BulletStatus);

	void TakeDamage(int damage = 0, bool eliminate = true, bool harmless = false, bool checkInterceptable = false);

	void TakeDamage(BulletDamage damageData, bool checkInterceptable = false);

	void ResetTarget(AbstractClass* pNewTarget, CoordStruct targetPos);

	void DrawVXL_Paintball(REGISTERS* R);

	void OnTechnoDelete(EventSystem* sender, Event e, void* args);

	void ActiveProximity();

	virtual void Awake() override;

	virtual void Destroy() override;

	virtual void OnPut(CoordStruct* pLocation, DirType dir) override;

	virtual void OnUpdate() override;

	virtual void OnUpdateEnd() override;

	virtual void OnDetonate(CoordStruct* pCoords, bool& skip) override;

	// 状态机
	STATE_VAR_DEFINE(GiftBox);
	STATE_VAR_DEFINE(DestroySelf);
	STATE_VAR_DEFINE(Paintball);

	void AttachState()
	{
		STATE_VAR_INIT(GiftBox);
		STATE_VAR_INIT(DestroySelf);
		STATE_VAR_INIT(Paintball);
	}

	template <typename T>
	bool TryGetState(IStateScript*& state)
	{
		if (false) {}
		STATE_VAR_TRYGET(GiftBox)
		STATE_VAR_TRYGET(DestroySelf)
		STATE_VAR_TRYGET(DestroySelf)
		return state != nullptr;
	}

	TechnoClass* pSource = nullptr;
	HouseClass* pSourceHouse = nullptr;

	ObjectClass* pFakeTarget = nullptr;

	// 生命值和伤害值
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
			.Process(this->pSource)
			.Process(this->pSourceHouse)
			.Process(this->pFakeTarget)
			.Process(this->life)
			.Process(this->damage)

			.Process(this->SubjectToGround)

			.Process(this->SpeedChanged)
			.Process(this->LocationLocked)

			.Process(this->_initFlag)
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
	AttachEffect* AEManager();

	void InitState();

	// 礼物盒
	bool IsOnMark_GiftBox();
	void ReleaseGift(std::vector<std::string> gifts, GiftBoxData data);

	// 反抛射体
	void CanAffectAndDamageBullet(BulletClass* pTarget, WarheadTypeClass* pWH);

	void InitState_BlackHole();
	void InitState_ECM();
	void InitState_Proximity();

	void OnUpdate_DestroySelf();

	void OnUpdate_BlackHole();
	void OnUpdate_ECM();
	void OnUpdate_GiftBox();
	void OnUpdate_RecalculateStatus();
	void OnUpdate_SelfLaunchOrPumpAction();

	void OnUpdateEnd_BlackHole(CoordStruct& sourcePos);
	void OnUpdateEnd_Proximity(CoordStruct& sourcePos);

	bool OnDetonate_AntiBullet(CoordStruct* pCoords);
	bool OnDetonate_GiftBox(CoordStruct* pCoords);
	bool OnDetonate_SelfLaunch(CoordStruct* pCoords);

	bool _initFlag = false;

	bool _targetToAircraftFlag = false;

	bool _isBounceSplit = false; // 是弹跳抛射体分裂的子抛射体

	// 碰撞引信配置
	ProximityData* _proximityData = nullptr;
	ProximityData* GetProximityData();
	// 碰撞引信
	Proximity proximity{};
	bool _activeProximity = false;
	// 近炸引信
	int _proximityRange = -1;
};
