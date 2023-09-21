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

enum class DrivingState
{
	Moving = 0, Stand = 1, Start = 2, Stop = 3
};

/// @brief base compoment, save the Techno status
class TechnoStatus : public TechnoScript
{
public:
	TechnoStatus(TechnoExt::ExtData* ext) : TechnoScript(ext)
	{
		this->Name = typeid(this).name();

		EventSystems::Render.AddHandler(Events::GScreenRenderEvent, this, &TechnoStatus::DrawINFO);
	}

	virtual void Destroy() override
	{
		EventSystems::Render.RemoveHandler(Events::GScreenRenderEvent, this, &TechnoStatus::DrawINFO);
	}

	void DrawINFO(EventSystem* sender, Event e, void* args)
	{
		if (args)
		{
#ifdef DEBUG
			std::wstring text = std::format(L"{} {} {}", String2WString(extId), String2WString(thisId), String2WString(baseId));
			Point2D pos{};
			CoordStruct location = _owner->GetCoords();
			TacticalClass::Instance->CoordsToClient(location, &pos);
			DSurface::Temp->DrawText(text.c_str(), &pos, Drawing::RGB_To_Int(Drawing::TooltipColor.get()));
#endif // DEBUG
		}
	}

	bool SkipDrawDamageText(WarheadTypeClass* pWH, DamageTextData* &damageTextType);
	void OrderDamageText(std::string text, CoordStruct location, DamageText* &data);

	virtual void OnUpdate() override;

	void OnUpdate_DamageText();

	virtual void OnUpdateEnd() override;

	virtual void OnReceiveDamageEnd(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse) override;

	void OnReceiveDamageEnd_DestroyAnim(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);
	void OnReceiveDamageEnd_BlackHole(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);
	void OnReceiveDamageEnd_DamageText(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);
	void OnReceiveDamageEnd_GiftBox(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse);

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
	Mission _lastMission = Mission::Guard;

	// 伤害数字
	bool _skipDamageText = false;
	std::map<DamageText*, DamageTextCache> _damageCache{};
	std::map<DamageText*, DamageTextCache> _repairCache{};
};
