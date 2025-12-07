#include "tabu.h"
#include <limits>
#include <algorithm>
#include <cmath>

bool isValidMove(const Route& targetRoute, int customerID, int position) {
	// napisz tu cos pls
	return true;
}

double calculateDelta(const Route& fromRoute, const Route& toRoute, int customerID, int positionDest) {
	// napisz tu cos pls
	return 0.0;
}

Solution TabuSearch::run(const Solution& initialSolution) { // na razie podstawa z autocomplete
	Solution bestSolution = initialSolution;
	Solution currentSolution = initialSolution;
	int iteration = 0;
	while (iteration < maxIterations) {
		Move bestMove;
		bestMove.costDelta = std::numeric_limits<double>::infinity();
		for (int fromRouteLP = 0; fromRouteLP < currentSolution.routes.size(); ++fromRouteLP) {
			const Route& fromRoute = currentSolution.routes[fromRouteLP];
			for (int toRouteLP = 0; toRouteLP < currentSolution.routes.size(); ++toRouteLP) {
				if (fromRouteLP == toRouteLP) continue;
				const Route& toRoute = currentSolution.routes[toRouteLP];
				for (size_t posFrom = 1; posFrom < fromRoute.path.size() - 1; ++posFrom) {
					int customerID = fromRoute.path[posFrom];
					for (size_t posTo = 1; posTo < toRoute.path.size(); ++posTo) {
						if (!isValidMove(toRoute, customerID, posTo)) continue;
						double delta = calculateDelta(fromRoute, toRoute, customerID, posTo);
						if (isTabu(customerID, toRouteLP, iteration) && delta >= 0) {
							continue;
						}
						if (delta < bestMove.costDelta) {
							bestMove.customerID = customerID;
							bestMove.fromRoute = fromRouteLP;
							bestMove.toRoute = toRouteLP;
							bestMove.positionInFromRoute = posFrom;
							bestMove.positionInToRoute = posTo;
							bestMove.costDelta = delta;
						}
					}
				}
			}
		}
		if (bestMove.costDelta == std::numeric_limits<double>::infinity()) {
			break; // brak mozliwych ruchow
		}
		// Wykonaj najlepszy ruch
		Route& fromRoute = currentSolution.routes[bestMove.fromRoute];
		Route& toRoute = currentSolution.routes[bestMove.toRoute];
		fromRoute.path.erase(fromRoute.path.begin() + bestMove.positionInFromRoute);
		toRoute.path.insert(toRoute.path.begin() + bestMove.positionInToRoute, bestMove.customerID);
		// Aktualizuj tabu matrix
		tabuMatrix[bestMove.customerID][bestMove.toRoute] = iteration + tabuTenure;
		// Aktualizuj najlepsze rozwiazanie
		if (currentSolution.totalDistance < bestSolution.totalDistance) {
			bestSolution = currentSolution;
		}
		iteration++;
	}
	return bestSolution;
}