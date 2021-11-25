#pragma once
#include<Windows.h>
#include<iostream>

class Memory {
private:
	DWORD pid = 0; //process id
	HANDLE process = NULL; //process handler

public:
	Memory(const char* processName);

	DWORD GetPID();
	HANDLE GetProcesHandle();
	uintptr_t GetModuleAddress(const char* moduleName);

	//wrappers for read and write process memory

	template <typename T>
	T Read(uintptr_t address) {
		T value;
		ReadProcessMemory(this->process,(LPCVOID)address, &value, sizeof(T), NULL);	
		return value;
	}

	template <typename T>
	bool Write(uintptr_t address, T value) {
		return WriteProcessMemory(this->process, (LPVOID)address, &value, sizeof(T), NULL);
	}

	~Memory();
};