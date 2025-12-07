#include "data.h"
#include "tabu.h"
#include <limits>
#include <algorithm>
#include <cmath>

bool TabuSearch::isValidMove(const Route& targetRoute, int customerID, int position) { // tak jak canInsert z solomona
	int demand = data.getCustomer(customerID).demand;
	if (targetRoute.currentLoad + demand > data.getCapacity()) return false;

	std::vector<int> tempPath = targetRoute.path;
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

double TabuSearch::calculateDelta(const Route& fromRoute, const Route& toRoute, int customerID, int positionDest) {
	double gain = 0.0;
	double cost = 0.0;
	int fp = -1;
	for (int i = 1; i < fromRoute.path.size() - 1; ++i) {
		if (fromRoute.path[i] == customerID) {
			fp = i;
			break;
		}
	}
	if (fp == -1) return 0.0;
	cost += data.getDistance(fromRoute.path[fp - 1], fromRoute.path[fp]);
	cost += data.getDistance(fromRoute.path[fp], fromRoute.path[fp + 1]);
	gain += data.getDistance(fromRoute.path[fp - 1], fromRoute.path[fp + 1]);

	cost += data.getDistance(toRoute.path[positionDest - 1], toRoute.path[positionDest]);
	gain += data.getDistance(toRoute.path[positionDest - 1], customerID);
	gain += data.getDistance(customerID, toRoute.path[positionDest]);

	return gain - cost;
}

void TabuSearch::updateRoute(Route& route) {
	route.totalDistance = 0.0;
	route.totalTime = 0.0;
	route.currentLoad = 0;
	double currentTime = 0.0;

	for (int i = 1; i < route.path.size(); ++i) {
		int prevNode = route.path[i - 1];
		int currNode = route.path[i];
		const Customer& cust = data.getCustomer(currNode);
		route.totalDistance += data.getDistance(prevNode, currNode);
		route.currentLoad += cust.demand;
		double travelTime = data.getDistance(prevNode, currNode);
		double arrivalTime = currentTime + travelTime;
		double startServiceTime = std::max(arrivalTime, (double)cust.readyTime);
		currentTime = startServiceTime + cust.serviceTime;
	}
	route.totalTime = currentTime;
}

Solution TabuSearch::run(const Solution& initialSolution) {
	Solution bestSolution = initialSolution;
	Solution currentSolution = initialSolution;
	for (int iteration = 0; iteration < maxIterations; ++iteration) {
		Move bestMove;
		bestMove.customerID = -1;
		bestMove.costDelta = INFINITY;
		for (int fr = 0; fr < currentSolution.routes.size(); ++fr) {
			for (int tr = 0; tr < currentSolution.routes.size(); ++tr) {
				if (fr == tr) continue;
				for (int fp = 1; fp < currentSolution.routes[fr].path.size() - 1; ++fp) {
					int custID = currentSolution.routes[fr].path[fp];
					if (currentSolution.routes[fr].path.size() <= 2) continue; // nie mozna usunac depotow
					if (currentSolution.routes[tr].currentLoad + data.getCustomer(custID).demand > data.getCapacity()) continue;
					for (int tp = 1; tp < currentSolution.routes[tr].path.size(); ++tp) {
						if (!isValidMove(currentSolution.routes[tr], custID, tp)) continue;
						double delta = calculateDelta(currentSolution.routes[fr], currentSolution.routes[tr], custID, tp);
						if (isTabu(custID, tr, iteration)) continue;
						if (delta < bestMove.costDelta) {
							bestMove.customerID = custID;
							bestMove.fromRoute = fr;
							bestMove.toRoute = tr;
							bestMove.positionInFromRoute = fp;
							bestMove.positionInToRoute = tp;
							bestMove.costDelta = delta;
						}
					}
				}
			}
		}
		if (bestMove.customerID == -1) break;
		// wykonaj ruch
		Route& fromRoute = currentSolution.routes[bestMove.fromRoute];
		Route& toRoute = currentSolution.routes[bestMove.toRoute];
		fromRoute.path.erase(fromRoute.path.begin() + bestMove.positionInFromRoute);
		toRoute.path.insert(toRoute.path.begin() + bestMove.positionInToRoute, bestMove.customerID);
		// aktualizuj
		tabuMatrix[bestMove.customerID][bestMove.toRoute] = iteration + tabuTenure;
		
		updateRoute(fromRoute);
		updateRoute(toRoute);

		currentSolution.totalTime = 0.0;
		for (int i = 0; i < currentSolution.routes.size(); ++i) {
			currentSolution.totalTime += currentSolution.routes[i].totalTime;
		}

		if (currentSolution.totalTime < bestSolution.totalTime) {
			bestSolution = currentSolution;
		}
	}
	return bestSolution;
}