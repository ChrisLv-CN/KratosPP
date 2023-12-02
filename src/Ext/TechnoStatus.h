#pragma once

#include <string>
#include <format>
#include <vector>
#include <map>

#include <TechnoClass.h>

#include <Utilities/Macro.h>
#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include <Ext/State/AntiBulletState.h>
#include <Ext/State/CrateBuffData.h>
#include <Ext/State/DestroyAnimData.h>
#include <Ext/State/DestroySelfState.h>
#include <Ext/State/FireSuperData.h>
#include <Ext/State/GiftBoxState.h>
#include <Ext/State/PaintballState.h>

#include <Ext/TechnoType/AutoFireAreaWeaponData.h>
#include <Ext/TechnoType/BaseNormalData.h>
#include <Ext/TechnoType/CrawlingFLHData.h>
#include <Ext/TechnoType/DamageTextData.h>
#include <Ext/TechnoType/HealthTextData.h>
#include <Ext/TechnoType/JumpjetFacingData.h>
#include <Ext/TechnoType/MissileHomingData.h>
#include <Ext/TechnoType/SpawnData.h>

/// @brief base compoment, save the Techno status
class TechnoStatus : public TechnoScript
{
public:
	TECHNO_SCRIPT(TechnoStatus);

	virtual void Destroy() override;

	AbstractType GetAbsType();

	bool IsBuilding();
	bool IsInfantry();
	bool IsUnit();
	bool IsAircraft();

	LocoType GetLocoType();

	bool IsFly();
	bool IsJumpjet();
	bool IsShip();

	bool IsRocket();

	bool AmIStand();

	/**
	 *@brief 踩箱子获得的buff
	 *
	 */
	void RecalculateStatus();
	bool CanICloakByDefault();

	bool PlayDestroyAnims();

	unsigned int GetBerserkColor2();
	void SetExtraSparkleAnim(AnimClass* pAnim);

	void DrawSHP_Paintball(REGISTERS* R);
	void DrawSHP_Paintball_BuildingAnim(REGISTERS* R);
	void DrawSHP_Colour(REGISTERS* R);
	void DrawVXL_Paintball(REGISTERS* R, bool isBuilding);

	SpawnData* GetSpawnData();
	bool TryGetSpawnType(int i, std::string& newId);

	virtual void OnPut(CoordStruct* pLocation, DirType dirType) override;

	virtual void OnUpdate() override;

	void OnUpdate_DeployToTransform(); // call by hook

	virtual void OnUpdateEnd() override;

	virtual void OnWarpUpdate() override;

	virtual void OnTemporalUpdate(TemporalClass* pTemporal) override;

	virtual void OnRemove() override;

	virtual void OnReceiveDamageEnd(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse) override;

	virtual void OnReceiveDamageDestroy() override;

	virtual void DrawHealthBar(int barLength, Point2D* pPos, RectangleStruct* pBound, bool isBuilding) override;

	virtual void OnFire(AbstractClass* pTarget, int weaponIdx) override;

	// 状态机
	AntiBulletState AntiBulletState{};
	State<CrateBuffData> CrateBuffState{};
	State<DestroyAnimData> DestroyAnimState{};
	DestroySelfState DestroySelfState{};
	State<FireSuperData> FireSuperState{};
	GiftBoxState GiftBoxState{};
	PaintballState PaintballState{};

	// 踩箱子获得的buff
	CrateBuffData CrateBuff{};

	DrivingState drivingState = DrivingState::Moving;

	bool DisableVoxelCache = false;

	bool DisableSelectVoice = false;

	bool Freezing = false;

	// 光环武器
	bool SkipROF = false;

	// 虚单位
	bool VirtualUnit = false;
	bool Disappear = false;

	// 子机导弹跟踪
	bool IsHoming = false;
	CoordStruct HomingTargetLocation = CoordStruct::Empty;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->AntiBulletState)
			.Process(this->CrateBuffState)
			.Process(this->DestroyAnimState)
			.Process(this->DestroySelfState)
			.Process(this->FireSuperState)
			.Process(this->GiftBoxState)
			.Process(this->PaintballState)
			.Process(this->_initStateFlag)

			.Process(this->CrateBuff)

			.Process(this->pKillerHouse)

			.Process(this->DisableVoxelCache)

			.Process(this->DisableSelectVoice)

			.Process(this->Freezing)

			.Process(this->VirtualUnit)
			.Process(this->Disappear)

			.Process(this->IsHoming)
			.Process(this->HomingTargetLocation)
			.Process(this->_initHomingFlag)

			.Process(this->_skipDamageText)

			.Process(this->_deactivateDimEMP)
			.Process(this->_deactivateDimPowered)
			.Process(this->_berserkColor2)
			.Process(this->_buildingWasBerzerk)
			.Process(this->_buildingWasEMP)
			.Process(this->pExtraSparkleAnim)
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
		return const_cast<TechnoStatus*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	void OnTransform();

	void ResetBaseNormal();

	// 伤害数字
	bool SkipDrawDamageText(WarheadTypeClass* pWH, DamageTextData*& damageTextType);
	void OrderDamageText(std::wstring text, CoordStruct location, DamageText*& data);

	// 血量数字
	HealthTextData GetHealthTextData();
	void PrintHealthText(int barLength, Point2D* pPos, RectangleStruct* pBound, bool isBuilding);
	void OffsetPosAlign(Point2D& pos, int textWidth, int barWidth, PrintTextAlign align, bool isBuilding, bool useSHP);

	// 礼物盒
	bool IsOnMark_GiftBox();
	void ReleaseGift(std::vector<std::string> gifts, GiftBoxData data);

	// 反抛射体
	bool WeaponNoAA(int weaponIdx);

	void InitState();

	void InitState_AntiBullet();
	void InitState_CrateBuff();
	void InitState_DestroyAnim();
	void InitState_DestroySelf();
	void InitState_FireSuper();
	void InitState_GiftBox();
	void InitState_Paintball();

	void OnPut_Trail(CoordStruct* pLocation, DirType dir);
	void OnPut_AutoArea(CoordStruct* pLocation, DirType dir);
	void OnPut_BaseNormarl(CoordStruct* pLocation, DirType dir);

	void OnUpdate_AntiBullet();
	void OnUpdate_AutoArea();
	void OnUpdate_BaseNormal();
	void OnUpdate_CrawlingFLH();
	void OnUpdate_DamageText();
	void OnUpdate_DestroySelf();
	void OnUpdate_GiftBox();
	void OnUpdate_JJFacing();
	void OnUpdate_MissileHoming();
	void OnUpdate_Paintball();

	void OnWarpUpdate_DestroySelf_Stand();

	void OnRemove_BaseNormarl();

	void OnReceiveDamageEnd_DestroyAnim(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);
	void OnReceiveDamageEnd_BlackHole(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);
	void OnReceiveDamageEnd_DamageText(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);
	void OnReceiveDamageEnd_GiftBox(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);

	void OnReceiveDamageDestroy_BaseNormarl();
	void OnReceiveDamageDestroy_GiftBox();

	void OnFire_FireSuper(AbstractClass* pTarget, int weaponIdx);

	// 单位类型
	AbstractType _absType = AbstractType::None;
	LocoType _locoType = LocoType::None;

	bool _initStateFlag = false;

	Mission _lastMission = Mission::Guard;

	CoordStruct _location{};
	bool _isMoving = false;

	// 建造节点
	BaseNormalData* _baseNormalData = nullptr;
	BaseNormalData* GetBaseNormalData();

	// Buff
	CrateBuffData* _crateBuffData = nullptr;
	CrateBuffData* GetCrateBuffData();

	// 死亡动画
	DestroyAnimData* _destroyAnimData = nullptr;
	DestroyAnimData* GetDestroyAnimData();
	HouseClass* pKillerHouse = nullptr;

	// 伤害数字
	bool _skipDamageText = false;
	std::map<DamageText*, DamageTextCache> _damageCache{};
	std::map<DamageText*, DamageTextCache> _repairCache{};

	// 血条数字
	HealthTextData _healthTextData{}; // 个体设置

	// 光环武器
	AutoFireAreaWeaponData* _autoAreaData = nullptr;
	AutoFireAreaWeaponData* GetAutoAreaData();
	CDTimerClass _areaInitDelayTimer{};
	CDTimerClass _areaDelayTimer{};

	// 卧倒FLH
	CrawlingFLHData* _crawlingFLHData = nullptr;
	CrawlingFLHData* GetCrawlingFLHData();

	// 部署变形
	DeployToTransformData* _transformData = nullptr;
	DeployToTransformData* GetTransformData();

	// 子机管理器
	SpawnData* _spawnData = nullptr;

	// 子机导弹
	MissileHomingData* _homingData = nullptr;
	MissileHomingData* GetHomingData();
	bool _initHomingFlag = false;

	// JJFacing
	JumpjetFacingData* _jjFacingData = nullptr;
	JumpjetFacingData* GetJJFacingData();
	bool _JJNeedTurn = false;
	DirStruct _JJTurnTo{};
	int _JJFacing = -1;

	// 染色状态
	float _deactivateDimEMP = 0.8f;
	float _deactivateDimPowered = 0.5f;
	unsigned int _berserkColor2 = 0;
	bool _buildingWasBerzerk = false;
	bool _buildingWasEMP = false;
	bool _buildingWasColor = false;
	// EMP动画
	AnimClass* pExtraSparkleAnim = nullptr;
};
