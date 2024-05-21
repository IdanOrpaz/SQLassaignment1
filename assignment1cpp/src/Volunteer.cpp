#include "../include/Volunteer.h"
#include <vector>
#include <string>
using std::string;  
using std::vector;

Volunteer::Volunteer(int _id, const string &_name):
completedOrderId(NO_ORDER), activeOrderId(NO_ORDER), id(_id), name(_name), isDeleted(false) {} ; 

Volunteer::~Volunteer() = default; // take the default destructor

int Volunteer::getId() const {                         // getter
    return id;
};  
                        
const string &Volunteer::getName() const {             // getter
    return name;
};              

int Volunteer::getActiveOrderId() const {              // getter -1 if no orders are being processed
    return activeOrderId;
};               

int Volunteer::getCompletedOrderId() const {           // getter -1 if no orders had been completed
    return completedOrderId;
};            

bool Volunteer::isBusy() const {                       // Signal whether the volunteer is currently processing an order 
    return !(activeOrderId == NO_ORDER);
};                         

void Volunteer::setCompletedOrderIdToNoOrder(){
    completedOrderId = NO_ORDER;
};

bool Volunteer::getIsDeleted(){
    return isDeleted;
};

void Volunteer::Delete() {
    isDeleted = true;
};




//------------COLLECTOR VOLUNTEER START---------------//

CollectorVolunteer:: CollectorVolunteer(int _id, const string &_name, int _coolDown): Volunteer( _id, _name), coolDown(_coolDown), timeLeft(0) {};

CollectorVolunteer* CollectorVolunteer::clone() const {     
    return new CollectorVolunteer(*this);
};

void CollectorVolunteer:: step()  { //Simulate volunteer step,if the volunteer finished the order, transfer activeOrderId to completedOrderId
    // case a: isBusy() == false => do nothing (volunteer has no mission)
    // case b: isBusy() == true => decrease timeLeft by 1
    // case b.1: timeleft > 0 => do nothing
    // case b.2: timeLeft == 0 => completedOrderId = activeOrderId ,  activeOrderId = NO_ORDER
    if (isBusy() == true) {
        timeLeft = timeLeft - 1;
        if (timeLeft == 0) {
            completedOrderId = activeOrderId;
            activeOrderId = NO_ORDER;
        }
    }
};

int CollectorVolunteer:: getCoolDown() const {
    return coolDown;
};

int CollectorVolunteer:: getTimeLeft() const{
    return timeLeft;
};

bool CollectorVolunteer:: decreaseCoolDown() {           //Decrease timeLeft by 1,return true if timeLeft=0,false otherwise
    timeLeft = timeLeft - 1;
    return (timeLeft == 0);
};

bool CollectorVolunteer:: hasOrdersLeft() const  {        // Signal whether the volunteer didn't reach orders limit,Always true for CollectorVolunteer
    return true;
};       

bool CollectorVolunteer::canTakeOrder(const Order &order) const  {   // Signal if the volunteer can take the order. 
    return !(isBusy());                           // for regular volunteer, cant take order only when collecting a different order?
};  

void CollectorVolunteer:: acceptOrder(const Order &order)  {  //Prepare for new order(Reset activeOrderId,TimeLeft)
    activeOrderId = order.getId();
    timeLeft = getCoolDown();             // do we need to reset more things? 
};

string CollectorVolunteer:: toString() const {          // VolunterId, isBUsy, OrderId, timeLeft, ordersLeft 
    return "VolunterID: " + std::to_string(getId()) + "\n" + "isBusy: " + (isBusy() ? "true" : "false") + "\n" + 
    "OrderId: " + ((isBusy() == 0) ? "None" : std::to_string(getActiveOrderId())) + "\n" +
    "timeLeft: " + ((isBusy() == 0) ? "None" : std::to_string(getTimeLeft())) + "\n" + 
    "ordersLeft: No Limit";
            
};

// methods we created

 int CollectorVolunteer::type() const{
    return 1;
 };

//------------------COLLECTOR VOLUNTEER END-------------------//



//---------------------LIMITED COLLECTOR VOLUNTEER  END------------------------//

LimitedCollectorVolunteer:: LimitedCollectorVolunteer(int _id, const string& _name, int _coolDown ,int _maxOrders):    
CollectorVolunteer(_id, _name, _coolDown), maxOrders(_maxOrders), ordersLeft(_maxOrders) {};

LimitedCollectorVolunteer* LimitedCollectorVolunteer::clone() const {
    return new LimitedCollectorVolunteer(*this); 
};

bool LimitedCollectorVolunteer:: hasOrdersLeft() const {                // if ordersLeft = 0 return false
    return !(getNumOrdersLeft() == 0) ;
};                

bool LimitedCollectorVolunteer:: canTakeOrder(const Order &order) const  {       // can take order if has orders left and not busy.
    if ((!(isBusy())) == true) {
        if (hasOrdersLeft() == true) {
            return true;
        };
    };
    return false;
};

void LimitedCollectorVolunteer:: acceptOrder(const Order &order)  {     // what needs to be done when accepting orders. reset activeOrderId,TimeLeft,OrdersLeft
// call parent function to fill up the parents fields.
    CollectorVolunteer:: acceptOrder(order);       
    ordersLeft = ordersLeft -1;
};         

int LimitedCollectorVolunteer:: getMaxOrders() const {
    return maxOrders;
};

int LimitedCollectorVolunteer:: getNumOrdersLeft() const {
    return ordersLeft;
};

string LimitedCollectorVolunteer:: toString() const {                    // can take order if has orders left and not busy. are there more options?
    return "VolunterID: " + std::to_string(getId()) + "\n" + "isBusy: " + (isBusy() ? "true" : "false") + "\n" + 
    "OrderId: " + ((isBusy() == 0) ? "None" : std::to_string(getActiveOrderId())) + "\n" +
    "timeLeft: " + ((isBusy() == 0) ? "None" : std::to_string(getTimeLeft()))  + "\n" + 
    "ordersLeft: " + std::to_string(getNumOrdersLeft());
};

// methods we created
 int LimitedCollectorVolunteer::type() const{
    return 2;
 };

//------------- LIMITED COLLECTOR VOLUNTEER END--------------------//



//---------------------------DRIVER VOLUNTEER START---------------//

DriverVolunteer:: DriverVolunteer(int _id, const string &_name, int _maxDistance, int _distancePerStep):
    Volunteer(_id, _name), maxDistance(_maxDistance), distancePerStep(_distancePerStep), distanceLeft(0){} ;


DriverVolunteer* DriverVolunteer::clone() const  {
    return new DriverVolunteer(*this);
};

int DriverVolunteer:: getDistanceLeft() const{
    return distanceLeft;
};

int DriverVolunteer:: getMaxDistance() const {
    return maxDistance;
};

int DriverVolunteer::getDistancePerStep() const {
    return distancePerStep;
};  

bool DriverVolunteer:: decreaseDistanceLeft() {               //Decrease distanceLeft by distancePerStep,return true if distanceLeft<=0,false otherwise
    distanceLeft = distanceLeft - distancePerStep;
    return (getDistanceLeft() <= 0 );
}; 

bool DriverVolunteer:: hasOrdersLeft() const  {       // always true for driver volunteer
    return true;
};

bool DriverVolunteer::canTakeOrder(const Order &order) const  { // Signal if the volunteer is not busy and the order is within the maxDistance
    if (isBusy() == false) {
        if(order.getOrderDistance() <= getMaxDistance()) {
            return true;
        }
    };
    return false;
};

void DriverVolunteer::acceptOrder(const Order &order)  {     // Assign distanceLeft to order's distance
    activeOrderId = order.getId();
    distanceLeft = order.getOrderDistance();
}; 

void DriverVolunteer::step()  {                  // Decrease distanceLeft by distancePerStep
    // a- not busy
    // b- busy
    // b1- distanse left = distanse left - distanse per step
    // b2- if distanse left <= 0 change transfer activeOrderId to completedOrderId
    if (isBusy()) {
        distanceLeft = distanceLeft - distancePerStep;
        if (distanceLeft <= 0){
            completedOrderId = activeOrderId; 
            activeOrderId = NO_ORDER;
        };
        
    };
};

string DriverVolunteer::toString() const  {
    return "VolunterID: " + std::to_string(getId()) + "\n" + "isBusy: " + (isBusy() ? "true" : "false") + "\n" + 
    "OrderId: " + ((isBusy() == 0) ? "None" : std::to_string(getActiveOrderId())) + "\n" +
     "TimeLeft: " + ((isBusy() == 0) ? "None" : std::to_string(getDistanceLeft())) + "\n" + 
    "ordersLeft: No Limit";
};

// methods we created
 int DriverVolunteer:: type() const{
    return 3;
 };

//------------------ DRIVER VOLUNTEER END----------------//




//----------- LIMITED DRIVER VOLUNTEER START----------------//

LimitedDriverVolunteer:: LimitedDriverVolunteer(int _id, const string &_name, int _maxDistance, int _distancePerStep,int _maxOrders): 
DriverVolunteer(_id,_name,_maxDistance,_distancePerStep), maxOrders(_maxOrders), ordersLeft(_maxOrders) {};

LimitedDriverVolunteer* LimitedDriverVolunteer:: clone() const  {
    return new LimitedDriverVolunteer(*this);
};


int LimitedDriverVolunteer:: getMaxOrders() const {
    return maxOrders;
};


int LimitedDriverVolunteer::getNumOrdersLeft() const { 
    return ordersLeft;
};


bool LimitedDriverVolunteer::hasOrdersLeft() const  {
    return !(getNumOrdersLeft() == 0) ;
};


bool LimitedDriverVolunteer::canTakeOrder(const Order &order) const  {
    if (isBusy() == false) {
        if((ordersLeft > 0) && (order.getOrderDistance() <= getMaxDistance())) {
            return true;
        }
    };
    return false;
}; // Signal if the volunteer is not busy, the order is within the maxDistance.


void LimitedDriverVolunteer:: acceptOrder(const Order &order)  {
    DriverVolunteer:: acceptOrder(order);      
    ordersLeft = ordersLeft -1;
}; // Assign distanceLeft to order's distance and decrease ordersLeft


string LimitedDriverVolunteer::toString() const  {
    return "VolunterID: " + std::to_string(getId()) + "\n" + "isBusy: " + (isBusy() ? "true" : "false") + "\n" + 
    "OrderId: " + ((isBusy() == 0) ? "None" : std::to_string(getActiveOrderId())) + "\n" + "TimeLeft: " + 
    ((isBusy() == 0) ? "None" : std::to_string(getDistanceLeft())) + "\n" + 
    "ordersLeft: " + std::to_string(getNumOrdersLeft()); 
};

// methods we created
 int LimitedDriverVolunteer::type() const{
    return 4;
 };


//----------------- LIMITED DRIVER VOLUNTEER END--------------------//