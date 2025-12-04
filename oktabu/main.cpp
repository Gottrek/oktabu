#include "data.h"
#include "Solomon.h"
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <random>
#include <chrono>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cerr << "Podaj nazwe pliku z instancja jako pierwszy argument i plik do zapisu rozwiazania jako drugi argument\n";
		return 1;
	}
	
	Instance instance;
	if (!instance.loadFromFile(argv[1])) {
		cerr << "Nie mozna otworzyc pliku: " << argv[1] << endl;
		return 1;
	}
	cout << "Wczytano instancje z pliku: " << argv[1] << " liczba klientow: " << instance.getCustomerCount() << endl;
	Solomon solver(instance);
	Solution solution = solver.run();
	
	solution.printToConsole();

	std::string outputFilename;
	if (argc < 3) { outputFilename = "rozwiazanie.txt"; }
	else { outputFilename = argv[2]; }
	solution.saveToFile(outputFilename);
}