#pragma once
#include <string>
#include <vector>

struct Customer {
	int id;
	double x, y;
	int demand;
	int readyTime;
	int dueDate;
	int serviceTime;
};

struct Route {
	std::vector<int> path; // IDs of customers in the route
	double totalDistance;
	double totalTime;
	int currentLoad;
};

struct Solution {
	std::vector<Route> routes;
	double totalDistance;
	double totalTime;
};

class Instance {
private:
	std::vector<Customer> customers;
	std::vector<std::vector<double>> distanceMatrix;
	int vehicleCapacity;
public:
	void loadFromFile(const std::string& filename);

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
};