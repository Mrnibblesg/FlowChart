#include "FlowChart.h"
#include "NodeInfo.h"
#include "LeftInfo.h"
#include "resource.h"
#include "Classes.h"
#include "ErrorHandler.h"
#include <windowsx.h>
#include <iostream>

INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
BOOL resizeChildProc(HWND, LPARAM);
void getSizeContraint(LPARAM);
void paint(HWND);
LRESULT command(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInst;

ATOM registerFlowChart(HINSTANCE hInstance) {
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = FlowChartProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FLOWCHART));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
    wcex.lpszClassName = _T(MAINWINCLASS);
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex); //Register window with the OS
}

//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
BOOL InitFlowChart(HINSTANCE hInstance, int nCmdShow) {

    hInst = hInstance;
    HWND hWnd = CreateWindow(
        _T(MAINWINCLASS), _T(APPTITLE),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr, hInstance, nullptr);


    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}


//  FUNCTION: FlowChartProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
LRESULT CALLBACK FlowChartProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_LBUTTONDOWN:
    {

    }
    break;
    case WM_CREATE:
        createLeftInfo(hInst, hWnd);
        createNodeInfo(hInst, hWnd);
        return 0;
    case WM_SIZE: {
        RECT rcClient;
        GetClientRect(hWnd, &rcClient);
        EnumChildWindows(hWnd, resizeChildProc, (LPARAM)&rcClient);
        break;
    }
    case WM_GETMINMAXINFO:
        getSizeContraint(lParam);
        return 0;
    case WM_COMMAND:
        command(hWnd, message, wParam, lParam);
        break;
    case WM_PAINT:
        paint(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

BOOL resizeChildProc(HWND hWndChild, LPARAM rect) {
    LPRECT rcParent = (LPRECT)rect;
    int windowId = GetWindowLong(hWndChild, GWL_ID);
    
    //Window sizes: left info: 1/5 screen, left
    //node info: 1/5 screen, right
    float width = max(rcParent->right / 5, 300);
    if (windowId == ID_LEFTINFO) {
        MoveWindow(hWndChild,
            0, 0, width, rcParent->bottom, TRUE);
    }
    else if (windowId == ID_NODEINFO) {
        MoveWindow(hWndChild,
            rcParent->right-width, 0, width, rcParent->bottom, TRUE);
    }
    ShowWindow(hWndChild, SW_SHOW);
    return TRUE;
}

void getSizeContraint(LPARAM minMaxInfo) {
    MINMAXINFO* mmi = (MINMAXINFO*)minMaxInfo;
    mmi->ptMinTrackSize.x = 800;
    mmi->ptMinTrackSize.y = 500;
}

void paint(HWND hWnd) {
    TCHAR msg[] = _T("Hello World!");
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    HFONT font = CreateFont(48, 0, 0, 0, 400,
        FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH,
        TEXT("Calibri"));

    SelectFont(hdc, font);
    //SetWindowTextW(hWnd, _T("Joe mama"));
    TextOut(hdc, 5, 5, msg, _tcslen(msg));

    EndPaint(hWnd, &ps);
}

LRESULT command(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    int wmId = LOWORD(wParam);
    // Parse the menu selections:
    switch (wmId)
    {
    case IDM_ABOUT:
        DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}