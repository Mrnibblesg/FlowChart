#include "Node.h"

int Node::count = 0;

Node::Node(): ID{ Node::count } {
	pos = {0,0};
	radius = 30;
	Node::count++;
}
Node::Node(POINT p): ID{ Node::count } {
	pos = p;
	radius = 30;
	name = L"Sample Name";
	desc = L"Sample Description";
	Node::count++;
}
Node::Node(std::wstring name, std::wstring desc, POINT p, int id, bool complete) :
	name{ name },
	desc{ desc },
	pos{ p },
	ID{ id },
	complete{ complete },
	radius{ 30 } {}

void Node::setCount(int amt) {
	count = amt + 1;
}

Node* Node::getNode(int id, std::vector<Node*> list) {
	for (Node* n : list) {
		if (n->getId() == id) {
			return n;
		}
	}
	return nullptr;
}
int Node::getId() const {
	return ID;
}

//Set to true when all reqs are fulfilled, otherwise don't
//Set to false anytime. Then notify fulfilling nodes to see if they need to change.
void Node::setFulfilled(bool f) {
	if (!f) {
		complete = false;
		for (Node* n : fulfills) {
			if (n->complete) {
				n->setFulfilled(false);
			}
		}
	}
	else if (areReqsFilled()) {
		complete = true;
	}
}
bool Node::getFulfilled() const {
	return complete;
}
void Node::toggleFulfilled() {
	setFulfilled(!complete);
}

bool Node::areReqsFilled() {
	for (Node* n : required) {
		if (!n->complete) {
			return false;
		}
	}
	return true;
}

void Node::addReq(Node* n) {
	int i = reqExists(n);
	if (i == -1) {
		required.push_back(n);
		n->addFulfill(this);
	}
	else {
		removeReq(n);
	}
}

void Node::removeReq(Node* n) {
	int i = reqExists(n);
	if (i != -1) {
		required.erase(required.begin() + i);
		n->removeFulfill(this);
	}
}

//Returns index if found, otherwise -1.
int Node::reqExists(Node* n) {
	for (int i = 0; i < required.size(); i++) {
		if (required.at(i) == n) {
			return i;
		}
	}
	return -1;
}

void Node::addFulfill(Node* n) {
	int i = fulfillExists(n);
	if (i == -1) {
		fulfills.push_back(n);
	}
}

void Node::removeFulfill(Node* n) {
	int i = fulfillExists(n);
	if (i != -1) {
		fulfills.erase(fulfills.begin() + i);
	}
}

int Node::fulfillExists(Node* n) {
	for (int i = 0; i < fulfills.size(); i++) {
		if (fulfills.at(i) == n) {
			return i;
		}
	}
	return -1;
}

const POINT& Node::getPos() const {
	return pos;
}

LPRECT Node::getBoundingRect() const {
	RECT bounds;
	bounds.left = pos.x - radius;
	bounds.right = pos.x + radius;
	bounds.top = pos.y - radius;
	bounds.bottom = pos.y + radius;
	return &bounds;
}

int Node::getRadius() const {
	return radius;
}

std::wstring Node::getName() const {
	return name;
}
std::wstring Node::getDesc() const {
	return desc;
}
 void Node::setName(std::wstring s) {
	 name = s;
}
 void Node::setDesc(std::wstring s) {
	 desc = s;
}

 const std::vector<Node*>& Node::getReqs() const {
	 return required;
 }
 const std::vector<Node*>& Node::getFulfills() const {
	 return fulfills;
 }

 void Node::setPos(int x, int y) {
	 pos.x = x;
	 pos.y = y;
 }