#ifndef WIN_H_
#define WIN_H_

#include <Windows.h>
#include <winternl.h>
#include <type_traits>

namespace win {
	struct _UNICODE_STRING {
		USHORT Length;         // 0x0
		USHORT MaximumLength;  // 0x2
		WCHAR* Buffer;         // 0x4
	};

	struct _LDR_DATA_TABLE_ENTRY {
		struct _LIST_ENTRY InLoadOrderLinks;            // 0x0
		struct _LIST_ENTRY InMemoryOrderLinks;          // 0x8
		struct _LIST_ENTRY InInitializationOrderLinks;  // 0x10
		VOID* DllBase;                                  // 0x18
		VOID* EntryPoint;                               // 0x1c
		ULONG SizeOfImage;                              // 0x20
		_UNICODE_STRING FullDllName;                    // 0x24
		_UNICODE_STRING BaseDllName;                    // 0x2c
	};
	struct _PEB_LDR_DATA {
		ULONG Length;                                        // 0x0
		UCHAR Initialized;                                   // 0x4
		VOID* SsHandle;                                      // 0x8
		struct _LIST_ENTRY InLoadOrderModuleList;            // 0xc
		struct _LIST_ENTRY InMemoryOrderModuleList;          // 0x14
		struct _LIST_ENTRY InInitializationOrderModuleList;  // 0x1c
		VOID* EntryInProgress;                               // 0x24
		UCHAR ShutdownInProgress;                            // 0x28
		VOID* ShutdownThreadId;                              // 0x2c
	};

	struct _CLIENT_ID32 {
		ULONG UniqueProcess;  // 0x0
		ULONG UniqueThread;   // 0x4
	};

	struct _PEB32 {
		UCHAR InheritedAddressSpace;     // 0x0
		UCHAR ReadImageFileExecOptions;  // 0x1
		UCHAR BeingDebugged;             // 0x2
		union {
			UCHAR BitField;  // 0x3
			struct {
				UCHAR ImageUsesLargePages : 1;           // 0x3
				UCHAR IsProtectedProcess : 1;            // 0x3
				UCHAR IsImageDynamicallyRelocated : 1;   // 0x3
				UCHAR SkipPatchingUser32Forwarders : 1;  // 0x3
				UCHAR IsPackagedProcess : 1;             // 0x3
				UCHAR IsAppContainer : 1;                // 0x3
				UCHAR IsProtectedProcessLight : 1;       // 0x3
				UCHAR IsLongPathAwareProcess : 1;        // 0x3
			};
		};
		ULONG Mutant;             // 0x4
		ULONG ImageBaseAddress;   // 0x8
		_PEB_LDR_DATA* Ldr;       // 0xc
		ULONG ProcessParameters;  // 0x10
		ULONG SubSystemData;      // 0x14
		ULONG ProcessHeap;        // 0x18
		ULONG FastPebLock;        // 0x1c
		ULONG AtlThunkSListPtr;   // 0x20
		ULONG IFEOKey;            // 0x24
	};

	struct _TEB32 {
		_NT_TIB32 NtTib;                               // 0x0
		ULONG EnvironmentPointer;                      // 0x1c
		_CLIENT_ID32 ClientId;                         // 0x20
		ULONG ActiveRpcHandle;                         // 0x28
		ULONG ThreadLocalStoragePointer;               // 0x2c
		_PEB32* ProcessEnvironmentBlock;               // 0x30
		ULONG LastErrorValue;                          // 0x34
		ULONG CountOfOwnedCriticalSections;            // 0x38
		ULONG CsrClientThread;                         // 0x3c
		ULONG Win32ThreadInfo;                         // 0x40
		ULONG User32Reserved[26];                      // 0x44
		ULONG UserReserved[5];                         // 0xac
		ULONG WOW32Reserved;                           // 0xc0
		ULONG CurrentLocale;                           // 0xc4
		ULONG FpSoftwareStatusRegister;                // 0xc8
		ULONG ReservedForDebuggerInstrumentation[16];  // 0xcc
		ULONG SystemReserved1[26];                     // 0x10c
		CHAR PlaceholderCompatibilityMode;             // 0x174
		UCHAR PlaceholderHydrationAlwaysExplicit;      // 0x175
		CHAR PlaceholderReserved[10];                  // 0x176
		ULONG ProxiedProcessId;                        // 0x180
	};
}

#endif  // WIN_H_