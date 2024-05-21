#include "../include/Order.h"
#include <string>
#include <vector>
using std::string;
using std::vector;
#define NO_VOLUNTEER -1



Order::Order(int _id, int _customerId, int _distance):  id(_id), customerId(_customerId), distance(_distance), 
status(OrderStatus::PENDING) , collectorId(NO_VOLUNTEER), driverId(NO_VOLUNTEER) {}; 

int Order::getId() const {
    return id;
};

int Order::getCustomerId() const {
    return customerId;
};

void Order::setStatus(OrderStatus _status){
    status = _status;
};

void Order::setCollectorId(int _collectorId){
    collectorId = _collectorId;
};

void Order::setDriverId(int _driverId){
    driverId = _driverId;
};

int Order::getCollectorId() const{
    return collectorId;
};
int Order::getDriverId() const{
    return driverId;
};
OrderStatus Order::getStatus() const{
    return status;
};
const string Order::toString() const{
    string collectorString = std::to_string(getCollectorId());
    string driverString = std::to_string(getDriverId());
    if(getDriverId() == -1 ){
        driverString = "None";
        if(getCollectorId() == -1){
            collectorString = "None";
        };
    };
    return "OrderID: " + std::to_string(getId()) + "\n" + "OrderStatus: " + enumToString(status)
             + "\n" + 
            "CustomerId: " + std::to_string(getCustomerId()) + "\n" + "Collector: " + collectorString + "\n" + 
            "Driver: " + driverString;
};



// methods we added
int Order::getOrderDistance() const {
        return distance;
};

std::string Order::enumToString(OrderStatus status) const {
    switch (status) {
        case OrderStatus::PENDING:
            return "PENDING";
        case OrderStatus::COLLECTING:
            return "COLLECTING";
        case OrderStatus::DELIVERING:
            return "DELIVERING";
        case OrderStatus::COMPLETED:
            return "COMPLETED";
        default:
            return "UNKNOWN";
        }
};

Order* Order::clone() const {
    return new Order(*this);
};