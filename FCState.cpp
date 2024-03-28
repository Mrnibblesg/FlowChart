#include "FCState.h"
#include <vector>
#include "Node.h"

std::vector<Node*> FCState::nodes;
Node* FCState::selected = nullptr;
Node* FCState::connectBegin = nullptr;
bool FCState::lButtonDown = false;