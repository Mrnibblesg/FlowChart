#include "NodeInfo.h"
#include "ErrorHandler.h"
#include "Classes.h"
#include "resource.h"
#include "windowsx.h"
#include "Node.h"
#include "FCState.h"
#include <cmath>
#include "ErrorHandler.h"

#define WM_UPDATESELECTION (WM_USER+0)

LRESULT CALLBACK NodeInfoProc(HWND, UINT, WPARAM, LPARAM);
void nodeInfoPaint(HWND);
void setSelected(Node*);

HWND editButton = nullptr;
HWND editName = nullptr;
HWND editDesc = nullptr;
HWND markComplete = nullptr;
bool editing = false;

const HFONT FONT = CreateFont(30, 0, 0, 0, 400,
	FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
	CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH,
	TEXT("Calibri"));

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
		RedrawWindow(hWnd, 0, 0, RDW_INVALIDATE);
		editing = false;
	case WM_PAINT:
		nodeInfoPaint(hWnd);
	break;
	case WM_COMMAND:
	{
		WORD notiCode = HIWORD(wParam);
		switch (notiCode) {
		case BN_CLICKED:
			
			RedrawWindow(hWnd, 0, 0, RDW_INVALIDATE);
			editing = !editing;
			//edit text boxes!
			break;
		}
	}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void nodeInfoPaint(HWND hWnd) {
	Node* selected = FCState::selected;
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	SelectFont(hdc, FONT);

	RECT area;
	GetClientRect(hWnd, &area);

	//magic number which represents the padding inside an edit control box
	const int inPad = 8;

	RECT textRect = area;

	//my own padding for inside the node info box
	int padding = 30;
	textRect.left += padding;
	textRect.right -= padding;
	textRect.top += padding;
	textRect.bottom -= padding;

	int textWidth = textRect.right - textRect.left;

	DWORD form = DT_LEFT | DT_NOCLIP | DT_WORDBREAK;

	if (selected == nullptr) {
		ShowWindow(editButton, SW_HIDE);
		ShowWindow(editName, SW_HIDE);
		ShowWindow(editDesc, SW_HIDE);
		Button_Enable(editButton, FALSE);
		Edit_Enable(editName, FALSE);
		Edit_Enable(editDesc, FALSE);

		textRect.top += DrawTextEx(hdc, (TCHAR*) _T("None selected"), -1, &textRect, form, NULL);
		EndPaint(hWnd, &ps);
		return;
	}

	int amt;
	if ((amt = DrawTextEx(hdc, (TCHAR*)_T("Name:"), -1, &textRect, form, NULL)) == 0) {
		errorHandler((LPTSTR)L"DrawTextEx");
	}
	textRect.top += amt;


	//to create the edit box properly, I need to know what text already exists

	if (editing){
		ShowWindow(editName, SW_SHOW);
		ShowWindow(editDesc, SW_SHOW);
		Edit_Enable(editName, TRUE);
		Edit_Enable(editDesc, TRUE);

		SIZE textSize;
		RECT rect;
		GetClientRect(hWnd, &rect);

		//Set the name edit control
		GetTextExtentPoint(hdc, (LPTSTR)selected->getName().c_str(), _tcslen(selected->getName().c_str()), &textSize);
		int lines = std::ceil((float)textSize.cx / textWidth);
		int boxHeight = lines * textSize.cy;

		SetWindowPos(editName, NULL, textRect.left-inPad, textRect.top, textWidth + (2*inPad), boxHeight, SWP_NOZORDER);
		Edit_SetText(editName, (LPTSTR)selected->getName().c_str());


		textRect.top += textSize.cy+textSize.cy;
		
		//Set up the description edit control
		textRect.top += DrawTextEx(hdc, (LPTSTR)_T("Description:"), -1, &textRect, form, NULL);
		GetTextExtentPoint(hdc, (LPTSTR)selected->getDesc().c_str(), _tcslen(selected->getDesc().c_str()), &textSize);
		lines = std::ceil((float)textSize.cx / textWidth);
		boxHeight = lines * textSize.cy;

		SetWindowPos(editDesc, NULL, textRect.left - inPad, textRect.top-2, textWidth + (2 * inPad), boxHeight + (2*inPad), SWP_NOZORDER);
		Edit_SetText(editDesc, (LPTSTR)selected->getDesc().c_str());
		textRect.top += textSize.cy;
		

	}
	else {
		ShowWindow(editButton, SW_SHOW);
		Button_Enable(editButton, TRUE);
		//I really gotta turn the text draw into a function this is ridiculous
		
		textRect.top += DrawTextEx(hdc, (TCHAR*)selected->getName().c_str(), -1, &textRect, form, NULL) + 30;
		textRect.top += DrawTextEx(hdc, (TCHAR*)_T("Description:"), -1, &textRect, form, NULL);
		textRect.top += DrawTextEx(hdc, (TCHAR*)selected->getDesc().c_str(), -1, &textRect, form, NULL) + 30;
		if (selected->getReqs().size() != 0) {
			textRect.top += DrawTextEx(hdc, (TCHAR*)_T("Requirements:"), -1, &textRect, form, NULL);
			for (Node* n : selected->getReqs()) {
				textRect.top += DrawTextEx(hdc, (TCHAR*)n->getName().c_str(), -1, &textRect, form, NULL);
			}
		}
		if (selected->getFulfills().size() != 0) {
			textRect.top += DrawTextEx(hdc, (TCHAR*)_T("Fulfills:"), -1, &textRect, form, NULL);
			for (Node* n : selected->getFulfills()) {
				textRect.top += DrawTextEx(hdc, (TCHAR*)n->getName().c_str(), -1, &textRect, form, NULL);
			}
		}
	}

	EndPaint(hWnd, &ps);
}

HWND createNodeInfo(HINSTANCE hInst, HWND hParent) {
	HWND hNodeInfo = CreateWindowEx(0,
		_T(NODEINFOCLASS),
		L"Node Window",
		WS_CHILD | WS_BORDER,
		0, 0, 50, 100,
		hParent,
		(HMENU)(int)(ID_NODEINFO),
		hInst,
		NULL
	);
	if (hNodeInfo == NULL) {
		errorHandler((LPTSTR)_T("CreateWindowEx"));
	}



	editButton = CreateWindowEx(0,
		_T("BUTTON"),
		_T("Edit"),
		WS_TABSTOP | WS_CHILD | BS_DEFPUSHBUTTON,
		30, 5, 50, 25,
		hNodeInfo,
		NULL,
		hInst,
		NULL
	);
	
	DWORD textStyles = WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER |
		ES_WANTRETURN | ES_MULTILINE | ES_AUTOVSCROLL;

	editName = CreateWindowEx(0,
		_T("EDIT"),
		_T("EditName"),
		textStyles,
		30, 35, 200, 80,
		hNodeInfo,
		NULL,
		hInst,
		NULL
	);
	editDesc = CreateWindowEx(0,
		_T("EDIT"),
		_T("EditDesc"),
		textStyles,
		30, 125, 200, 30,
		hNodeInfo,
		NULL,
		hInst,
		NULL
	);

	SendMessage(editName, WM_SETFONT, (WPARAM)FONT, NULL);
	SendMessage(editDesc, WM_SETFONT, (WPARAM)FONT, NULL);

	return hNodeInfo;
}