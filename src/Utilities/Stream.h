#pragma once

#include <memory>
#include <type_traits>
#include <vector>

struct IStream;

class ExStreamReader;
class ExStreamWriter;

namespace Savegame
{
	template <typename T>
	bool ReadExStream(ExStreamReader& Stm, T& Value, bool RegisterForChange);

	template <typename T>
	bool WriteExStream(ExStreamWriter& Stm, const T& Value);
}

class ExByteStream
{
public:
	using data_t = unsigned char;

protected:
	std::vector<data_t> Data;
	size_t CurrentOffset;

public:
	ExByteStream(size_t Reserve = 0x1000);
	~ExByteStream();

	size_t Size() const
	{
		return this->Data.size();
	}

	size_t Offset() const
	{
		return this->CurrentOffset;
	}

	/**
	* reads {Length} bytes from {pStm} into its storage
	*/
	bool ReadFromStream(IStream* pStm, const size_t Length);

	/**
	* writes all internal storage to {pStm}
	*/
	bool WriteToStream(IStream* pStm) const;

	/**
	* reads the next block of bytes from {pStm} into its storage,
	* the block size is prepended to the block
	*/
	size_t ReadBlockFromStream(IStream* pStm);

	/**
	* writes all internal storage to {pStm}, prefixed with its length
	*/
	bool WriteBlockToStream(IStream* pStm) const;


	// primitive save/load - should not be specialized

	/**
	* if it has {Size} bytes left, assigns the first {Size} unread bytes to {Value}
	* moves the internal position forward
	*/
	bool Read(data_t* Value, size_t Size);

	/**
	* ensures there are at least {Size} bytes left in the internal storage, and assigns {Value} casted to byte to that buffer
	* moves the internal position forward
	*/
	void Write(const data_t* Value, size_t Size);


	/**
	* attempts to read the data from internal storage into {Value}
	*/
	template<typename T>
	bool Load(T& Value)
	{
		// get address regardless of overloaded & operator
		auto Bytes = &reinterpret_cast<data_t&>(Value);
		return this->Read(Bytes, sizeof(T));
	}

	/**
	* writes the data from {Value} into internal storage
	*/
	template<typename T>
	void Save(const T& Value)
	{
		// get address regardless of overloaded & operator
		auto Bytes = &reinterpret_cast<const data_t&>(Value);
		this->Write(Bytes, sizeof(T));
	};
};

class StreamWorkerBase
{
public:
	explicit StreamWorkerBase(ExByteStream& Stream) :
		stream(&Stream),
		success(true)
	{ }

	StreamWorkerBase(const StreamWorkerBase&) = delete;

	StreamWorkerBase& operator = (const StreamWorkerBase&) = delete;

	bool Success() const
	{
		return this->success;
	}

protected:
	// set to false_type or true_type to disable or enable debugging checks
	using stream_debugging_t =
#ifdef DEBUG
		std::true_type;
#else // DEBUG
		std::false_type;
#endif // DEBUG


	bool IsValid(std::true_type) const
	{
		return this->success;
	}

	bool IsValid(std::false_type) const
	{
		return true;
	}

	ExByteStream* stream;
	bool success;
};

class ExStreamReader : public StreamWorkerBase
{
public:
	explicit ExStreamReader(ExByteStream& Stream) : StreamWorkerBase(Stream) { }
	ExStreamReader(const ExStreamReader&) = delete;

	ExStreamReader& operator = (const ExStreamReader&) = delete;

	template <typename T>
	ExStreamReader& Process(T& value, bool RegisterForChange = true)
	{
		if (this->IsValid(stream_debugging_t()))
			this->success &= Savegame::ReadExStream(*this, value, RegisterForChange);
		return *this;
	}

	// helpers

	bool ExpectEndOfBlock() const
	{
		if (!this->Success() || this->stream->Size() != this->stream->Offset())
		{
			this->EmitExpectEndOfBlockWarning(stream_debugging_t());
			return false;
		}

		return true;
	}

	template <typename T>
	bool Load(T& buffer)
	{
		if (!this->stream->Load(buffer))
		{
			this->EmitLoadWarning(sizeof(T), stream_debugging_t());
			this->success = false;
			return false;
		}
		return true;
	}

	bool Read(ExByteStream::data_t* Value, size_t Size)
	{
		if (!this->stream->Read(Value, Size))
		{
			this->EmitLoadWarning(Size, stream_debugging_t());
			this->success = false;
			return false;
		}
		return true;
	}

	bool Expect(unsigned int value)
	{
		unsigned int buffer = 0;
		if (this->Load(buffer))
		{
			if (buffer == value)
				return true;

			this->EmitExpectWarning(buffer, value, stream_debugging_t());
		}
		return false;
	}

	bool RegisterChange(void* newPtr);

private:
	void EmitExpectEndOfBlockWarning(std::true_type) const;
	void EmitExpectEndOfBlockWarning(std::false_type) const { }

	void EmitLoadWarning(size_t size, std::true_type) const;
	void EmitLoadWarning(size_t size, std::false_type) const { }

	void EmitExpectWarning(unsigned int found, unsigned int expect, std::true_type) const;
	void EmitExpectWarning(unsigned int found, unsigned int expect, std::false_type) const { }

	void EmitSwizzleWarning(long id, void* pointer, std::true_type) const;
	void EmitSwizzleWarning(long id, void* pointer, std::false_type) const { }
};

class ExStreamWriter : public StreamWorkerBase
{
public:
	explicit ExStreamWriter(ExByteStream& Stream) : StreamWorkerBase(Stream) { }
	ExStreamWriter(const ExStreamWriter&) = delete;

	ExStreamWriter& operator = (const ExStreamWriter&) = delete;

	template <typename T>
	ExStreamWriter& Process(T& value, bool RegisterForChange = true)
	{
		if (this->IsValid(stream_debugging_t()))
			this->success &= Savegame::WriteExStream(*this, value);

		return *this;
	}

	// helpers

	template <typename T>
	void Save(const T& buffer)
	{
		this->stream->Save(buffer);
	}

	void Write(const ExByteStream::data_t* Value, size_t Size)
	{
		this->stream->Write(Value, Size);
	}

	bool Expect(unsigned int value)
	{
		this->Save(value);
		return true;
	}

	bool RegisterChange(const void* oldPtr)
	{
		this->Save(oldPtr);
		return true;
	}
};
