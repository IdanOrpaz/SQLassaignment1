#include <string>
#include <vector>
#include "../include/Customer.h"
using std::string;  
using std::vector;


//------------CUSTOMER START---------//

Customer::Customer(int _id, const string &_name, int _locationDistance, int _maxOrders): id(_id),name(_name),
locationDistance(_locationDistance), maxOrders(_maxOrders) , ordersId(std::vector<int>()) {};

Customer::~Customer() = default;

const string &Customer::getName() const{
return name;
};

int Customer::getId() const {
return id;
};

int Customer::getCustomerDistance() const {
return locationDistance;
};

int Customer::getMaxOrders() const {
return maxOrders;
};

int Customer::getNumOrders() const {
return ordersId.size();
}; 

bool Customer::canMakeOrder() const {
return getNumOrders() < getMaxOrders();
}; 

const vector<int> &Customer::getOrdersIds() const {
return ordersId;
};

int Customer::addOrder(int orderId) { //return OrderId if order was added successfully, -1 otherwise
if (canMakeOrder()) {
    ordersId.push_back(orderId);
    return orderId;
};

return -1;
}; 

// --------CUSTOMER END-----------//


// --------SOLDIER CUSTOMER START-----------//

SoldierCustomer::SoldierCustomer(int id, const string &name, int locationDistance, int maxOrders):
    Customer( id, name, locationDistance, maxOrders){};

SoldierCustomer* SoldierCustomer::clone() const {
    return new SoldierCustomer(*this);
};

// methods we created
string SoldierCustomer::toString(WareHouse& wareHouse) const {

    string toPrint = "CustomerId: " + std::to_string(getId()) + "\n";           // enter Id of Customer
    if(getOrdersIds().size() > 0){
        for (int orderId : getOrdersIds()){
            Order orderTemp = wareHouse.getOrder(orderId);                          
            toPrint = toPrint + "OrderId: " + std::to_string(orderId) + "\n" +      // add order id and order status of every order
            "OrderStatus: " + orderTemp.enumToString(orderTemp.getStatus()) +  "\n" ;
        };
    }
    
    toPrint =  toPrint + "numOrdersLeft: " + std:: to_string(getMaxOrders() - getNumOrders());
    return toPrint;
};

//---------- SOLDIER CUSTOMER END----------//




//---------- CIVILIAN CUSTOMER START----------//

CivilianCustomer::CivilianCustomer(int id, const string &name, int locationDistance, int maxOrders):
Customer(id, name, locationDistance, maxOrders) {};

CivilianCustomer* CivilianCustomer::clone() const {
    return new CivilianCustomer(*this);
};

// methods we created
string CivilianCustomer::toString(WareHouse& wareHouse) const {

    string toPrint = "CustomerId: " + std::to_string(getId()) + "\n";           // enter Id of Customer

    if(getOrdersIds().size() > 0){
        for (int orderId : getOrdersIds()){
            Order orderTemp = wareHouse.getOrder(orderId);                          
            toPrint = toPrint + "OrderId: " + std::to_string(orderId) + "\n" +      
            "OrderStatus: " + orderTemp.enumToString(orderTemp.getStatus()) +  "\n" ;
        };
    }

    toPrint = toPrint + "numOrdersLeft: " + std:: to_string(getMaxOrders() - getNumOrders()) + "\n" ;
    return toPrint;

};

//---------- CIVILIAN CUSTOMER END----------//
