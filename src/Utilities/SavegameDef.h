#pragma once

// include this file whenever something is to be saved.

#include "Savegame.h"

#include <vector>
#include <map>
#include <bitset>
#include <memory>

#include <ArrayClasses.h>
#include <FileSystem.h>
#include <FileFormats/SHP.h>
#include <RulesClass.h>
#include <SidebarClass.h>
#include <Utilities/Constructs.h>

#include "Swizzle.h"
#include "Debug.h"

namespace Savegame
{
	template <typename T>
	concept ImplementsUpperCaseSaveLoad = requires (ExStreamWriter& stmWriter, ExStreamReader& stmReader, T& value, bool registerForChange)
	{
		value.Save(stmWriter);
		value.Load(stmReader, registerForChange);
	};

	template <typename T>
	concept ImplementsLowerCaseSaveLoad = requires (ExStreamWriter & stmWriter, ExStreamReader & stmReader, T& value, bool registerForChange)
	{
		value.save(stmWriter);
		value.load(stmReader, registerForChange);
	};

	#pragma warning(push)
	#pragma warning(disable: 4702) // MSVC isn't smart enough and yells about unreachable code

	template <typename T>
	bool ReadExStream(ExStreamReader& stm, T& value, bool registerForChange)
	{
		if constexpr (ImplementsUpperCaseSaveLoad<T>)
			return value.Load(stm, registerForChange);

		else if constexpr (ImplementsLowerCaseSaveLoad<T>)
			return value.load(stm, registerForChange);

		ExStreamObject<T> item;
		return item.ReadFromStream(stm, value, registerForChange);
	}

	template <typename T>
	bool WriteExStream(ExStreamWriter& stm, const T& value)
	{
		if constexpr (ImplementsUpperCaseSaveLoad<T>)
			return value.Save(stm);

		if constexpr (ImplementsLowerCaseSaveLoad<T>)
			return value.save(stm);

		ExStreamObject<T> item;
		return item.WriteToStream(stm, value);
	}

	#pragma warning(pop)

	template <typename T>
	T* RestoreObject(ExStreamReader& Stm, bool RegisterForChange)
	{
		T* ptrOld = nullptr;
		if (!Stm.Load(ptrOld))
			return nullptr;

		if (ptrOld)
		{
			std::unique_ptr<T> ptrNew = ObjectFactory<T>()(Stm);

			if (Savegame::ReadExStream(Stm, *ptrNew, RegisterForChange))
			{
				ExSwizzle::Instance.RegisterChange(ptrOld, ptrNew.get());
				return ptrNew.release();
			}
		}

		return nullptr;
	}

	template <typename T>
	bool PersistObject(ExStreamWriter& Stm, const T* pValue)
	{
		if (!Savegame::WriteExStream(Stm, pValue))
			return false;

		if (pValue)
			return Savegame::WriteExStream(Stm, *pValue);

		return true;
	}

	template <typename T>
	bool ExStreamObject<T>::ReadFromStream(ExStreamReader& Stm, T& Value, bool RegisterForChange) const
	{
		bool ret = Stm.Load(Value);

		if (RegisterForChange)
			Swizzle swizzle(Value);

		return ret;
	}

	template <typename T>
	bool ExStreamObject<T>::WriteToStream(ExStreamWriter& Stm, const T& Value) const
	{
		Stm.Save(Value);
		return true;
	}


	// specializations

	template <typename T>
	struct Savegame::ExStreamObject<VectorClass<T>>
	{
		bool ReadFromStream(ExStreamReader& Stm, VectorClass<T>& Value, bool RegisterForChange) const
		{
			Value.Clear();
			int Capacity = 0;

			if (!Stm.Load(Capacity))
				return false;

			Value.Reserve(Capacity);

			for (auto ix = 0; ix < Capacity; ++ix)
			{
				if (!Savegame::ReadExStream(Stm, Value.Items[ix], RegisterForChange))
					return false;
			}

			return true;
		}

		bool WriteToStream(ExStreamWriter& Stm, const VectorClass<T>& Value) const
		{
			Stm.Save(Value.Capacity);

			for (auto ix = 0; ix < Value.Capacity; ++ix)
			{
				if (!Savegame::WriteExStream(Stm, Value.Items[ix]))
					return false;
			}

			return true;
		}
	};

	template <typename T>
	struct Savegame::ExStreamObject<DynamicVectorClass<T>>
	{
		bool ReadFromStream(ExStreamReader& Stm, DynamicVectorClass<T>& Value, bool RegisterForChange) const
		{
			Value.Clear();
			int Capacity = 0;

			if (!Stm.Load(Capacity))
				return false;

			Value.Reserve(Capacity);

			if (!Stm.Load(Value.Count) || !Stm.Load(Value.CapacityIncrement))
				return false;

			for (auto ix = 0; ix < Value.Count; ++ix)
			{
				if (!Savegame::ReadExStream(Stm, Value.Items[ix], RegisterForChange))
					return false;
			}

			return true;
		}

		bool WriteToStream(ExStreamWriter& Stm, const DynamicVectorClass<T>& Value) const
		{
			Stm.Save(Value.Capacity);
			Stm.Save(Value.Count);
			Stm.Save(Value.CapacityIncrement);

			for (auto ix = 0; ix < Value.Count; ++ix)
			{
				if (!Savegame::WriteExStream(Stm, Value.Items[ix]))
					return false;
			}

			return true;
		}
	};

	template <typename T>
	struct Savegame::ExStreamObject<TypeList<T>>
	{
		bool ReadFromStream(ExStreamReader& Stm, TypeList<T>& Value, bool RegisterForChange) const
		{
			if (!Savegame::ReadExStream<DynamicVectorClass<T>>(Stm, Value, RegisterForChange))
				return false;

			return Stm.Load(Value.unknown_18);
		}

		bool WriteToStream(ExStreamWriter& Stm, const TypeList<T>& Value) const
		{
			if (!Savegame::WriteExStream<DynamicVectorClass<T>>(Stm, Value))
				return false;

			Stm.Save(Value.unknown_18);
			return true;
		}
	};

	template <>
	struct Savegame::ExStreamObject<CounterClass>
	{
		bool ReadFromStream(ExStreamReader& Stm, CounterClass& Value, bool RegisterForChange) const
		{
			if (!Savegame::ReadExStream<VectorClass<int>>(Stm, Value, RegisterForChange))
				return false;

			return Stm.Load(Value.Total);
		}

		bool WriteToStream(ExStreamWriter& Stm, const CounterClass& Value) const
		{
			if (!Savegame::WriteExStream<VectorClass<int>>(Stm, Value))
				return false;

			Stm.Save(Value.Total);
			return true;
		}
	};

	template <size_t Size>
	struct Savegame::ExStreamObject<std::bitset<Size>>
	{
		bool ReadFromStream(ExStreamReader& Stm, std::bitset<Size>& Value, bool RegisterForChange) const
		{
			unsigned char value = 0;
			for (auto i = 0u; i < Size; ++i)
			{
				auto pos = i % 8;

				if (pos == 0 && !Stm.Load(value))
					return false;

				Value.set(i, ((value >> pos) & 1) != 0);
			}

			return true;
		}

		bool WriteToStream(ExStreamWriter& Stm, const std::bitset<Size>& Value) const
		{
			unsigned char value = 0;
			for (auto i = 0u; i < Size; ++i)
			{
				auto pos = i % 8;

				if (Value[i])
					value |= 1 << pos;

				if (pos == 7 || i == Size - 1)
				{
					Stm.Save(value);
					value = 0;
				}
			}

			return true;
		}
	};

	template <>
	struct Savegame::ExStreamObject<std::string>
	{
		bool ReadFromStream(ExStreamReader& Stm, std::string& Value, bool RegisterForChange) const
		{
			size_t size = 0;

			if (Stm.Load(size))
			{
				std::vector<char> buffer(size);

				if (!size || Stm.Read(reinterpret_cast<byte*>(&buffer[0]), size))
				{
					Value.assign(buffer.begin(), buffer.end());
					return true;
				}
			}
			return false;
		}

		bool WriteToStream(ExStreamWriter& Stm, const std::string& Value) const
		{
			Stm.Save(Value.size());
			Stm.Write(reinterpret_cast<const byte*>(Value.c_str()), Value.size());

			return true;
		}
	};

	template <typename T>
	struct Savegame::ExStreamObject<std::unique_ptr<T>>
	{
		bool ReadFromStream(ExStreamReader& Stm, std::unique_ptr<T>& Value, bool RegisterForChange) const
		{
			Value.reset(RestoreObject<T>(Stm, RegisterForChange));
			return true;
		}

		bool WriteToStream(ExStreamWriter& Stm, const std::unique_ptr<T>& Value) const
		{
			return PersistObject(Stm, Value.get());
		}
	};

	template <typename T>
	struct Savegame::ExStreamObject<std::vector<T>>
	{
		bool ReadFromStream(ExStreamReader& Stm, std::vector<T>& Value, bool RegisterForChange) const
		{
			Value.clear();

			size_t Capacity = 0;

			if (!Stm.Load(Capacity))
				return false;

			Value.reserve(Capacity);

			size_t Count = 0;

			if (!Stm.Load(Count))
				return false;

			Value.resize(Count);

			for (auto ix = 0u; ix < Count; ++ix)
			{
				if (!Savegame::ReadExStream(Stm, Value[ix], RegisterForChange))
					return false;
			}

			return true;
		}

		bool WriteToStream(ExStreamWriter& Stm, const std::vector<T>& Value) const
		{
			Stm.Save(Value.capacity());
			Stm.Save(Value.size());

			for (auto ix = 0u; ix < Value.size(); ++ix)
			{
				if (!Savegame::WriteExStream(Stm, Value[ix]))
					return false;
			}

			return true;
		}
	};

	template <typename TKey, typename TValue>
	struct Savegame::ExStreamObject<std::map<TKey, TValue>>
	{
		bool ReadFromStream(ExStreamReader& Stm, std::map<TKey, TValue>& Value, bool RegisterForChange) const
		{
			Value.clear();

			size_t Count = 0;
			if (!Stm.Load(Count))
			{
				return false;
			}

			for (auto ix = 0u; ix < Count; ++ix)
			{
				std::pair<TKey, TValue> buffer;
				if (!Savegame::ReadExStream(Stm, buffer, RegisterForChange))
				{
					return false;
				}
				Value.insert(buffer);
			}

			return true;
		}

		bool WriteToStream(ExStreamWriter& Stm, const std::map<TKey, TValue>& Value) const
		{
			Stm.Save(Value.size());

			for (const auto& item : Value)
			{
				if (!Savegame::WriteExStream(Stm, item))
				{
					return false;
				}
			}
			return true;
		}
	};

	template <>
	struct Savegame::ExStreamObject<SHPStruct*>
	{
		bool ReadFromStream(ExStreamReader& Stm, SHPStruct*& Value, bool RegisterForChange) const
		{
			if (Value && !Value->IsReference())
				Debug::Log("Value contains SHP file data. Possible leak.\n");

			Value = nullptr;

			bool hasValue = true;
			if (Savegame::ReadExStream(Stm, hasValue) && hasValue)
			{
				std::string name;
				if (Savegame::ReadExStream(Stm, name))
				{
					if (auto pSHP = FileSystem::LoadSHPFile(name.c_str()))
					{
						Value = pSHP;
						return true;
					}
				}
			}

			return !hasValue;
		}

		bool WriteToStream(ExStreamWriter& Stm, SHPStruct* const& Value) const
		{
			const char* filename = nullptr;
			if (Value)
			{
				if (auto pRef = Value->AsReference())
					filename = pRef->Filename;
				else
					Debug::Log("Cannot save SHPStruct, because it isn't a reference.\n");
			}

			if (Savegame::WriteExStream(Stm, filename != nullptr))
			{
				if (filename)
				{
					std::string file(filename);
					return Savegame::WriteExStream(Stm, file);
				}
			}

			return filename == nullptr;
		}
	};

	template <>
	struct Savegame::ExStreamObject<RocketStruct>
	{
		bool ReadFromStream(ExStreamReader& Stm, RocketStruct& Value, bool RegisterForChange) const
		{
			if (!Stm.Load(Value))
				return false;

			if (RegisterForChange)
				Swizzle swizzle(Value.Type);

			return true;
		}

		bool WriteToStream(ExStreamWriter& Stm, const RocketStruct& Value) const
		{
			Stm.Save(Value);
			return true;
		}
	};

	template <>
	struct Savegame::ExStreamObject<BuildType>
	{
		bool ReadFromStream(ExStreamReader& Stm, BuildType& Value, bool RegisterForChange) const
		{
			if (!Stm.Load(Value))
				return false;

			if (RegisterForChange)
				Swizzle swizzle(Value.CurrentFactory);

			return true;
		}

		bool WriteToStream(ExStreamWriter& Stm, const BuildType& Value) const
		{
			Stm.Save(Value);
			return true;
		}
	};

	template <>
	struct Savegame::ExStreamObject<TranslucencyLevel*>
	{
		bool ReadFromStream(ExStreamReader& Stm, TranslucencyLevel*& Value, bool RegisterForChange) const
		{
			return Value->Load(Stm, RegisterForChange);
		}

		bool WriteToStream(ExStreamWriter& Stm, TranslucencyLevel* const& Value) const
		{
			return Value->Save(Stm);
		}
	};
}
