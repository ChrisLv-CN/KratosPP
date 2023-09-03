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
	bool OverrideExpireAnimOnWater() { return false;  };

	TechnoClass *pCreater;

	virtual void InvalidatePointer(void *ptr) override
	{
		AnnounceInvalidPointer(this->pCreater, ptr);
	};

#pragma region save/load
	template <typename T>
	void Serialize(T &stream){
		// stream.Process(this->laserColor).Process(this->colorChanged);
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


// Helper
static AnimClass *SetAnimOwner(AnimClass *pAnim, HouseClass *pHouse)
{
	pAnim->Owner = pHouse;
	return pAnim;
}

static AnimClass *SetAnimOwner(AnimClass *pAnim, TechnoClass *pTechno)
{
	pAnim->Owner = pTechno->Owner;
	return pAnim;
}

static AnimClass *SetAnimOwner(AnimClass *pAnim, BulletClass *pBullet)
{
	if (BulletStatus *status = GetStatus<BulletExt, BulletStatus>(pBullet))
	{
		pAnim->Owner = status->pSourceHouse;
	}
	return pAnim;
}

static AnimClass *SetAnimCreater(AnimClass *pAnim, TechnoClass *pTechno)
{
	if (AnimStatus *status = GetStatus<AnimExt, AnimStatus>(pAnim))
	{
		status->pCreater = pTechno;
	}
	return pAnim;
}

static AnimClass *SetAnimCreater(AnimClass *pAnim, BulletClass *pBullet)
{
	TechnoClass *Source = pBullet->Owner;
	if (!IsDead(Source))
	{
		if (AnimStatus *status = GetStatus<AnimExt, AnimStatus>(pAnim))
		{
			status->pCreater = Source;
		}
	}
	return pAnim;
}
