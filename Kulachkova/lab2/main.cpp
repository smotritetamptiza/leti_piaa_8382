#include <iostream>
#include <string>
#include <vector>

#define MAX_VERTICES 26

using namespace std;

//структура для хранения ребёр
struct Edge {
	int vertex; //индекс конца ребра в графе
	double weight; //длина ребра
	int heuristics; //значение эвристической функции для вершины на конце ребра
};

//компаратор для быстрой сортировки вершин по приоритету
int compare(const void * a, const void * b) {
	return (((Edge*)a)->heuristics - ((Edge*)b)->heuristics);
}

//структура для хранения вершин графа
struct Node {
	char name; //обозначение вершины
	int num; //число смежных вершин
	Edge * edges; //массив исходящих ребер
	string path; //кратчайший путь до вершины
	double pathLength; //длина кратчайшего пути до вершины
	bool processed; //была ли вершина обработана алгоритмом
	Node(char name = 0); //инициализация вершины
	~Node() { delete[] edges; }
	void addNeighboor(int ind, double weight, int h); //добавление исходящего ребра
};

//структура для хранения графа
struct Graph {
	int num; //число вершин
	Node vertices[MAX_VERTICES]; //массив вершин
	char start; //начало искомого пути
	char finish; //конец искомого пути
	Graph(char st, char fin); //инициализация графа
	int find(char name); //возвращает индекс искомой вершины в графе; если  вершины нет - возвращает -1
	void addVertex(char name); //добавление вершины в граф
	void addEdge(char from, char to, double weight); //добавление ребра в  граф
	void preprocessing(); //предобработка
	void search(); //поиск пути
	void alphSort();//сортировка вершин по алфавиту
	string path; //путь, находимый жадным алгоритмом
	void greedyAlgorithm(); //жадный алгоритм
	int h(char a) { return abs(a - finish); } //эвристическая оценка расстояния
	double f(int ind); //значение эвристической функции "расстояние + стоимость" для вершины с данным индексом
	void resetVertices(); //помечает все вершины как необработанные
	void print(); //вывод списка вершин на экран
};

//инициализация графа
Graph::Graph(char st, char fin) {
	start = st;
	finish = fin;
	path = "";
	num = 0;
	addVertex(start);
	addVertex(finish);
}

//добавление ребра в граф
void Graph::addEdge(char from, char to, double weight) {
	//находим концы ребра в массиве вершин
	int indFrom = find(from);
	int indTo = find(to);
	//добавляем начало ребра, если его нет в массиве
	if (indFrom == -1) {
		addVertex(from);
		indFrom = num - 1;
	}
	//добавляем конец ребра, если его нет в массиве
	if (indTo == -1) {
		addVertex(to);
		indTo = num - 1;
	}
	vertices[indFrom].addNeighboor(indTo, weight, h(to));
}

//добавление вершины в граф
void Graph::addVertex(char name) {
	if (find(name) >= 0) return;
	vertices[num] = Node(name);
	num++;
}

//возвращает индекс искомой вершины в графе; если  вершины нет - возвращает -1
int Graph::find(char name) {
	for (int i = 0; i < num; i++) {
		if (vertices[i].name == name) return i;
	}
	return -1;
}

//вывод списка вершин на экран
void Graph::print() {
	cout << "List and number of vertices: ";
	for (int i = 0; i < num; i++) {
		cout << vertices[i].name << " ";
	}
	cout << endl;
}

//предобработка графа (сортировка смежных вершин по приоритету)
void Graph::preprocessing() {
	for (int i = 0; i < num; i++) {
		qsort(vertices[i].edges, vertices[i].num, sizeof(Edge), compare);
	}
}

//отметка всех вершин как необработанных
void Graph::resetVertices() {
	for (int i = 0; i < num; i++) {
		vertices[i].processed = false;
	}
}

//эвристическая функция
double Graph::f(int ind) {
	return vertices[ind].pathLength + h(vertices[ind].name);
}

//жадный алгоритм поиска пути
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

//алгоритм поиска кратчайшего пути
void Graph::search() {
	int startInd = find(start);
	vector<int> toVisit; //индексы вершин, которые нужно посетить
	toVisit.push_back(startInd);
	vertices[startInd].pathLength = 0;
	vertices[startInd].path = string(1, start);
	while (!toVisit.empty()) {
		double minF = 10000;
		int curr = 0, currInd = 0;
		//находим среди вершин, которые нужно обработать, вершину с наименьшим значением
		//эвристической функции
		for (int i = 0; i < toVisit.size(); i++) {
			double fI = f(toVisit[i]);
			if (fI < minF) {
				minF = fI;
				curr = toVisit[i];
				currInd = i;
			}
		}
		//если нашли конечную вершину - выводим путь до неё и завершаем алгоритм
		if (vertices[curr].name == finish) {
			cout << vertices[curr].path << endl;
			return;
		}
		//удаляем текущую вершину из множества вершин, которые нужно рассмотреть
		toVisit.erase(toVisit.begin() + currInd);
		//отмечаем её как просмотренную
		vertices[curr].processed = true;
		//рассматриваем вершины, смежные с текущей
		for (int v = 0; v < vertices[curr].num; v++) {
			//вычисляем длину пути до рассматриваемой смежной вершины:
			//"путь до текущей" + "вес ребра между текущей и смежной"
			double tentScore = vertices[curr].pathLength + vertices[curr].edges[v].weight;
			//если вершина ещё не обработана или удалось улучшить длину пути до неё, сохраняем новую длину пути
			if (!vertices[vertices[curr].edges[v].vertex].processed || vertices[vertices[curr].edges[v].vertex].pathLength == -1
				|| tentScore < vertices[vertices[curr].edges[v].vertex].pathLength) {
				vertices[vertices[curr].edges[v].vertex].path = vertices[curr].path + vertices[vertices[curr].edges[v].vertex].name;
				vertices[vertices[curr].edges[v].vertex].pathLength = tentScore;
				//вывод пути до рассматриваемой вершины на экран
				cout << "Intermediate path: ";
				cout << vertices[vertices[curr].edges[v].vertex].path << endl;
				//добавляем смежные вершины в множество вершин, которые нужно рассмотреть
				toVisit.push_back(vertices[curr].edges[v].vertex);
				//удаляем последнее добавление, если вершина там уже была
				for (int i = 0; i < toVisit.size() - 1; i++) {
					if (toVisit[i] == vertices[curr].edges[v].vertex) {
						toVisit.pop_back();
						break;
					}
				}
			}
		}
	}
	//если алгоритм не завершился до этого момента, пути между вершинами нет
	cout << "There must be no path." << endl;
	return;
}

//инициализация вершины
Node::Node(char name) {
	this->name = name;
	num = 0;
	edges = nullptr;
	pathLength = -1;
	path = "";
	processed = false;
}

//добавление исходящего ребра
void Node::addNeighboor(int ind, double weight, int h) {
	for (int i = 0; i < num; i++) {
		//если добавляемое ребро уже существует, можно изменить его вес
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