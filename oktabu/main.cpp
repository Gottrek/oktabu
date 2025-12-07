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
		cerr << "Daj mi plik wejsciowy\n";
		return 1;
	}

	string inputFilename = argv[1];
	string outputFilename = "rozwiazanie.txt";
	int tabuTenure = 10;
	int maxIterations = 1000;
	double alphaPenalty = 1.0;
	double betaPenalty = 1.0;

	for (int i = 2; i < argc; ++i) { // flagowe odczytywanki
		string arg = argv[i];
		if (arg == "-o" && i + 1 < argc) {
			outputFilename = argv[++i];
		}
		else if (arg == "-t" && i + 1 < argc) {
			tabuTenure = stoi(argv[++i]);
		}
		else if (arg == "-i" && i + 1 < argc) {
			maxIterations = stoi(argv[++i]);
		}
		else if (arg == "-a" && i + 1 < argc) {
			alphaPenalty = stod(argv[++i]);
		}
		else if (arg == "-b" && i + 1 < argc) {
			betaPenalty = stod(argv[++i]);
		}
		else {
			cerr << "Nieznany argument: " << arg << endl;
			return 1;
		}
	}
	
	Instance instance;
	if (!instance.loadFromFile(argv[1])) {
		cerr << "Nie mozna otworzyc pliku: " << argv[1] << endl;
		return 1;
	}
	cout << "Wczytano instancje z pliku: " << argv[1] << " liczba klientow: " << instance.getCustomerCount() << endl;

	auto start = chrono::high_resolution_clock::now();
	Solomon solver(instance);
	Solution solution = solver.run();
	auto end = chrono::high_resolution_clock::now();
	long long duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

	solution.saveToFile(outputFilename);
	cout << "STATS;"
		<< instance.getCustomerCount() << ";"
		<< solution.routes.size() << ";"
		<< fixed << setprecision(2) << solution.totalDistance << ";"
		<< fixed << setprecision(2) << solution.totalTime << ";"
		<< duration << endl;
	return 0;
}