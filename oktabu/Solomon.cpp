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
		if (cust.demand > data.getCapacity()) {
			return Solution(); // nie ma rozwiazania
		}
		if (cust.dueDate < data.getDistance(0, i)) {
			return Solution(); // nie ma rozwiazania
		}
		if ((double)depot.dueDate < std::max((double)cust.readyTime, (double)data.getDistance(0, i)) + data.getDistance(0, i) + (double)cust.serviceTime) {
			return Solution(); // nie ma rozwiazania
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
				// aktualizuj dystans i czas trasy
				route.totalDistance = 0.0;
				route.totalTime = 0.0;
				for (int i = 1; i < route.path.size(); ++i) {
					int prevNode = route.path[i - 1];
					int currNode = route.path[i];
					const Customer& cust = data.getCustomer(currNode);
					route.totalDistance += data.getDistance(prevNode, currNode);

					double travelTime = data.getDistance(prevNode, currNode);
					double arrivalTime = route.totalTime + travelTime;
					double startServiceTime = std::max(arrivalTime, (double)cust.readyTime);
					route.totalTime = startServiceTime + cust.serviceTime;
				}
			}
			else {
				clientAdded = false;
			}
		}
		solution.routes.push_back(route);
	}
	return solution;
}