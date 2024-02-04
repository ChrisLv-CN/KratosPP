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

	ColorStruct Color1 = Colors::Empty;
	ColorStruct Color2 = Colors::Empty;
	ColorStruct Color3 = Colors::Empty;

	bool Disable1 = false;
	bool Disable2 = false;
	bool Disable3 = false;

#pragma region Save/Load
	template <typename T>
	bool Serialize(T &stream)
	{
		return stream
			.Process(this->Color1)
			.Process(this->Color2)
			.Process(this->Color3)
			.Process(this->Disable1)
			.Process(this->Disable2)
			.Process(this->Disable3)
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
		return const_cast<EBoltStatus*>(this)->Serialize(stream);
	}
#pragma endregion

private:
};
