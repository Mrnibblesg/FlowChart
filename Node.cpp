#include "Node.h"

Node::Node() {
	pos = {0,0};
	radius = 30;
}
Node::Node(POINT p) {
	pos = p;
	radius = 30;
}

//Set to true when all reqs are fulfilled, otherwise don't.

//Set to false anytime. Then notify fulfilling nodes to see if they need to change.
void Node::setFulfilled(bool f) {
	if (!f) {
		complete = false;
		//notify fulfilling
	}
	else if (areReqsFilled()) {
		complete = true;
	}
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
	}
}

void Node::removeReq(Node* n) {
	int i = reqExists(n);
	if (i != -1) {
		required.erase(required.begin() + i);
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

int Node::getRadius() const {
	return radius;
}

std::string Node::getName() const {
	return name;
}
std::string Node::getDesc() const {
	return desc;
}
 void Node::setName(std::string s) {
	 name = s;
}
 void Node::setDesc(std::string s) {
	 desc = s;
}