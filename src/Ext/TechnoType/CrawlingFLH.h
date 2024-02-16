#pragma once

#include <string>
#include <vector>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>

#include "CrawlingFLHData.h"

/// @brief 动态载入组件
class CrawlingFLH : public TechnoScript
{
public:

	TECHNO_SCRIPT(CrawlingFLH);

	void Setup();

	virtual void Awake() override;

	virtual void ExtChanged() override;

	virtual void OnUpdate() override;

	bool SkipROF = false;

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
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
		return const_cast<CrawlingFLH*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	// 卧倒FLH
	CrawlingFLHData* _crawlingFLHData = nullptr;
	CrawlingFLHData* GetCrawlingFLHData();

};
