#include "memory.h"
#include <thread>
#include <iostream>

namespace offsets {
	//client
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDA746C;
	constexpr ::std::ptrdiff_t m_fFlags = 0x104;
	constexpr ::std::ptrdiff_t dwForceJump = 0x526B5A0;
	constexpr ::std::ptrdiff_t dwGlowObjectManager = 0x5309C90;
	constexpr ::std::ptrdiff_t dwEntityList = 0x4DC178C;

	//entiry
	constexpr ::std::ptrdiff_t m_iTeamNum = 0xF4;
	constexpr ::std::ptrdiff_t m_iGlowIndex = 0x10488;
}

__declspec(align(16)) struct Color
{
	constexpr Color(const float r, const float g, const float b, const float a = 1.f) noexcept :
		r(r), g(g), b(b), a(a) { }

	float r, g, b, a;
};

using namespace std;
using namespace offsets;

int main() {
	auto mem = Memory("csgo.exe");

	cout << "Process Id: " << mem.GetPID() << endl;

	const uintptr_t client = mem.GetModuleAddress("client.dll");
	cout << "client.dll: " << client << endl;

	constexpr const auto color = Color{ 1.f, 1.f, 1.f };

	while (true) {
		const auto localPlayer = mem.Read<uintptr_t>(client + dwLocalPlayer);
		const auto localPlayerTeam = mem.Read<std::uintptr_t>(localPlayer + offsets::m_iTeamNum);
		if (localPlayer) {
			const auto onGround = mem.Read<bool>(localPlayer + m_fFlags);
			//m_fFlags for onGround is (1 << 0)
			if (GetAsyncKeyState(VK_SPACE) && onGround & (1 << 0)) {
				mem.Write<BYTE>(client + dwForceJump, 6);
			}
			const auto glowObjectManager = mem.Read<std::uintptr_t>(client + offsets::dwGlowObjectManager);

			for (auto i = 1; i <= 64; ++i)
			{
				const auto entity = mem.Read<std::uintptr_t>(client + offsets::dwEntityList + i * 0x10);

				// dont glow if they are on our team
				if (mem.Read<std::uintptr_t>(entity + offsets::m_iTeamNum) == localPlayerTeam)
					continue;

				const auto glowIndex = mem.Read<std::int32_t>(entity + offsets::m_iGlowIndex);

				mem.Write<Color>(glowObjectManager + (glowIndex * 0x38) + 0x8, color);

				mem.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x27, true);
				mem.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x28, true);
			}
		}
		this_thread::sleep_for(chrono::milliseconds(1));
	}

}