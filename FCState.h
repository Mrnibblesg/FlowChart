#pragma once
#include <vector>
#include <string>   
#include "Node.h"

class FCState {
public:
    static std::vector<Node*> nodes;
    static Node* selected;
    static Node* connectBegin;
    static bool lButtonDown;
    static POINT globalOffset;

    static bool loaded;
    static std::wstring fileName;

    static std::wstring title;
    static std::wstring desc;

    static double completionPercent();
    static void deleteNode(Node*);
    
};
