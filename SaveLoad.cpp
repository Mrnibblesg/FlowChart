#include "SaveLoad.h"
#include "framework.h"
#include "commdlg.h"
#include "FCState.h"
#include <iostream>
#include <fstream>
#include "Node.h"

void encodeFile(std::wstring);
void decodeFile(std::wstring);
template <typename T>
std::wstring encode(T);
std::wstring encode(std::wstring);
std::wstring _encode(std::wstring);

void toNextSection(std::wifstream&);
std::wstring decodeNextString(std::wifstream&);
int decodeNextNum(std::wifstream& ifs);

void setupDialog(HWND hWnd, OPENFILENAME& ofn) {
    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    const int maxSize = 256;
    wchar_t* fileName = new wchar_t[maxSize];

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = _T("FlowChart files(*.fc)\0 * .fc\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrFileTitle = NULL;
    ofn.lpstrFile = fileName;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = maxSize;
}

std::wstring openSaveSetup(HWND hWnd, OPENFILENAME& ofn) {
    setupDialog(hWnd, ofn);

    if (GetSaveFileName(&ofn) == 0) {
        MessageBox(NULL, L"Error opening file.", L"Error", MB_OK);
        return L"";
    }
    
    return ofn.lpstrFile;
}
std::wstring openLoadSetup(HWND hWnd, OPENFILENAME& ofn) {
    setupDialog(hWnd, ofn);

    if (GetOpenFileName(&ofn) == 0) {
        MessageBox(NULL, L"Error opening file.", L"Error", MB_OK);
        return L"";
    }
    std::wstring result = ofn.lpstrFile;
    return result;
}

//save to a predetermined file, without a prompt.
void save(HWND hWnd) {
    if (!FCState::fileSelected) {
        saveAs(hWnd);
        return;
    }
    
    encodeFile(FCState::fileName);
}
//save to a new file, with a prompt.
void saveAs(HWND hWnd) {
    OPENFILENAME ofn;
    std::wstring fName = openSaveSetup(hWnd, ofn);
    delete[] ofn.lpstrFile;

    if (fName.empty()) return;

    FCState::fileSelected = true;
    FCState::fileName = fName;
    encodeFile(fName);
}

void load(HWND hWnd) {
    OPENFILENAME ofn;
    std::wstring fName = openLoadSetup(hWnd, ofn);
    delete[] ofn.lpstrFile;

    if (fName.empty()) return;

    FCState::fileSelected = true;
    FCState::fileName = fName;
    decodeFile(fName);    
}

void encodeFile(std::wstring fName) {
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

        POINT p = n->getPos();
        fcFile << ", ";
        fcFile << p.x;
        fcFile << ", ";
        fcFile << p.y;

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

void decodeFile(std::wstring fName) {
    //Read to FCState.
    std::wifstream fcFile;
    fcFile.open(fName);

    //load from a file.
    //4. node connections

    //sections dictated by curly braces
    //strings dictated by quotes
    //other parts dictated by commas
    //read in title and description

    FCState::title = decodeNextString(fcFile);
    FCState::desc = decodeNextString(fcFile);
    FCState::nodes.clear();

    int maxId = 0;
    //Create nodes
    while (fcFile.good() && fcFile.peek() != '}') {


        std::wstring name;
        std::wstring desc;
        POINT p;
        int id;
        bool complete;

        name = decodeNextString(fcFile);
        desc = decodeNextString(fcFile);
        id = decodeNextNum(fcFile);
        maxId = max(id, maxId);

        complete = decodeNextNum(fcFile);
        p.x = decodeNextNum(fcFile);
        p.y = decodeNextNum(fcFile);
        FCState::nodes.push_back(new Node(name, desc, p, id, complete));
        
        //end of node brace
        fcFile.ignore();
        //go to either beginning of next node, or end of node list
        toNextSection(fcFile);
    }
    Node::setCount(maxId);

    //escape node list
    fcFile.ignore();
    //get to connection list list
    toNextSection(fcFile);
    //enter connection list list
    fcFile.ignore();
    //get to brace of first connection list
    toNextSection(fcFile);

    //read connections now
    int i = 0;
    char c = fcFile.peek();
    while (fcFile.good() && fcFile.peek() != '}') {
        //ignore open brace
        fcFile.ignore();
        
        while (fcFile.good()) {
            char c = fcFile.peek();
            //end of list
            if (fcFile.peek() == '}') {
                fcFile.ignore();
                break;
            }

            Node* current = FCState::nodes.at(i);
            int id = decodeNextNum(fcFile);
            Node* n = Node::getNode(id, FCState::nodes);
            current->addReq(n);
            
            //to next number or section.
            toNextSection(fcFile);
            c = fcFile.peek();
        }
        i++;
        //to next connection list, or the end.
        toNextSection(fcFile);
    }

    fcFile.close();
}

//keep ignoring until either {, }, or 0-9 are reached. Does not consume them.
void toNextSection(std::wifstream& ifs) {
    while (ifs.good()) {
        if (ifs.peek() != '{' && ifs.peek() != '}' &&
            (ifs.peek() < '0' || ifs.peek() > '9')) {
            ifs.ignore();
        }
        else {
            break;
        }
    }
}

std::wstring encode(std::wstring str) {
    return _encode(str);
}

template <typename T>
std::wstring encode(T str) {
    return _encode(std::to_wstring(str));
}
std::wstring _encode(std::wstring str) {
    std::wstring result = L"\"";
    for (wchar_t c : str) {
        switch (c) {
        case '\0':
            //break x2
            goto end;
        case '\\':
            result += L"\\\\";
            break;
        case '"':
        case ',':
        case '{':
        case '}':
        {
            result += L"\\";
            result += c;
        }
            
            break;
        default:
            result += c;
        }
    }
end:
    return result + L"\"";
}

//Extract from the stream until the next string is begun and completed.
std::wstring decodeNextString(std::wifstream& ifs) {
    std::wstring result = L"";
    //Find the beginning of the next string
    while (ifs.good()) {
        wchar_t c = ifs.get();
        if (c == '"') {
            break;
        }
    }

    while (ifs.good()) {
        wchar_t c = ifs.get();
        switch (c) {
        case '\\':
            if (ifs.good()){
                result += ifs.get();
            }
            break;
        case '"':
            //break x2
            goto end;
        default:
            result += c;
        }
    }
end:
    return result;
}

int decodeNextNum(std::wifstream& ifs) {
    int result = 0;

    //consume until we reach our number
    while (ifs.good() && (ifs.peek() < '0' ||
            ifs.peek() > '9')) {
        ifs.ignore();
    }
    
    while (ifs.good()) {
        wchar_t c = ifs.peek();
        if (c >= '0' && c <= '9') {
            result *= 10;
            result += c - '0';
            ifs.ignore();
        }
        else {
            break;
        }
    }
    return result;
}