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
void updateFields(HWND, WORD);
std::wstring windowTextToStr(HWND);
void drawLines(HDC, std::wstring, RECT&);

const HFONT FONT = CreateFont(30, 0, 0, 0, 400,
    FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH,
    TEXT("Calibri"));

static HWND editButton;
static const WORD editBtnId = 0;

static HWND editTitle;
static const WORD editTitleId = 1;

static HWND editDesc;
static const WORD editDescId = 2;

static bool editing = false;

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
    case WM_COMMAND:
    {
        WORD notiCode = HIWORD(wParam);
        switch (notiCode) {
        case BN_CLICKED:
            if (LOWORD(wParam) == editBtnId) {
                editing = !editing;
                InvalidateRect(hWnd, NULL, RDW_INVALIDATE);
            }
            break;
        case EN_CHANGE:
            updateFields(hWnd, LOWORD(wParam));
            break;
        }
        break;
    }
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
    //all of this stuff is basically the same as in NodeInfo. I should find a way to
    //abstract it and to adhere to good principles. TODO
    editButton = CreateWindowEx(0,
        _T("BUTTON"),
        L"Edit",
        WS_TABSTOP | WS_CHILD | BS_DEFPUSHBUTTON,
        30, 5, 50, 25,
        hLeftInfo,
        NULL,
        hInst,
        NULL);

    DWORD textStyles = WS_TABSTOP | WS_CHILD | WS_BORDER |
        ES_WANTRETURN | ES_MULTILINE | ES_AUTOVSCROLL;

    editTitle = CreateWindowEx(0,
        _T("EDIT"),
        _T("Edit Title"),
        textStyles,
        0,0,200,30,
        hLeftInfo,
        NULL,
        hInst,
        NULL);

    editDesc = CreateWindowEx(0,
        _T("EDIT"),
        _T("Edit Title"),
        textStyles,
        0, 0, 200, 30,
        hLeftInfo,
        NULL,
        hInst,
        NULL);

    SetWindowLong(editButton, GWL_ID, editBtnId);
    SetWindowLong(editTitle, GWL_ID, editTitleId);
    SetWindowLong(editDesc, GWL_ID, editDescId);

    ShowWindow(editTitle, SW_HIDE);
    ShowWindow(editDesc, SW_HIDE);

    Edit_SetText(editTitle, (LPTSTR)FCState::title.c_str());
    Edit_SetText(editDesc, (LPTSTR)FCState::desc.c_str());

    SendMessage(hLeftInfo, WM_SETFONT, (WPARAM)FONT, NULL);
    SendMessage(editTitle, WM_SETFONT, (WPARAM)FONT, NULL);
    SendMessage(editDesc, WM_SETFONT, (WPARAM)FONT, NULL);
}

void paintLeftInfo(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    SelectFont(hdc, FONT);
    //TODO make these just sit in text boxes, just turn border on and off.


    std::wstringstream percentageStream;
    percentageStream << _T("Complete: ") << std::round(FCState::completionPercent()) << _T("%");

    RECT area;
    GetClientRect(hWnd, &area);

    RECT textRect = area;
    int padding = 30;
    int lineSize = 30;
    int inPad = 8;
    textRect.left += padding;
    textRect.right -= padding;
    textRect.top += padding;
    textRect.bottom -= padding;
    int textWidth = textRect.right - textRect.left;

    textRect.top += lineSize;

    if (editing) {
        ShowWindow(editTitle, SW_SHOW);
        ShowWindow(editDesc, SW_SHOW);
        Edit_Enable(editTitle, TRUE);
        Edit_Enable(editDesc, TRUE);

        //The height depends on the width, so make sure the width is correct.
        //SetWindowPos(editTitle, NULL, );
        int boxHeight = lineSize * Edit_GetLineCount(editTitle);

        //set edit control size and adjust remaining space
        SetWindowPos(editTitle, NULL,
            textRect.left - inPad,
            textRect.top - 2,
            textWidth + (2 * inPad),
            boxHeight + (inPad),
            SWP_NOZORDER);

        textRect.top += boxHeight;

        drawLines(hdc, percentageStream.str(), textRect);
        textRect.top += lineSize;

        //setEditControlHeight
        boxHeight = lineSize * Edit_GetLineCount(editDesc);

        SetWindowPos(editDesc, NULL,
            textRect.left - inPad,
            textRect.top - 2,
            textWidth + (2 * inPad),
            boxHeight + (inPad),
            SWP_NOZORDER);

    }
    else {
        ShowWindow(editTitle, SW_HIDE);
        ShowWindow(editDesc, SW_HIDE);
        Edit_Enable(editTitle, FALSE);
        Edit_Enable(editDesc, FALSE);

        drawLines(hdc, FCState::title, textRect);
        drawLines(hdc, percentageStream.str(), textRect);
        textRect.top += lineSize;
        drawLines(hdc, FCState::desc, textRect);
    }

    EndPaint(hWnd, &ps);
}

void updateFields(HWND hWnd, WORD code) {
    switch ((int)code) {
    case editTitleId:
        FCState::title = windowTextToStr(editTitle);
        break;
    case editDescId:
        FCState::desc = windowTextToStr(editDesc);
        break;
    }
    RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
}

static std::wstring windowTextToStr(HWND hWnd) {
    TCHAR buf[4096];
    GetWindowText(hWnd, buf, 4096);
    std::wstring str;
    for (TCHAR c : buf) {
        str += c;
    }
    return str;
}

static void drawLines(HDC hdc, std::wstring text, RECT& textRect) {
    DWORD form = DT_LEFT | DT_NOCLIP | DT_WORDBREAK;
    textRect.top += DrawTextEx(hdc, (LPTSTR)text.c_str(), -1, &textRect, form, NULL);
}