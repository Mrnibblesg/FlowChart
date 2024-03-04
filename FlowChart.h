#pragma once
#include "framework.h"

ATOM registerFlowChart(HINSTANCE);
BOOL InitFlowChart(HINSTANCE, int);

LRESULT FlowChartProc(HWND, UINT, WPARAM, LPARAM);