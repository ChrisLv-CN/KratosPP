#pragma once

#include <string>
#include <format>
#include <vector>
#include <map>

#include <TechnoClass.h>

#include <Utilities/Macro.h>
#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>

#include <Common/INI/INIConfig.h>

#include <Ext/Common/PrintTextData.h>
#include <Ext/EffectType/Effect/CrateBuffData.h>
#include <Ext/EffectType/Effect/StandData.h>
#include <Ext/ObjectType/State/AntiBulletState.h>
#include <Ext/ObjectType/State/DestroyAnimData.h>
#include <Ext/ObjectType/State/DestroySelfState.h>
#include <Ext/ObjectType/State/FireSuperData.h>
#include <Ext/ObjectType/State/GiftBoxState.h>
#include <Ext/ObjectType/State/PaintballState.h>
#include <Ext/ObjectType/State/TransformData.h>

class AttachEffect;

/// @brief base compoment, save the Techno status
class TechnoStatus : public TechnoScript
{
public:
	TECHNO_SCRIPT(TechnoStatus);

	virtual void Awake() override;

	virtual void Destroy() override;

	virtual void ExtChanged() override;

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

	virtual void OnPut(CoordStruct* pLocation, DirType dirType) override;

	virtual void OnUpdate() override;

	void OnUpdate_DeployToTransform(); // call by hook
	void OnUpdate_DestroySelf(); // call by Stand

	virtual void OnUpdateEnd() override;

	virtual void OnWarpUpdate() override;

	virtual void OnTemporalUpdate(TemporalClass* pTemporal) override;

	virtual void OnRemove() override;

	virtual void OnReceiveDamage(args_ReceiveDamage* args) override;

	virtual void OnReceiveDamageEnd(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse) override;

	virtual void OnReceiveDamageDestroy() override;

	virtual void OnRegisterDestruction(TechnoClass* pKiller, int cost, bool& skip);

	virtual void OnFire(AbstractClass* pTarget, int weaponIdx) override;

	virtual void OnSelect(bool& selectable) override;

	// 状态机
	AntiBulletState AntiBulletState{};
	State<CrateBuffData> CrateBuffState{};
	State<DestroyAnimData> DestroyAnimState{};
	DestroySelfState DestroySelfState{};
	State<FireSuperData> FireSuperState{};
	GiftBoxState GiftBoxState{};
	PaintballState PaintballState{};
	State<TransformData> TransformState{};

	// 踩箱子获得的buff
	CrateBuffData CrateBuff{};
	// 替身的配置
	StandData StandData{};
	TechnoClass* pMyMaster = nullptr;
	bool MyMasterIsSpawned = false;
	bool MyMasterIsAnim = false;
	bool StandIsMoving = false;

	DrivingState drivingState = DrivingState::Moving;

	bool DisableVoxelCache = false;

	bool DisableSelectVoice = false;

	bool CaptureByBlackHole = false;
	bool Jumping = false;
	bool Freezing = false;

	// 虚单位
	bool VirtualUnit = false;
	bool Disappear = false;

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
			.Process(this->TransformState)

			.Process(this->_initStateFlag)

			.Process(this->CrateBuff)
			.Process(this->StandData)
			.Process(this->pMyMaster)
			.Process(this->MyMasterIsSpawned)

			.Process(this->pKillerHouse)

			.Process(this->DisableVoxelCache)

			.Process(this->DisableSelectVoice)

			.Process(this->Freezing)

			.Process(this->VirtualUnit)
			.Process(this->Disappear)

			.Process(this->_deactivateDimEMP)
			.Process(this->_deactivateDimPowered)
			.Process(this->_berserkColor2)
			.Process(this->_buildingWasBerzerk)
			.Process(this->_buildingWasEMP)
			.Process(this->_buildingWasColor)

			.Process(this->pSourceType)
			.Process(this->pTargetType)
			.Process(this->_changeToType)
			.Process(this->_hasBeenChanged)
			.Process(this->_transformLocked)

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
	AttachEffect* AEManager();

	/**
	 *@brief 从TechnoType中读取扩展的ini标签，并初始化相应的component
	 *
	 */
	void InitExt();

	// 礼物盒
	bool IsOnMark_GiftBox();
	void ReleaseGift(std::vector<std::string> gifts, GiftBoxData data);

	// 反抛射体
	bool WeaponNoAA(int weaponIdx);

	// 变形
	void ChangeTechnoTypeTo(TechnoTypeClass* pNewType);

	void InitState();

	void InitState_AntiBullet();
	void InitState_CrateBuff();
	void InitState_DestroyAnim();
	void InitState_DestroySelf();
	void InitState_FireSuper();
	void InitState_GiftBox();
	void InitState_Paintball();
	void InitState_Transform();
	void InitState_VirtualUnit();

	void OnPut_Stand(CoordStruct* pCoord, DirType dirType);
	void OnRemove_Stand();

	void OnUpdate_AntiBullet();
	void OnUpdate_GiftBox();
	void OnUpdate_Paintball();
	void OnUpdate_Transform();

	void OnWarpUpdate_DestroySelf_Stand();

	void OnReceiveDamage_Stand(args_ReceiveDamage* args);

	void OnReceiveDamageEnd_DestroyAnim(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);
	void OnReceiveDamageEnd_BlackHole(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);
	void OnReceiveDamageEnd_GiftBox(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);

	void OnReceiveDamageDestroy_GiftBox();
	void OnReceiveDamageDestroy_Transform();

	void OnRegisterDestruction_Stand(TechnoClass* pKiller, int cost, bool& skip);

	void OnFire_FireSuper(AbstractClass* pTarget, int weaponIdx);

	bool OnSelect_VirtualUnit();
	bool OnSelect_Deselect();

	bool _initStateFlag = false;

	Mission _lastMission = Mission::Guard;

	CoordStruct _location{};
	bool _isMoving = false;

	// Buff
	CrateBuffData* _crateBuffData = nullptr;
	CrateBuffData* GetCrateBuffData();

	// 死亡动画
	DestroyAnimData* _destroyAnimData = nullptr;
	DestroyAnimData* GetDestroyAnimData();
	HouseClass* pKillerHouse = nullptr;

	// 部署变形
	DeployToTransformData* _transformData = nullptr;
	DeployToTransformData* GetTransformData();

	// 染色状态
	float _deactivateDimEMP = 0.8f;
	float _deactivateDimPowered = 0.5f;
	unsigned int _berserkColor2 = 0;
	bool _buildingWasBerzerk = false;
	bool _buildingWasEMP = false;
	bool _buildingWasColor = false;

	// 变形
	TechnoTypeClass* pSourceType = nullptr;
	TechnoTypeClass* pTargetType = nullptr;
	std::string _changeToType{ "" };
	bool _hasBeenChanged = false; // 处于变形状态
	bool _transformLocked = false; // 处于变形状态时锁定不允许再变形

	// EMP动画
	AnimClass* pExtraSparkleAnim = nullptr;
};
