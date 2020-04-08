#include <iostream>

#define MAX_N 20

using namespace std;
int minK, varCounter;

struct Tiles {
	int x[MAX_N * MAX_N]; //координаты х квадратов разбиения
	int y[MAX_N * MAX_N]; //координаты у квадратов разбиения
	int w[MAX_N * MAX_N]; //длины сторон квадратов разбиения
	int n; //число квадратов в разбиении
	Tiles() { n = 0; }
	void printList(); //печать решения
	void addTile(int x, int y, int w); //добавление квадрата в решение
	void removeTile(int ind = -1); //удаление квадрата из решения
};


void Tiles::printList() {
	for (int i = 0; i < n; i++) {
		cout << y[i] + 1 << " " << x[i] + 1 << " " << w[i] << endl;
	}
}

void Tiles::addTile(int x, int y, int w) {
	this->x[n] = x;
	this->y[n] = y;
	this->w[n] = w;
	n++;
}

void Tiles::removeTile(int ind) {
	if (ind == -1) ind = n - 1;
	for (int i = ind; i < n - 1; i++) {
		x[i] = x[i + 1];
		y[i] = y[i + 1];
		w[i] = w[i + 1];
	}
	n--;
}

struct Rectangle {
	int width; //ширина поля
	int height; //высота поля
	int freeCells; //число пустых клеток
	int ** table; //матрица поля
	Rectangle(int width, int height);
	~Rectangle();
	bool isFull(); //проверка заполненности поля
	void print(); //вывод матрицы поля на экран
	void clear(Tiles tl); //удаление последнего квадрата с поля
	bool place(Tiles tl); //размещение последнего квадрата на поле
	void tryToFit(Tiles& curr, int tileSize, int x, int y); //поиск наибольшего квадрата, который можно разместить на поле
	bool removeTail(Tiles& curr); //удаление «хвоста» разбиения
	void covering(int tileSize, Tiles& curr, Tiles& best); //поиск минимального разбиения
};


Rectangle::Rectangle(int width, int height) : width(width), height(height) {
	freeCells = width * height;
	table = new int*[height];
	for (int i = 0; i < height; i++) {
		table[i] = new int[width];
		for (int j = 0; j < width; j++)
			table[i][j] = 0;
	}
}

Rectangle::~Rectangle() {
	for (int i = 0; i < height; i++) {
		delete[] table[i];
	}
	delete[] table;
}

bool Rectangle::isFull() {
	return (freeCells == 0);
}

void Rectangle::print() {
	streamsize w = cout.width();
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			cout.width(3);
			cout << table[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	cout.width(w);
}

void Rectangle::clear(Tiles tl) {
	for (int i = tl.y[tl.n - 1]; i < tl.y[tl.n - 1] + tl.w[tl.n - 1] && i < height; i++) {
		for (int j = tl.x[tl.n - 1]; j < tl.x[tl.n - 1] + tl.w[tl.n - 1] && j < width; j++) {
			if (table[i][j] == tl.n) {
				table[i][j] = 0;
				freeCells++;
			}
		}
	}
}

bool Rectangle::place(Tiles tl) {
	for (int i = tl.y[tl.n - 1]; i < tl.y[tl.n - 1] + tl.w[tl.n - 1]; i++) {
		for (int j = tl.x[tl.n - 1]; j < tl.x[tl.n - 1] + tl.w[tl.n - 1]; j++) {
			//если при размещении квадрата произошел выход за пределы поля или пересечение с другим квадратом,
			//часть квадрата, уже размещенная на поле, удаляется
			if (i >= height || j >= width || table[i][j] != 0) {
				clear(tl);
				tl.x[tl.n - 1] = j;
				return false;
			}
			table[i][j] = tl.n;
			freeCells--;
		}
	}
	return true;
}

void Rectangle::tryToFit(Tiles& curr, int tileSize, int x, int y) {
	//для каждой возможной длины стороны квадрата осуществляется попытка разместить его по заданным координатам
	for (int w = tileSize; w >= 1; w--) {
		if (freeCells < w*w) continue;
		curr.addTile(x, y, w);
		if (place(curr)) {
			return;
		}
		curr.removeTile();
	}
}

bool Rectangle::removeTail(Tiles& curr) {
	//удаляется конец массива, состоящий из единичных квадратов
	if (curr.w[curr.n - 1] == 1) {
		for (int k = curr.n - 1; k >= 0; k--) {
			if (curr.w[k] == 1) {
				clear(curr);
				curr.removeTile(k);
			}
			else {
				break;
			}
		}
	}

	/*if (width == height) {
		if (curr.n == 3) { return false; }
	}*/

	//если удалены все квадраты, возвращаем false, чтобы завершить алгоритм
	if (curr.n == 0) { return false; }
	//иначе последний квадрат перед единичными заменяется квадратом со стороной на 1 меньше
	clear(curr);
	curr.addTile(curr.x[curr.n - 1], curr.y[curr.n - 1], curr.w[curr.n - 1] - 1);
	curr.removeTile(curr.n - 2);
	place(curr);
	return true;
}

void Rectangle::covering(int tileSize, Tiles& curr, Tiles& best) {
	while (1) {
		//осуществляется обход матрицы поля
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				//добавление наибольшего возможного квадрата
				tryToFit(curr, tileSize, j, i);
				//print();
				//если число элементов в текущем неполном решении не меньше лучшего решения, необходимо удалить квадраты с поля
				if (curr.n >= minK && !isFull()) {
					//удаление "хвоста": при возвращении false алгоритм завершает работу
					if (!removeTail(curr)) return;
					//совершается прыжок по индексам правого верхнего угла последнего квадрата
					i = curr.y[curr.n - 1];
					j = curr.x[curr.n - 1] + curr.w[curr.n - 1] - 1;
				}
				else if (isFull()) { //поле заполнено
					//если текущее решение лучше лучшего, лучшее заменяется текущим
					if (curr.n < minK) {
						print();
						minK = curr.n;
						best = curr;
						varCounter = 1;
					}
					else if (curr.n == minK) {
						//если текущее решение равно лучшему, увеличивается счетчик минимальных разбиений
						//print();
						varCounter++;
					}
					//с поля удаляются последние квадраты, чтобы рассмотреть другие варианты разбиения
					if (!removeTail(curr)) return;
					//print();
					//прыжок (см. строчки 147-148)
					i = curr.y[curr.n - 1];
					j = curr.x[curr.n - 1] + curr.w[curr.n - 1] - 1;
				}
			}
		}
	}
}


int main() {
	int height = 0, width = 0;
	while (height < 2 || height > MAX_N || width < 2 || width > MAX_N){
		cout << "Enter the height and the width of a rectangle (2 <= N <= 20): ";
		cin >> height >> width;
	}

	/*while (width < 2 || width > MAX_N) {
		cin >> width;
	}
	height = width;*/

	Rectangle rec(width, height);
	minK = width * height; //наименьшее разбиение; первоначально указано наибольшее возможное число квадратов
	varCounter = 1; //счетчик вариантов минимального разбиения - для индивидуализации
	int maxTileSide; //наибольший возможный размер квадрата
	Tiles tiles; //массив для хранения информации о разбиении
	Tiles best;

	if (width == height) {
		if (width % 2 == 0) {
			minK = 4;
			for (int i = 0; i < width; i += width / 2) {
				for (int j = 0; j < width; j += width / 2) {
					best.addTile(j, i, width / 2);
					rec.place(best);
				}
			}
			rec.print();
		}
		else if (width % 3 == 0) {
			minK = 6;
			varCounter = 4;
			int bigSide = 2 * width / 3;
			best.addTile(0, 0, bigSide);
			rec.place(best);
			for (int i = 0; i < bigSide; i += width / 3) {
				best.addTile(i, bigSide, width / 3);
				rec.place(best);
				best.addTile(bigSide, i, width / 3);
				rec.place(best);
			}
			best.addTile(bigSide, bigSide, width / 3);
			rec.place(best);
			rec.print();
		}
		else if (width % 5 == 0) {
			minK = 8;
			varCounter = 32;
			int oneFifth = width / 5;
			best.addTile(0, 0, oneFifth * 3);
			rec.place(best);
			best.addTile(oneFifth * 3, 0, oneFifth * 2);
			rec.place(best);
			best.addTile(0, oneFifth * 3, oneFifth * 2);
			rec.place(best);
			best.addTile(oneFifth * 3, oneFifth * 3, oneFifth * 2);
			rec.place(best);
			best.addTile(oneFifth * 3, oneFifth * 2, oneFifth);
			rec.place(best);
			best.addTile(oneFifth * 4, oneFifth * 2, oneFifth);
			rec.place(best);
			best.addTile(oneFifth * 2, oneFifth * 3, oneFifth);
			rec.place(best);
			best.addTile(oneFifth * 2, oneFifth * 4, oneFifth);
			rec.place(best);
			rec.print();
		}
		else {

			/*tiles.addTile(0, 0, (width + 1) / 2);
			rec.place(tiles);
			tiles.addTile((width + 1) / 2, 0, (width - 1) / 2);
			rec.place(tiles);
			tiles.addTile(0, (width + 1) / 2, (width - 1) / 2);
			rec.place(tiles);
			maxTileSide = (width - 1)/ 2;*/

			maxTileSide = (width + 1) / 2;
			rec.covering(maxTileSide, tiles, best);
		}
	}
	else {
		maxTileSide = (width <= height ? width : height) - 1; //наибольший возможный размер квадрата
		rec.covering(maxTileSide, tiles, best);
	}
	cout << minK << endl;
	best.printList();
	cout << "Number of minimum fragmentation varieties: " << varCounter << endl;
	return 0;
}