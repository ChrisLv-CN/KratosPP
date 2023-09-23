#pragma once

#include <string>

#include <Extension.h>
#include <TechnoClass.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>
#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>
#include <Extension/AnimExt.h>
#include <Ext/BulletStatus.h>
#include <Ext/TechnoStatus.h>
#include <Ext/Data/OffsetData.h>

/// @brief base compoment, save the Techno status
class AnimStatus : public AnimScript
{
public:
	AnimStatus(AnimExt::ExtData* ext) : AnimScript(ext)
	{
		this->Name = typeid(this).name();
	}

	void SetOffset(OffsetData data);

	/// @brief 接管伤害制造
	/// @param isBounce 是否流星、碎片类
	/// @param bright 弹头闪光
	void Explosion_Damage(bool isBounce = false, bool bright = false) {};

	/// @brief 替换流星、碎片击中水中的动画
	/// @return true=替换成其他的动画
	bool OverrideExpireAnimOnWater() { return false; };

	virtual void OnUpdate() override;

	void OnUpdate_Visibility();
	void OnUpdate_Damage();
	void OnUpdate_SpawnAnims();

	virtual void OnLoop() override;

	void OnLoop_SpawnAnims();

	virtual void OnDone() override;

	void OnDone_SpawnAnims();

	virtual void OnNext(AnimTypeClass* pNext) override;

	void OnNext_SpawnAnims(AnimTypeClass* pNext);

	TechnoClass* pCreater = nullptr;
	TechnoClass* pAttachOwner = nullptr; // 动画附着的对象
	CoordStruct Offset = CoordStruct::Empty; // 附着的偏移位置

	virtual void InvalidatePointer(void* ptr) override
	{
		AnnounceInvalidPointer(this->pCreater, ptr);
	};
#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->pCreater)
			.Process(this->pAttachOwner)
			.Process(this->Offset)
			.Process(this->_offsetData)
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
		return const_cast<AnimStatus*>(this)->Serialize(stream);
	}
#pragma endregion

private:
	OffsetData _offsetData{};
};
