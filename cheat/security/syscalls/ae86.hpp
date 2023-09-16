#ifndef AE86_H_
#define AE86_H_

/*
	Made with LOVE by Soufiw & infirms
	07-06-23
 */

#include <Windows.h>

#include <vector>
#include <unordered_map>
#include <iostream>
#include <format>

#include "win.hpp"
#include "hash.hpp"

#include "../../callbacks.hpp"
#include "../../main_definitions.hpp"

#define EM_DB(_VALUE_) __asm __emit(_VALUE_)
#define PTR_64_(p) ((DWORD64)(ULONG_PTR)(p))
#define HADNLE_64_(p) ((DWORD64)(LONG_PTR)(p))
#define NtCurrentProcess() ((HANDLE)(LONG_PTR)-1)

namespace ae86 {
    __forceinline NTSTATUS syscall(int id, uint64_t arg1) {
        struct params_t {
            uint64_t arg1;
            int id;
        };

        params_t params = { arg1, id };
        params_t* ref = &params;

        __asm
        {
            mov eax, ref

            push 0x33
            call $ + 5
            add dword ptr[esp], 5
            retf

            EM_DB(0x55) EM_DB(0x48) EM_DB(0x89) EM_DB(0xE5) EM_DB(0x48) EM_DB(0x83) EM_DB(0xC4) EM_DB(0xF0) EM_DB(0x48)
            EM_DB(0x31) EM_DB(0xFF) EM_DB(0x89) EM_DB(0xC7) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x0F) EM_DB(0x48) EM_DB(0x8B)
            EM_DB(0x47) EM_DB(0x08) EM_DB(0x49) EM_DB(0x89) EM_DB(0xCA) EM_DB(0x0F) EM_DB(0x05) EM_DB(0x48) EM_DB(0x89)
            EM_DB(0xEC) EM_DB(0x5D) EM_DB(0xE8) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0xC7) EM_DB(0x44)
            EM_DB(0x24) EM_DB(0x04) EM_DB(0x23) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0x83) EM_DB(0x04) EM_DB(0x24)
            EM_DB(0x0D) EM_DB(0xCB)

        }
    }

    __forceinline NTSTATUS syscall(int id, uint64_t arg1, uint64_t arg2) {
        struct params_t {
            uint64_t arg1;
            uint64_t arg2;
            int id;
        };

        params_t params = { arg1, arg2, id };
        params_t* ref = &params;

        __asm
        {
            mov eax, ref

            push 0x33
            call $ + 5
            add dword ptr[esp], 5
            retf

            EM_DB(0x55) EM_DB(0x48) EM_DB(0x89) EM_DB(0xE5) EM_DB(0x48) EM_DB(0x83) EM_DB(0xC4) EM_DB(0xF0) EM_DB(0x48)
            EM_DB(0x31) EM_DB(0xFF) EM_DB(0x89) EM_DB(0xC7) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x0F) EM_DB(0x48) EM_DB(0x8B)
            EM_DB(0x57) EM_DB(0x08) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x10) EM_DB(0x49) EM_DB(0x89) EM_DB(0xCA)
            EM_DB(0x0F) EM_DB(0x05) EM_DB(0x48) EM_DB(0x89) EM_DB(0xEC) EM_DB(0x5D) EM_DB(0xE8) EM_DB(0x00) EM_DB(0x00)
            EM_DB(0x00) EM_DB(0x00) EM_DB(0xC7) EM_DB(0x44) EM_DB(0x24) EM_DB(0x04) EM_DB(0x23) EM_DB(0x00) EM_DB(0x00)
            EM_DB(0x00) EM_DB(0x83) EM_DB(0x04) EM_DB(0x24) EM_DB(0x0D) EM_DB(0xCB)
        }
    }

    __forceinline NTSTATUS syscall(int id, uint64_t arg1, uint64_t arg2,
        uint64_t arg3) {
        struct params_t {
            uint64_t arg1;
            uint64_t arg2;
            uint64_t arg3;
            int id;
        };

        params_t params = { arg1, arg2, arg3, id };
        params_t* ref = &params;

        __asm
        {
            mov eax, ref

            push 0x33
            call $ + 5
            add dword ptr[esp], 5
            retf

            EM_DB(0x55) EM_DB(0x48) EM_DB(0x89) EM_DB(0xE5) EM_DB(0x48) EM_DB(0x83) EM_DB(0xC4) EM_DB(0xF0) EM_DB(0x48)
            EM_DB(0x31) EM_DB(0xFF) EM_DB(0x89) EM_DB(0xC7) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x0F) EM_DB(0x48) EM_DB(0x8B)
            EM_DB(0x57) EM_DB(0x08) EM_DB(0x4C) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x10) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47)
            EM_DB(0x18) EM_DB(0x49) EM_DB(0x89) EM_DB(0xCA) EM_DB(0x0F) EM_DB(0x05) EM_DB(0x48) EM_DB(0x89) EM_DB(0xEC)
            EM_DB(0x5D) EM_DB(0xE8) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0xC7) EM_DB(0x44) EM_DB(0x24)
            EM_DB(0x04) EM_DB(0x23) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0x83) EM_DB(0x04) EM_DB(0x24) EM_DB(0x0D)
            EM_DB(0xCB)
        }
    }

    __forceinline NTSTATUS syscall(int id, uint64_t arg1, uint64_t arg2,
        uint64_t arg3, uint64_t arg4) {
        struct params_t {
            uint64_t arg1;
            uint64_t arg2;
            uint64_t arg3;
            uint64_t arg4;
            int id;
        };

        params_t params = { arg1, arg2, arg3, arg4, id };
        params_t* ref = &params;

        __asm
        {
            mov eax, ref

            push 0x33
            call $ + 5
            add dword ptr[esp], 5
            retf

            EM_DB(0x55) EM_DB(0x48) EM_DB(0x89) EM_DB(0xE5) EM_DB(0x48) EM_DB(0x83) EM_DB(0xC4) EM_DB(0xF0) EM_DB(0x48)
            EM_DB(0x31) EM_DB(0xFF) EM_DB(0x89) EM_DB(0xC7) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x0F) EM_DB(0x48) EM_DB(0x8B)
            EM_DB(0x57) EM_DB(0x08) EM_DB(0x4C) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x10) EM_DB(0x4C) EM_DB(0x8B) EM_DB(0x4F)
            EM_DB(0x18) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x20) EM_DB(0x49) EM_DB(0x89) EM_DB(0xCA) EM_DB(0x0F)
            EM_DB(0x05) EM_DB(0x48) EM_DB(0x89) EM_DB(0xEC) EM_DB(0x5D) EM_DB(0xE8) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00)
            EM_DB(0x00) EM_DB(0xC7) EM_DB(0x44) EM_DB(0x24) EM_DB(0x04) EM_DB(0x23) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00)
            EM_DB(0x83) EM_DB(0x04) EM_DB(0x24) EM_DB(0x0D) EM_DB(0xCB)
        }
    }

    __forceinline NTSTATUS syscall(int id, uint64_t arg1, uint64_t arg2,
        uint64_t arg3, uint64_t arg4, uint64_t arg5) {
        struct params_t {
            uint64_t arg1;
            uint64_t arg2;
            uint64_t arg3;
            uint64_t arg4;
            uint64_t arg5;
            int id;
        };

        params_t params = { arg1, arg2, arg3, arg4,arg5, id };
        params_t* ref = &params;

        __asm
        {
            mov eax, ref

            push 0x33
            call $ + 5
            add dword ptr[esp], 5
            retf

            EM_DB(0x55) EM_DB(0x48) EM_DB(0x89) EM_DB(0xE5) EM_DB(0x48) EM_DB(0x83) EM_DB(0xC4) EM_DB(0xF0) EM_DB(0x48)
            EM_DB(0x83) EM_DB(0xEC) EM_DB(0x30) EM_DB(0x48) EM_DB(0x31) EM_DB(0xFF) EM_DB(0x89) EM_DB(0xC7) EM_DB(0x48)
            EM_DB(0x8B) EM_DB(0x0F) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x57) EM_DB(0x08) EM_DB(0x4C) EM_DB(0x8B) EM_DB(0x47)
            EM_DB(0x10) EM_DB(0x4C) EM_DB(0x8B) EM_DB(0x4F) EM_DB(0x18) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x20)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x28) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x28)
            EM_DB(0x49) EM_DB(0x89) EM_DB(0xCA) EM_DB(0x0F) EM_DB(0x05) EM_DB(0x48) EM_DB(0x89) EM_DB(0xEC) EM_DB(0x5D)
            EM_DB(0xE8) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0xC7) EM_DB(0x44) EM_DB(0x24) EM_DB(0x04)
            EM_DB(0x23) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0x83) EM_DB(0x04) EM_DB(0x24) EM_DB(0x0D) EM_DB(0xCB)
        }
    }

    __forceinline NTSTATUS syscall(int id, uint64_t arg1, uint64_t arg2,
        uint64_t arg3, uint64_t arg4, uint64_t arg5,
        uint64_t arg6) {
        struct params_t {
            uint64_t arg1;
            uint64_t arg2;
            uint64_t arg3;
            uint64_t arg4;
            uint64_t arg5;
            uint64_t arg6;
            int id;
        };

        params_t params = { arg1, arg2, arg3, arg4, arg5, arg6, id };
        params_t* ref = &params;

        __asm
        {
            mov eax, ref

            push 0x33
            call $ + 5
            add dword ptr[esp], 5
            retf


            EM_DB(0x55) EM_DB(0x48) EM_DB(0x89) EM_DB(0xe5) EM_DB(0x48) EM_DB(0x83) EM_DB(0xc4) EM_DB(0xf0) EM_DB(0x48) EM_DB(0x83)
            EM_DB(0xec) EM_DB(0x40) EM_DB(0x48) EM_DB(0x31) EM_DB(0xff) EM_DB(0x89) EM_DB(0xc7) EM_DB(0x48) EM_DB(0x8b)
            EM_DB(0xf) EM_DB(0x48) EM_DB(0x8b) EM_DB(0x57) EM_DB(0x8) EM_DB(0x4c) EM_DB(0x8b) EM_DB(0x47) EM_DB(0x10)
            EM_DB(0x4c) EM_DB(0x8b) EM_DB(0x4f) EM_DB(0x18) EM_DB(0x48) EM_DB(0x8b) EM_DB(0x47) EM_DB(0x20) EM_DB(0x48)
            EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x28) EM_DB(0x48) EM_DB(0x8b) EM_DB(0x47) EM_DB(0x28) EM_DB(0x48)
            EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x30) EM_DB(0x48) EM_DB(0x8b) EM_DB(0x47) EM_DB(0x30) EM_DB(0x49)
            EM_DB(0x89) EM_DB(0xca) EM_DB(0xf) EM_DB(0x5) EM_DB(0x48) EM_DB(0x89) EM_DB(0xec) EM_DB(0x5d) EM_DB(0xe8)
            EM_DB(0x0) EM_DB(0x0) EM_DB(0x0) EM_DB(0x0) EM_DB(0xc7) EM_DB(0x44) EM_DB(0x24) EM_DB(0x4) EM_DB(0x23)
            EM_DB(0x0) EM_DB(0x0) EM_DB(0x0) EM_DB(0x83) EM_DB(0x4) EM_DB(0x24) EM_DB(0xd) EM_DB(0xcb)
        }
    }

    __forceinline NTSTATUS syscall(int id, uint64_t arg1, uint64_t arg2,
        uint64_t arg3, uint64_t arg4, uint64_t arg5,
        uint64_t arg6, uint64_t arg7) {
        struct params_t {
            uint64_t arg1;
            uint64_t arg2;
            uint64_t arg3;
            uint64_t arg4;
            uint64_t arg5;
            uint64_t arg6;
            uint64_t arg7;
            int id;
        };

        params_t params = { arg1, arg2, arg3, arg4, arg5, arg6, arg7, id };
        params_t* ref = &params;

        __asm
        {
            mov eax, ref

            push 0x33
            call $ + 5
            add dword ptr[esp], 5
            retf

            EM_DB(0x55) EM_DB(0x48) EM_DB(0x89) EM_DB(0xE5) EM_DB(0x48) EM_DB(0x83) EM_DB(0xC4) EM_DB(0xF0) EM_DB(0x48)
            EM_DB(0x83) EM_DB(0xEC) EM_DB(0x40) EM_DB(0x48) EM_DB(0x31) EM_DB(0xFF) EM_DB(0x89) EM_DB(0xC7) EM_DB(0x48)
            EM_DB(0x8B) EM_DB(0x0F) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x57) EM_DB(0x08) EM_DB(0x4C) EM_DB(0x8B) EM_DB(0x47)
            EM_DB(0x10) EM_DB(0x4C) EM_DB(0x8B) EM_DB(0x4F) EM_DB(0x18) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x20)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x28) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x28)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x30) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x30)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x38) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x38)
            EM_DB(0x49) EM_DB(0x89) EM_DB(0xCA) EM_DB(0x0F) EM_DB(0x05) EM_DB(0x48) EM_DB(0x89) EM_DB(0xEC) EM_DB(0x5D)
            EM_DB(0xE8) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0xC7) EM_DB(0x44) EM_DB(0x24) EM_DB(0x04)
            EM_DB(0x23) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0x83) EM_DB(0x04) EM_DB(0x24) EM_DB(0x0D) EM_DB(0xCB)
        }
    }

    __forceinline NTSTATUS syscall(int id, uint64_t arg1, uint64_t arg2,
        uint64_t arg3, uint64_t arg4, uint64_t arg5,
        uint64_t arg6, uint64_t arg7, uint64_t arg8) {
        struct params_t {
            uint64_t arg1;
            uint64_t arg2;
            uint64_t arg3;
            uint64_t arg4;
            uint64_t arg5;
            uint64_t arg6;
            uint64_t arg7;
            uint64_t arg8;
            int id;
        };

        params_t params = { arg1, arg2, arg3, arg4, arg5, arg6, arg7,arg8, id };
        params_t* ref = &params;

        __asm
        {
            mov eax, ref

            push 0x33
            call $ + 5
            add dword ptr[esp], 5
            retf


            EM_DB(0x55) EM_DB(0x48) EM_DB(0x89) EM_DB(0xE5) EM_DB(0x48) EM_DB(0x83) EM_DB(0xC4) EM_DB(0xF0) EM_DB(0x48)
            EM_DB(0x83) EM_DB(0xEC) EM_DB(0x50) EM_DB(0x48) EM_DB(0x31) EM_DB(0xFF) EM_DB(0x89) EM_DB(0xC7) EM_DB(0x48)
            EM_DB(0x8B) EM_DB(0x0F) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x57) EM_DB(0x08) EM_DB(0x4C) EM_DB(0x8B) EM_DB(0x47)
            EM_DB(0x10) EM_DB(0x4C) EM_DB(0x8B) EM_DB(0x4F) EM_DB(0x18) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x20)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x28) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x28)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x30) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x30)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x38) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x38)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x40) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x40)
            EM_DB(0x49) EM_DB(0x89) EM_DB(0xCA) EM_DB(0x0F) EM_DB(0x05) EM_DB(0x48) EM_DB(0x89) EM_DB(0xEC) EM_DB(0x5D)
            EM_DB(0xE8) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0xC7) EM_DB(0x44) EM_DB(0x24) EM_DB(0x04)
            EM_DB(0x23) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0x83) EM_DB(0x04) EM_DB(0x24) EM_DB(0x0D) EM_DB(0xCB)
        }
    }

    __forceinline NTSTATUS syscall(int id, uint64_t arg1, uint64_t arg2,
        uint64_t arg3, uint64_t arg4, uint64_t arg5,
        uint64_t arg6, uint64_t arg7, uint64_t arg8,
        uint64_t arg9) {
        struct params_t {
            uint64_t arg1;
            uint64_t arg2;
            uint64_t arg3;
            uint64_t arg4;
            uint64_t arg5;
            uint64_t arg6;
            uint64_t arg7;
            uint64_t arg8;
            uint64_t arg9;
            int id;
        };

        params_t params = { arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, id };
        params_t* ref = &params;

        __asm
        {
            mov eax, ref

            push 0x33
            call $ + 5
            add dword ptr[esp], 5
            retf


            EM_DB(0x55) EM_DB(0x48) EM_DB(0x89) EM_DB(0xE5) EM_DB(0x48) EM_DB(0x83) EM_DB(0xC4) EM_DB(0xF0) EM_DB(0x48)
            EM_DB(0x83) EM_DB(0xEC) EM_DB(0x50) EM_DB(0x48) EM_DB(0x31) EM_DB(0xFF) EM_DB(0x89) EM_DB(0xC7) EM_DB(0x48)
            EM_DB(0x8B) EM_DB(0x0F) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x57) EM_DB(0x08) EM_DB(0x4C) EM_DB(0x8B) EM_DB(0x47)
            EM_DB(0x10) EM_DB(0x4C) EM_DB(0x8B) EM_DB(0x4F) EM_DB(0x18) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x20)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x28) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x28)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x30) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x30)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x38) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x38)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x40) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x40)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x48) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x48)
            EM_DB(0x49) EM_DB(0x89) EM_DB(0xCA) EM_DB(0x0F) EM_DB(0x05) EM_DB(0x48) EM_DB(0x89) EM_DB(0xEC) EM_DB(0x5D)
            EM_DB(0xE8) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0xC7) EM_DB(0x44) EM_DB(0x24) EM_DB(0x04)
            EM_DB(0x23) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0x83) EM_DB(0x04) EM_DB(0x24) EM_DB(0x0D) EM_DB(0xCB)
        }
    }

    __forceinline NTSTATUS syscall(int id, uint64_t arg1, uint64_t arg2,
        uint64_t arg3, uint64_t arg4, uint64_t arg5,
        uint64_t arg6, uint64_t arg7, uint64_t arg8,
        uint64_t arg9, uint64_t arg10) {
        struct params_t {
            uint64_t arg1;
            uint64_t arg2;
            uint64_t arg3;
            uint64_t arg4;
            uint64_t arg5;
            uint64_t arg6;
            uint64_t arg7;
            uint64_t arg8;
            uint64_t arg9;
            uint64_t arg10;
            int id;
        };

        params_t params = { arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9,arg10, id };
        params_t* ref = &params;

        __asm
        {
            mov eax, ref

            push 0x33
            call $ + 5
            add dword ptr[esp], 5
            retf

            EM_DB(0x55) EM_DB(0x48) EM_DB(0x89) EM_DB(0xE5) EM_DB(0x48) EM_DB(0x83) EM_DB(0xC4) EM_DB(0xF0) EM_DB(0x48)
            EM_DB(0x83) EM_DB(0xEC) EM_DB(0x60) EM_DB(0x48) EM_DB(0x31) EM_DB(0xFF) EM_DB(0x89) EM_DB(0xC7) EM_DB(0x48)
            EM_DB(0x8B) EM_DB(0x0F) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x57) EM_DB(0x08) EM_DB(0x4C) EM_DB(0x8B) EM_DB(0x47)
            EM_DB(0x10) EM_DB(0x4C) EM_DB(0x8B) EM_DB(0x4F) EM_DB(0x18) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x20)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x28) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x28)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x30) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x30)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x38) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x38)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x40) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x40)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x48) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x48)
            EM_DB(0x48) EM_DB(0x89) EM_DB(0x44) EM_DB(0x24) EM_DB(0x50) EM_DB(0x48) EM_DB(0x8B) EM_DB(0x47) EM_DB(0x50)
            EM_DB(0x49) EM_DB(0x89) EM_DB(0xCA) EM_DB(0x0F) EM_DB(0x05) EM_DB(0x48) EM_DB(0x89) EM_DB(0xEC) EM_DB(0x5D)
            EM_DB(0xE8) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0xC7) EM_DB(0x44) EM_DB(0x24) EM_DB(0x04)
            EM_DB(0x23) EM_DB(0x00) EM_DB(0x00) EM_DB(0x00) EM_DB(0x83) EM_DB(0x04) EM_DB(0x24) EM_DB(0x0D) EM_DB(0xCB)
        }
    }

    win::_TEB32* nt_current_teb() noexcept;
    win::_PEB32* nt_current_peb() noexcept;

    const IMAGE_DOS_HEADER* get_image_dos_header(std::uintptr_t image);
    const IMAGE_NT_HEADERS* get_image_nt_headers(std::uintptr_t image);
    const IMAGE_DATA_DIRECTORY* get_image_data_directory(std::uintptr_t image,
        std::uint16_t directory);

    std::uintptr_t get_image(hash_t name_hash);

	class c_syscall_parser : public c_sdk {
	public:
		c_syscall_parser();
		~c_syscall_parser();

		void parse();
		int get_index(hash_t hash) { return syscalls_[hash]; }

        virtual void store() override {
            DEBUG_LOG("  [+] Store syscalls \n\n");

            parse();
        }

	private:
		int count_;
		std::unordered_map<hash_t, int> syscalls_;
	};
}

#endif  // AE86_H_
