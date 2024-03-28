#include "FCState.h"
#include <vector>
#include "Node.h"

std::vector<Node*> FCState::nodes;
Node* FCState::selected = nullptr;
Node* FCState::connectBegin = nullptr;
bool FCState::lButtonDown = false;

double FCState::completionPercent() {
	double total = 0;
	double complete = 0;
	for (Node* n : nodes) {
		total++;
		if (n->getFulfilled()) {
			complete++;
		}
	}
	if (total == 0) {
		return 0;
	}
	return complete / total * 100;
}