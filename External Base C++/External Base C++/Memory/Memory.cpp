#include "../include.h"


typedef NTSTATUS(NTAPI* _NtQuerySystemInformation)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
    );

typedef NTSTATUS(NTAPI* _NtDuplicateObject)(
    HANDLE SourceProcessHandle,
    HANDLE SourceHandle,
    HANDLE TargetProcessHandle,
    PHANDLE TargetHandle,
    ACCESS_MASK DesiredAccess,
    ULONG Attributes,
    ULONG Options
    );

typedef NTSTATUS(NTAPI* _NtOpenProcess)(
    PHANDLE ProcessHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    CLIENT_ID ClientId
    );

typedef NTSTATUS(NTAPI* _RtlAdjustPrivilege)(
    ULONG Privilege,
    BOOLEAN Enable,
    BOOLEAN CurrentThread,
    PBOOLEAN Enabled
    );

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
{
    USHORT ProcessId;
    USHORT CreatorBackTraceIndex;
    UCHAR ObjectTypeNumber;
    UCHAR Flags;
    USHORT Handle;
    PVOID Object;
    ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, * PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
    ULONG HandleCount;
    SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
} SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION;

#define InitObjectAttributes(p, n, a, r) { (p)->Length = sizeof(OBJECT_ATTRIBUTES); (p)->RootDirectory = (r); (p)->Attributes = (a); (p)->ObjectName = (n); (p)->SecurityDescriptor = NULL; (p)->SecurityQualityOfService = NULL; }
#define SeDebugPriv 20
#define ProcessHandleType 0x07

#define IsHandleValid(h) ((h) && (h) != INVALID_HANDLE_VALUE)

#ifndef STATUS_INFO_LENGTH_MISMATCH
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#endif

#ifndef SystemHandleInformation
#define SystemHandleInformation (SYSTEM_INFORMATION_CLASS)16
#endif

HANDLE Memory::HijackExistingHandle(DWORD dwTargetProcessId) {
    HMODULE Ntdll = GetModuleHandleA("ntdll");

    _RtlAdjustPrivilege RtlAdjustPrivilege = (_RtlAdjustPrivilege)GetProcAddress(Ntdll, "RtlAdjustPrivilege");
    _NtQuerySystemInformation NtQuerySystemInformation = (_NtQuerySystemInformation)GetProcAddress(Ntdll, "NtQuerySystemInformation");
    _NtDuplicateObject NtDuplicateObject = (_NtDuplicateObject)GetProcAddress(Ntdll, "NtDuplicateObject");
    _NtOpenProcess NtOpenProcess = (_NtOpenProcess)GetProcAddress(Ntdll, "NtOpenProcess");

    BOOLEAN OldPriv;
    RtlAdjustPrivilege(SeDebugPriv, TRUE, FALSE, &OldPriv);

    OBJECT_ATTRIBUTES Obj_Attribute;
    InitObjectAttributes(&Obj_Attribute, NULL, 0, NULL);

    CLIENT_ID clientID = { 0 };
    DWORD size = sizeof(SYSTEM_HANDLE_INFORMATION);
    SYSTEM_HANDLE_INFORMATION* hInfo = (SYSTEM_HANDLE_INFORMATION*) new unsigned char[size];

    ZeroMemory(hInfo, size);
    NTSTATUS NtRet = NULL;

    do {
        delete[] hInfo;
        size *= 1.5;
        try {
            hInfo = (SYSTEM_HANDLE_INFORMATION*) new unsigned char[size];
        }
        catch (std::bad_alloc) {
            printf("Bad Heap Allocation");
        }
        Sleep(1);
    } while ((NtRet = NtQuerySystemInformation(SystemHandleInformation, hInfo, size, NULL)) == STATUS_INFO_LENGTH_MISMATCH);

    if (!NT_SUCCESS(NtRet)) {
        printf("NtQuerySystemInformation Failed");
        return NULL;
    }

    HANDLE hProcess = NULL, HijackedHandle = NULL, procHandle = NULL;

    for (unsigned int i = 0; i < hInfo->HandleCount; ++i) {
        DWORD NumOfOpenHandles;
        GetProcessHandleCount(GetCurrentProcess(), &NumOfOpenHandles);

        if (!IsHandleValid((HANDLE)hInfo->Handles[i].Handle)) continue;
        if (hInfo->Handles[i].ObjectTypeNumber != ProcessHandleType) continue;

        clientID.UniqueProcess = (PVOID)(ULONG_PTR)hInfo->Handles[i].ProcessId;

        if (procHandle) {
            CloseHandle(procHandle);
            procHandle = NULL;
        }

        NtRet = NtOpenProcess(&procHandle, PROCESS_DUP_HANDLE, &Obj_Attribute, clientID);
        if (!IsHandleValid(procHandle) || !NT_SUCCESS(NtRet)) continue;

        NtRet = NtDuplicateObject(procHandle, (HANDLE)hInfo->Handles[i].Handle, GetCurrentProcess(), &HijackedHandle, PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, 0, 0);
        if (!IsHandleValid(HijackedHandle) || !NT_SUCCESS(NtRet)) continue;

        if (GetProcessId(HijackedHandle) != dwTargetProcessId) {
            CloseHandle(HijackedHandle);
            continue;
        }

        hProcess = HijackedHandle;
        break;
    }

    delete[] hInfo;
    return hProcess;
}

void Memory::initMemoryFunctions() {
    HMODULE hNtDll = GetModuleHandleA("ntdll.dll");
    if (!hNtDll) {
        std::cerr << "Failed to load ntdll.dll\n";
        return;
    }

    NtReadVirtualMemory = reinterpret_cast<pNtReadVirtualMemory>(
        GetProcAddress(hNtDll, "NtReadVirtualMemory"));
    NtWriteVirtualMemory = reinterpret_cast<pNtWriteVirtualMemory>(
        GetProcAddress(hNtDll, "NtWriteVirtualMemory"));

    if (!NtReadVirtualMemory || !NtWriteVirtualMemory) {
        std::cerr << "Failed to get function addresses\n";
    }
}