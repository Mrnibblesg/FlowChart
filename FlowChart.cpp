#include "FlowChart.h"
#include "NodeInfo.h"
#include "LeftInfo.h"
#include "resource.h"
#include "Classes.h"
#include "ErrorHandler.h"
#include <windowsx.h>
#include "Node.h"
#include <cmath>
#include <vector>
#include "FCState.h"

#define WM_UPDATESELECTION (WM_USER+0)

INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
BOOL resizeChildProc(HWND, LPARAM);
void getSizeContraint(LPARAM);
void paint(HWND);
LRESULT command(HWND, UINT, WPARAM, LPARAM);
void createNode(HWND, LPARAM);
void drawCircle(const HDC&, const POINT&, int);
Node* checkClickedNode(LPARAM);

HINSTANCE hInst;

HWND hNodeInfo = nullptr;

ATOM registerFlowChart(HINSTANCE hInstance) {
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
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
        //what is clicked?
        //background click+drag: move background
        //node click+drag: move node
        //node rclick: nothing
        //node rclick+drag to node: set as required to second node
    case WM_RBUTTONDOWN:

        break;
    case WM_RBUTTONUP:

        break;
    case WM_LBUTTONDOWN:
    {
        checkClickedNode(lParam);
        RedrawWindow(hWnd, 0, 0, RDW_INVALIDATE);
    }
    break;
    case WM_LBUTTONDBLCLK:
    {
        createNode(hWnd, lParam);
    }
    break;
    case WM_CREATE:
        createLeftInfo(hInst, hWnd);
        hNodeInfo = createNodeInfo(hInst, hWnd);
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

BOOL resizeChildProc(HWND hWndChild, LPARAM rect) {
    LPRECT rcParent = (LPRECT)rect;
    int windowId = GetWindowLong(hWndChild, GWL_ID);
    
    //Window sizes: left info: 1/5 screen, left
    //node info: 1/5 screen, right
    int width = max((int)rcParent->right / 5, 300);
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

//Set minimum window size
void getSizeContraint(LPARAM minMaxInfo) {
    MINMAXINFO* mmi = (MINMAXINFO*)minMaxInfo;
    mmi->ptMinTrackSize.x = 800;
    mmi->ptMinTrackSize.y = 500;
}

//Every node, and all of their connections.
void paint(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    
    HFONT font = CreateFont(48, 0, 0, 0, 400,
        FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH,
        TEXT("Calibri"));

    SelectFont(hdc, font);

    for (Node* n : FCState::nodes) {
        POINT pos = n->getPos();
        HGDIOBJ orig = SelectObject(hdc, GetStockObject(DC_PEN));
        HBRUSH selectBrush = CreateSolidBrush(RGB(255,255,0));
        HBRUSH deselectBrush = CreateSolidBrush(RGB(255,255,255));
        
        
        if (n == FCState::selected) {
            SelectObject(hdc, selectBrush);
            drawCircle(hdc, pos, n->getRadius());
            SelectObject(hdc, deselectBrush);
        }
        else {
            drawCircle(hdc, pos, n->getRadius());
        }
    }
    
    EndPaint(hWnd, &ps);
}

void createNode(HWND hWnd, LPARAM lParam) {
    POINT p{
        GET_X_LPARAM(lParam),
        GET_Y_LPARAM(lParam)
    };
    Node* newNode = new Node(p);
    FCState::nodes.push_back(newNode);
    FCState::selected = newNode;
    RedrawWindow(hWnd, 0, 0, RDW_INVALIDATE);
}

void drawCircle(const HDC& hdc, const POINT& center, int radius) {
    Ellipse(hdc, center.x - radius,
        center.y - radius,
        center.x + radius,
        center.y + radius);
}

//returns a pointer to the node that was clicked. null if none were clicked.
Node* checkClickedNode(LPARAM lParam) {
    POINT click{
    GET_X_LPARAM(lParam),
    GET_Y_LPARAM(lParam)
    };
    FCState::selected = nullptr;

    for (int i = FCState::nodes.size() - 1; i >= 0; i--) {
        Node* n = FCState::nodes.at(i);
        POINT pos = n->getPos();
        int rad = n->getRadius();
        int dx = pos.x - click.x;
        int dy = pos.y - click.y;

        if (std::sqrt(dx * dx + dy * dy) < rad) {
            FCState::selected = n;
            break;
        }
    }

    SendMessage(hNodeInfo, WM_UPDATESELECTION, 0, (LPARAM)FCState::selected);
    return FCState::selected;
}