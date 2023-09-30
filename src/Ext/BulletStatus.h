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
#include <Common/EventSystems/EventSystem.h>
#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>

#include <Ext/Helper/CastEx.h>

#include <Ext/State/GiftBoxState.h>
#include <Ext/State/PaintballState.h>

#include <Ext/BulletType/ProximityData.h>
#include <Ext/BulletType/TrajectoryData.h>

/// @brief 抛射体的生存属性
struct BulletLife
{
public:
	bool Interceptable = false; // 可被伤害
	int Strength = -1;			// 自定义血量

	int Health = 1;			 // 生命值
	bool IsDetonate = false; // 已损毁
	bool IsHarmless = false; // 无害
	bool SkipAE = false;	 // 爆炸不赋予AE

	void Read(INIBufferReader* ini)
	{
		this->Interceptable = ini->Get("Interceptable", Interceptable);
		this->Strength = ini->Get("Strength", Strength);
		if (Strength > 0)
		{
			this->Health = Strength;
		}
	}

	/// @brief 直接摧毁
	/// @param harmless 无害
	/// @param skipAE 不赋予AE
	void Detonate(bool harmless = false, bool skipAE = false)
	{
		this->Health = -1;
		this->IsDetonate = true;
		this->IsHarmless = harmless;
		this->SkipAE = skipAE;
	}

	/// @brief 收到伤害
	/// @param damage 伤害数值
	/// @param harmless 无害
	/// @param skipAE 不赋予AE
	void TakeDamage(int damage, bool harmless, bool skipAE = false)
	{
		this->Health -= damage;
		this->IsDetonate = this->Health <= 0;
		this->IsHarmless = harmless;
		if (IsDetonate)
		{
			this->SkipAE = skipAE;
		}
	}
};

/// @brief 对抛射体的伤害属性
struct BulletDamage
{
public:
	int Damage = 0;		   // 伤害
	bool Eliminate = true; // 一击必杀
	bool Harmless = false; // 和平处置
};

/// @brief base compoment, save the Techno status
class BulletStatus : public BulletScript
{
public:
	BulletStatus(BulletExt::ExtData* ext) : BulletScript(ext)
	{
		this->Name = typeid(this).name();
	}

	BulletType GetBulletType();

	bool IsArcing();
	bool IsMissile();
	bool IsRocket();
	bool IsBomb();

	virtual void Awake() override;
	virtual void Destroy() override;

	void TakeDamage(int damage, bool eliminate, bool harmless, bool checkInterceptable = false);

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

	virtual void OnPut(CoordStruct* pLocation, DirType dir) override;

	virtual void OnUpdate() override;

	virtual void OnUpdateEnd() override;

	virtual void OnDetonate(CoordStruct* pCoords, bool& skip) override;

	GiftBoxState GiftBoxState{};
	PaintballState PaintballState{};

	TechnoClass* pSource = nullptr;
	HouseClass* pSourceHouse = nullptr;

	ObjectClass* pFakeTarget = nullptr;

	BulletLife life = {};
	BulletDamage damage = {};

	// 碰触地面会炸
	bool SubjectToGround = false;
	// 是弹跳抛射体分裂的子抛射体
	bool IsBounceSplit = false;
	// 正在被黑洞吸引
	bool CaptureByBlackHole = false;

	static std::vector<BulletClass*> TargetAircraftBullets;

	virtual void InvalidatePointer(void* ptr) override
	{
		AnnounceInvalidPointer(this->pSource, ptr);
		AnnounceInvalidPointer(this->pSourceHouse, ptr);
		AnnounceInvalidPointer(this->pFakeTarget, ptr);
	};

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->GiftBoxState)

			.Process(this->pSource)
			.Process(this->pSourceHouse)
			.Process(this->life)
			.Process(this->damage)

			.Process(this->SubjectToGround)
			.Process(this->IsBounceSplit)
			.Process(this->pFakeTarget)
			.Process(this->TargetAircraftBullets)
			.Process(this->_initFlag)
			// 弹道控制
			.Process(this->_arcingTrajectoryInitFlag)
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

	// 弹道控制
	bool _arcingTrajectoryInitFlag = false;
	bool _missileShakeVelocityFlag = false;

	// 直线
	StraightBullet straightBullet{};
	bool _resetTargetFlag = false;

	// 碰撞引信配置
	ProximityData* _proximityData = nullptr;
	ProximityData* GetProximityData();
	__declspec(property(get = GetProximityData)) ProximityData* proximityData;
	// 碰撞引信
	Proximity proximity{};
	bool _activeProximity = false;
	// 近炸引信
	int _proximityRange = -1;
};
