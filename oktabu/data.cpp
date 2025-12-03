#include "data.h"
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <fstream>

using namespace std;

void Instance::loadFromFile(const string& filename) {
	ifstream file(filename);
	if (!file.is_open()) {
		cerr << "Nie mozna otworzyc pliku: " << filename << endl;
		exit(1);
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

	distanceMatrix.resize(customers.size()); // Gemini powiedzia³ ¿e pomo¿e z indeksami

	for (size_t i = 0; i < customers.size(); ++i) {
		for (size_t j = 0; j < customers.size(); ++j) {
			double dx = customers[i].x - customers[j].x;
			double dy = customers[i].y - customers[j].y;
			distanceMatrix[i].push_back(sqrt(dx * dx + dy * dy));
		}
	}
}