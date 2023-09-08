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

/// @brief base compoment, save the Techno status
class AnimStatus : public AnimScript
{
public:
	AnimStatus(Extension<AnimClass> *ext) : AnimScript(ext)
	{
		this->Name = typeid(this).name();
	}

	virtual void Awake() override
	{
	}

	/// @brief 接管伤害制造
	/// @param isBounce 是否流星、碎片类
	/// @param bright 弹头闪光
	void Explosion_Damage(bool isBounce = false, bool bright = false){};

	/// @brief 替换流星、碎片击中水中的动画
	/// @return true=替换成其他的动画
	bool OverrideExpireAnimOnWater() { return false; };

	TechnoClass *pCreater;

	virtual void InvalidatePointer(void *ptr) override
	{
		AnnounceInvalidPointer(this->pCreater, ptr);
	};
#pragma region Save/Load
	template <typename T>
	void Serialize(T &stream)
	{ };

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
