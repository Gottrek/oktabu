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
		cerr << "Podaj nazwe pliku jako argument\n";
		return 1;
	}

	
	Instance instance;
	instance.loadFromFile(argv[1]);
	cout << "Wczytano instancje z pliku: " << argv[1] << endl;
	cout << "Liczba klientow: " << instance.getCustomerCount() << endl;
	cout << "Pojemnosc pojazdu: " << instance.getCapacity() << endl;
	cout << "Odleglosc miedzy klientem 0 a 1: " <<  instance.getDistance(0, 1) << endl;
	Solomon solver(instance);
	Solution solution = solver.run();
	if (solution.routes.empty()) {
		cout << "Nie znaleziono rozwiazania.\n";
		return 0;
	}
	for (int i = 0; i < solution.routes.size(); ++i) {
		cout << "Trasa " << i + 1 << ": ";
		for (int customerID : solution.routes[i].path) {
			cout << customerID << " ";
		}
		cout << "| Dystans: "  << solution.routes[i].totalDistance
			<< " | Czas: " << solution.routes[i].totalTime << endl;
	}
}