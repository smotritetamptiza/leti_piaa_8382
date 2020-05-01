#include <iostream>
#include <string>
#include <vector>

#define MAX_VERTICES 26

using namespace std;

//��������� ��� �������� ����
struct Edge {
	int vertex; //������ ����� ����� � �����
	double weight; //����� �����
	int heuristics; //�������� ������������� ������� ��� ������� �� ����� �����
};

//���������� ��� ������� ���������� ������ �� ����������
int compare(const void * a, const void * b) {
	return (((Edge*)a)->heuristics - ((Edge*)b)->heuristics);
}

//��������� ��� �������� ������ �����
struct Node {
	char name; //����������� �������
	int num; //����� ������� ������
	Edge * edges; //������ ��������� �����
	string path; //���������� ���� �� �������
	double pathLength; //����� ����������� ���� �� �������
	bool processed; //���� �� ������� ���������� ����������
	Node(char name = 0); //������������� �������
	~Node() { delete[] edges; }
	void addNeighboor(int ind, double weight, int h); //���������� ���������� �����
};

//��������� ��� �������� �����
struct Graph {
	int num; //����� ������
	Node vertices[MAX_VERTICES]; //������ ������
	char start; //������ �������� ����
	char finish; //����� �������� ����
	Graph(char st, char fin); //������������� �����
	int find(char name); //���������� ������ ������� ������� � �����; ����  ������� ��� - ���������� -1
	void addVertex(char name); //���������� ������� � ����
	void addEdge(char from, char to, double weight); //���������� ����� �  ����
	void preprocessing(); //�������������
	void search(); //����� ����
	void alphSort();//���������� ������ �� ��������
	string path; //����, ��������� ������ ����������
	void greedyAlgorithm(); //������ ��������
	int h(char a) { return abs(a - finish); } //������������� ������ ����������
	double f(int ind); //�������� ������������� ������� "���������� + ���������" ��� ������� � ������ ��������
	void resetVertices(); //�������� ��� ������� ��� ��������������
	void print(); //����� ������ ������ �� �����
};

//������������� �����
Graph::Graph(char st, char fin) {
	start = st;
	finish = fin;
	path = "";
	num = 0;
	addVertex(start);
	addVertex(finish);
}

//���������� ����� � ����
void Graph::addEdge(char from, char to, double weight) {
	//������� ����� ����� � ������� ������
	int indFrom = find(from);
	int indTo = find(to);
	//��������� ������ �����, ���� ��� ��� � �������
	if (indFrom == -1) {
		addVertex(from);
		indFrom = num - 1;
	}
	//��������� ����� �����, ���� ��� ��� � �������
	if (indTo == -1) {
		addVertex(to);
		indTo = num - 1;
	}
	vertices[indFrom].addNeighboor(indTo, weight, h(to));
}

//���������� ������� � ����
void Graph::addVertex(char name) {
	if (find(name) >= 0) return;
	vertices[num] = Node(name);
	num++;
}

//���������� ������ ������� ������� � �����; ����  ������� ��� - ���������� -1
int Graph::find(char name) {
	for (int i = 0; i < num; i++) {
		if (vertices[i].name == name) return i;
	}
	return -1;
}

//����� ������ ������ �� �����
void Graph::print() {
	cout << "List and number of vertices: ";
	for (int i = 0; i < num; i++) {
		cout << vertices[i].name << " ";
	}
	cout << endl;
}

//������������� ����� (���������� ������� ������ �� ����������)
void Graph::preprocessing() {
	for (int i = 0; i < num; i++) {
		qsort(vertices[i].edges, vertices[i].num, sizeof(Edge), compare);
	}
}

//������� ���� ������ ��� ��������������
void Graph::resetVertices() {
	for (int i = 0; i < num; i++) {
		vertices[i].processed = false;
	}
}

//������������� �������
double Graph::f(int ind) {
	return vertices[ind].pathLength + h(vertices[ind].name);
}

//������ �������� ������ ����
void Graph::greedyAlgorithm() {
	int startInd = find(start);
	vertices[startInd].processed = true;
	int processed = 1;
	path = (1, start);
	while (processed < num) {
		int lastVisited = find(path.back());
		double minWeight = 10000;
		int minWeightInd = -1;
		for (int i = 0; i < vertices[lastVisited].num; i++) {
			if (!vertices[vertices[lastVisited].edges[i].vertex].processed &&
				(minWeight > vertices[lastVisited].edges[i].weight ||
				(minWeight == vertices[lastVisited].edges[i].weight &&
					vertices[vertices[lastVisited].edges[i].vertex].name < vertices[minWeightInd].name))) {
				minWeight = vertices[lastVisited].edges[i].weight;
				minWeightInd = vertices[lastVisited].edges[i].vertex;
			}
		}
		if (minWeightInd == -1) {
			path.erase(path.length() - 1);
			continue;
		}
		vertices[minWeightInd].processed = true;
		path += vertices[minWeightInd].name;
		cout << "Intermediate path: " << path << endl;
		if (vertices[minWeightInd].name == finish) {
			cout << path << endl;
			return;
		}
		processed++;
	}

	cout << "There must be no path." << endl;
	return;
}

//�������� ������ ����������� ����
void Graph::search() {
	int startInd = find(start);
	vector<int> toVisit; //������� ������, ������� ����� ��������
	toVisit.push_back(startInd);
	vertices[startInd].pathLength = 0;
	vertices[startInd].path = string(1, start);
	while (!toVisit.empty()) {
		double minF = 10000;
		int curr = 0, currInd = 0;
		//������� ����� ������, ������� ����� ����������, ������� � ���������� ���������
		//������������� �������
		for (int i = 0; i < toVisit.size(); i++) {
			double fI = f(toVisit[i]);
			if (fI < minF) {
				minF = fI;
				curr = toVisit[i];
				currInd = i;
			}
		}
		//���� ����� �������� ������� - ������� ���� �� �� � ��������� ��������
		if (vertices[curr].name == finish) {
			cout << vertices[curr].path << endl;
			return;
		}
		//������� ������� ������� �� ��������� ������, ������� ����� �����������
		toVisit.erase(toVisit.begin() + currInd);
		//�������� � ��� �������������
		vertices[curr].processed = true;
		//������������� �������, ������� � �������
		for (int v = 0; v < vertices[curr].num; v++) {
			//��������� ����� ���� �� ��������������� ������� �������:
			//"���� �� �������" + "��� ����� ����� ������� � �������"
			double tentScore = vertices[curr].pathLength + vertices[curr].edges[v].weight;
			//���� ������� ��� �� ���������� ��� ������� �������� ����� ���� �� ��, ��������� ����� ����� ����
			if (!vertices[vertices[curr].edges[v].vertex].processed || vertices[vertices[curr].edges[v].vertex].pathLength == -1
				|| tentScore < vertices[vertices[curr].edges[v].vertex].pathLength) {
				vertices[vertices[curr].edges[v].vertex].path = vertices[curr].path + vertices[vertices[curr].edges[v].vertex].name;
				vertices[vertices[curr].edges[v].vertex].pathLength = tentScore;
				//����� ���� �� ��������������� ������� �� �����
				cout << "Intermediate path: ";
				cout << vertices[vertices[curr].edges[v].vertex].path << endl;
				//��������� ������� ������� � ��������� ������, ������� ����� �����������
				toVisit.push_back(vertices[curr].edges[v].vertex);
				//������� ��������� ����������, ���� ������� ��� ��� ����
				for (int i = 0; i < toVisit.size() - 1; i++) {
					if (toVisit[i] == vertices[curr].edges[v].vertex) {
						toVisit.pop_back();
						break;
					}
				}
			}
		}
	}
	//���� �������� �� ���������� �� ����� �������, ���� ����� ��������� ���
	cout << "There must be no path." << endl;
	return;
}

//������������� �������
Node::Node(char name) {
	this->name = name;
	num = 0;
	edges = nullptr;
	pathLength = -1;
	path = "";
	processed = false;
}

//���������� ���������� �����
void Node::addNeighboor(int ind, double weight, int h) {
	for (int i = 0; i < num; i++) {
		//���� ����������� ����� ��� ����������, ����� �������� ��� ���
		if (edges[i].vertex == ind) {
			cout << "This edge already exists. Do you want to rewrite it? y/n" << endl;
			char answ;
			cin >> answ;
			if (answ == 'y' || answ == 'Y') {
				edges[i].weight = weight;
				return;
			}
			else if (answ != 'n' && answ != 'N') {
				cout << "I'll take it as a \"No\"" << endl;
			}
			return;
		}
	}
	Edge * tmp = new Edge[num + 1];
	for (int i = 0; i < num; i++) {
		tmp[i].vertex = edges[i].vertex;
		tmp[i].weight = edges[i].weight;
		tmp[i].heuristics = edges[i].heuristics;
	}
	tmp[num].vertex = ind;
	tmp[num].weight = weight;
	tmp[num].heuristics = h;
	delete[] edges;
	edges = tmp;
	num++;
}


int main() {
	char start, finish;
	cout << "Enter a starting and a finishing point: " << endl;
	cin >> start >> finish;
	Graph * graph = new Graph(start, finish);
	char from, to, waste;
	double weight;
	int input = 0;
	cout << "Now enter a sequence of edges with weights: " << endl;
	while (cin >> from >> to >> weight) {
		graph->addEdge(from, to, weight);
	}
	graph->print();
	cout << "Greedy algorithm: " << endl;
	graph->greedyAlgorithm();
	graph->resetVertices();
	graph->preprocessing();
	cout << "A* algorithm: " << endl;
	graph->search();
	delete graph;
	return 0;
}