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

	for (int i = 1; i < argc; ++i) {
		Instance instance;
		instance.loadFromFile(argv[i]);
		cout << "Wczytano instancje z pliku: " << argv[i] << endl;
		cout << "Liczba klientow: " << instance.getCustomerCount() << endl;
		cout << "Pojemnosc pojazdu: " << instance.getCapacity() << endl;
		cout << "Odleglosc miedzy klientem 0 a 1: " << fixed << setprecision(2) << instance.getDistance(0, 1) << endl;
	}
}