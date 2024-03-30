#include "SaveLoad.h"
#include "framework.h"
#include "commdlg.h"
#include "FCState.h"
#include <iostream>
#include <fstream>

std::wstring str(LPTSTR, int);

OPENFILENAME& dialogSetup(HWND hWnd) {
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    const int maxSize = 256;
    wchar_t fileName[maxSize]{ L"" };

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = _T("FlowChart files (*.fc)\0*.fc\0");
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = (LPTSTR)fileName;
    ofn.nMaxFile = maxSize;
    return ofn;
}

//save to a predetermined file, without a prompt.
void save() {

}
//save to a new file, with a prompt.
void saveAs(HWND hWnd) {
    OPENFILENAME ofn = dialogSetup(hWnd);

    GetSaveFileName(&ofn);
    FCState::fileName = str(ofn.lpstrFile, ofn.nMaxFile);
    FCState::loaded = true;

    std::ofstream fcFile;
    fcFile.open(FCState::fileName);
    //how to encode the data? newlines allowed?

}
//load from a file.
void load(HWND hWnd) {
    OPENFILENAME ofn = dialogSetup(hWnd);
    GetOpenFileName(&ofn);

    //Read to FCState.
}

std::wstring str(LPTSTR arr, int len) {
    //read arr for len chars or until null?
    std::wstring result = L"";
    for (int i = 0; i < len; i++) {
        result += arr[i];
    }
    return result;
}