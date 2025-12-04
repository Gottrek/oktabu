#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>

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
	int currentLoad;
	double totalDistance;
	double totalTime;
};

class Solution {
public:
	std::vector<Route> routes;
	double totalDistance;
	double totalTime;
	bool found;

	Solution();
	void setNoSolution();
	void addRoute(const Route& route);

	void printToConsole() const;
	void saveToFile(const std::string& filename) const;
};

class Instance {
private:
	std::vector<Customer> customers;
	std::vector<std::vector<double>> distanceMatrix;
	int vehicleCapacity;
public:
	bool loadFromFile(const std::string& filename);

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