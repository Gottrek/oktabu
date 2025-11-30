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

// Solomon::run() {

// }