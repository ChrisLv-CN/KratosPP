#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>
#include <SuperWeaponTypeClass.h>
#include <SuperClass.h>
#include <HouseClass.h>

#include <Common/EventSystems/EventSystem.h>
#include <Ext/Helper/MathEx.h>

#include <Ext/StateType/State/FireSuperData.h>

class FireSuperManager
{
public:
	struct FireSuper
	{
	public:
		int HouseIndex = -1;
		CellStruct TargetPos{};
		FireSuperEntity Data{};

		FireSuper()
		{ }

		FireSuper(HouseClass* pHouse, CellStruct targetPos, FireSuperEntity data)
		{
			this->HouseIndex = pHouse->ArrayIndex;
			this->TargetPos = targetPos;
			this->Data = data;

			this->count = 0;
			this->initDelay = GetRandomValue(data.RandomInitDelay, data.InitDelay);
			this->initDelayTimer.Start(initDelay);
			this->delay = GetRandomValue(data.RandomDelay, data.Delay);
			this->delayTimer.Start(0);
		}

		HouseClass* GetHouse()
		{
			HouseClass* pHouse = nullptr;
			if (HouseIndex > -1)
			{
				pHouse = HouseClass::Array->GetItem(HouseIndex);
			}
			return pHouse;
		}

		bool CanLaunch()
		{
			return initDelayTimer.Expired() && delayTimer.Expired();
		}

		bool IsDone()
		{
			return Data.LaunchCount > 0 && count >= Data.LaunchCount;
		}

		bool Cooldown()
		{
			count++;
			delayTimer.Start(delay);
			return IsDone();
		}

#pragma region save/load
		template <typename T>
		bool Serialize(T& stream)
		{
			return stream
				.Process(this->HouseIndex)
				.Process(this->TargetPos)
				.Process(this->Data) // call FireSuperEntity::Save/Load
				.Process(this->count)
				.Process(this->initDelay)
				.Process(this->initDelayTimer)
				.Process(this->delay)
				.Process(this->delayTimer)
				.Success();
		};

		virtual bool Load(ExStreamReader& stream, bool registerForChange)
		{
			return this->Serialize(stream);
		}
		virtual bool Save(ExStreamWriter& stream) const
		{
			return const_cast<FireSuper*>(this)->Serialize(stream);
		}
#pragma endregion
	private:
		int count;
		int initDelay;
		CDTimerClass initDelayTimer;
		int delay;
		CDTimerClass delayTimer;
	};

	/**
	 *@brief 添加一个超级武器发射订单，将在Update执行时发射
	 *
	 * @param pHouse 发射方
	 * @param location 目标地点
	 * @param data 射击诸元
	 */
	static void Order(HouseClass* pHouse, CoordStruct location, FireSuperEntity data);

	/**
	 *@brief 当场发射一个超武器
	 *
	 * @param pHouse 发射方
	 * @param location 目标地点
	 * @param data 射击诸元
	 */
	static void Launch(HouseClass* pHouse, CoordStruct location, FireSuperEntity data);

	static void Clear(EventSystem* sender, Event e, void* args);

	static void Update(EventSystem* sender, Event e, void* args);

	static SuperClass* FindSuperWeapon(HouseClass* pHouse, SuperWeaponTypeClass* pType);

#pragma region Save/Load
	static void SaveSuperQueue(EventSystem* sender, Event e, void* args)
	{
		SaveGameEventArgs* arg = (SaveGameEventArgs*)args;
		if (arg->IsStartInStream())
		{
			int size = _superWeapons.size();
			ExByteStream saver(sizeof(_superWeapons) + (size * sizeof(FireSuper)));
			ExStreamWriter writer(saver);
			// 写入容器
			writer.Process(_superWeapons, false);
			// 写入元素
			for (FireSuper super : _superWeapons)
			{
				writer.Process(super);
			}
			saver.WriteBlockToStream(arg->Stream);
		}
	}
	static void LoadSuperQueue(EventSystem* sender, Event e, void* args)
	{
		LoadGameEventArgs* arg = (LoadGameEventArgs*)args;
		if (arg->IsStartInStream())
		{
			ExByteStream loader(0);
			loader.ReadBlockFromStream(arg->Stream);
			ExStreamReader reader(loader);
			// 写入容器
			reader.Process(_superWeapons, false);
			// 写入元素
			int size = _superWeapons.size();
			// 清空容器，重新填入
			std::vector<FireSuper> empty{};
			for (int i = 0; i < size; i++)
			{
				FireSuper super{};
				reader.Process(super);
				empty.emplace_back(super);
			}
			std::swap(empty, _superWeapons);
		}
	}
#pragma endregion

private:
	static void RealLaunch(HouseClass* pHouse, CellStruct targetPos, FireSuperEntity data);

	inline static std::vector<FireSuper> _superWeapons{};
};
