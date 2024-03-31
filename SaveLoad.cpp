#include "SaveLoad.h"
#include "framework.h"
#include "commdlg.h"
#include "FCState.h"
#include <iostream>
#include <fstream>

std::wstring str(LPTSTR, int);
template <typename T>
std::wstring encode(T data);
std::wstring encode(std::wstring data);
std::wstring _encode(std::wstring str);

std::wstring dialogSetup(HWND hWnd, OPENFILENAME& ofn) {
    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    const int maxSize = 256;
    wchar_t fileName[maxSize];

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = _T("All Files (*.*)\0*.*\0FlowChart files (*.fc)\0*.fc\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrFileTitle = NULL;
    ofn.lpstrFile = fileName;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = maxSize;

    if (GetSaveFileName(&ofn) == 0) {
        MessageBox(NULL, L"Error opening file.", L"Error", MB_OK);
        return L"";
    }

    return ofn.lpstrFile;
}

//save to a predetermined file, without a prompt.
void save(HWND hWnd) {
    if (!FCState::fileSelected) {
        saveAs(hWnd);
        return;
    }
}
//save to a new file, with a prompt.
void saveAs(HWND hWnd) {
    OPENFILENAME ofn;
    std::wstring fName = dialogSetup(hWnd, ofn);
    if (fName.empty()) {
        return;
    }

    FCState::fileSelected = true;
    std::wofstream fcFile;
    fcFile.open(fName);

    fcFile << encode(FCState::title) << "\n";
    fcFile << encode(FCState::desc) << "\n";

    //Encode node information
    fcFile << "{" << "\n";
    for (Node* n : FCState::nodes) {
        
        fcFile << "\t{ ";
        fcFile << encode(n->getName());
        fcFile << ", ";
        fcFile << encode(n->getDesc());
        fcFile << ", ";
        fcFile << n->getId();
        fcFile << ", ";
        fcFile << n->getFulfilled();

        fcFile << "}" << "\n";
    }
    fcFile << "}" << "\n";

    //Encode node connections.
    fcFile << "{" << "\n";
    for (Node* n : FCState::nodes) {
        std::vector<Node*> reqs = n->getReqs();
        if (reqs.empty()) {
            fcFile << "\t{}\n";
            continue;
        }
        fcFile << "\t{";

        int i = 0;
        for (Node* req : reqs) {
            if (i != 0) fcFile << ", "; 
            fcFile << req->getId();
            i++;
        }
        fcFile << "}" << "\n";
    }
    fcFile << "}" << "\n";

    fcFile.close();
}


//load from a file.
void load(HWND hWnd) {

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

std::wstring encode(std::wstring str) {
    return _encode(str);
}

template <typename T>
std::wstring encode(T data) {
    return _encode(std::to_wstring(data));
}
std::wstring _encode(std::wstring str) {
    std::wstring result = L"\"";
    for (wchar_t c : str) {
        switch (c) {
        case '\0':
            break;
        case '"':
            result += L"\\\"";
            break;
        case '\\':
            result += L"\\\\";
            break;
        case ',':
        case '{':
        case '}':
            result += L"\\" + c;
            break;
        default:
            result += c;
        }

        if (c == '\0') {
            break;
        }
    }
    return result + L"\"";
}