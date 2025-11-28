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

const int MAX_N = 200;

struct Customer {
	int id;
	double x, y;
	int demand;
	int readyTime;
	int dueDate;
	int serviceTime;
};

class Instance {
private:
	vector<Customer> customers;
	double distanceMatrix[MAX_N][MAX_N]; // nie wiem czy może być prekompilowane max_n
	int vehicleCapacity;
public:
	double getDistance(int i, int j) const { // gettery
		return distanceMatrix[i][j];
	}
	const Customer& getCustomer(int id) const {
		return customers[id];
	}
	int getCapacity() const {
		return vehicleCapacity;
	}
	int getCustomerCount() const {
		return customers.size();
	}

	void loadFromFile(const string& filename) {
		ifstream file(filename);
		if (!file.is_open()) {
			cerr << "Nie mozna otworzyc pliku: " << filename << endl;
			return;
		}
		string smieci;
		file >> smieci;
		file >> smieci >> smieci >> smieci; // zjada VEHICLE NUMBER CAPACITY

		int k;
		file >> k >> vehicleCapacity;
		file >> smieci; // zjada CUSTOMER
		for (int i = 0; i < 11; ++i) {
			file >> smieci; // zjada naglowek
		}

		Customer c;
		while (file >> c.id >> c.x >> c.y >> c.demand >> c.readyTime >> c.dueDate >> c.serviceTime)
		{
			customers.push_back(c);
		}

		file.close();
	}
};

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