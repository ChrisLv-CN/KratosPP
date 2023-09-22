#pragma once

#include <string>
#include <format>
#include <codecvt>
#include <vector>
#include <map>

#include <TechnoClass.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>

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

	AbstractType GetAbsType();

	bool IsBuilding();
	bool IsInfantry();
	bool IsUnit();
	bool IsAircraft();

	LocoType GetLocoType();

	bool IsFly();
	bool IsJumpjet();
	bool IsShip();

	virtual void OnUpdate() override;

	virtual void OnUpdateEnd() override;

	virtual void OnTemporalUpdate(TemporalClass* pTemporal) override;

	virtual void OnReceiveDamageEnd(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse) override;

	virtual void DrawHealthBar(int barLength, Point2D* pPos, RectangleStruct* pBound, bool isBuilding) override;

	DrivingState drivingState = DrivingState::Moving;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->_skipDamageText)
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

	void OnUpdate_DamageText();

	void OnReceiveDamageEnd_DestroyAnim(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);
	void OnReceiveDamageEnd_BlackHole(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);
	void OnReceiveDamageEnd_DamageText(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);
	void OnReceiveDamageEnd_GiftBox(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);

	// 单位类型
	AbstractType _absType = AbstractType::None;
	LocoType _locoType = LocoType::None;

	bool _initFlag = false;

	bool _isVoxel = true;
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
};
