// BedrockPP.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.
//

#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <Psapi.h>
#include <string>

HMODULE GetModule(HANDLE minecraft)
{
	HMODULE hMods[1024];
	HANDLE pHandle = minecraft;
	DWORD cbNeeded;
	unsigned int i;
	if (EnumProcessModules(pHandle, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];
			if (GetModuleFileNameEx(pHandle, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
			{
				std::wstring wstrModName = szModName;
				std::wstring wstrModContain = L"Minecraft.Windows.exe";
				if (wstrModName.find(wstrModContain) != std::string::npos)
				{
					return hMods[i];
				}
			}
		}
	}
	return nullptr;
}

int injectMinecraft(HANDLE minecraft) {
	//Let user know injections have started
	std::cout << "\nInjecting...\n";

	//Get module base
	HMODULE mcBase = GetModule(minecraft);

	//Injections here

	//Optifine zoom injection
	//unsigned char nopPatch[8] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
	unsigned char entryPatch[8] = { 0xE9, 0xF1, 0xD8, 0x47, 0x01, 0x90, 0x90, 0x90 };
	unsigned char codeCavePatch[40] = { 0x83, 0x3D, 0x11, 0x00, 0x13, 0x01, 0x00, 0x74, 0x0D, 0xF3, 0x0F, 0x10, 0x0D, 0x12, 0x00, 0x00, 0x00, 0xE9, 0xFB, 0x26, 0xB8, 0xFE, 0xF3, 0x0F, 0x10, 0x88, 0xF0, 0x00, 0x00, 0x00, 0xE9, 0xEE, 0x26, 0xB8, 0xFE, 0x00, 0x00, 0xF0, 0x41, 0x90 };
	//uint64_t nopInjectAddr = (uint64_t)mcBase + 0x188D536;
	//if (WriteProcessMemory(minecraft, (LPVOID)nopInjectAddr, nopPatch, 8, NULL) == 0) {
	//	std::cout << GetLastError();
	//}
	uint64_t entryAddr = (uint64_t)mcBase + 0xA164DE;
	if (WriteProcessMemory(minecraft, (LPVOID)entryAddr, entryPatch, 8, NULL) == 0) {
		std::cout << GetLastError();
	}
	uint64_t codeCaveAddr = (uint64_t)mcBase + 0x1E93DD4;
	if (WriteProcessMemory(minecraft, (LPVOID)codeCaveAddr, codeCavePatch, 47, NULL) == 0) {
		std::cout << GetLastError();
	}

	std::cout << "Injected!";
	return 0;
}

int main(int, char* [])
{
	//Credit me <3
	std::cout << "Made by DisabledMallis\n" << "Discord: ASM#6173\n" << "Github: https://github.com/DisabledMallis\n";
	//Tell them the game version & build this made for
	std::cout << "\nBedrock++ is designed for:\n" << "Minecraft 1.14.9.0_x64\n";
	std::cout << "Thanks to Danny for helping me learn C++ <3\n";


	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (wcscmp(entry.szExeFile, L"Minecraft.Windows.exe") == 0)
			{
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
				injectMinecraft(hProcess);
				CloseHandle(hProcess);
			}
		}
	}

	CloseHandle(snapshot);
	std::this_thread::sleep_for(std::chrono::milliseconds(8000));
	return 0;
}