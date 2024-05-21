#include <string>
#include <vector>
#include "../include/WareHouse.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

// Warehouse responsible for Volunteers, Customers Actions, and Orders.




// ------------RULE OF 5 START -------------------// 

// rule of 5: Constructor
WareHouse::WareHouse(const string &configFilePath): isOpen(false), actionsLog(), volunteers(),
  pendingOrders(), inProcessOrders(), completedOrders() , customers(), customerCounter(-1), volunteerCounter(-1),
  orderCounter(-1), default_customer(new SoldierCustomer(-1,"default_customer",-1,-1)),
  default_volunteer(new CollectorVolunteer(-1,"default_volunteer", -1)),
   default_order(new Order(-1,-1,-1)) {

    // Parse the file and populate the vectors
    std::ifstream inputFile(configFilePath);
    if (!inputFile.is_open() || inputFile.fail()) {
        std:: cout << "Configuration file is not accessible or empty, starting with empty configuration" << std:: endl;
    }
    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string category;
        iss >> category;

        if (category == "customer") {
            std::string name, type;
            int distance, maxOrders;
            iss >> name >> type >> distance >> maxOrders;

            if (type == "soldier") {
                generateNewCustomerId();
                SoldierCustomer *temp = new SoldierCustomer(getCustomerCounter(), name, distance, maxOrders);
                customers.push_back(temp);
                temp = nullptr;
            }

            else if (type == "civilian") {
                generateNewCustomerId();
                CivilianCustomer *temp = new CivilianCustomer(getCustomerCounter(), name, distance, maxOrders);
                customers.push_back(temp);
                temp = nullptr; 
            }

        }

        else if (category == "volunteer") {
            std::string name, type;
            iss >> name >> type;

            if (type == "collector") {
                int coolDown;
                iss >> coolDown;
                generateNewVolunteerId();
                CollectorVolunteer *temp = new CollectorVolunteer(getVolunteerCounter(), name, coolDown);
                volunteers.push_back(temp);
                temp = nullptr; 
            }

            else if (type == "limited_collector") {     
                int coolDown , maxOrders;
                iss >> coolDown >> maxOrders;
                generateNewVolunteerId();
                LimitedCollectorVolunteer *temp = new LimitedCollectorVolunteer(getVolunteerCounter(), name, coolDown, maxOrders);
                volunteers.push_back(temp);
                temp = nullptr; 

            }

            else if (type == "driver") {
                int maxDistance, distance_per_step;
                iss >> maxDistance >> distance_per_step;
                generateNewVolunteerId();
                DriverVolunteer *temp = new DriverVolunteer(getVolunteerCounter(), name, maxDistance, distance_per_step);
                volunteers.push_back(temp);
                temp = nullptr; 

            }

            else if (type == "limited_driver") {
                int maxDistance, distance_per_step , maxOrders;
                iss >> maxDistance >> distance_per_step >> maxOrders;
                generateNewVolunteerId();
                LimitedDriverVolunteer *temp = new LimitedDriverVolunteer(getVolunteerCounter(), name, maxDistance, distance_per_step, maxOrders);
                volunteers.push_back(temp);
                temp = nullptr; 
            }
        }
    };

    inputFile.close(); // release the file resource
}


// rule of 5: Destructor helper function
template<typename T>
void WareHouse::deleteVectorElements(std::vector<T*>&vec) {
    for (auto p : vec) {
        if (p != nullptr) {
            delete p; // delete the elements that the vector cells point 
        }
    }
    vec.clear(); // clear the vector space , size becomes 0
};

// rule of 5: Destructor
WareHouse::~WareHouse() {
    deleteVectorElements(actionsLog);
    deleteVectorElements(volunteers);
    deleteVectorElements(pendingOrders);
    deleteVectorElements(inProcessOrders);
    deleteVectorElements(completedOrders);
    deleteVectorElements(customers);
    delete default_customer;
    delete default_volunteer;
    delete default_order;
};

// rule of 5: Copy Constructor helper function
template<typename T>
void WareHouse::deepCopyVectorElements(std::vector<T*>& myVec , const std::vector<T*>& otherVec) {
    for (std::size_t i = 0; i < otherVec.size(); i++) {
        myVec.push_back(otherVec.at(i)->clone()); // copy the elements from the other vector to me
    }
};

// rule of 5: Copy Constructor
WareHouse::WareHouse(const WareHouse &other): 
isOpen(other.isOpen),actionsLog(), volunteers(),pendingOrders(), inProcessOrders(),
completedOrders(), customers(),customerCounter(other.customerCounter),
volunteerCounter(other.volunteerCounter),orderCounter(other.orderCounter),
 default_customer(new SoldierCustomer(-1,"default_customer",-1,-1)),
  default_volunteer(new CollectorVolunteer(-1,"default_volunteer", -1)),
   default_order(new Order(-1,-1,-1)) {
     
    for (auto action : other.actionsLog) {
        actionsLog.push_back(action->clone());
    }
    for (auto volunteer : other.volunteers) {
        volunteers.push_back(volunteer->clone());
    }
    for (auto pending : other.pendingOrders) {
        pendingOrders.push_back(pending->clone());
    }
    for (auto process : other.inProcessOrders) {
        inProcessOrders.push_back(process->clone());
    }
    for (auto complete : other.completedOrders) {
        completedOrders.push_back(complete->clone());
    }
    for (auto customer : other.customers) {
        customers.push_back(customer->clone());
    }
};


// rule of 5: Copy Assignment Operator helper function: PS.6 
template<typename T>
void WareHouse::deleteCopyVectorElements(std::vector<T*>& myVec , const std::vector<T*>& otherVec) {
    deleteVectorElements(myVec); // empty my vector
    deepCopyVectorElements(myVec,otherVec); // deep copy the other's content to me
};

// rule of 5: Copy Assignment Operator
WareHouse& WareHouse::operator=(const WareHouse &other) {   
    if (&other != this) {
        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;
        deleteCopyVectorElements(actionsLog, other.getActions());
        deleteCopyVectorElements(volunteers, other.volunteers);
        deleteCopyVectorElements(pendingOrders, other.pendingOrders);
        deleteCopyVectorElements(inProcessOrders, other.inProcessOrders);
        deleteCopyVectorElements(completedOrders, other.completedOrders);
        deleteCopyVectorElements(customers, other.customers);
        // I didn't copy default customer, volunteer, order for this object
        // because it already contains them with the same data
    }
    return *this;
};


// rule of 5: Move Constructor
// source1: https://stackoverflow.com/questions/14784041/move-constructor-move-vector-between-two-objects-using-stdmove
WareHouse::WareHouse(WareHouse&& other) noexcept
    : isOpen(other.isOpen),
      actionsLog(std::move(other.actionsLog)),
      volunteers(std::move(other.volunteers)),
      pendingOrders(std::move(other.pendingOrders)),
      inProcessOrders(std::move(other.inProcessOrders)),
      completedOrders(std::move(other.completedOrders)),
      customers(std::move(other.customers)),
      customerCounter(other.customerCounter),
      volunteerCounter(other.volunteerCounter),
      orderCounter(other.orderCounter),
      default_customer(other.default_customer),
      default_volunteer(other.default_volunteer),
      default_order(other.default_order) {
    
    // Reset the source object's state
    other.isOpen = false;
    other.customerCounter = 0;
    other.volunteerCounter = 0;
    other.orderCounter = 0;
    other.default_customer = nullptr;
    other.default_volunteer = nullptr;
    other.default_order = nullptr;
}



// rule of 5: Move assignment operator
// source: https://stackoverflow.com/questions/46208967/move-assignment-operator-c
WareHouse& WareHouse::operator=(WareHouse&& other) noexcept {
    if (this != &other) {
        // Release current resources
        deleteVectorElements(actionsLog);
        deleteVectorElements(volunteers);
        deleteVectorElements(pendingOrders);
        deleteVectorElements(inProcessOrders);
        deleteVectorElements(completedOrders);
        deleteVectorElements(customers);

        // Transfer ownership of resources from 'other' to 'this'
        isOpen = other.isOpen;
        actionsLog = std::move(other.actionsLog);
        volunteers = std::move(other.volunteers);
        pendingOrders = std::move(other.pendingOrders);
        inProcessOrders = std::move(other.inProcessOrders);
        completedOrders = std::move(other.completedOrders);
        customers = std::move(other.customers);
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;
        default_customer = other.default_customer;
        default_volunteer = other.default_volunteer;
        default_order = other.default_order;

        // Reset the source object's state
        other.isOpen = false;
        other.customerCounter = 0;
        other.volunteerCounter = 0;
        other.orderCounter = 0;
        other.default_customer = nullptr;
        other.default_volunteer = nullptr;
        other.default_order = nullptr;
    }
    return *this;
}

//-------- RULE OF 5 END------------//




//---------- METHODS -----------------//

void WareHouse::start() {
    open();
    std::cout << "Warehouse is open!" << std::endl;
    std::string userInput;
    while (isOpen == true) {
        std::cout << "";
        std::getline(std::cin, userInput); // updated user input
        std::istringstream iss(userInput); // parsable object
        std::string command; // first word == action
        iss >> command;

        if (command == "step") { // SimulateStep => step <number_of_steps>
            string number_of_steps;
            iss >> number_of_steps;
            SimulateStep step_act(std::stoi(number_of_steps));
            step_act.act(*this);
        }

        else if (command == "order") { // AddOrder => order <customer_id>
            string customer_id;
            iss >> customer_id;
            AddOrder order_act(std::stoi(customer_id));
            order_act.act(*this);
        }

        else if (command == "customer") { // AddCustomer => customer <customer_name> <customer_type> <customer_distance> <max_orders>
                string customer_name, customer_type, customer_distance, max_orders;
                iss >> customer_name >> customer_type >> customer_distance >> max_orders;
                AddCustomer customer_act(customer_name, customer_type, std::stoi(customer_distance), std::stoi(max_orders));
                customer_act.act(*this);
        }

        else if (command == "orderStatus") { // PrintOrderStatus => orderStatus <order_id>
            string order_id;
            iss >> order_id;
            PrintOrderStatus orderStatus_act(std::stoi(order_id));
            orderStatus_act.act(*this);
        }

        else if (command == "customerStatus") { // PrintCustomerStatus => customerStatus <customer_id>
            string order_id;
            iss >> order_id;
            PrintCustomerStatus customerStatus_act(std::stoi(order_id));
            customerStatus_act.act(*this);
        }

        else if (command == "volunteerStatus") { // PrintVolunteerStatus => volunteerStatus <volunteer_id>
            string volunteer_id;
            iss >> volunteer_id;
            PrintVolunteerStatus volunteerStatus_act(std::stoi(volunteer_id));
            volunteerStatus_act.act(*this);
        }

        else if (command == "log") { // PrintActionsLog => log
            PrintActionsLog log_act;
            log_act.act(*this);
        } 

        else if (command == "close") { // Close => close
            Close close_act;
            close_act.act(*this);
        }

        else if (command == "backup") { // BackupWarehouse => backup
            BackupWareHouse backup_act;
            backup_act.act(*this);
        }

        else if (command == "restore") { // RestoreWarehouse => restore
            RestoreWareHouse restore_act;
            restore_act.act(*this);
        }

        else {
            std::cout << "Command was not identified, try again: " << std::endl;
        }
    }

};

void WareHouse::addOrder(Order* order) {
    // add the order to the pending vector
    pendingOrders.push_back(order);
};

void WareHouse::addAction(BaseAction* action) {
    actionsLog.push_back(action->clone());
};

Customer &WareHouse::getCustomer(int customerId) const {
    if (customerId > customerCounter || customerId < 0) {
        std::cout << "Customer with " + std::to_string(customerId) + " does not exist"  << std::endl;
        return *default_customer;
    }
    else {
        return *customers[customerId];
    }
};

Volunteer &WareHouse::getVolunteer(int volunteerId) const {         //do we also need to check if the volunteer reached max. volunteers can be deleted.
    if (volunteerId > volunteerCounter || volunteerId < 0) {
        std::cout << "Volunteer with " + std::to_string(volunteerId) + " does not exist"  << std::endl;
        return *default_volunteer;
    }
    else {
        if ((volunteers[volunteerId] == nullptr)){                // check if volunteer hadn't been deleted
            std::cout << "Volunteer with " + std::to_string(volunteerId) + " does not exist"  << std::endl;
            return *default_volunteer;
        }
        else {
          return *volunteers[volunteerId];
        }
    }
};

Order &WareHouse::getOrder(int orderId) const {
    if ((orderId > orderCounter) || (orderId< 0) ) {            // if no order, print no such order 
        std::cout << "Order Num " + std::to_string(orderId) + "does not exist"  << std::endl;
        return *default_order;
    }
    else{                                                      // look for order in all 3 vectors.
        for(Order* order : pendingOrders){
            if(order->getId() == orderId){
                return *order;
            };
        };
        for(Order* order : inProcessOrders){
            if(order->getId() == orderId){
                return *order;
            };
        };
        for(Order* order : completedOrders){
            if(order->getId() == orderId){
                return *order;
            };
        };
        return *default_order; // in case it still wasn't found
    }

};

const vector<BaseAction*> &WareHouse::getActions() const {
    return actionsLog;
};

void WareHouse::close() {
    isOpen = false;
};


void WareHouse::open() {
    // https://moodle.bgu.ac.il/moodle/mod/forum/discuss.php?d=689818#p1042021
    isOpen = true;
};





// ----- Method we created ---------//

void WareHouse::addCustomer(Customer* customer) {
    customers.push_back(customer);
};

int WareHouse::getCustomerCounter() const{
    return customerCounter;
};
int WareHouse::getVolunteerCounter()const {
    return volunteerCounter;
};
int WareHouse::getOrderCounter() const {
    return orderCounter;
};


void WareHouse::fromPendingToInprocess(Order *order) {
    inProcessOrders.push_back(order);
    pendingOrders.erase(find(pendingOrders.begin(), pendingOrders.end(), order));
};

void WareHouse::frominToInprocessToComplete(Order *order){
    completedOrders.push_back(order);
    inProcessOrders.erase(find(inProcessOrders.begin(), inProcessOrders.end(), order));
};


void WareHouse::frominToInprocessPending(Order *order){
    pendingOrders.push_back(order);
    inProcessOrders.erase(find(inProcessOrders.begin(), inProcessOrders.end(), order));
};

vector<Volunteer*> WareHouse::getVolunteersVector() const {
    return volunteers;
};

vector<Order*> WareHouse::getPendingOrdersVector() const {
    return pendingOrders;
}; 

vector<Order*> WareHouse::getinProcessOrdersVector() const {
    return inProcessOrders;
};

vector<Order*> WareHouse::getCompletedOrdersVector() const {
    return completedOrders;
};

vector<Customer*> WareHouse::getCustomersVector() const {
    return customers;
};

void WareHouse::generateNewCustomerId() {              // generate new customer id (customercounter++) and return the generated number. 
    customerCounter= customerCounter + 1;
};   
void WareHouse::generateNewVolunteerId() {             // generate new volunteer id (volunteercounter++) and return the generated number. 
    volunteerCounter = volunteerCounter + 1;
};      
void WareHouse::generateNewOrderId() {                 // generate new order id (ordercounter++) and return the generated number. 
    orderCounter = orderCounter + 1 ;       
};          

//---------- METHODS END -----------------//