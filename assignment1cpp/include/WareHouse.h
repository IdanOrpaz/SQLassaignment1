#pragma once
#include <string>
#include <vector>
#include "Order.h"
#include "Customer.h"
#include "Action.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>


class BaseAction;
class Volunteer;
class Customer;
class SoldierCustomer;
class CollectorVolunteer;
class Order;


// Warehouse responsible for Volunteers, Customers Actions, and Orders.

class WareHouse {

    public:
        WareHouse(const string &configFilePath);
        void start();
        void addOrder(Order* order);
        void addAction(BaseAction* action);
        Customer &getCustomer(int customerId) const;
        Volunteer &getVolunteer(int volunteerId) const;
        Order &getOrder(int orderId) const;
        const vector<BaseAction*> &getActions() const;
        void close();
        void open();

        // methods we added (rule of 5)
        ~WareHouse();
        WareHouse(const WareHouse& other);
        template<typename T>
        void deleteVectorElements(std::vector<T*> &vec);
        template<typename T>
        void deepCopyVectorElements(std::vector<T*>& myVec , const std::vector<T*>& otherVec);
        template<typename T>
        void deleteCopyVectorElements(std::vector<T*>& myVec , const std::vector<T*>& otherVec);
        WareHouse& operator=(const WareHouse &other);
        WareHouse(WareHouse&& other) noexcept;
        WareHouse& operator=(WareHouse&& other)noexcept;

        // methods we added
        void addCustomer(Customer* customer);
        int getCustomerCounter() const;
        int getVolunteerCounter() const;
        int getOrderCounter() const;
        
        void generateNewCustomerId();   
        void generateNewVolunteerId();
        void generateNewOrderId();


        void fromPendingToInprocess(Order *order);
        void frominToInprocessToComplete(Order *order);
        void frominToInprocessPending(Order *order);

        vector<Volunteer*> getVolunteersVector() const;
        vector<Order*> getPendingOrdersVector() const;
        vector<Order*> getinProcessOrdersVector() const;
        vector<Order*> getCompletedOrdersVector() const;
        vector<Customer*> getCustomersVector() const;

    private:
        bool isOpen;
        vector<BaseAction*> actionsLog;
        vector<Volunteer*> volunteers;
        vector<Order*> pendingOrders;           // orders that are waiting for action(not only waiting for collectors)
        vector<Order*> inProcessOrders;
        vector<Order*> completedOrders;
        vector<Customer*> customers;
        int customerCounter; //For assigning unique customer IDs
        int volunteerCounter; //For assigning unique volunteer IDs


        // fields we added--
        int orderCounter;        //For assigning unique custome ID's
        SoldierCustomer* default_customer; // when id is not valid in getCustomer(id) 
        CollectorVolunteer* default_volunteer; // when id is not valid in getVolunteer(id)
        Order* default_order; // when id is not valid in getOrder(id)

};