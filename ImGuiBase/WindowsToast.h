#pragma once
namespace toast
{
	inline void push()
	{
        // Set the title and message for the notification
        LPCWSTR title = L"My Notification";
        LPCWSTR message = L"Hello, World!";

        // Create the notification object
        NOTIFYICONDATA notification = { sizeof(NOTIFYICONDATA) };
        notification.hWnd = NULL;
        notification.uID = 0;
        notification.uFlags = NIF_INFO;
        notification.dwInfoFlags = NIIF_INFO;
        notification.uTimeout = 10000;
        notification.hIcon = LoadIcon(NULL, IDI_INFORMATION);
        wcscpy_s(notification.szInfoTitle, title);
        wcscpy_s(notification.szInfo, message);

        Shell_NotifyIcon(NIM_DELETE, &notification);

        // Show the notification
        Shell_NotifyIcon(NIM_ADD, &notification);

        // Clean up
        DestroyIcon(notification.hIcon);
	}
}