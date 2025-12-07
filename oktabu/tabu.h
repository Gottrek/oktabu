#pragma once
#include "data.h"
#include <vector>

struct Move {
	int customerID;
	int fromRoute;
	int toRoute;
	int positionInFromRoute;
	int positionInToRoute;
	double costDelta;

};

class TabuSearch {
private:
	const Instance& data;
	int tabuTenure;
	int maxIterations;
	std::vector<std::vector<int>> tabuMatrix;

	bool isTabu(int customerID, int targetRouteLP, int iteration) {
				return tabuMatrix[customerID][targetRouteLP] > iteration;
	}
	bool isValidMove(const Route& targetRoute, int customerID, int position);
	double calculateDelta(const Route& fromRoute, const Route& toRoute, int customerID, int positionDest);
public:
	TabuSearch(Instance& instance, int tenure, int maxIterations)
		: data(instance),
		tabuTenure(tenure),
		maxIterations(maxIterations) {
		int size = instance.getCustomerCount() + 1;
		tabuMatrix.resize(size, std::vector<int>(size, 0));
	}
	Solution run(const Solution& initialSolution);
};