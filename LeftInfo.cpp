#include "LeftInfo.h"
#include "ErrorHandler.h"
#include "Classes.h"
#include "resource.h"
#include <cmath>
#include <sstream>
#include <windowsx.h>
#include "FCState.h"


LRESULT CALLBACK LeftInfoProc(HWND, UINT, WPARAM, LPARAM);
void paintLeftInfo(HWND);

const HFONT FONT = CreateFont(30, 0, 0, 0, 400,
    FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH,
    TEXT("Calibri"));

ATOM registerLeftInfo(HINSTANCE hInst) {
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = LeftInfoProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInst;
    wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_FLOWCHART));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_FLOWCHART);
    wcex.lpszClassName = _T(LEFTINFOCLASS);
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

LRESULT CALLBACK LeftInfoProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT:
        paintLeftInfo(hWnd);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void createLeftInfo(HINSTANCE hInst, HWND hParent) {
    HWND hLeftInfo = CreateWindowEx(0,
        _T(LEFTINFOCLASS),
        L"Info Window",
        WS_CHILD | WS_BORDER,
        0, 0, 50, 100,
        hParent,
        (HMENU)(int)(ID_LEFTINFO),
        hInst,
        NULL);
    if (hLeftInfo == NULL) {
        errorHandler((LPTSTR)_T("CreateWindowEx"));
    }
    
    SendMessage(hLeftInfo, WM_SETFONT, (WPARAM)FONT, NULL);
}

void paintLeftInfo(HWND hWnd) {
    TCHAR msg[] = _T("Hello World");
    TCHAR title[] = _T("Flowchart title");

    std::wstringstream percentageStream;
    percentageStream << _T("Complete: ") << std::round(FCState::completionPercent()) << _T("%");

    TCHAR description[] = _T("Description: This is a flowchart description. Use it to describe your flowchart, and what the end of the flowchart means.");
    


    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);


    SelectFont(hdc, FONT);
    
    RECT area;
    GetClientRect(hWnd, &area);

    RECT textRect = area;
    int padding = 30;
    textRect.left += padding;
    textRect.right -= padding;
    textRect.top += padding;
    textRect.bottom -= padding;
    
    textRect.top += DrawTextEx(hdc, title, -1, &textRect, DT_LEFT | DT_NOCLIP | DT_WORDBREAK, NULL);

    textRect.top += DrawTextEx(hdc, (LPWSTR)percentageStream.str().c_str(), -1, &textRect, DT_LEFT | DT_NOCLIP | DT_WORDBREAK, NULL) * 2;

    textRect.top += DrawTextEx(hdc, description, -1, &textRect, DT_LEFT | DT_NOCLIP | DT_WORDBREAK, NULL);


    EndPaint(hWnd, &ps);
}