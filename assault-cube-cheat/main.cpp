#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>

uintptr_t getBaseAddress(DWORD procid, const wchar_t* module_name) {
	uintptr_t module_base{};

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procid);

	if (hSnapshot != INVALID_HANDLE_VALUE) {
		MODULEENTRY32W me32{};
		me32.dwSize = sizeof(me32);

		if (Module32FirstW(hSnapshot, &me32)) {
			do {
				if (_wcsicmp(me32.szModule, module_name) == 0) {
					module_base = (uintptr_t)me32.modBaseAddr;
					break;
				}
			} while (Module32NextW(hSnapshot, &me32));
		}
	}

	CloseHandle(hSnapshot);
	return module_base;
}

int main() {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	DWORD procid{};

	if (hSnapshot != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32W pe32;
		pe32.dwSize = sizeof(pe32);

		if (Process32FirstW(hSnapshot, &pe32)) {
			do {
				if (_wcsicmp(pe32.szExeFile, L"ac_client.exe") == 0) {
					procid = pe32.th32ProcessID;
					break;
				}
			} while (Process32NextW(hSnapshot, &pe32));
		}
	}

	HANDLE ac_client = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procid);
	if (!ac_client) {
		std::cerr << GetLastError() << '\n';
		return 1;
	}

	uintptr_t base_address = getBaseAddress(procid, L"ac_client.exe");
	uintptr_t player_offset = 0x17E0A8;
	uintptr_t player_address{};

	int health_offset = 0xEC;
	int ammo_offset = 0x140;

	ReadProcessMemory(ac_client, (LPCVOID)(base_address + player_offset), &player_address, sizeof(player_address), NULL);

	uintptr_t health_address = player_address + health_offset;
	uintptr_t ammo_address = player_address + ammo_offset;

	int infinite_health = 2147483647;
	int infinite_ammo = 2147483647;

	if (!WriteProcessMemory(ac_client, (LPVOID)health_address, &infinite_health, sizeof(infinite_health), NULL)) {
		std::cerr << GetLastError() << '\n';
		return 1;
	}
	else {
		std::cout << "Changed health to " << infinite_health << '\n';
	}

	if (!WriteProcessMemory(ac_client, (LPVOID)ammo_address, &infinite_ammo, sizeof(infinite_ammo), NULL)) {
		std::cerr << GetLastError() << '\n';
		return 1;
	}
	else {
		std::cout << "Changed ammo to " << infinite_ammo << '\n';
	}

	CloseHandle(ac_client);
	return 0;
}