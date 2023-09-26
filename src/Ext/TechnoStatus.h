#pragma once

#include <string>
#include <format>
#include <codecvt>
#include <vector>
#include <map>

#include <TechnoClass.h>

#include <Utilities/Macro.h>
#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>

#include <Ext/State/PaintballState.h>

#include <Ext/TechnoType/DamageTextData.h>
#include <Ext/TechnoType/HealthTextData.h>

enum class DrivingState
{
	Moving = 0, Stand = 1, Start = 2, Stop = 3
};

enum class LocoType
{
	None = 0,
	Drive = 1,
	Hover = 2,
	Tunnel = 3,
	Walk = 4,
	Droppod = 5,
	Fly = 6,
	Teleport = 7,
	Mech = 8,
	Ship = 9,
	Jumpjet = 10,
	Rocket = 11
};

/// @brief base compoment, save the Techno status
class TechnoStatus : public TechnoScript
{
public:
	TechnoStatus(TechnoExt::ExtData* ext) : TechnoScript(ext)
	{
		this->Name = typeid(this).name();
	}

	virtual void Awake() override;

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

	bool AmIStand();

	unsigned int GetBerserkColor2();
	void SetExtraSparkleAnim(AnimClass* pAnim);

	void DrawSHP_Paintball(REGISTERS* R);
	void DrawSHP_Paintball_BuildingAnim(REGISTERS* R);
	void DrawSHP_Colour(REGISTERS* R);
	void DrawVXL_Paintball(REGISTERS* R, bool isBuilding);

	virtual void OnPut(CoordStruct* pLocation, DirType dirType) override;

	void InitState_Paintball();

	virtual void OnUpdate() override;

	void OnUpdate_Paintball();

	virtual void OnUpdateEnd() override;

	virtual void OnTemporalUpdate(TemporalClass* pTemporal) override;

	virtual void OnReceiveDamageEnd(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse) override;

	virtual void DrawHealthBar(int barLength, Point2D* pPos, RectangleStruct* pBound, bool isBuilding) override;

	virtual void OnFire(AbstractClass* pTarget, int weaponIdx) override;

	void OnFire_FireSuper(AbstractClass* pTarget, int weaponIdx);

	PaintballState PaintballState{};

	DrivingState drivingState = DrivingState::Moving;
	bool DisableVoxelCache = false;
	float VoxelShadowScaleInAir = 2.0f;

	bool DisableSelectVoice = false;

	bool Freezing = false;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->PaintballState)
			.Process(this->_initStateFlag)

			.Process(this->DisableVoxelCache)
			.Process(this->VoxelShadowScaleInAir)

			.Process(this->DisableSelectVoice)

			.Process(this->Freezing)

			.Process(this->_skipDamageText)
			.Process(this->_isFearless)

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
	// 伤害数字
	bool SkipDrawDamageText(WarheadTypeClass* pWH, DamageTextData*& damageTextType);
	void OrderDamageText(std::wstring text, CoordStruct location, DamageText*& data);

	// 血量数字
	static HealthTextControlData GetHealthTextControlData();
	HealthTextData GetHealthTextData();
	void PrintHealthText(int barLength, Point2D* pPos, RectangleStruct* pBound, bool isBuilding);
	void OffsetPosAlign(Point2D& pos, int textWidth, int barWidth, PrintTextAlign align, bool isBuilding, bool useSHP);

	void InitState();

	void OnUpdate_DamageText();

	void OnReceiveDamageEnd_DestroyAnim(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);
	void OnReceiveDamageEnd_BlackHole(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);
	void OnReceiveDamageEnd_DamageText(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);
	void OnReceiveDamageEnd_GiftBox(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);

	// 单位类型
	AbstractType _absType = AbstractType::None;
	LocoType _locoType = LocoType::None;

	bool _initStateFlag = false;

	bool _isFearless = false;

	Mission _lastMission = Mission::Guard;

	CoordStruct _location{};
	bool _isMoving = false;

	// 伤害数字
	bool _skipDamageText = false;
	std::map<DamageText*, DamageTextCache> _damageCache{};
	std::map<DamageText*, DamageTextCache> _repairCache{};

	// 血条数字
	inline static HealthTextControlData _healthControlData{}; // 全局默认设置
	HealthTextData _healthTextData{}; // 个体设置

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
