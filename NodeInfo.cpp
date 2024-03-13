#include "NodeInfo.h"
#include "ErrorHandler.h"
#include "Classes.h"
#include "resource.h"

LRESULT CALLBACK NodeInfoProc(HWND, UINT, WPARAM, LPARAM);

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

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void createNodeInfo(HINSTANCE hInst, HWND hParent) {
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
}