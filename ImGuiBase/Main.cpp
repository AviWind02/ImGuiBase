#include "common.hpp"
#include "Rendering/Rendering.h"

ImFont *normaltext, *biggertext, *titlesizetext;
int main()
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, render::WindowProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, (LPCWSTR)ProjectName, NULL };
	::RegisterClassEx(&wc);

    HWND hwnd = ::CreateWindowW(wc.lpszClassName, (LPCWSTR)ProjectName, WS_POPUP, 150, 150, render::windowsize.x, render::windowsize.y, NULL, NULL, wc.hInstance, NULL);
    hwnd == NULL ? push::Log_Error("Window Not Initialized.") : push::Log("Window Initialized.");
    render::gamewindow = hwnd;


    // Initialize Direct3D
    if (!render::CreateDeviceD3D(hwnd))
    {
        render::CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }
    push::Log_debug("Direct3D Initialized.");

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();

    ImGuiContext* ctx = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    push::Log_debug("Context Initialized.");

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(render::g_pd3dDevice, render::g_pd3dDeviceContext);
    push::Log_debug("Renderer Initialized.");



    ImFontConfig font_cfg{};
    font_cfg.FontDataOwnedByAtlas = false;
    strcpy_s(font_cfg.Name, "Rubik");
    normaltext = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(font_rubik), sizeof(font_rubik), 20.f, &font_cfg);
    notify::init();
    biggertext = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(font_rubik), sizeof(font_rubik), 25.f, &font_cfg);
    titlesizetext = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(font_rubik), sizeof(font_rubik), 40.f, &font_cfg);
    push::Log_debug("Fonts & Notify  Initialized.");


    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    ImVec4 clear_color = { 0.f, 0.f, 0.f, 0.f };

    while (msg.message != WM_QUIT)
    {

        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }
        //p2Detection.debugger_loop();
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        dx_init();
        // Create a window
        ImGui::Begin("Hello, World!");

        // Add a button
        if (ImGui::Button("Click Me!"))
        {
            notify::insert({ toast_type::toast_type_info, 1000, "Test"});
        }

        ImGui::End();
        notify::render();
        ImGui::Render();
        render::g_pd3dDeviceContext->OMSetRenderTargets(1, &render::g_mainRenderTargetView, NULL);
        render::g_pd3dDeviceContext->ClearRenderTargetView(render::g_mainRenderTargetView, (float*)&clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        render::g_pSwapChain->Present(1, 0);


    }


    push::Log_debug("Cleaning up.");
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    render::CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);


    push::Log_debug("Good bye.");

	return NULL;
}