#pragma once
#include <string>
#include <vector>
#include <windows.h>
class Node {
public:
	Node();
	Node(POINT);
	
	void setFulfilled(bool);
	bool getFulfilled() const; 
	void toggleFulfilled();

	bool areReqsFilled();
	void addReq(Node*);
	void removeReq(Node*);

	const POINT& getPos() const;
	int getRadius() const;
	LPRECT getBoundingRect() const;

	std::wstring getName() const;
	std::wstring getDesc() const;
	void setName(std::wstring);
	void setDesc(std::wstring);

	const std::vector<Node*>& getReqs() const;
	const std::vector<Node*>& getFulfills() const;

	void setPos(int, int);

	int getId() const; //for encoding
	static Node* getNode(int ID, std::vector<Node*> list); //for decoding
	

private:
	
	std::wstring name = L"";
	std::wstring desc = L"";
	const int ID;

	POINT pos;
	int radius;
	bool complete = false;
	std::vector<Node*> required; //Required to be fulfilled for this node to be completed
	std::vector<Node*> fulfills; //Nodes that this node fulfills
	
	//Client need only worry about making nodes req each other.
	void addFulfill(Node*);
	void removeFulfill(Node*);

	//prevents dupes
	int reqExists(Node*);
	int fulfillExists(Node*);
	static int count;
};