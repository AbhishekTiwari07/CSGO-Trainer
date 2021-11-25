#include "memory.h"
#include <tlHelp32.h>
#include<iostream>
#include <wchar.h>

Memory::Memory(const char* processName) {
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	//HANDLE CreateToolhelp32Snapshot([in] DWORD dwFlags,[in] DWORD th32ProcessID);
	auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	//Match our process and return pid and processhandler
	while (Process32Next(snapShot, &entry)) {
		if (!lstrcmpW(entry.szExeFile, L"csgo.exe")) {
			this->pid = entry.th32ProcessID;
			this->process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->pid);
			break;
		}
	}
	if (snapShot)
		CloseHandle(snapShot);
}

Memory::~Memory() {
	if (this->process)
		CloseHandle(this->process);
}

DWORD Memory::GetPID() {
	return this->pid;
}

HANDLE Memory::GetProcesHandle() {
	return this->process;
}

uintptr_t Memory::GetModuleAddress(const char* moduleName) {
	MODULEENTRY32 entry;
	entry.dwSize = sizeof(MODULEENTRY32);

	auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->pid);

	uintptr_t result = 0;

	while (Module32Next(snapShot, &entry)) {
		if (!lstrcmpW(entry.szModule, L"client.dll")) {
			result = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
			break;
		}
	}

	if (snapShot) {
		CloseHandle(snapShot);
	}

	return result;
}