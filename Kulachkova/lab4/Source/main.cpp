#include <iostream>
#include <vector>
#include <string>

using namespace std;

//префикс-функция
void prefixFunction(vector<int> &answ, string str, string patt) {
	cout << "Calculating preffix-function" << endl;
	vector<int> pi(patt.length());
	pi[0] = 0;
	cout << "\tPrefix(pattern[0]) = 0" << endl;
	//заполняем массив префиксов для образца
	for (int i = 1; i < patt.length(); i++) {
		int j = pi[i - 1]; //берем предыдущее значение и пытаемся его увеличить
		while ((j > 0) && (patt[j] != patt[i])) { //не выходит - 
			j = pi[j - 1]; //берем максимально возможное из ранее рассчитанных
		}
		if (patt[i] == patt[j]) {
			j++;
		}
		pi[i] = j;
		cout << "\tPrefix(pattern[" << i <<"]) = " << j << endl;
	}
	int j = 0; //число совпавших символов
	//оно же индекс сравниваемого элемента в образце
	for (int i = 0; i < str.length(); i++) {
		cout << "\tCurrent symbol in main string: " << str[i] << endl;
		while ((j > 0) && (patt[j] != str[i])) { //символ не совпал
			cout << "\tCurrent symbol in pattern: " << patt[j] << endl;
			j = pi[j - 1]; //сдвигаем образец
		}
		if (str[i] == patt[j]) { //символ совпал - увеличиваем длину совпавшего фрагмента
			cout << "\tCurrent symbol in pattern: " << patt[j] << endl;
			j++;				
		}
		if (j == patt.length()) {	//обошли весь образец
			cout << "\tFound a match" << endl;
			answ.push_back(i - patt.length() + 1); //добавили индекс его вхождения
		}
	}
}

//алгоритм КМП
vector<int> kmp(string pattern, string str) {
	vector<int> answ;
	if (str.length() < pattern.length()) {
		answ.push_back(-1);
		return answ;
	}
	//вычисляем префикс-функцию
	prefixFunction(answ, str, pattern);
	if (answ.empty()) {
		answ.push_back(-1);
	}
	return answ;
}

//определение циклического сдвига
int cycle(string strA, string strB) {
	if (strA.length() != strB.length()) {
		cout << "Different string lengths!" << endl;
		return -1;
	}
	if (strA == strB) {
		cout << "Strings are equal" << endl;
		return 0;
	}
	string tmp = strA + strA;
	cout << "Transforming \"" << strA << "\" into \"" << tmp << "\"" << endl;
	cout << "Starting KMP" << endl;
	vector<int> answ = kmp(strB, tmp);
	return answ[0];
}

int main() {
	string strA, strB;
	//ввод для КМП
	//cout << "Enter pattern, then enter text: " << endl;
	//ввод для определения циклического сдвига
	cout << "Enter string A and string B: " << endl;
	cin >> strA >> strB;
	
	//КМП
	/*vector<int> answ = kmp(strA, strB);
	cout << "Pattern positions within a main string: " << endl;
	for (int i = 0; i < answ.size(); i++) {
		if (i == answ.size() - 1) {
			cout << answ[i] << endl;
		}
		else {
			cout << answ[i] << ",";
		}
	}*/

	//определениe циклического сдвига
	int res = cycle(strA, strB);
	cout << "Starting position of string B within string A: ";
	cout << res << endl;
	return 0;
}