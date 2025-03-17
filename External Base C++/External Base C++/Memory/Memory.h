#pragma once

typedef NTSTATUS(WINAPI* pNtReadVirtualMemory)(
	HANDLE ProcessHandle,
	PVOID BaseAddress,
	PVOID Buffer,
	ULONG NumberOfBytesToRead,
	PULONG NumberOfBytesRead OPTIONAL
	);

typedef NTSTATUS(WINAPI* pNtWriteVirtualMemory)(
	HANDLE ProcessHandle,
	PVOID BaseAddress,
	PVOID Buffer,
	ULONG NumberOfBytesToWrite,
	PULONG NumberOfBytesWritten OPTIONAL
	);

struct HandleDisposer
{
	using pointer = HANDLE;
	void operator()(HANDLE handle) const
	{
		if (handle != NULL || handle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(handle);
		}
	}
};

using unique_handle = std::unique_ptr<HANDLE, HandleDisposer>;

class Memory
{
private:
	int processId = 0;
	HANDLE processHandle = nullptr;

	pNtReadVirtualMemory NtReadVirtualMemory;
	pNtWriteVirtualMemory NtWriteVirtualMemory;

	HANDLE HijackExistingHandle(DWORD dwTargetProcessId);
	void initMemoryFunctions();

public:
	uintptr_t clientDLL;
	uintptr_t engineDLL;

public:
	void GetHandle(std::string_view process_name)
	{
		this->initMemoryFunctions();

		PROCESSENTRY32 processentry;
		const unique_handle snapshot_handle(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL));

		if (snapshot_handle.get() == INVALID_HANDLE_VALUE)
			return;

		processentry.dwSize = sizeof(MODULEENTRY32);

		while (Process32Next(snapshot_handle.get(), &processentry) == TRUE)
		{
			if (process_name.compare(processentry.szExeFile) == NULL)
			{
				this->processId = processentry.th32ProcessID;
				this->processHandle = HijackExistingHandle(this->processId);
			}
		}
	}

	const std::uintptr_t GetModuleAddress(const std::string_view moduleName) const noexcept
	{
		::MODULEENTRY32 entry = { };
		entry.dwSize = sizeof(::MODULEENTRY32);

		const auto snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->processId);

		std::uintptr_t result = 0;

		while (::Module32Next(snapShot, &entry))
		{
			if (!moduleName.compare(entry.szModule))
			{
				result = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
				break;
			}
		}

		if (snapShot)
			::CloseHandle(snapShot);

		return result;
	}

	template <typename T>
	T Read(UINT_PTR read_address)
	{
		T x{};
		this->NtReadVirtualMemory(this->processHandle, (PVOID)read_address, &x, sizeof(T), nullptr);
		return x;
	}

	template<typename S>
	bool Write(UINT_PTR write_address, const S& value)
	{
		ULONG bytesWritten;
		NTSTATUS status = this->NtWriteVirtualMemory(this->processHandle, (PVOID)write_address, (PVOID)&value, sizeof(S), &bytesWritten);
		return (status == 0) && (bytesWritten == sizeof(S));
	}

	template<std::size_t N>
	std::string ReadStr(std::uintptr_t address)
	{
		std::string result;
		result.reserve(N);

		for (std::size_t i = 0; i < N; ++i) {
			char ch = Read<char>(address + i);
			if (ch == '\0')
				break;
			result += ch;
		}

		return result;
	}

	bool ReadRaw(UINT_PTR read_address, void* output, size_t size)
	{
		ULONG bytesRead;
		NTSTATUS status = this->NtReadVirtualMemory(this->processHandle, (PVOID)read_address, output, size, &bytesRead);
		return (status == 0) && (bytesRead == size);
	}
}; inline Memory memory;