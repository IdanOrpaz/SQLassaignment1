# define some Makefile variables for the compiler and compiler flags

# to use Makefile variables later in the Makefile: $()

CC = g++

CFLAGS = -g -Wall -Weffc++ -std=c++11

LFLAGS = -L/usr/lib


# All Targets

all: warehouse


# Tool invocations

# Executable "warehouse" depends on the files Customer.o , WareHouse.o , Order.o , Action.o , Volunteer.o , main.o

warehouse: clean bin/Customer.o bin/WareHouse.o bin/Order.o bin/Action.o bin/Volunteer.o bin/main.o

	@echo 'Building target: warehouse'

	@echo 'Invoking: C++ Linker'

	$(CC) -o bin/warehouse bin/Customer.o bin/WareHouse.o bin/Order.o bin/Action.o bin/Volunteer.o bin/main.o $(LFLAGS)

	@echo 'Finished building target: testCustomer'

	@echo ' '


# Depends on the source and header files

bin/Customer.o: src/Customer.cpp

	$(CC) $(CFLAGS) -c -Iinclude -o bin/Customer.o src/Customer.cpp


# Depends on the source and header files

bin/WareHouse.o: src/WareHouse.cpp

	$(CC) $(CFLAGS) -c -Iinclude -o bin/WareHouse.o src/WareHouse.cpp



# Depends on the source and header files

bin/Order.o: src/Order.cpp

	$(CC) $(CFLAGS) -c -Iinclude -o bin/Order.o src/Order.cpp


# Depends on the source and header files

bin/Action.o: src/Action.cpp

	$(CC) $(CFLAGS) -c -Iinclude -o bin/Action.o src/Action.cpp


# Depends on the source and header files

bin/Volunteer.o: src/Volunteer.cpp

	$(CC) $(CFLAGS) -c -Iinclude -o bin/Volunteer.o src/Volunteer.cpp


# Depends on the source and header files

bin/main.o: src/main.cpp

	$(CC) $(CFLAGS) -c -Iinclude -o bin/main.o src/main.cpp


#Clean the build directory

clean:

	rm -f bin/*