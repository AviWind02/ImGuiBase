#pragma once

namespace render
{
	// Data
	inline ID3D11Device* g_pd3dDevice = NULL;
	inline ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
	inline IDXGISwapChain* g_pSwapChain = NULL;
	inline ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

	extern HWND gamewindow;
	extern bool CreateDeviceD3D(HWND hWnd);
	extern void CleanupDeviceD3D();
	extern void CreateRenderTarget();
	extern void CleanupRenderTarget();
	extern ImVec2 windowsize;
	extern LRESULT WINAPI WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}