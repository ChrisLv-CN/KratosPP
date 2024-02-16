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

#include "Status/AutoFireAreaWeaponData.h"
#include "Status/BaseNormalData.h"
#include "Status/CrawlingFLHData.h"
#include "Status/DamageTextData.h"
#include "Status/HealthTextData.h"
#include "Status/SpawnData.h"

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

	SpawnData* GetSpawnData();
	bool TryGetSpawnType(int i, std::string& newId);

	virtual void OnPut(CoordStruct* pLocation, DirType dirType) override;

	virtual void OnUpdate() override;

	void OnUpdate_DeployToTransform(); // call by hook
	void OnUpdate_DestroySelf(); // call by Stand

	virtual void OnUpdateEnd() override;

	virtual void OnWarpUpdate() override;

	virtual void OnTemporalUpdate(TemporalClass* pTemporal) override;

	virtual void OnRemove() override;

	virtual void OnReceiveDamageEnd(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse) override;

	virtual void OnReceiveDamageDestroy() override;

	virtual void DrawHealthBar(int barLength, Point2D* pPos, RectangleStruct* pBound, bool isBuilding) override;

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

	bool Freezing = false;

	// 光环武器
	bool SkipROF = false;

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

			.Process(this->_skipDamageText)

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

	void OnPut_AutoArea(CoordStruct* pLocation, DirType dir);
	void OnPut_BaseNormarl(CoordStruct* pLocation, DirType dir);

	void OnUpdate_AntiBullet();
	void OnUpdate_AutoArea();
	void OnUpdate_BaseNormal();
	void OnUpdate_CrawlingFLH();
	void OnUpdate_DamageText();
	void OnUpdate_GiftBox();
	void OnUpdate_Paintball();
	void OnUpdate_Transform();

	void OnWarpUpdate_DestroySelf_Stand();

	void OnRemove_BaseNormarl();

	void OnReceiveDamageEnd_DestroyAnim(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);
	void OnReceiveDamageEnd_BlackHole(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);
	void OnReceiveDamageEnd_DamageText(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);
	void OnReceiveDamageEnd_GiftBox(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);

	void OnReceiveDamageDestroy_BaseNormarl();
	void OnReceiveDamageDestroy_GiftBox();
	void OnReceiveDamageDestroy_Transform();

	void OnFire_FireSuper(AbstractClass* pTarget, int weaponIdx);

	bool OnSelect_VirtualUnit();
	bool OnSelect_Deselect();

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
