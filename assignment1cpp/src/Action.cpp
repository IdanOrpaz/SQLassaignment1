#include <string>
#include <vector>
#include "../include/Action.h"
#include <iostream>
#include <map>
using std::string;
using std::vector;



// --------------------BASEACTION START-----------------//


BaseAction:: BaseAction(): errorMsg(""), status(ActionStatus::ERROR) {};
BaseAction::~BaseAction() = default;


    ActionStatus BaseAction:: getStatus() const {
    return status;
};

// methods we adde
string BaseAction::actionStatusToString() const {
    switch (getStatus()) {
    case ActionStatus::COMPLETED:
        return "COMPLETED";
    case ActionStatus::ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
};



void BaseAction::complete() {
    status = ActionStatus:: COMPLETED;
};

void BaseAction::error(string _errorMsg) {
    status = ActionStatus:: ERROR;
    errorMsg = _errorMsg ;
};

string BaseAction::getErrorMsg() const {
    return errorMsg;
};

// --------------------BASEACTION END--------------------//



//-------------SIMULATE STEP START---------------//

SimulateStep::SimulateStep(int _numOfSteps): numOfSteps(_numOfSteps)  {};             


void SimulateStep::act(WareHouse &wareHouse) {
    
    for(int i =0 ; i<numOfSteps ; i++){                                                 // for each step
        // phase 1: 
        for (std::size_t j = 0 ; j<wareHouse.getPendingOrdersVector().size() ; j++){            // go through pending orders                           
            Order* order = wareHouse.getPendingOrdersVector()[j];
            if (order->getStatus() == OrderStatus::PENDING ){                          // search for collectors 
                for(Volunteer* volunteer : wareHouse.getVolunteersVector()){
                    if(volunteer->type() == 1 && volunteer->canTakeOrder(*order) && order->getStatus() == OrderStatus::PENDING && volunteer->getIsDeleted() == false){      //when regullar collector found
                        volunteer->acceptOrder(*order);                 
                        order->setStatus(OrderStatus:: COLLECTING);
                        order->setCollectorId(volunteer->getId());
                        wareHouse.fromPendingToInprocess(order);                        // order at [j+1] is now at [j]
                        j = j-1;
                    }
                    else if(volunteer->type() == 2 && volunteer->canTakeOrder(*order) && order->getStatus() == OrderStatus::PENDING && volunteer->getIsDeleted() == false){     // when limited collector found
                        volunteer->acceptOrder(*order);                                 // are there more things that need to be updated?
                        order->setStatus(OrderStatus:: COLLECTING);
                        order->setCollectorId(volunteer->getId());
                        wareHouse.fromPendingToInprocess(order);
                        j = j-1;
                    }
                };
            }
            else if (order->getStatus() == OrderStatus::COLLECTING ){                      //search for drivers
                for(Volunteer* volunteer : wareHouse.getVolunteersVector()){
                    if(volunteer->type() == 3 && volunteer->canTakeOrder(*order) && order->getStatus() == OrderStatus::COLLECTING && volunteer->getIsDeleted() == false){     // when regullar collector found
                        volunteer->acceptOrder(*order);
                        order->setStatus(OrderStatus:: DELIVERING);
                        order->setDriverId(volunteer->getId());
                        wareHouse.fromPendingToInprocess(order);
                        j = j - 1;
                    };
                    if(volunteer->type() == 4 && volunteer->canTakeOrder(*order) && order->getStatus() == OrderStatus::COLLECTING && volunteer->getIsDeleted() == false){      // when limited collector found
                        volunteer->acceptOrder(*order);                                  // are there more things that need to be updated?
                        order->setStatus(OrderStatus:: DELIVERING);
                        order->setDriverId(volunteer->getId());
                        wareHouse.fromPendingToInprocess(order);
                        j = j - 1;
                    };
                };
            }
        };

        //phase 2:     
        for(Volunteer* volunteer : wareHouse.getVolunteersVector()){        //go through all Volunteers
            if(volunteer->isBusy() == true){                            // check if volunteer is currently working on an order
                volunteer->step();                                      // perform a decrease in distance left or tome left
            }                                                           // if reached 0. is busy will change to False
        };

        //phase 3:
        for(Volunteer* volunteer : wareHouse.getVolunteersVector()){        // go through all volunteers
            if((volunteer->type() == 1) || (volunteer->type() == 2)){       // if collector or limited collector volunteer
                if((volunteer->isBusy() == false) && (volunteer->getCompletedOrderId() != NO_ORDER)){                          
                    Order* order = &wareHouse.getOrder(volunteer->getCompletedOrderId()); // get the finished order of the volunteer
                    volunteer->setCompletedOrderIdToNoOrder();                        // delete volunteer field noting that he has no complete order. 
                    wareHouse.frominToInprocessPending(order);                        // move order from inprocess vector to pending
                }
            }
            if((volunteer->type() == 3)||(volunteer->type() == 4)){         // if driver or limited driver volunteer
                if((volunteer->isBusy() == false) && (volunteer->getCompletedOrderId() != NO_ORDER)){
                    Order* order = &wareHouse.getOrder(volunteer->getCompletedOrderId()); // get the finished order of the volunteer
                    volunteer->setCompletedOrderIdToNoOrder();                        // delete volunteer field noting that he has no complete order. 
                    wareHouse.frominToInprocessToComplete(order);                       // move order from inprocess to completed
                    order->setStatus(OrderStatus::COMPLETED);                           //change order status to completed
                }
            }

        };

        //phase4:
        for(Volunteer* volunteer : wareHouse.getVolunteersVector()){
            if((volunteer->isBusy() == false) && (volunteer->hasOrdersLeft() == false)){ // check if not busy and also if reached limit.
                volunteer->Delete();               
            }
        };

        complete();

    };
    wareHouse.addAction(this);                              //add action to action vector
};

std::string SimulateStep::toString() const  {
    return "simulateStep " + std::to_string(numOfSteps) + " " + actionStatusToString();
};
SimulateStep* SimulateStep::clone() const  {
    return new SimulateStep(*this);
};




//------------SIMULATE STEP END------------------//


//--------------------ADDORDER START-----------------//

AddOrder::AddOrder(int id): customerId(id){};                       // id is customer id.

void AddOrder::act(WareHouse &wareHouse) {

    if (customerId > wareHouse.getCustomerCounter() || customerId < 0) {         //check if customer exists.
        error("Error: Cannot place this order");
        std::cout << getErrorMsg() + "\n" << std::endl;
    }
    else {
        Customer* customerTemp = &wareHouse.getCustomer(customerId) ;           //  find the customer

        if ((customerTemp -> canMakeOrder()) == false){                        // check if customer can make orders
            error("Error: Cannot place this order");
            std::cout << getErrorMsg() +"\n"  << std::endl;
        }
        else {
        wareHouse.generateNewOrderId();  
        int newOrderId = wareHouse.getOrderCounter();   // Generate the number of the new order

        Order* newOrder= new Order(newOrderId , customerId, customerTemp -> getCustomerDistance());        // create new order

        customerTemp -> addOrder(newOrderId) ;          // add the new order to the customer's orders vector

        wareHouse.addOrder(newOrder);                  // add the new order to the wareHouse.
        complete();
        newOrder = nullptr;
        }
    };
    wareHouse.addAction(this);                              //add action to action vector
};


string AddOrder::toString() const  {
    return "order " + std::to_string(customerId) + " " + actionStatusToString();
};                   

AddOrder* AddOrder::clone() const {
    return new AddOrder(*this);
};                   

// --------------------ADDORDER END--------------------//




// --------------------ADDCUSTOMER START-----------------//

// helper function to AddCustomer
CustomerType AddCustomer:: getTypeIndex(const std::string& s) {
    static std::map<std::string,CustomerType> string2CustomerType = {
       { "solider", CustomerType::Soldier}, 
       { "civilian", CustomerType::Civilian }
    };
    return string2CustomerType[s];
}


AddCustomer::AddCustomer(const string &_customerName, const string &_customerType, int _distance, int _maxOrders) : customerName(_customerName),
customerType(getTypeIndex(_customerType)) , distance(_distance) , maxOrders(_maxOrders) {};

void AddCustomer::act(WareHouse &wareHouse)  {

    if (customerType == CustomerType::Soldier) {
        wareHouse.generateNewCustomerId();
        SoldierCustomer* temp = new SoldierCustomer(wareHouse.getCustomerCounter(),
        customerName, distance, maxOrders); // create new instance
        wareHouse.addCustomer(temp); // add it to customers vector
    }
    else if  (customerType == CustomerType::Civilian) { 
        wareHouse.generateNewCustomerId();
        SoldierCustomer* temp = new SoldierCustomer(wareHouse.getCustomerCounter(),
        customerName, distance, maxOrders); // create new instance
        wareHouse.addCustomer(temp); // add it to customers vector
    }
    complete();
    wareHouse.addAction(this);                              //add action to action vector       
};

AddCustomer* AddCustomer::clone() const  {
    return new AddCustomer(*this);
};

// method we added
string AddCustomer::customerTypeToString() const {
    switch (customerType) {
    case CustomerType::Soldier:
        return "Soldier";
    case CustomerType::Civilian:
        return "Civilian";
    default:
        return "Unknown";
    }
};

string AddCustomer::toString() const {
    return "customer " + customerName + " " + customerTypeToString() + " " +
        std::to_string(distance) + " " + std::to_string(maxOrders) + " " + actionStatusToString() ;
}; 

// --------------------ADDCUSTOMER END-----------------//





//---------------------PRINTORDERSTATUS START------------------//

PrintOrderStatus:: PrintOrderStatus(int id): orderId(id) {};                                           //constructor


void PrintOrderStatus::act(WareHouse &wareHouse) {;          

    if ((orderId > wareHouse.getOrderCounter()) || (orderId < 0)){              // check if order exists
        error("Error: Order does not exist");
        std::cout << getErrorMsg() +"\n" << std::endl;
    }
    else{                                                                   // print the order status
        Order tempOrder = wareHouse.getOrder(orderId) ;                     // find the order

        std::cout << tempOrder.toString() + "\n" << std::endl;                     // print the orderstatus
        complete();
    }
    wareHouse.addAction(this);                              //add action to action vector       

};


PrintOrderStatus* PrintOrderStatus::clone() const {
    return new PrintOrderStatus(*this);
};

string PrintOrderStatus::toString() const  {
    return "orderStatus " + std::to_string(orderId) + " " + actionStatusToString();
};

//---------------------PRINT ORDERORDERSTATUS END----------//



//---------------------PRINT CUSTOMERSTATUS START----------//

PrintCustomerStatus::PrintCustomerStatus(int _customerId): customerId(_customerId) {};               // constructor

void PrintCustomerStatus::act(WareHouse &wareHouse) {

    if (customerId > wareHouse.getCustomerCounter() || customerId < 0) {         //check if customer exists.
        error("Customer Does not exist");
        std::cout << getErrorMsg() + "\n" << std::endl;
    }
    else{
        // Customer* tempCustomer = &wareHouse.getCustomer(customerId);
        std::cout <<  wareHouse.getCustomer(customerId).toString(wareHouse) + "\n" << std::endl;  
        complete();
    }
    wareHouse.addAction(this);                              //add action to action vector       
};

PrintCustomerStatus* PrintCustomerStatus::clone() const {
    return new PrintCustomerStatus(*this);
};


string PrintCustomerStatus::toString() const  {
    return "customerStatus " + std::to_string(customerId) + " " + actionStatusToString(); 
};

//----------------------PRINT CUSTOMERSTATUS END-----------//


//----------------------PRINT VOUNTEERSTATUS START--------------//

PrintVolunteerStatus::PrintVolunteerStatus(int id): volunteerId(id) {};

void PrintVolunteerStatus::act(WareHouse &wareHouse) {
                    
    if (volunteerId > wareHouse.getVolunteerCounter() || volunteerId < 0) {         //check if volunteer exists.
        error("ERROR: Volunteer does not exist");
        std::cout << getErrorMsg()  << std::endl;
    }
    else if ((wareHouse.getVolunteersVector()[volunteerId]->getIsDeleted() == true)){                // check if volunteer hadn't been deleted
            error("ERROR: Volunteer does not exist");
            std::cout << getErrorMsg() + "\n" << std::endl;  
    }
    else {
        std::cout << wareHouse.getVolunteer(volunteerId).toString() + "\n" << std::endl;
        complete();
    }
    wareHouse.addAction(this);                          //add action to action vector  
};

PrintVolunteerStatus* PrintVolunteerStatus::clone() const {
    return new PrintVolunteerStatus(*this);
};

string PrintVolunteerStatus::toString() const {
    return "volunteerStatus " + std::to_string(volunteerId) + " " + actionStatusToString();
};

//----------------------PRINT VOLUNTEERS END-----------------//




//----------------------PRINT ACTIONSLOG START-----------------//

PrintActionsLog::PrintActionsLog() {};

void PrintActionsLog::act(WareHouse &wareHouse) {
    string output = "\n" ;
    for(BaseAction* action : wareHouse.getActions()) {
        output = output + action->toString() + "\n";
    };

    std:: cout << output + "\n" << std:: endl; 

    complete();
    wareHouse.addAction(this);     
};

PrintActionsLog* PrintActionsLog::clone() const {
    return new PrintActionsLog(*this);
};

string PrintActionsLog::toString() const  {
    return "log " +  actionStatusToString();
};


//----------------------PRINT ACTIONSLOG END-----------------//



//--------------------------CLOSE START-----------------//
Close::Close() {};

void Close::act(WareHouse &wareHouse) {
    string ordersToPrint = "";
    for (Order* order : wareHouse.getPendingOrdersVector()) {
        ordersToPrint = ordersToPrint +     
        "OrderID: " + std::to_string(order->getId()) + 
        ",  CustomerID: " + std:: to_string(order->getCustomerId()) +
            ", Status: " + order->enumToString(order->getStatus()) + "\n" ;
    };
    for (Order* order : wareHouse.getinProcessOrdersVector()) {

        ordersToPrint = ordersToPrint +     
        "OrderID: " + std::to_string(order->getId()) + 
        ",  CustomerID: " + std:: to_string(order->getCustomerId()) +
            ", Status: " + order->enumToString(order->getStatus()) + "\n" ;
    };
    for (Order* order : wareHouse.getCompletedOrdersVector()) {
        ordersToPrint = ordersToPrint +     
        "OrderID: " + std::to_string(order->getId()) + 
        ",  CustomerID: " + std:: to_string(order->getCustomerId()) +
            ", Status: " + order->enumToString(order->getStatus()) + "\n" ;
    };
    std:: cout << ordersToPrint + "\n" << std:: endl;
    wareHouse.close(); // exit loop
    complete();
};

Close*Close::clone() const {
    return new Close(*this);
};

string Close::toString() const {
    return "close " +  actionStatusToString();
};

//--------------------------CLOSE END-----------------//



//---------------------BackupWareHouse START----------------------------//

BackupWareHouse:: BackupWareHouse() {};

void BackupWareHouse:: act(WareHouse &wareHouse) {
    complete();
    wareHouse.addAction(this);                              //add action to action vector     
    if (backup == nullptr){
        backup = new WareHouse(wareHouse);          // creating a copy of current warehouse
    }
    else {
        delete backup; 
        backup = new WareHouse(wareHouse);          // creating a copy of current warehouse
    }
};

BackupWareHouse* BackupWareHouse:: clone() const {
    return new BackupWareHouse(*this);
};

string BackupWareHouse:: toString() const {
    return "backup " +  actionStatusToString();
};

//---------------------BackupWareHouse END----------------------------//


//-------------------------RESTOREWAREHOUSE START--------------//

RestoreWareHouse::RestoreWareHouse() {};

void RestoreWareHouse::act(WareHouse &wareHouse) {                    
    if (backup == nullptr){                         // if backup = null
        error("ERROR: No backup available");
        std:: cout << getErrorMsg() + "\n" << std:: endl;
        wareHouse.addAction(this); 
    }
    else {                                          // call the move 
        wareHouse = *backup;                        // we didn't use the move constructor\operator since "restore" might be called again without a prior "backup" call
        complete();
        wareHouse.addAction(this);                              //add action to action vector       
    }

};

RestoreWareHouse* RestoreWareHouse::clone() const {
    return new RestoreWareHouse(*this); 
};

string RestoreWareHouse::toString() const  {
    return "restore " +  actionStatusToString();
};

//-------------------------RESTOREWAREHOUSE END--------------//