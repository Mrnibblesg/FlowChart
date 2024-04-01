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
#include "SaveLoad.h"


#define WM_UPDATESELECTION (WM_USER+0)

INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
BOOL resizeChildProc(HWND, LPARAM);
void getSizeContraint(LPARAM);
void paint(HWND);
LRESULT command(HWND, UINT, WPARAM, LPARAM);
void createNode(HWND, LPARAM);
void drawCircle(const HDC&, const POINT&, int);
Node* checkClickedNode(LPARAM);

void drawArrow(const HDC&, const Node&, const Node&);

const double PI = std::atan(1.0) * 4;

const HFONT FONT = CreateFont(48, 0, 0, 0, 400,
    FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH,
    TEXT("Calibri"));

const HBRUSH deselected = CreateSolidBrush(RGB(255, 255, 255));
const HBRUSH selected = CreateSolidBrush(RGB(255, 255, 0));
const HBRUSH complete = CreateSolidBrush(RGB(0, 255, 0));
const HBRUSH completeSelected = CreateSolidBrush(RGB(43, 121, 255));

//const HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));


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
        WS_OVERLAPPEDWINDOW | WS_BORDER,
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
    case WM_RBUTTONDOWN:
    {
        Node* rightClicked = checkClickedNode(lParam);
        if (rightClicked != nullptr) {
            FCState::connectBegin = rightClicked;
            FCState::selected = rightClicked;
            RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
        }
    }
        break;
    case WM_RBUTTONUP:
    {
        Node* rightClicked = checkClickedNode(lParam);
        if (rightClicked != nullptr &&
            FCState::connectBegin != nullptr &&
            rightClicked != FCState::connectBegin) {

            rightClicked->addReq(FCState::connectBegin);
            FCState::selected = rightClicked;
            InvalidateRect(hWnd, NULL, TRUE);
        }
        FCState::connectBegin = nullptr;
    }   
        break;
    case WM_LBUTTONDOWN:
    {
        FCState::lButtonDown = true;
        checkClickedNode(lParam);
        InvalidateRect(hWnd, NULL, TRUE);
    }
    break;
    case WM_MOUSEMOVE:
    {
        if (FCState::lButtonDown) {
            Node* selected = FCState::selected;
            if (selected != nullptr) {
                int nx = selected->getPos().x;
                int ny = selected->getPos().y;
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                int rad = selected->getRadius()+1;

                RECT redraw;
                redraw.left = min(x,nx) - rad;
                redraw.right = max(x,ny) + rad;
                redraw.top = min(y,ny) - rad;
                redraw.bottom = max(y,ny) + rad;

                FCState::selected->setPos(x, y);
                InvalidateRect(hWnd, &redraw, TRUE);
            }
            else {
                //TODO drag background
            }
        }
        
    }
    break;
    case WM_LBUTTONUP:
    {
        InvalidateRect(hWnd, NULL, TRUE);
        FCState::lButtonDown = false;
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
    case ID_FILE_SAVE:
        save(hWnd);
        break;
    case ID_FILE_LOAD:
        load(hWnd);
        InvalidateRect(hWnd, NULL, RDW_ERASE);
        break;
    case ID_FILE_NEW:
        saveAs(hWnd);
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

    SelectFont(hdc, FONT);

    for (Node* n : FCState::nodes) {
        POINT pos = n->getPos();
        HGDIOBJ orig = SelectObject(hdc, GetStockObject(DC_PEN));
        
        //Arrows to this node
        for (Node* req : n->getReqs()) {
            drawArrow(hdc, *req, *n);
        }
        
        if (n == FCState::selected) {
            if (FCState::selected->getFulfilled()) {
                SelectObject(hdc, completeSelected);
            }
            else {
                SelectObject(hdc, selected);
            }
            
            drawCircle(hdc, pos, n->getRadius());
            SelectObject(hdc, deselected);
        }
        else {
            if (n->getFulfilled()) {
                SelectObject(hdc, complete);
            }
            drawCircle(hdc, pos, n->getRadius());
            SelectObject(hdc, deselected);
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

//arrow from p1 to p2
void drawArrow(const HDC& hdc, const Node& n1, const Node& n2) {
    POINT n1Pos = n1.getPos();
    POINT n2Pos = n2.getPos();
    double dy = n2Pos.y - n1.getPos().y;
    double dx = n2Pos.x - n1.getPos().x;
    double angle = std::atan2(dy,dx);
    double lineLen = std::sqrt(dx * dx + dy * dy) - n1.getRadius() - n2.getRadius();

    int startX = n1Pos.x + std::cos(angle) * n1.getRadius();
    int startY = n1Pos.y + std::sin(angle) * n1.getRadius();
    int endX = n1Pos.x + std::cos(angle) * (lineLen + n1.getRadius());
    int endY = n1Pos.y + std::sin(angle) * (lineLen + n1.getRadius());

    MoveToEx(hdc, startX, startY, NULL);
    LineTo(hdc, endX, endY);

    //Draw the 2 little fin arrow fin things, +- 20 degrees
    double spread = 25 * PI / 180;
    int finLength = 20;
    int finEndX = endX - std::cos(angle + spread) * finLength;
    int finEndY = endY - std::sin(angle + spread) * finLength;

    LineTo(hdc, finEndX, finEndY);
    MoveToEx(hdc, endX, endY, NULL);

    finEndX = endX - std::cos(angle - spread) * finLength;
    finEndY = endY - std::sin(angle - spread) * finLength;

    LineTo(hdc, finEndX, finEndY);
}