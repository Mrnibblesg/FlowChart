#pragma once
#include <vector>
#include "Node.h"

class FCState {
public:
    static std::vector<Node*> nodes;
    static Node* selected;
    static Node* connectBegin;
    static bool lButtonDown;
    static POINT globalOffset;
    static double completionPercent();
};
