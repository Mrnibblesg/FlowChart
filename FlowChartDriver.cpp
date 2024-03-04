// FlowChart.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "FlowChartDriver.h"
#include "FlowChart.h"
#include "LeftInfo.h"
#include "NodeInfo.h"

LRESULT CALLBACK    MainWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void registerClasses(HINSTANCE);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    registerClasses(hInstance);

    // Perform application initialization:
    if (!InitFlowChart(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FLOWCHART));
    
    MSG msg;
    BOOL msgRet;
    // Main message loop:
    while ((msgRet = GetMessage(&msg, nullptr, 0, 0)) != 0)
    {
        if (msgRet == -1) {
            //some error has occurred.
            break;
        }
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}
void registerClasses(HINSTANCE hInstance) {
    registerFlowChart(hInstance);
    registerLeftInfo(hInstance);
    registerNodeInfo(hInstance);
}
