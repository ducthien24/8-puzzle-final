#include<iostream>
#include <vector>
#include <queue>
#include<conio.h>
#include<stdio.h>
#include<map>
#include <cstdlib> // for rand() and srand()
#include <ctime> // for time()

using namespace std;



#define SIDE 3
enum Actions { UP, DOWN, LEFT, RIGHT };
int X[4] = { -1,1,0,0 };
int Y[4] = { 0,0,-1,1 };

typedef struct Node_array
{
	int state[SIDE][SIDE];
	Actions action;
	struct Node_array* parent; // tree search

	int path_cost;
	int depth;
	struct Node_array* nextNode; // list

} Node;

///

typedef struct NodeList {
	unsigned int nodeCount;    //the number of nodes in the list
	Node* head;            //pointer to the first node in the list
	Node* tail;            //pointer to the last node in the list
};

///
int n = 9;
vector<int> goalState = { 1, 2, 3, 4, 5, 6, 7, 8, 0 };
vector<int> start = { 1, 2, 3, 4, 5, 6, 7, 8, 0 };
map<vector<int>, bool> visit;
vector<int> moves;


int indexBefore;
int indexAfter;

bool check[9] = { false, false, false, false , false , false , false , false , false };
bool show[9] = { false, false, false, false , true , false , false , false , false };

struct node {
	vector<int> state;
	node* parent;

	node()
	{
		parent = NULL;
	}
};
/// <summary>
/// /
/// </summary>
/// <param name="a"></param>
void print_array(int a[][SIDE])
{
	int i, j;
	for (i = 0; i < SIDE; i++)
	{
		for (j = 0; j < SIDE; j++)
			printf("%2d  | ", a[i][j]);
		printf("\n");
		for (j = 0; j < SIDE; j++)
			printf("----|-");
		printf("\n");
	}
	printf("\n");
}

Node* newNode()
{
	Node* temp = new Node();
	temp->parent = temp->nextNode = NULL;

	for (int i = 0; i < SIDE; i++)
		for (int j = 0; j < SIDE; j++)
			temp->state[i][j] = -1;
	return temp;
}
int Heuristic(Node* root, Node* goal)
{
	int Xr[9] = {};
	int Yr[9] = {};
	int Xg[9] = {};
	int Yg[9] = {};

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			Xr[root->state[i][j]] = i; Yr[root->state[i][j]] = j;
			Xg[goal->state[i][j]] = i; Yg[goal->state[i][j]] = j;
		}
	int point = 0;
	for (int i = 1; i < 9; i++)
	{
		int Xpoint = (Xr[i] > Xg[i]) ? (Xr[i] - Xg[i]) : (-Xr[i] + Xg[i]);
		int Ypoint = (Yr[i] > Yg[i]) ? (Yr[i] - Yg[i]) : (-Yr[i] + Yg[i]);
		point += i * (Xpoint + Ypoint);
	}
	return point;
}

int Heuristic2(Node* root, Node* goal)
{
	bool test[9] = { false };
	int Xr[9] = {};
	int Yr[9] = {};
	int Xg[9] = {};
	int Yg[9] = {};

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			if (root->state[i][j] != -1)
			{
				Xr[root->state[i][j]] = i; Yr[root->state[i][j]] = j;
				Xg[goal->state[i][j]] = i; Yg[goal->state[i][j]] = j;
				test[root->state[i][j]] = true;
			}
		}
	int point = 0;
	for (int i = 0; i < 9; i++)
		if (test[i] == true)
		{
			int Xpoint = (Xr[i] > Xg[i]) ? (Xr[i] - Xg[i]) : (-Xr[i] + Xg[i]);
			int Ypoint = (Yr[i] > Yg[i]) ? (Yr[i] - Yg[i]) : (-Yr[i] + Yg[i]);
			point += i * (Xpoint + Ypoint) + 1;
		}
	return point;
}

void FIFO_add(NodeList* list, Node* node)
{
	if (list->head == NULL)
	{
		list->head = node;
		list->head->nextNode = NULL;
		list->nodeCount = 1;
	}
	else if (list->tail == NULL)
	{
		list->tail = node;
		list->head->nextNode = list->tail;
		list->tail->nextNode = NULL;
		list->nodeCount = 2;
	}
	else
	{
		list->nodeCount++;
		list->tail->nextNode = node;
		list->tail = node;
	}
}

Node* FIFO_pop(NodeList* list)
{
	if (list->head != NULL)
	{
		list->nodeCount--;
		Node* temp = list->head;
		list->head = list->head->nextNode;
		return temp;
	}
	return NULL;
}

bool is_equal(int a[][SIDE], int b[][SIDE])
{
	for (int i = 0; i < SIDE; i++)
		for (int j = 0; j < SIDE; j++)
			if (a[i][j] != b[i][j])
				return false;
	return true;
}

Actions reverseAction(Actions a)
{
	if (a == Actions::UP)
		return Actions::DOWN;
	if (a == Actions::DOWN)
		return Actions::UP;
	if (a == Actions::LEFT)
		return Actions::RIGHT;
	if (a == Actions::RIGHT)
		return Actions::LEFT;
}


void FIFO_insert(NodeList* list, Node* node)
{
	if (list->head == NULL)
	{
		list->head = node;
		list->head->nextNode = NULL;
		list->nodeCount = 1;
	}
	else if (list->tail == NULL)
	{
		list->tail = node;
		list->tail->nextNode = NULL;
		list->head->nextNode = list->tail;
		list->nodeCount = 2;
	}
	else
	{
		list->nodeCount++;
		if (list->head->path_cost > node->path_cost)
		{
			node->nextNode = list->head;
			list->head = node;
		}
		else
		{
			Node* run = list->head;
			while (run->nextNode != NULL && run->nextNode->path_cost < node->path_cost)
				run = run->nextNode;
			if (run->nextNode == NULL)
			{
				run->nextNode = node;
				list->tail = node;
				list->tail->nextNode = NULL;
			}
			else
			{
				node->nextNode = run->nextNode;
				run->nextNode = node;
			}
		}
	}
}

bool isLegalAction(Node* node, Actions action) {
	int index_i, index_j;
	for (int i = 0; i < SIDE; i++) {
		for (int j = 0; j < SIDE; j++) {
			if (node->state[i][j] == 0) {
				index_i = i;
				index_j = j;
				i = SIDE;
				break;
			}
		}
	}
	if (action == Actions::LEFT && index_j == 0)  return false;
	if (action == Actions::RIGHT && index_j == 2)  return false;

	switch (index_i)
	{
	case 0:
		if (action == Actions::UP) return false;
		return true;
	case 2:
		if (action == Actions::DOWN) return false;
		return true;
	default:
		return true;
	}
	return true;
}


Node* Child_node(Node* node, Actions action)
{
	if (isLegalAction(node, action) == false)
		return NULL;
	Node* childNode = (Node*)malloc(sizeof(Node));
	childNode->parent = node;
	childNode->action = action;
	childNode->depth = node->depth + 1;
	childNode->nextNode = NULL;

	int x0, y0;
	for (int i = 0; i < SIDE; i++)
		for (int j = 0; j < SIDE; j++)
		{
			childNode->state[i][j] = node->state[i][j];
			if (childNode->state[i][j] == 0)
			{
				x0 = i;
				y0 = j;
			}
		}

	int xNew = x0, yNew = y0;
	switch (action)
	{
	case Actions::UP: xNew--;
		break;
	case Actions::DOWN: xNew++;
		break;
	case Actions::LEFT: yNew--;
		break;
	default: yNew++;
	}

	childNode->path_cost = node->path_cost + childNode->state[xNew][yNew];

	childNode->state[x0][y0] = childNode->state[xNew][yNew];
	childNode->state[xNew][yNew] = 0;

	return childNode;
}

bool checkExist(NodeList* list, Node* node) {
	//
	Node* temp = list->head;
	while (temp != NULL)
	{
		if (is_equal(temp->state, node->state))
			return true;
		temp = temp->nextNode;
	}
	return false;
}

void showResult(Node* goal)
{
	while (goal != NULL)
	{
		Node* run = goal;
		while (run->parent != NULL && run->parent->parent != NULL)
		{
			run = run->parent;
		}
		if (run->parent != NULL)
		{
			printf("Next Step:\n");
			print_array(run->parent->state);
			delete run->parent;
			run->parent = NULL;
		}
		else
		{
			printf("Last Step:\n");
			print_array(run->state);
			delete run;
			run = NULL;
			printf("\n***********************************");
			return;
		}
	}
}

void Asearch(Node* root, Node*& goal)
{
	clock_t start = clock();
	NodeList* queue = new NodeList();
	NodeList* Checked = new NodeList();
	Checked->head = Checked->tail = queue->head = queue->tail = NULL;

	FIFO_add(queue, root);

	Node* temp = NULL;

	while ((double)(clock() - start) / CLOCKS_PER_SEC < 100)
	{
		temp = FIFO_pop(queue);

		if (is_equal(temp->state, goal->state) == true)
		{
			temp->path_cost += Heuristic(root, goal);
			printf("DA TIM RA , %d", temp->depth);
			delete goal;
			goal = temp;
			showResult(goal);
			return;
		}
		else
		{
			int tempHeuristic = Heuristic(temp, goal);
			Actions reverse = reverseAction(temp->action);
			for (int i = 0; i < 4; i++)
			{
				if (isLegalAction(temp, (Actions)i) && (Actions)i != reverse)
				{
					Node* child = Child_node(temp, (Actions)i);
					if (checkExist(Checked, child) == false)
					{
						child->path_cost = child->path_cost - tempHeuristic + Heuristic(child, goal);
						FIFO_insert(queue, child);
					}
				}
			}
		}
		FIFO_add(Checked, temp);
	}
}


// ----------------------------------------------------------



void printState(vector<int> state)
{
	for (int i = 0; i < 9; )
	{
		for (int j = 0; j < 3; ++j, ++i)
			cout << state[i] << " ";
		cout << endl;
	}
	cout << endl;
}
void ACTION(int x, int y)
{
	switch (x)
	{
	case 0:
	{
		if (y == 0)
			printf("No Change\n");
		if (y == 1)
			printf("Right\n");
		if (y == 3)
			printf("Down\n");
		break;
	}
	case 1:
	{
		if (y == 0)
			printf("Left\n");
		if (y == 2)
			printf("Right\n");
		if (y == 4)
			printf("Down\n");
		break;
	}
	case 2:
	{
		if (y == 1)
			printf("Left\n");
		if (y == 5)
			printf("Down\n");
		break;
	}
	case 3:
	{
		if (y == 0)
			printf("Up\n");
		if (y == 4)
			printf("Right\n");
		if (y == 6)
			printf("Down\n");
		break;
	}
	case 4:
	{
		if (y == 1)
			printf("Up\n");
		if (y == 7)
			printf("Down\n");
		if (y == 3)
			printf("Left\n");
		if (y == 5)
			printf("Right\n");
		break;
	}
	case 5:
	{
		if (y == 2)
			printf("Up\n");
		if (y == 4)
			printf("Left\n");
		if (y == 8)
			printf("Down\n");
		break;
	}
	case 6:
	{
		if (y == 3)
			printf("Up\n");
		if (y == 7)
			printf("Right\n");
		break;
	}
	case 7:
	{
		if (y == 4)
			printf("Up\n");
		if (y == 6)
			printf("Left\n");
		if (y == 8)
			printf("Right\n");
		break;
	}
	case 8:
	{
		if (y == 5)
			printf("Up\n");
		if (y == 7)
			printf("Left\n");
		break;
	}
	default:
		printf("X\n");
		break;
	}
}

int find0ofpath(vector<int> path) // tìm vị trí số 0 của trạng thái cha
{
	for (int i = 0; i < path.size(); i++)
	{
		if (path[i] == 0)
		{
			return i;
		}
	}

}


void printPath(node* state)
{
	vector<node*> path;
	while (state)
	{
		path.push_back(state);
		state = state->parent;
	}

	cout << "Moves Required: " << path.size() - 1 << endl;


	for (int i = path.size() - 1; i >= 0; --i) {
		if (i != 0) {
			int Be = find0ofpath(path[i]->state);
			int Af = find0ofpath(path[i - 1]->state);
			printState(path[i]->state);
			ACTION(Be, Af);
		}
		else {
			//printState(path[i]->state);
		}


	}
	printState(goalState);

}

node* createNewState(node* state, int x, int y)
{
	//x is current, y next step
	if (x == y + 1) {
		moves.push_back(4);
	}
	else if (x == y - 1) {
		moves.push_back(3);
	}
	else if (x == y + 3) {
		moves.push_back(2);
	}
	else if (x == y - 3) {
		moves.push_back(1);
	}
	node* new_state = new node();
	new_state->state = state->state;
	swap(new_state->state[x], new_state->state[y]);
	return new_state;
}

void BFS(node* start_state)
{

	visit[start_state->state] = 1;

	int pos, row, col;

	node* curr = new node(), * child = new node();
	queue<node*> q;
	q.push(start_state);

	while (!q.empty())
	{
		curr = q.front();
		q.pop();

		if (curr->state == goalState)
		{
			printPath(curr);
			return;
		}

		for (int i = 0; i < n; ++i)
		{
			if (curr->state[i] == 0)
			{
				pos = i;
				break;
			}
		}

		row = pos / 3;
		col = pos % 3;

		if (col != 0)
		{
			child = createNewState(curr, pos, pos - 1);

			if (visit[child->state] == 0)
			{
				visit[child->state] = 1;
				child->parent = curr;
				q.push(child);
			}
		}

		if (col != 2)
		{

			child = createNewState(curr, pos, pos + 1);

			if (visit[child->state] == 0)
			{
				visit[child->state] = 1;
				child->parent = curr;
				q.push(child);
			}

		}

		if (row != 0)
		{

			child = createNewState(curr, pos, pos - 3);

			if (visit[child->state] == 0)
			{
				visit[child->state] = 1;
				child->parent = curr;
				q.push(child);
			}

		}

		if (row != 2)
		{

			child = createNewState(curr, pos, pos + 3);

			if (visit[child->state] == 0)
			{
				visit[child->state] = 1;
				child->parent = curr;
				q.push(child);
			}

		}
	}
}

void printCurState(int startState[9]) {
	for (int i = 0; i < 9; )
	{
		for (int j = 0; j < 3; ++j, ++i)
		{
			if (show[i]) {
				cout << startState[i] << " ";
			}
			else {
				cout << "*" << " ";
			}
		}

		i -= 3;

		cout << "\t\t";

		for (int j = 0; j < 3; j++, ++i)
		{
			cout << startState[i] << " ";
		}

		cout << endl;
	}

	cout << endl;
	cout << endl;
}

int findPos0(int startState[9]) {
	for (int i = 0; i < 9; i++)
	{
		if (startState[i] == 0) {
			return i;
		}
	}
}

void printAction(int indexBe, int indexAf) {
	if (indexBe == indexAf) {
		cout << "No Changes" << endl;
	}
	else {
		int rowBef = indexBe / 3;
		int colBef = indexBe % 3;

		int rowAf = indexAf / 3;
		int colAf = indexAf % 3;

		if (rowBef < rowAf) {
			cout << "Down" << endl;
		}
		else if (rowBef > rowAf) {
			cout << "Up" << endl;
		}
		else if (colBef < colAf) {
			cout << "Right" << endl;
		}
		else {
			cout << "Left" << endl;
		}
	}
}

void swap(int startState[9], int index1, int index2) {
	int temp = startState[index1];
	startState[index1] = startState[index2];
	startState[index2] = temp;

	bool showT = show[index1];
	show[index1] = show[index2];
	show[index2] = showT;
}

void parentSwap(int startState[9], int index1, int index2, int index3, int index4) {
	int indexBef = findPos0(startState);
	swap(startState, index1, index2);
	int indexAf = findPos0(startState);
	printAction(indexBef, indexAf);
	printCurState(startState);

	indexBef = findPos0(startState);
	swap(startState, index1, index3);
	indexAf = findPos0(startState);
	printAction(indexBef, indexAf);
	printCurState(startState);

	indexBef = findPos0(startState);
	swap(startState, index3, index4);
	indexAf = findPos0(startState);
	printAction(indexBef, indexAf);
	printCurState(startState);

	indexBef = findPos0(startState);
	swap(startState, index4, index2);
	indexAf = findPos0(startState);
	printAction(indexBef, indexAf);
	printCurState(startState);

	indexBef = findPos0(startState);
	swap(startState, index2, index1);
	indexAf = findPos0(startState);
	printAction(indexBef, indexAf);
	show[4] = true;
	printCurState(startState);

	indexBef = findPos0(startState);
	swap(startState, index2, index1);
	indexAf = findPos0(startState);
	printAction(indexBef, indexAf);
	printCurState(startState);
}

void indexOf0Step2(int startState[9]) {

	parentSwap(startState, 1, 4, 2, 5);

	parentSwap(startState, 1, 4, 0, 3);

	parentSwap(startState, 7, 4, 6, 3);

	parentSwap(startState, 7, 4, 8, 5);

	cout << "____________Finish_____________" << endl;

	
}

void subAction(int action) {
	switch (action)
	{
	case 1:
		cout << "Up" << endl;
		break;
	case 2:
		cout << "Down" << endl;
		break;
	case 3:
		cout << "Left" << endl;
		break;
	case 4:
		cout << "Right" << endl;
		break;
	default:
		break;
	}
}

void indexOf0Step1(int startState[9]) {
	int index0 = findPos0(startState);
	printCurState(startState);

	if (index0 == 0 || index0 == 2) {
		swap(startState, index0, 1);
		if (index0 == 0) {
			subAction(4);
		}
		else {
			subAction(3);
		}
		printCurState(startState);
		swap(startState, 1, 4);
		subAction(2);
		show[4] = true;
		printCurState(startState);

		swap(startState, 3, 4);
		show[4] = true;
		subAction(3);
		printCurState(startState);

		swap(startState, 3, 4);
		subAction(4);
		printCurState(startState);

		swap(startState, 5, 4);
		show[4] = true;
		subAction(4);
		printCurState(startState);

		swap(startState, 5, 4);
		subAction(3);
		printCurState(startState);

		swap(startState, 7, 4);
		show[4] = true;
		subAction(2);
		printCurState(startState);

		swap(startState, 7, 4);
		subAction(1);
		printCurState(startState);
	}
	else if (index0 == 6 || index0 == 8) {
		swap(startState, index0, 7);
		if (index0 == 6) {
			subAction(4);
		}
		else {
			subAction(3);
		}
		printCurState(startState);
		swap(startState, 7, 4);
		subAction(1);
		show[4] = true;
		printCurState(startState);

		swap(startState, 1, 4);
		show[4] = true;
		subAction(1);
		printCurState(startState);

		swap(startState, 1, 4);
		subAction(2);
		printCurState(startState);


		swap(startState, 3, 4);
		show[4] = true;
		subAction(3);
		printCurState(startState);

		swap(startState, 3, 4);
		subAction(4);
		printCurState(startState);


		swap(startState, 5, 4);
		show[4] = true;
		subAction(4);
		printCurState(startState);

		swap(startState, 5, 4);
		subAction(3);
		printCurState(startState);

	}
	else if (index0 == 1 || index0 == 7 || index0 == 3 || index0 == 5) {
		swap(startState, index0, 4);
		if (index0 == 1) {
			subAction(2);
		}
		else if (index0 == 3) {
			subAction(4);
		}
		else if (index0 == 5) {
			subAction(3);
		}
		else {
			subAction(1);
		}
		show[4] = true;
		printCurState(startState);

		if (index0 != 1) {
			swap(startState, 1, 4);
			show[4] = true;
			subAction(1);
			printCurState(startState);

			swap(startState, 1, 4);
			subAction(2);
			printCurState(startState);
		}


		if (index0 != 3) {
			swap(startState, 3, 4);
			show[4] = true;
			subAction(3);
			printCurState(startState);

			swap(startState, 3, 4);
			subAction(4);
			printCurState(startState);
		}


		if (index0 != 5) {
			swap(startState, 5, 4);
			show[4] = true;
			subAction(4);
			printCurState(startState);

			swap(startState, 5, 4);
			subAction(3);
			printCurState(startState);
		}

		if (index0 != 7) {
			swap(startState, 7, 4);
			show[4] = true;
			subAction(2);
			printCurState(startState);

			swap(startState, 7, 4);
			subAction(1);
			printCurState(startState);
		}
	}
	else {

		if (index0 != 1) {
			swap(startState, 1, 4);
			show[4] = true;
			subAction(1);
			printCurState(startState);

			swap(startState, 1, 4);
			subAction(2);
			printCurState(startState);
		}


		if (index0 != 3) {
			swap(startState, 3, 4);
			subAction(3);
			show[4] = true;
			printCurState(startState);

			swap(startState, 3, 4);
			subAction(4);
			printCurState(startState);
		}


		if (index0 != 5) {
			swap(startState, 5, 4);
			show[4] = true;
			subAction(4);
			printCurState(startState);

			swap(startState, 5, 4);
			subAction(3);
			printCurState(startState);
		}

		if (index0 != 7) {
			swap(startState, 7, 4);
			show[4] = true;
			subAction(2);
			printCurState(startState);

			swap(startState, 7, 4);
			subAction(1);
			printCurState(startState);
		}
	}

	indexOf0Step2(startState);
}
void BFSS(Node* root, Node*& goal)
{
	clock_t start = clock();
	NodeList* queue = (NodeList*)malloc(sizeof(NodeList));
	NodeList* Checked = new NodeList();
	Checked->head = Checked->tail = queue->head = queue->tail = NULL;

	FIFO_add(queue, root);

	Node* temp = NULL;

	while ((double)(clock() - start) / CLOCKS_PER_SEC < 1000)
	{
		temp = FIFO_pop(queue);

		if (is_equal(temp->state, goal->state) == true)
		{
			printf("DA TIM RA , %d", temp->depth);
			delete goal;
			goal = temp;
			showResult(goal);
			return;
		}
		else
		{
			Actions reverse = reverseAction(temp->action);
			for (int i = 0; i < 4; i++)
			{
				if (isLegalAction(temp, (Actions)i) && (Actions)i != reverse)
				{
					Node* child = Child_node(temp, (Actions)i);
					FIFO_add(queue, child);
				}
			}
		}
	}
}


void nhapState(int a[][3], int b[9])
{
	int z = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			a[i][j] = b[z++];
		}
	}
}
void printf_array(int a[][3])
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			cout << a[i][j] << " ";
		}
		cout << "\n";
	}
}
int main()
{
	int goal[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 0 };
	Node* GOAL = newNode();
	Node* START = newNode();


	// Phase 1

	int startState[9] = { -1, -1, -1, -1, -1, -1, -1, -1, -1 };
	for (int i = 0; i < 9; i++)
	{
		srand(time(NULL));
		int r = 0 + rand() % (8 + 1 - 0);
		startState[i] = r;
		while (check[r]) {
			r = 0 + rand() % (8 + 1 - 0);
			startState[i] = r;
		}
		check[r] = true;
	}
	indexOf0Step1(startState);
	/*int x;
	node* startStatee = new node();

	for (int i = 0; i < 9; ++i)
	{
		startStatee->state.push_back(startState[i]);
	}

	BFS(startStatee);*/

	// Phase 2

	nhapState(START->state, startState);
	nhapState(GOAL->state, goal);
	clock_t start = clock();

	// do sth here

	
	//BFSS(START, GOAL);        // case 1: BFS
	Asearch(START, GOAL);      // Case 2: A*
	printf("Time: %.5fs\n", (double)(clock() - start) / CLOCKS_PER_SEC);
	//_getch();
	return 0;
}
