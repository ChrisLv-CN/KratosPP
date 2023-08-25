#pragma once

#include "Stream.h"

#include <memory>
#include <type_traits>

namespace Savegame {
	template <typename T>
	bool ReadExStream(ExStreamReader& Stm, T& Value, bool RegisterForChange = true);

	template <typename T>
	bool WriteExStream(ExStreamWriter& Stm, const T& Value);

	template <typename T>
	T* RestoreObject(ExStreamReader& Stm, bool RegisterForChange = true);

	template <typename T>
	bool PersistObject(ExStreamWriter& Stm, const T* pValue);

	template <typename T>
	struct PhobosStreamObject
	{
		bool ReadFromStream(ExStreamReader& Stm, T& Value, bool RegisterForChange) const;
		bool WriteToStream(ExStreamWriter& Stm, const T& Value) const;
	};

	template <typename T>
	struct ObjectFactory
	{
		std::unique_ptr<T> operator() (ExStreamReader& Stm) const
		{
			return std::make_unique<T>();
		}
	};
}
