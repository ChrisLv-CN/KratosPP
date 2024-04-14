#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include "HealthTextData.h"

/// @brief 动态载入组件
class HealthText : public TechnoScript
{
public:

	TECHNO_SCRIPT(HealthText);

	virtual void Clean() override
	{
		TechnoScript::Clean();

		_healthTextData = {};
	}

	virtual void Awake() override;

	virtual void DrawHealthBar(int barLength, Point2D* pPos, RectangleStruct* pBound, bool isBuilding) override;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->_healthTextData)
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
		return const_cast<HealthText*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	void PrintHealthText(int barLength, Point2D* pPos, RectangleStruct* pBound, bool isBuilding);
	void OffsetPosAlign(Point2D& pos, int textWidth, int barWidth, PrintTextAlign align, bool isBuilding, bool useSHP);

	// 血条数字
	HealthTextData _healthTextData{}; // 个体设置
	HealthTextData GetHealthTextData();
};
