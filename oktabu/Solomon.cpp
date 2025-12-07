#include "data.h"
#include "Solomon.h"
#include <vector>
#include <algorithm>
#include <cmath>

bool Solomon::canInsert(const Route& route, int customerID, int position) {
	int demand = data.getCustomer(customerID).demand;
	if (route.currentLoad + demand > data.getCapacity()) return false;

	std::vector<int> tempPath = route.path;
	tempPath.insert(tempPath.begin() + position, customerID);

	double currentTime = 0.0;
	for (int i = 1; i < tempPath.size(); ++i) {
		int prevNode = tempPath[i - 1];
		int currNode = tempPath[i];

		const Customer& cust = data.getCustomer(currNode);

		double travelTime = data.getDistance(prevNode, currNode);
		double arrivalTime = currentTime + travelTime;

		if (arrivalTime > cust.dueDate) return false;
		
		double startServiceTime = std::max(arrivalTime, (double)cust.readyTime);
		currentTime = startServiceTime + cust.serviceTime;
	}
	return true;
}

double Solomon::calculateScore(const Route& currentRoute, int customerID, int position) {
	std::vector<int> tempPath = currentRoute.path;
	tempPath.insert(tempPath.begin() + position, customerID);

	if (!canInsert(currentRoute, customerID, position)) {
		return -INFINITY;
	}

	double currentTime = 0.0;
	double arrivalTime = 0.0;
	double currentDistance = 0.0;
	double finishServiceTime = 0.0;

	for (int i = 1; i < tempPath.size(); ++i) {
		int prevNode = tempPath[i - 1];
		int currNode = tempPath[i];

		const Customer& cust = data.getCustomer(currNode);

		currentDistance += data.getDistance(prevNode, currNode);
		double travelTime = data.getDistance(prevNode, currNode);
		arrivalTime = currentTime + travelTime;
		double startServiceTime = std::max(arrivalTime, (double)cust.readyTime);
		finishServiceTime = startServiceTime + cust.serviceTime;
		currentTime = finishServiceTime;
	}
	double timeDiff = finishServiceTime - currentRoute.totalTime;
	double distanceDiff = currentDistance - currentRoute.totalDistance;
	double score = data.getDistance(0, customerID) - (timeDiff + distanceDiff); // wzor na c2
	return score;
}

Solution Solomon::run() {
	Solution solution;
	solution.totalDistance = 0.0;
	Customer depot = data.getCustomer(0);

	for (int i = 1; i < data.getCustomerCount(); ++i) {
		isVisited[i] = false;
		Customer cust = data.getCustomer(i);
		bool impossible = false;

		if (cust.demand > data.getCapacity()) impossible = true;
		if (cust.dueDate < data.getDistance(0, i)) impossible = true;

		double arrivalTime = data.getDistance(0, i);
		double startServiceTime = std::max(arrivalTime, (double)cust.readyTime);
		double returnTime = startServiceTime + cust.serviceTime + data.getDistance(i, 0);
		if (arrivalTime > cust.dueDate) impossible = true;
		if (returnTime > depot.dueDate) impossible = true;
		if (impossible) {
			solution.setNoSolution();
			return solution;
		}
	}
	int visitedCount = 1; // uwaga
	while (visitedCount < data.getCustomerCount()) {
		Route route;
		route.path = { 0, 0 }; // depot na start i koniec
		route.currentLoad = 0;
		route.totalDistance = 0.0;
		route.totalTime = 0.0;

		bool clientAdded = true;
		bool isRouteEmpty = true;
		double bestScore = -INFINITY;
		int bestCustomer = -1;
		int bestPosition = -1;

		while (clientAdded) { // dopoki moge dodawac klientow do trasy
			clientAdded = false;
			bestScore = -INFINITY;
			bestCustomer = -1;
			bestPosition = -1;
			for (int customerID = 1; customerID < data.getCustomerCount(); ++customerID) { // dla kazdego klienta
				if (isVisited[customerID]) continue;

				for (int position = 1; position < route.path.size(); ++position) { // sprawdz kazda pozycje w trasie
					double score = calculateScore(route, customerID, position);
					if (score == -INFINITY) continue;
					if (score > bestScore) {
						bestScore = score;
						bestCustomer = customerID;
						bestPosition = position;
					}
				}
			}
			if (bestCustomer != -1 && bestPosition != -1) { // jesli znalazlem klienta do dodania
				route.path.insert(route.path.begin() + bestPosition, bestCustomer);
				route.currentLoad += data.getCustomer(bestCustomer).demand;
				isVisited[bestCustomer] = true;
				visitedCount++;
				clientAdded = true;
				isRouteEmpty = false;
				// aktualizuj dystans i czas trasy
				route.totalDistance = 0.0;
				double currentTime = 0.0;
				// route.totalTime = 0.0;
				for (int i = 1; i < route.path.size(); ++i) {
					int prevNode = route.path[i - 1];
					int currNode = route.path[i];
					const Customer& cust = data.getCustomer(currNode);

					route.totalDistance += data.getDistance(prevNode, currNode);

					double travelTime = data.getDistance(prevNode, currNode);
					double arrivalTime = currentTime + travelTime;
					double startServiceTime = std::max(arrivalTime, (double)cust.readyTime);
					currentTime = startServiceTime + cust.serviceTime;
				}
				route.totalTime = currentTime;
			}
			else {
				clientAdded = false;
			}
		}
		if (isRouteEmpty) {
			solution.setNoSolution();
			return solution;
		}
		solution.addRoute(route);
	}
	return solution;
}

Solution Solomon::reduceVehicles(const Solution& initialSolution) {
		Solution bestSolution = initialSolution;
		Solution tempSolution = initialSolution;
		std::vector<bool> checkedRoutes(initialSolution.routes.size(), false);
		while (true) {
			tempSolution = initialSolution;
			int smallestRouteLP = -1;
			int smallestLoad = INFINITY;
			for (int i = 0; i < initialSolution.routes.size(); ++i) {
				if (initialSolution.routes[i].currentLoad < smallestLoad && checkedRoutes[i] == false) {
					smallestLoad = initialSolution.routes[i].currentLoad;
					smallestRouteLP = i;
				}
			}
			if (smallestRouteLP == -1) return bestSolution;

			tempSolution.routes.erase(tempSolution.routes.begin() + smallestRouteLP);
			std::vector<int> lonelyCustomers = initialSolution.routes[smallestRouteLP].path;
			lonelyCustomers.erase(lonelyCustomers.begin()); // usun depot na poczatku
			lonelyCustomers.pop_back(); // i na koncu
			
			bool everyClientAdded = true;

			for (int i = 0; i < lonelyCustomers.size(); ++i) { // dla kazdego klienta do upchniecia
				bool clientAdded = false;
				double bestScore = -INFINITY;
				int bestPosition = -1;
				int bestRouteLP = -1;
				for (int j = 0; j < tempSolution.routes.size(); ++j) { // dla kazdej trasy w rozwiazaniu tymczasowym
					if (tempSolution.routes[j].currentLoad + data.getCustomer(lonelyCustomers[i]).demand > data.getCapacity()) continue;
					bestPosition = -1;
					for (int k = 1; k < tempSolution.routes[j].path.size(); ++k) { // dla kazdej pozycji w trasie
						double score = calculateScore(tempSolution.routes[j], lonelyCustomers[i], k);
						std::cout << "Trasa " << j << " Poz " << k << " - Brak miejsca/czasu\n";
						if (score == -INFINITY) std::cout << "wyrzucilem\n";
						if (score == -INFINITY) continue;
						if (score > bestScore) {
							clientAdded = true;
							bestScore = score;
							bestRouteLP = j;
							bestPosition = k;
						}
					}
				}
				if (!clientAdded) { // nie udalo sie dodac klienta
					checkedRoutes[smallestRouteLP] = true;
					everyClientAdded = false;
					break;
				}
				if (bestRouteLP < 0) return bestSolution; // bezpieczenstwo, moze sie da usunac
				Route najRoute = tempSolution.routes[bestRouteLP];
				najRoute.path.insert(najRoute.path.begin() + bestPosition, lonelyCustomers[i]);
				najRoute.currentLoad += data.getCustomer(lonelyCustomers[i]).demand;
				clientAdded = true;
				// aktualizuj dystans i czas trasy
				najRoute.totalDistance = 0.0;
				double currentTime = 0.0;
				for (int m = 1; m < najRoute.path.size(); ++m) {
					int prevNode = najRoute.path[m - 1];
					int currNode = najRoute.path[m];
					const Customer& cust = data.getCustomer(currNode);

					najRoute.totalDistance += data.getDistance(prevNode, currNode);

					double travelTime = data.getDistance(prevNode, currNode);
					double arrivalTime = currentTime + travelTime;
					double startServiceTime = std::max(arrivalTime, (double)cust.readyTime);
					currentTime = startServiceTime + cust.serviceTime;
				}
				najRoute.totalTime = currentTime;
			}
			if (!everyClientAdded) continue;
			break;
		}
	tempSolution.totalDistance = 0.0;
	tempSolution.totalTime = 0.0;
	for (int i = 0; i < tempSolution.routes.size(); ++i) {
		tempSolution.totalDistance += tempSolution.routes[i].totalDistance;
		tempSolution.totalTime += tempSolution.routes[i].totalTime;
	}
	bestSolution = tempSolution;
	return bestSolution;
}