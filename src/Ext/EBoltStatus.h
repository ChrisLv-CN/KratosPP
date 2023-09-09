#pragma once

#include <string>

#include <Extension.h>
#include <EBolt.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

/// @brief base compoment, save the EBolt status
class EBoltStatus : public ScriptComponent<EBolt>
{
public:
	EBoltStatus(Extension<EBolt> *ext) : ScriptComponent(ext)
	{
		this->Name = typeid(this).name();
	}

	ColorStruct Color1 = Colors::Empty;
	ColorStruct Color2 = Colors::Empty;
	ColorStruct Color3 = Colors::Empty;

	bool Disable1 = false;
	bool Disable2 = false;
	bool Disable3 = false;

#pragma region Save/Load
	template <typename T>
	void Serialize(T &stream)
	{
		stream
			.Process(this->Color1)
			.Process(this->Color2)
			.Process(this->Color3)
			.Process(this->Disable1)
			.Process(this->Disable2)
			.Process(this->Disable3)
			;
	};

	virtual void LoadFromStream(ExStreamReader &stream) override
	{
		Component::LoadFromStream(stream);
		this->Serialize(stream);
	}
	virtual void SaveToStream(ExStreamWriter &stream) override
	{
		Component::SaveToStream(stream);
		this->Serialize(stream);
	}
#pragma endregion

private:
};
