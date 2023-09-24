#pragma once

#include <string>
#include <queue>

#include <GeneralStructures.h>
#include <SuperWeaponTypeClass.h>
#include <SuperClass.h>
#include <HouseClass.h>

#include <Common/EventSystems/EventSystem.h>

#include <Ext/FireSuper/FireSuper.h>

class FireSuperManager
{
public:
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
			int size = _superWeaponQueue.size();
			ExByteStream saver(sizeof(_superWeaponQueue) + (size * sizeof(FireSuper)));
			ExStreamWriter writer(saver);
			// 写入容器
			writer.Process(_superWeaponQueue, false);
			// 写入元素
			for (int i = 0; i < size; i++)
			{
				FireSuper super = _superWeaponQueue.front();
				writer.Process(super, false);
				_superWeaponQueue.pop();
				_superWeaponQueue.push(super);
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
			reader.Process(_superWeaponQueue, false);
			// 写入元素
			int size = _superWeaponQueue.size();
			// 清空容器，重新填入
			std::queue<FireSuper> empty{};
			for (int i = 0; i < size; i++)
			{
				FireSuper super{};
				reader.Process(super);
				empty.push(super);
			}
			std::swap(empty, _superWeaponQueue);
		}
	}
#pragma endregion

private:
	static void RealLaunch(HouseClass* pHouse, CellStruct targetPos, FireSuperEntity data);

	inline static std::queue<FireSuper> _superWeaponQueue{};
};
