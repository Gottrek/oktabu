#include "data.h"
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <fstream>

using namespace std;

Solution::Solution()
	: totalDistance(0.0),
	totalTime(0.0),
	found(true) {
}

void Solution::setNoSolution() {
	found = false;
	routes.clear();
	totalDistance = 0.0;
	totalTime = 0.0;
}

void Solution::addRoute(const Route& route) {
	routes.push_back(route);
	totalDistance += route.totalDistance;
	totalTime += route.totalTime;
}

void Solution::printToConsole() const {
	if (!found) {
		cout << "Brak rozwiazania." << endl;
		return;
	}
	cout << "Liczba tras: " << routes.size() << endl;
	cout << fixed << setprecision(2);
	for (size_t i = 0; i < routes.size(); ++i) {
		const Route& route = routes[i];
		cout << "Trasa " << i + 1 << ": ";
		for (size_t j = 0; j < route.path.size(); ++j) {
			cout << route.path[j];
			if (j < route.path.size() - 1) cout << " -> ";
		}
		cout << " | Dystans: " << route.totalDistance << " | Czas: " << route.totalTime << "| Zaladunek: " << route.currentLoad << endl;
	}
	cout << "Calkowity dystans: " << totalDistance << endl;
	cout << "Calkowity czas: " << totalTime << endl;
}

void Solution::saveToFile(const std::string& filename) const {
	std::ofstream file(filename);

	if (!file.is_open()) {
		std::cerr << "Blad: Nie udalo sie otworzyc pliku " << filename << " do zapisu.\n";
		return;
	}

	// 1. Jeœli brak rozwi¹zania -> wpisz -1
	if (!found) {
		file << "-1\n";
		file.close();
		return;
	}

	// 2. Pierwsza linia: liczba tras i dystans (5 miejsc po przecinku)
	file << routes.size() << " "
		<< std::fixed << std::setprecision(5) << totalTime << "\n";

	// 3. Kolejne linie: trasy
	for (const auto& route : routes) {
		// Pêtla od 1 do size-2 pomija Depot na pocz¹tku (indeks 0) i na koñcu (ostatni indeks)
		// Zak³adamy strukturê trasy w pamiêci: {0, klient, klient, 0}
		for (size_t i = 1; i < route.path.size() - 1; ++i) {
			file << route.path[i];

			// Dodajemy spacjê, jeœli to nie jest ostatni element w tej linii
			if (i < route.path.size() - 2) {
				file << " ";
			}
		}
		file << "\n"; // Nowa linia po ka¿dej trasie
	}

	file.close();
	std::cout << "Zapisano wynik do pliku: " << filename << "\n";
}

bool Instance::loadFromFile(const string& filename) {
	ifstream file(filename);
	if (!file.is_open()) {
		cerr << "Nie mozna otworzyc pliku: " << filename << endl;
		return false;
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
	return true;
}