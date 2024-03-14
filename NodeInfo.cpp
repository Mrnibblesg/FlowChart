#include "NodeInfo.h"
#include "ErrorHandler.h"
#include "Classes.h"
#include "resource.h"
#include "windowsx.h"
#include "Node.h"

#define WM_UPDATESELECTION (WM_USER+0)

LRESULT CALLBACK NodeInfoProc(HWND, UINT, WPARAM, LPARAM);
void nodeInfoPaint(HWND, Node*);
void setSelected(Node*);

ATOM registerNodeInfo(HINSTANCE hInst) {
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = NodeInfoProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_FLOWCHART));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_FLOWCHART);
	wcex.lpszClassName = _T(NODEINFOCLASS);
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

LRESULT CALLBACK NodeInfoProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_UPDATESELECTION:
		//make it so this window redraws. Currently RedrawWindow doesn't seem to work.
	case WM_PAINT:
		nodeInfoPaint(hWnd, (Node*)lParam);
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void nodeInfoPaint(HWND hWnd, Node* n) {

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	HFONT font = CreateFont(30, 0, 0, 0, 400,
		FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH,
		TEXT("Calibri"));

	SelectFont(hdc, font);

	RECT area;
	GetClientRect(hWnd, &area);

	RECT textRect = area;
	int padding = 30;
	textRect.left += padding;
	textRect.right -= padding;
	textRect.top += padding;
	textRect.bottom -= padding;

	DWORD form = DT_LEFT | DT_NOCLIP | DT_WORDBREAK;

	if (n == nullptr) {
		textRect.top -= DrawTextEx(hdc, (TCHAR*) _T("None selected"), -1, &textRect, form, NULL);
	}
	else {
		textRect.top -= DrawTextEx(hdc, (TCHAR*)n->getName().c_str(), -1, &textRect, form, NULL);
	}

	EndPaint(hWnd, &ps);
}

HWND createNodeInfo(HINSTANCE hInst, HWND hParent) {
	HWND hNodeInfo = CreateWindowExW(0,
		_T(NODEINFOCLASS),
		L"Node Window",
		WS_CHILD | WS_BORDER,
		0, 0, 50, 100,
		hParent,
		(HMENU)(int)(ID_NODEINFO),
		hInst,
		NULL);
	if (hNodeInfo == NULL) {
		errorHandler((LPTSTR)_T("CreateWindowEx"));
	}
	return hNodeInfo;
}