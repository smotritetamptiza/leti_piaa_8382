#include <iostream>
#include <string>

#define MAX_VERTICES 26

using namespace std;

struct Edge {		//ребро
	char end;		//конец ребра
	int endInd;		//индекс конца ребра
	int cap;		//остаточная пропускная способность ребра
	int factFlow;	//фактический поток в ребре
	Edge(char ver = 0, int w = 0);
};

Edge::Edge(char ver, int w) {
	end = ver;
	cap = w;
	endInd = -1;
	factFlow = 0;
}

struct Vertex {		//вершина
	char name;		//имя вершины
	int neighNum;	//количество смежных вершин
	bool visited;	//флаг просмотренности
	string path; //путь до вершины
	Edge edges[MAX_VERTICES];	//массив инцидентных ребер
	Vertex(char name = 0);
	void addNeighbour(char nName, int cap);	//добавляет нового соседа
	int findEdge(char end); //возвращает индекс ребра с заданным концом
	bool cycle(char ver); //проверяет, не создаст ли добавление новой вершины цикла
};

Vertex::Vertex(char name) {
	this->name = name;
	neighNum = 0;
	path = "";
	visited = false;
}

//добавление нового соседа
void Vertex::addNeighbour(char nName, int cap) {
	edges[neighNum] = Edge(nName, cap);
	neighNum++;
}

//поиск ребра с заданным концом
int Vertex::findEdge(char end) {
	for (int i = 0; i < neighNum; i++) {
		if (edges[i].end == end) return i;
	}
	return -1;
}

//проверка на цикл
bool Vertex::cycle(char ver) {
	if (path.find(ver) == string::npos) return false;
	return true;
}

struct Graph {		//граф
	int vnum;		//количество вершин в графе
	char source;	//исток
	char sink;		//сток
	int maxFlow;	//максимальный поток
	Vertex vertices[MAX_VERTICES];	//массив вершин
	Graph(char s, char t);
	void addEdge(char v1, char v2, int w);	//добавляет ребро в граф
	void addVertex(char name);				//добавляет вершину в граф
	int find(char verName);	//возвращает индекс вершины с заданным именем
	void sort();	//сортирвка вершин в графе
	int dfs(); //поиск пути в глубину
	int letItFlow(string path); //пускает поток по заданному пути
};

Graph::Graph(char s, char t) {
	vnum = 0;
	source = s;
	sink = t;
	maxFlow = 0;
}

//добавление ребра в граф
void Graph::addEdge(char v1, char v2, int w) {
	int ind1 = find(v1);
	if (ind1 < 0) {
		ind1 = vnum;
		addVertex(v1);
	}
	int ind2 = find(v2);
	if (ind2 < 0) {
		ind2 = vnum;
		addVertex(v2);
	}
	vertices[ind1].addNeighbour(v2, w);
}

//добавление вершины в граф
void Graph::addVertex(char name) {
	if (find(name) >= 0) return;
	vertices[vnum] = Vertex(name);
	vnum++;
}

//поиск вершины с заданным именем в массиве вершин графа
int Graph::find(char verName) {
	for (int i = 0; i < vnum; i++) {
		if (vertices[i].name == verName)
			return i;
	}
	return -1;
}

//компаратор для сортировки вершин графа в алфавитном порядке
int alphVerCmp(const void * a, const void * b) {
	return (((Vertex*)a)->name - ((Vertex*)b)->name);
}

//компаратор для сортировки ребер в алфавитном порядке
int alphEdgeCmp(const void * a, const void * b) {
	return (((Edge*)a)->end - ((Edge*)b)->end);
}

//сортировка вершин в графе
void Graph::sort() {
	qsort(vertices, vnum, sizeof(Vertex), alphVerCmp);
	for (int i = 0; i < vnum; i++) {
		qsort(vertices[i].edges, vertices[i].neighNum, sizeof(Edge), alphEdgeCmp);
		for (int j = 0; j < vertices[i].neighNum; j++) {
			vertices[i].edges[j].endInd = find(vertices[i].edges[j].end);
		}
	}
}

//поиск путей в глубину
int Graph::dfs() {
	int st[MAX_VERTICES];
	int ind = 0;
	st[ind] = find(source);
	vertices[find(source)].visited = true;
	vertices[find(source)].path = vertices[find(source)].name;
	while (ind >= 0) {
		int currVer = st[ind];
		ind--;
		if (vertices[currVer].name == sink) {
			return letItFlow(vertices[currVer].path); //если путь найден, пускаем по нему поток и возвращаем величину потока
		}
		for (int i = 0; i < vertices[currVer].neighNum; i++) {
			if ((!vertices[vertices[currVer].edges[i].endInd].visited || vertices[currVer].edges[i].cap > 0) &&
				!vertices[currVer].cycle(vertices[vertices[currVer].edges[i].endInd].name)) {
				vertices[vertices[currVer].edges[i].endInd].path = vertices[currVer].path + vertices[vertices[currVer].edges[i].endInd].name;
				st[++ind] = vertices[currVer].edges[i].endInd;
				vertices[vertices[currVer].edges[i].endInd].visited = true;
			}
		}
	}
	return -1;	//если путь не найден, возвращаем -1
}

//пуск потока по пути
int Graph::letItFlow(string path) {
	cout << "Found path: " << path << endl;
	int Cmin = 10000;
	cout << "\tCapacity of the edges: ";
	for (int i = 0; i < path.length() - 1; i++) {
		if (vertices[find(path[i])].findEdge(path[i + 1]) < 0) continue;
		cout << vertices[find(path[i])].edges[vertices[find(path[i])].findEdge(path[i + 1])].cap << " ";
		if (vertices[find(path[i])].edges[vertices[find(path[i])].findEdge(path[i + 1])].cap < Cmin) {
			Cmin = vertices[find(path[i])].edges[vertices[find(path[i])].findEdge(path[i + 1])].cap;
		}
	}
	cout << endl;
	cout << "\tFlow in the path: " << Cmin << endl;
	for (int i = 0; i < path.length() - 1; i++) {
		if (vertices[find(path[i])].findEdge(path[i + 1]) < 0) continue;
		vertices[find(path[i])].edges[vertices[find(path[i])].findEdge(path[i + 1])].cap -= Cmin;
		vertices[find(path[i])].edges[vertices[find(path[i])].findEdge(path[i + 1])].factFlow += Cmin;
	}
	return Cmin;
}

int main() {
	int n;
	char s, t;
	cout << "Enter the number of edges: ";
	cin >> n;
	cout << "Enter source vertex: ";
	cin >> s;
	cout << "Enter target vertex: ";
	cin >> t;
	Graph gr(s, t);
	char v1, v2;
	int w;
	cout << "Enter edges: " << endl;
	for (int i = 0; i < n; i++) {
		cin >> v1 >> v2 >> w;
		gr.addEdge(v1, v2, w);
	}
	gr.sort();	//сортировка вершин и ребер графа в алфавитном порядке, 
			    //чтобы потом было удобно их выводить
	int c = gr.dfs();	//находим путь, по которому пускаем поток
	while (c >= 0) {
		gr.maxFlow += c;
		c = gr.dfs();
	}
	cout << "Maximum flow: " << gr.maxFlow << endl;
	cout << "Flow through the edges: " << endl;
	for (int i = 0; i < gr.vnum; i++) {
		for (int j = 0; j < gr.vertices[i].neighNum; j++) {
			cout << gr.vertices[i].name << " " << gr.vertices[i].edges[j].end << " " << gr.vertices[i].edges[j].factFlow << endl;
		}
	}
	return 0;
}
