#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include "JumpjetFacingData.h"

/// @brief 动态载入组件
class JumpjetFacing : public TechnoScript
{
public:

	TECHNO_SCRIPT(JumpjetFacing);

	virtual void Awake() override;

	virtual void ExtChanged() override;

	virtual void OnUpdate() override;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->_JJNeedTurn)
			.Process(this->_JJTurnTo)
			.Process(this->_JJFacing)
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
		return const_cast<JumpjetFacing*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	// JJFacing
	JumpjetFacingData* _jjFacingData = nullptr;
	JumpjetFacingData* GetJJFacingData();

	bool _JJNeedTurn = false;
	DirStruct _JJTurnTo{};
	int _JJFacing = -1;

	void SetupJJFacing();
};
