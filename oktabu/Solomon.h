#pragma once
#include "data.h"
#include <vector>

class Solomon {
private:
	const Instance& data;
	std::vector<bool> isVisited;
public:
	Solomon(Instance& instance)
		: data(instance),
		isVisited(instance.getCustomerCount(), false) {
	}
	bool canInsert(const Route& currentRoute, int customerID, int position);
	double calculateScore(const Route& currentRoute, int customerID, int position);
	Solution run();
	Solution reduceVehicles(Solution initialSolution);
};