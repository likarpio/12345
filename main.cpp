#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <array>
#include <string>

#include <../imgui/imgui.h>
#include <../imgui/imgui_impl_dx11.h>
#include <../imgui/imgui_impl_win32.h>

#include "memory/memory.h"
#include "offsets/offsets.h"
#include "calc/math.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param))
	{
		return 0L;
	}


	if (message == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0L;
	}

	return DefWindowProc(window, message, w_param, l_param);
}



INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmd_show)
{

	//Window Setup

	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = window_procedure;
	wc.hInstance = instance;
	wc.lpszClassName = L"ExternelOverlayClass";

	RegisterClassExW(&wc);

	const HWND window = CreateWindowExW(
		WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
		wc.lpszClassName,
		L"External ESP for AC (Cillo)",
		WS_POPUP,
		0,
		0,
		1920,
		1080,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);


	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);


	{
		RECT client_area{};
		GetClientRect(window, &client_area);

		RECT window_area{};
		GetWindowRect(window, &window_area);

		POINT diff{};
		ClientToScreen(window, &diff);

		const MARGINS margins
		{
			window_area.left + (diff.x - window_area.left),
			window_area.top + (diff.y - window_area.top),
			client_area.right,
			client_area.bottom
		};

		DwmExtendFrameIntoClientArea(window, &margins);
	}

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.RefreshRate.Numerator = 60U;
	sd.BufferDesc.RefreshRate.Denominator = 1U;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1U;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2U;
	sd.OutputWindow = window;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


	constexpr D3D_FEATURE_LEVEL levels[2]
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};

	ID3D11Device* device{ nullptr };
	ID3D11DeviceContext* device_context{ nullptr };
	IDXGISwapChain* swap_chain{ nullptr };
	ID3D11RenderTargetView* render_target_view{ nullptr };
	D3D_FEATURE_LEVEL level{};

	
	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0U,
		levels,
		2U,
		D3D11_SDK_VERSION,
		&sd,
		&swap_chain,
		&device,
		&level,
		&device_context
	);

	ID3D11Texture2D* back_buffer{ nullptr };
	swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));

	if (back_buffer)
	{
		device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
		back_buffer->Release();
	}
	else
	{
		return 1;
	}

	ShowWindow(window, cmd_show);
	UpdateWindow(window);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, device_context);

	bool running = true;


	//declare memory instance
	memory mem;

	//get processID
	HWND hwnd = FindWindowA(0, "AssaultCube");
	DWORD pID;
	GetWindowThreadProcessId(hwnd, &pID);

	//get baseAddress
	DWORD baseAddr;
	baseAddr = mem.GetModuleBaseAddress(pID, L"ac_client.exe");

	std::cout << baseAddr << "\n\n";

	//get HANDLE with ALL_ACCESS
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, pID);

	uintptr_t locPlayer = memory::readMem<uintptr_t>(baseAddr + offsets::localPlayer, handle);

	uintptr_t entityList = memory::readMem<uintptr_t>(baseAddr + offsets::entityList, handle);

	uintptr_t currentEntity;
	std::array<char, 32> name;
	int entHealth;

	std::array<float, 16> viewmatrix;
	Vector2 screen;
	Vector2 start;
	Vector2 end;
	Vector3 pos;
	Vector3 locPos;
	Math math;

	float yaw;
	float pitch;

	while (running)
	{
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				running = false;
			}
		}


		if (!running)
		{
			break;
		}
		
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		


		ImGui::NewFrame();

		// RENDERING

		
		viewmatrix = memory::readMem<std::array<float, 16>>(offsets::viewMatrix, handle); //viewmatrix set
		float leastDst  = 999999;
		DWORD nearestEntity = 0;
	
		//loop entList
		for (int i = 1; i < 32; i++)
		{
			currentEntity = memory::readMem<DWORD>(entityList + 0x4 * i, handle); // get current entity

			entHealth = memory::readMem<uintptr_t>(currentEntity + offsets::health, handle); // entitys health set
			    
            name = memory::readMem<std::array<char, 32>>(currentEntity + offsets::name, handle); //entitys name set

			// get localPlayer position
			locPos.x = memory::readMem<float>(locPlayer + offsets::entityPosX, handle);
			locPos.y = memory::readMem<float>(locPlayer + offsets::entityPosY, handle);
			locPos.z = memory::readMem<float>(locPlayer + offsets::entityPosZ, handle);

			// get entity position
			pos.x = memory::readMem<float>(currentEntity + offsets::entityPosX, handle);
			pos.y = memory::readMem<float>(currentEntity + offsets::entityPosY, handle);
			pos.z = memory::readMem<float>(currentEntity + offsets::entityPosZ, handle);

			int myTeam = memory::readMem<uintptr_t>(locPlayer + offsets::teamNum, handle);
			int entTeam = memory::readMem<uintptr_t>(currentEntity + offsets::teamNum, handle);

			if (math.WorldToScreen(pos, screen, viewmatrix, 1920, 1080) && entHealth > 0 && myTeam != entTeam)
			{
				
				ImGui::GetBackgroundDrawList()->AddCircleFilled({ screen.x, screen.y }, 2.f, ImColor(5.f, 200.f, 0.f));
				
				if (math.getRectPos(locPos, pos, screen, start, end))
				{
					ImGui::GetBackgroundDrawList()->AddRect({start.x, start.y}, {end.x, end.y}, IM_COL32(255, 0, 0, 255), 0.0f, 0, 2.0f);
					
					ImGui::GetBackgroundDrawList()->AddText({ screen.x, start.y-20.f }, ImColor(1.f, 200.f, 0.f), name.data());
	
				}
				
			}

			// get nearest Entity
			if (leastDst > math.DistanceTo(pos, locPos) && memory::readMem<uintptr_t>(locPlayer + offsets::teamNum, handle) != memory::readMem<uintptr_t>(currentEntity + offsets::teamNum, handle))
			{
				if (memory::readMem<uintptr_t>(currentEntity + offsets::health, handle) <= 0 || memory::readMem<uintptr_t>(currentEntity + offsets::health, handle) > 100)
					continue;
				else {
					leastDst = math.DistanceTo(pos, locPos);
					nearestEntity = currentEntity;
				}
									
			}
			
		}

		// get nearestEntPos
		pos.x = memory::readMem<float>(nearestEntity + offsets::entityPosX, handle);
		pos.y = memory::readMem<float>(nearestEntity + offsets::entityPosY, handle);
		pos.z = memory::readMem<float>(nearestEntity + offsets::entityPosZ, handle);


		// aimbot
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			if (math.calcViewAngles(yaw, pitch, locPos, pos))
			{
				memory::writeMem<float>(locPlayer + offsets::viewAngleYaw, yaw, handle);
				memory::writeMem<float>(locPlayer + offsets::viewAnglePitch, pitch, handle);
			}
		}

		// RENDERING LAST
		ImGui::Render();
	
		constexpr float color[4]{ 0.f, 0.f, 0.f, 0.f };
		device_context->OMSetRenderTargets(1U, &render_target_view, nullptr);
		device_context->ClearRenderTargetView(render_target_view, color);

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		swap_chain->Present(1U, 0U);

	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();

	if (swap_chain)
		swap_chain->Release();

	if (device_context)
		device_context->Release();

	if (device)
		device->Release();

	if (render_target_view)
		render_target_view->Release();


	DestroyWindow(window);

	UnregisterClassW(wc.lpszClassName, wc.hInstance);



	return 0;

}