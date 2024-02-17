#pragma once

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <type_traits>

#include <GeneralDefinitions.h>
#include <TechnoClass.h>
#include <HouseClass.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>

#include <Ext/Helper/MathEx.h>

#include <Ext/ObjectType/AttachEffect.h>
#include "AttachEffectData.h"

class IAEScript
{
public:
	virtual void Start() {};
	virtual void End(CoordStruct location) {};
	virtual void ResetDuration() {};

	virtual bool IsAlive() { return true; };

	/**
	 *@brief 渲染事件开始
	 *
	 * @param location 定位
	 */
	virtual void OnGScreenRender(CoordStruct location) {};
	/**
	 *@brief 渲染事件结束
	 *
	 * @param location 定位
	 */
	virtual void OnGScreenRenderEnd(CoordStruct location) {};

	/**
	 *@brief 子机导弹爆炸
	 *
	 */
	virtual void OnRocketExplosion() {};
};

/// @brief AE组件
/// GameObject
///		|__ AttachEffect
///				|__ AttachEffectScript#0
///						|__ EffectScript#0
///						|__ EffectScript#1
///				|__ AttachEffectScript#1
///						|__ EffectScript#0
///						|__ EffectScript#1
///						|__ EffectScript#2
class AttachEffectScript : public ObjectScript, public IAEScript
{
public:
	OBJECT_SCRIPT(AttachEffectScript);

	std::string Token = GetUUID();
	AttachEffectData AEData{};

	AttachEffect* _aeManager = nullptr;
	AttachEffect* GetAEManager();
	__declspec(property(get = GetAEManager)) AttachEffect* AEManager;

	TechnoClass* pSource = nullptr; // AE来源单位
	HouseClass* pSourceHouse = nullptr; // AE来源所属

	CoordStruct WarheadLocation{};
	bool FromWarhead = false;

	int AEMode = -1;
	bool FromPassenger = false;

	bool NonInheritable = false; // 不允许继承

	virtual void ResetDuration() override;
	int GetDuration();
	bool TryGetInitDelayTimeLeft(int& timeLeft);
	bool TryGetDurationTimeLeft(int& timeLeft);
	void MergeDuration(int otherDuration);
	void ForceStartLifeTimer(int timeLeft);
	void ResetEffectsDuration();

	void TimeToDie();

	bool IsSameGroup(AttachEffectData otherType);
	void GetMarks(std::vector<std::string>& marks);

	bool OwnerIsDead();

	void UpdateStandLocation(LocationMark locationMark);
	void UpdateAnimOffset(CoordStruct offset);

	/**
	 *@brief 检查AE是否仍然可用，只要有任意一个Effect失效，则判定为失活，失活的AE会从AE管理器中移除
	 *
	 * @return true
	 * @return false
	 */
	virtual bool IsAlive() override;

	/**
	 *@brief 根据AEData初始化AE配置，如果延迟生效则令Component失活等待延迟激活，同时调用InitEffects初始化子Effects
	 *
	 */
	virtual void Awake() override;

	virtual void Destroy() override;

	/**
	 *@brief 启用AE，根据初始延迟设置延迟启动效果
	 *
	 * @param pSource 来源
	 * @param pSourceHouse 来源所属
	 * @param warheadLocation 弹头赋予时爆炸的位置
	 * @param aeMode AE模式
	 * @param fromPassenger 由乘客赋予
	 */
	void Start(TechnoClass* pSource, HouseClass* pSourceHouse, CoordStruct warheadLocation, int aeMode, bool fromPassenger);

	/**
	 *@brief 强制关闭AE
	 *
	 * @param location 结束时的位置
	 */
	virtual void End(CoordStruct location) override;

	virtual void OnGScreenRender(CoordStruct location) override;
	virtual void OnGScreenRenderEnd(CoordStruct location) override;

#pragma region Save/Load
	template <typename T>
	bool Serialize(T& stream) {
		return stream
			.Process(this->Token)
			.Process(this->AEData)
			.Process(this->pSource)
			.Process(this->pSourceHouse)
			.Process(this->WarheadLocation)
			.Process(this->FromWarhead)
			.Process(this->AEMode)
			.Process(this->FromPassenger)
			.Process(this->NonInheritable)

			.Process(this->_duration)
			.Process(this->_immortal)
			.Process(this->_lifeTimer)
			.Process(this->_initialDelayTimer)
			.Process(this->_delayToEnable)
			.Process(this->_inBuilding)
			.Process(this->_isDelayToEnable)
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
		return const_cast<AttachEffectScript*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	int _duration = -1; // 寿命
	bool _immortal = true; // 永生
	CDTimerClass _lifeTimer{};
	CDTimerClass _initialDelayTimer{};
	bool _delayToEnable = false; // 延迟开启
	bool _inBuilding = false; // 是否在建造中
	bool _isDelayToEnable = false; // 需要延迟激活

	/**
	 *@brief 初始化所有的Effects并加入Sub-Component
	 *
	 */
	void InitEffects();

	/**
	 *@brief 启动生命计时器开始计时
	 *
	 */
	void SetupLifeTimer();

	/**
	 *@brief 是否仍在延迟启动中
	 *
	 * @return true 延迟结束
	 * @return false 延迟中
	 */
	bool InDelayToEnable();
	/**
	 *@brief 激活所有的Effects，开始执行Foreach逻辑
	 *
	 */
	void EnableEffects();
};
