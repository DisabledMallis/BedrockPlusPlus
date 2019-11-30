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
	unsigned char nopPatch[8] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

	//Inejctions here
	WriteProcessMemory(minecraft, mcBase + 0x188D536, nopPatch, 8, NULL);
	std::cout << GetLastError();

	std::cout << "Injected!";
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	return 0;
}

int main(int, char* [])
{
	//Credit me <3
	std::cout << "Made by DisabledMallis\n" << "Discord: ASM#6173\n" << "Github: https://github.com/DisabledMallis\n";
	//Tell them the game version & build this made for
	std::cout << "\nBedrock++ is designed for:\n" << "Minecraft 1.13.105.0_x64\n";


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
	return 0;
}