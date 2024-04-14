#pragma once

#include <string>

#include <EBolt.h>

#include <Extension/EBoltExt.h>
#include <Common/Components/ScriptComponent.h>

/// @brief base compoment, save the EBolt status
class EBoltStatus : public EBoltScript
{
public:
	EBOLT_SCRIPT(EBoltStatus);

	void AttachTo(TechnoClass* pTechno, CoordStruct flh, bool isOnTurret, AbstractClass* pTarget);

	void OnTechnoDelete(EventSystem* sender, Event e, void* args);

	void OnDraw();

	void Destroy() override;

	int ArcCount = 8;

	ColorStruct Color1 = Colors::Empty;
	ColorStruct Color2 = Colors::Empty;
	ColorStruct Color3 = Colors::Empty;

	bool Disable1 = false;
	bool Disable2 = false;
	bool Disable3 = false;

	bool DisableParticle = false;

#pragma region Save/Load
	template <typename T>
	bool Serialize(T &stream)
	{
		return stream
			.Process(this->ArcCount)

			.Process(this->Color1)
			.Process(this->Color2)
			.Process(this->Color3)
			.Process(this->Disable1)
			.Process(this->Disable2)
			.Process(this->Disable3)

			.Process(this->DisableParticle)

			.Process(_owner)
			.Process(_flh)
			.Process(_isOnTurret)
			.Success();
	};
	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		Component::Load(stream, registerForChange);
		bool res = this->Serialize(stream);
		if (_owner)
		{
			EventSystems::General.AddHandler(Events::ObjectUnInitEvent, this, &EBoltStatus::OnTechnoDelete);
		}
		return res;
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		Component::Save(stream);
		return const_cast<EBoltStatus*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	TechnoClass* _owner = nullptr;
	CoordStruct _flh = CoordStruct::Empty;
	bool _isOnTurret = true;

	CoordStruct _targetFLH = CoordStruct::Empty;
};
