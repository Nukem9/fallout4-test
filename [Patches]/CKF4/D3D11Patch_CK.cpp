//////////////////////////////////////////
/*
* Copyright (c) 2020 Nukem9 <email:Nukem@outlook.com>
* Copyright (c) 2020-2022 Perchik71 <email:perchik71@outlook.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files (the "Software"), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/
//////////////////////////////////////////

#include "..\..\[Common]\StdAfx.h"
#include "..\..\[Common]\D3D11Proxy.h"
#include "..\..\[EditorAPI]\TESRender.h"

ID3D11Texture2D* g_OcclusionTexture;
ID3D11ShaderResourceView* g_OcclusionTextureSRV;

decltype(&IDXGISwapChain::Present) ptrPresent;
decltype(&CreateDXGIFactory) ptrCreateDXGIFactory;
decltype(&D3D11CreateDeviceAndSwapChain) ptrD3D11CreateDeviceAndSwapChain;

ID3D11Device2* g_Device_2;
ID3D11Device* g_Device;
IDXGISwapChain* g_SwapChain;
ID3D11DeviceContext* g_DeviceContext;

HRESULT WINAPI hk_CreateDXGIFactory(REFIID riid, void** ppFactory) {
	_F4CKMSG("// Creating DXGI factory...");

	if (SUCCEEDED(ptrCreateDXGIFactory(__uuidof(IDXGIFactory3), ppFactory)))
		return S_OK;

	if (SUCCEEDED(ptrCreateDXGIFactory(__uuidof(IDXGIFactory2), ppFactory)))
		return S_OK;

	return ptrCreateDXGIFactory(__uuidof(IDXGIFactory), ppFactory);
}

HRESULT WINAPI NsightHack_D3D11CreateDeviceAndSwapChain(
	IDXGIAdapter* pAdapter,
	D3D_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	const D3D_FEATURE_LEVEL* pFeatureLevels,
	UINT FeatureLevels,
	UINT SDKVersion,
	const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
	IDXGISwapChain** ppSwapChain,
	ID3D11Device** ppDevice,
	D3D_FEATURE_LEVEL* pFeatureLevel,
	ID3D11DeviceContext** ppImmediateContext) {
	//
	// Nvidia NSight checks the return address of D3D11CreateDeviceAndSwapChain to see if it's
	// a blacklisted directx dll. "d3dx9_42.dll" happens to be in that list. So, now I need to
	// generate code which spoofs the return address to something random.
	//
	// NOTE: Do NOT touch rcx, rdx, r8, r9
	//
	class d3djmp : public Xbyak::CodeGenerator {
	public:
		d3djmp() : Xbyak::CodeGenerator() {
			push(rbx);
			push(rsi);
			push(rdi);
			sub(rsp, 0x60);
			mov(rax, qword[rsp + 0xD8]);
			mov(r10, qword[rsp + 0xD0]);
			mov(r11, qword[rsp + 0xC8]);
			mov(rbx, qword[rsp + 0xC0]);
			mov(rdi, qword[rsp + 0xB8]);
			mov(rsi, qword[rsp + 0xA0]);
			mov(qword[rsp + 0x58], rax);
			mov(eax, dword[rsp + 0xB0]);
			mov(qword[rsp + 0x50], r10);
			mov(qword[rsp + 0x48], r11);
			mov(qword[rsp + 0x40], rbx);
			mov(qword[rsp + 0x38], rdi);
			mov(dword[rsp + 0x30], eax);
			mov(eax, dword[rsp + 0xA8]);
			mov(dword[rsp + 0x28], eax);
			mov(qword[rsp + 0x20], rsi);

			mov(rax, (uintptr_t)ptrD3D11CreateDeviceAndSwapChain);
			call(rax);

			add(rsp, 0x60);
			pop(rdi);
			pop(rsi);
			pop(rbx);
			ret();
		}
	} hack;

	auto newPtr = hack.getCode<decltype(&D3D11CreateDeviceAndSwapChain)>();
	return newPtr(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
}

HRESULT WINAPI hk_D3D11CreateDeviceAndSwapChain(
	IDXGIAdapter* pAdapter,
	D3D_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	const D3D_FEATURE_LEVEL* pFeatureLevels,
	UINT FeatureLevels,
	UINT SDKVersion,
	const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
	IDXGISwapChain** ppSwapChain,
	ID3D11Device** ppDevice,
	D3D_FEATURE_LEVEL* pFeatureLevel,
	ID3D11DeviceContext** ppImmediateContext) {
	//
	// From MSDN:
	//
	// If the Direct3D 11.1 runtime is present on the computer and pFeatureLevels is set to NULL,
	// this function won't create a D3D_FEATURE_LEVEL_11_1 device. To create a D3D_FEATURE_LEVEL_11_1
	// device, you must explicitly provide a D3D_FEATURE_LEVEL array that includes
	// D3D_FEATURE_LEVEL_11_1. If you provide a D3D_FEATURE_LEVEL array that contains
	// D3D_FEATURE_LEVEL_11_1 on a computer that doesn't have the Direct3D 11.1 runtime installed,
	// this function immediately fails with E_INVALIDARG.
	//
	const D3D_FEATURE_LEVEL testFeatureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL level;
	HRESULT hr;

	for (INT i = 0; i < ARRAYSIZE(testFeatureLevels); i++) {
		hr = NsightHack_D3D11CreateDeviceAndSwapChain(
			pAdapter,
			DriverType,
			Software,
			Flags,
			&testFeatureLevels[i],
			1,
			SDKVersion,
			pSwapChainDesc,
			ppSwapChain,
			ppDevice,
			&level,
			ppImmediateContext);

		if (SUCCEEDED(hr)) {
			if (pFeatureLevel)
				*pFeatureLevel = level;

			break;
		}
	}

	if (FAILED(hr)) {
		ShowWindow(pSwapChainDesc->OutputWindow, SW_HIDE);
		AssertMsg(FALSE, "DirectX11 device creation failed. Creation Kit will now exit.");
		ExitProcess(0);
	}

	// Create ImGui globals BEFORE the device is proxied
	_F4CKMSG_FMT("// Created D3D11 device with feature level %X...", level);

	if (SUCCEEDED((*ppDevice)->QueryInterface<ID3D11Device2>(&g_Device_2))) {
		// Force DirectX11.2 in case we use features later (11.3+ requires Win10 or higher)
		ID3D11Device2* proxyDevice = new D3D11DeviceProxy(*ppDevice);
		ID3D11DeviceContext2* proxyContext = new D3D11DeviceContextProxy(*ppImmediateContext);

		g_Device = proxyDevice;
		*ppDevice = proxyDevice;
		g_DeviceContext = proxyContext;
		*ppImmediateContext = proxyContext;

		_F4CKMSG("// Force DirectX11.2");
	}
	else {
		g_Device = *ppDevice;
		g_DeviceContext = *ppImmediateContext;
	}

	(*ppDevice)->SetExceptionMode(D3D11_RAISE_FLAG_DRIVER_INTERNAL_ERROR);
	g_SwapChain = *ppSwapChain;

	return hr;
}

VOID FIXAPI Fix_PatchD3D11_CreationKit(VOID) {
	BGSRenderWindow::InitializeHook();
	
	if (!g_INI->GetBoolean("CreationKit", "D3D11Patch", FALSE))
		return;

	_MESSAGE_BEGIN_PATCH("D3D11Patch");
	
	// Grab the original function pointers
	if (!g_DllDXGI) {
		g_DllDXGI = LoadLibraryExA("dxgi.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
		Assert(g_DllDXGI);
	}

	if (!g_DllD3D11) {
		g_DllD3D11 = LoadLibraryExA("d3d11.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
		Assert(g_DllD3D11);
	}

	*(FARPROC*)&ptrCreateDXGIFactory = GetProcAddress(g_DllDXGI, "CreateDXGIFactory1");

	if (!ptrCreateDXGIFactory)
		*(FARPROC*)&ptrCreateDXGIFactory = GetProcAddress(g_DllDXGI, "CreateDXGIFactory");

	*(FARPROC*)&ptrD3D11CreateDeviceAndSwapChain = GetProcAddress(g_DllD3D11, "D3D11CreateDeviceAndSwapChain");

	AssertMsg(ptrCreateDXGIFactory, "CreateDXGIFactory import not found");
	AssertMsg(ptrD3D11CreateDeviceAndSwapChain, "D3D11CreateDeviceAndSwapChain import not found");

	XUtil::PatchMemory(OFFSET(0x2A48B09, 0), { 0x7D });
	
	PatchIAT(hk_CreateDXGIFactory, "dxgi.dll", "CreateDXGIFactory");
	PatchIAT(hk_D3D11CreateDeviceAndSwapChain, "d3d11.dll", "D3D11CreateDeviceAndSwapChain");

	_MESSAGE_END_PATCH;
}
