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
void drawLines(HDC, std::wstring, RECT&);
void updateNodeFields(HWND, LONG);
std::wstring windowTextToStr(HWND);

static HWND editButton = nullptr;
const LONG editBtnId = 3;

static HWND completeButton = nullptr;
const LONG completeBtnId = 4;

static HWND deleteButton = nullptr;
const LONG deleteBtnId = 5;

static HWND editName = nullptr;
const LONG editNameId = 1;

static HWND editDesc = nullptr;
const LONG editDescId = 2;

static HWND markComplete = nullptr;
static bool editing = false;

static const HFONT FONT = CreateFont(30, 0, 0, 0, 400,
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
		editing = false;
		updateTextFields();
	case WM_PAINT:
		nodeInfoPaint(hWnd);
	break;
	case WM_COMMAND:
	{
		WORD notiCode = HIWORD(wParam);
		switch (notiCode) {
		case BN_CLICKED:
			switch ((int)LOWORD(wParam)) {
			case editBtnId:
				editing = !editing;
				Edit_SetText(editName, (LPTSTR)FCState::selected->getName().c_str());
				Edit_SetText(editDesc, (LPTSTR)FCState::selected->getDesc().c_str());
				RedrawWindow(hWnd, 0, 0, RDW_INVALIDATE);
				break;
			case completeBtnId: // Update node completion and also the visuals in the parent window
				if (FCState::selected != nullptr) {
					FCState::selected->toggleFulfilled();
					HWND hFlowChart = GetParent(hWnd);
					HWND leftInfo = GetWindow(hFlowChart, GW_HWNDLAST);
					RedrawWindow(hFlowChart, NULL, NULL, RDW_INVALIDATE);
					RedrawWindow(leftInfo, NULL, NULL, RDW_INVALIDATE);
				}
				break;
			case deleteBtnId:
				if (FCState::selected != nullptr) {
					FCState::deleteNode(FCState::selected);
					HWND hFlowChart = GetParent(hWnd);
					HWND leftInfo = GetWindow(hFlowChart, GW_HWNDLAST);
					FCState::selected = nullptr;
					InvalidateRect(hFlowChart, NULL, TRUE);
					RedrawWindow(leftInfo, NULL, NULL, RDW_INVALIDATE);
				}
			}
			break;
		case EN_CHANGE:
		{
			updateNodeFields(hWnd, LOWORD(wParam));
			break;
		}
		}
	}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void nodeInfoPaint(HWND hWnd) {
	InvalidateRect(hWnd, NULL, TRUE);

	Node* selected = FCState::selected;
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	SelectFont(hdc, FONT);

	RECT area;
	GetClientRect(hWnd, &area);

	//magic number which represents the padding inside an edit control box. Used for alignment
	const int inPad = 8;
	const int lineSize = 30;
	RECT textRect = area;

	//my own padding for inside the node info box
	int padding = 30;
	textRect.left += padding;
	textRect.right -= padding;
	textRect.top += padding;
	textRect.bottom -= padding;

	int textWidth = textRect.right - textRect.left;

	if (selected == nullptr) {
		ShowWindow(editButton, SW_HIDE);
		ShowWindow(completeButton, SW_HIDE);
		ShowWindow(deleteButton, SW_HIDE);
		ShowWindow(editName, SW_HIDE);
		ShowWindow(editDesc, SW_HIDE);
		Button_Enable(editButton, FALSE);
		Button_Enable(completeButton, FALSE);
		Button_Enable(deleteButton, FALSE);
		Edit_Enable(editName, FALSE);
		Edit_Enable(editDesc, FALSE);


		drawLines(hdc, L"None Selected", textRect);
		EndPaint(hWnd, &ps);
		return;
	}

	ShowWindow(completeButton, SW_SHOW);
	ShowWindow(deleteButton, SW_SHOW);
	ShowWindow(editButton, SW_SHOW);
	ShowWindow(editName, SW_SHOW);
	ShowWindow(editDesc, SW_SHOW);
	Button_Enable(editButton, TRUE);
	Button_Enable(completeButton, TRUE);
	Button_Enable(deleteButton, TRUE);

	int nameSpot = textRect.top;
	int descSpot;
	drawLines(hdc, L"Name:", textRect);

	if (editing){
		Edit_Enable(editName, TRUE);
		Edit_Enable(editDesc, TRUE);
		LONG style = GetWindowLong(editName, GWL_STYLE);
		style |= WS_BORDER;

		//SetWindowLong(editName, GWL_STYLE, style);
		//SetWindowLong(editDesc, GWL_STYLE, style);
	}
	else {
		Edit_Enable(editName, FALSE);
		Edit_Enable(editDesc, FALSE);
		LONG style = GetWindowLong(editName, GWL_STYLE);

		style &= ~WS_BORDER;

		//SetWindowLong(editName, GWL_STYLE, style);
		//SetWindowLong(editDesc, GWL_STYLE, style);
		
	}

	//Set props of the name edit control
	int boxHeight = lineSize * Edit_GetLineCount(editName);

	//set edit control size and adjust remaining space
	SetWindowPos(editName, NULL,
		textRect.left - inPad,
		textRect.top - 2,
		textWidth + (2 * inPad),
		boxHeight + (inPad),
		SWP_NOZORDER);

	textRect.top += boxHeight + lineSize;

	//Set up the description edit control.
	descSpot = textRect.top;
	drawLines(hdc, L"Description:", textRect);

	//setEditControlHeight
	boxHeight = lineSize * Edit_GetLineCount(editDesc);

	SetWindowPos(editDesc, NULL,
		textRect.left - inPad,
		textRect.top - 2,
		textWidth + (2 * inPad),
		boxHeight + (inPad),
		SWP_NOZORDER);

	//Go back and redraw these lines because edit controls inexplicably overwrite them sometimes
	int textReturnSpot = textRect.top;

	textRect.top = nameSpot;
	drawLines(hdc, L"Name:", textRect);

	textRect.top = descSpot;
	drawLines(hdc, L"Description:", textRect);

	textRect.top = textReturnSpot;
	textRect.top += boxHeight + lineSize;

	if (selected->getReqs().size() != 0) {
		drawLines(hdc, L"Requires:", textRect);
		for (Node* n : selected->getReqs()) {
			drawLines(hdc, n->getName(), textRect);
		}
	}
	if (selected->getFulfills().size() != 0) {
		drawLines(hdc, L"Fulfills:", textRect);
		for (Node* n : selected->getFulfills()) {
			drawLines(hdc, n->getName(), textRect);
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
	completeButton = CreateWindowEx(0,
		_T("BUTTON"),
		_T("Complete"),
		WS_TABSTOP | WS_CHILD | BS_DEFPUSHBUTTON,
		90, 5, 90, 25,
		hNodeInfo,
		NULL,
		hInst,
		NULL
	);
	deleteButton = CreateWindowEx(0,
		_T("BUTTON"),
		_T("Delete"),
		WS_TABSTOP | WS_CHILD | BS_DEFPUSHBUTTON,
		190, 5, 70, 25,
		hNodeInfo,
		NULL,
		hInst,
		NULL
	);
	
	DWORD textStyles = WS_TABSTOP | WS_CHILD | WS_BORDER |
		ES_WANTRETURN | ES_MULTILINE | ES_AUTOVSCROLL;

	editName = CreateWindowEx(0,
		_T("EDIT"),
		_T("Sample Name"),
		textStyles,
		30, 35, 200, 80,
		hNodeInfo,
		NULL,
		hInst,
		NULL
	);
	editDesc = CreateWindowEx(0,
		_T("EDIT"),
		_T("Sample Description"),
		textStyles,
		30, 125, 200, 30,
		hNodeInfo,
		NULL,
		hInst,
		NULL
	);
	SetWindowLong(editName, GWL_ID, editNameId);
	SetWindowLong(editDesc, GWL_ID, editDescId);
	SetWindowLong(editButton, GWL_ID, editBtnId);
	SetWindowLong(completeButton, GWL_ID, completeBtnId);
	SetWindowLong(deleteButton, GWL_ID, deleteBtnId);

	SendMessage(editName, WM_SETFONT, (WPARAM)FONT, NULL);
	SendMessage(editDesc, WM_SETFONT, (WPARAM)FONT, NULL);

	return hNodeInfo;
}

static void drawLines(HDC hdc, std::wstring text, RECT& textRect) {
	DWORD form = DT_LEFT | DT_NOCLIP | DT_WORDBREAK;
	textRect.top += DrawTextEx(hdc, (LPTSTR)text.c_str(), -1, &textRect, form, NULL);
}

//Get the text from edit controls by ID and put them inside the corresponding node field.
void updateNodeFields(HWND hWnd, LONG id){
	if (id == GetWindowLong(editName, GWL_ID)) {
		FCState::selected->setName(windowTextToStr(editName));
	}
	else if (id == GetWindowLong(editDesc, GWL_ID)) {
		FCState::selected->setDesc(windowTextToStr(editDesc));
	}
	else {
		MessageBoxW(NULL, L"updateNodeFields: unknown ID", L"Error", MB_YESNO);
		return;
	}
	
	RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
}

//sync the text boxes with the currently selected node
void updateTextFields() {
	if (FCState::selected == nullptr) return;
	Edit_SetText(editName, FCState::selected->getName().c_str());
	Edit_SetText(editDesc, FCState::selected->getDesc().c_str());
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