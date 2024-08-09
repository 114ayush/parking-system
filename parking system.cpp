#include <iostream>
#include <vector>
#include <ctime>
#include <thread>
#include <mutex>

// Class to manage each parking space
class ParkingSpace {
private:
    int id;
    bool isOccupied;
    time_t startTime;

public:
    ParkingSpace(int id) : id(id), isOccupied(false), startTime(0) {}

    int getId() const { return id; }
    bool getIsOccupied() const { return isOccupied; }

    void occupySpace() {
        isOccupied = true;
        startTime = time(0);
    }

    void vacateSpace() {
        isOccupied = false;
        startTime = 0;
    }

    time_t getStartTime() const { return startTime; }
};

// Class to handle user interactions
class User {
public:
    void viewAvailableSpaces(const std::vector<ParkingSpace>& spaces) {
        for (const auto& space : spaces) {
            if (!space.getIsOccupied()) {
                std::cout << "Space " << space.getId() << " is available.\n";
            }
        }
    }

    void reserveSpace(std::vector<ParkingSpace>& spaces, int spaceId) {
        for (auto& space : spaces) {
            if (space.getId() == spaceId && !space.getIsOccupied()) {
                space.occupySpace();
                std::cout << "Space " << spaceId << " has been reserved.\n";
                return;
            }
        }
        std::cout << "Space not available.\n";
    }
};

// Class to handle billing functionality
class Billing {
public:
    double calculateFee(time_t startTime) {
        time_t endTime = time(0);
        double duration = difftime(endTime, startTime) / 60; // Duration in minutes
        return duration * 0.05; // Assume $0.05 per minute
    }

    void generateBill(ParkingSpace& space) {
        if (space.getIsOccupied()) {
            double fee = calculateFee(space.getStartTime());
            std::cout << "Total fee for space " << space.getId() << ": $" << fee << "\n";
            space.vacateSpace();
        } else {
            std::cout << "Space is not occupied.\n";
        }
    }
};

// Class to handle admin operations
class Admin {
public:
    void viewStatistics(const std::vector<ParkingSpace>& spaces) {
        int occupied = 0;
        for (const auto& space : spaces) {
            if (space.getIsOccupied()) occupied++;
        }
        std::cout << "Occupied spaces: " << occupied << "\n";
        std::cout << "Total spaces: " << spaces.size() << "\n";
    }

    void manageSpaces(std::vector<ParkingSpace>& spaces, int spaceId, bool action) {
        for (auto& space : spaces) {
            if (space.getId() == spaceId) {
                if (action) space.occupySpace();
                else space.vacateSpace();
                return;
            }
        }
        std::cout << "Space not found.\n";
    }
};

// Class to integrate all components and manage the system
class ParkingSystem {
private:
    std::vector<ParkingSpace> spaces;
    User user;
    Admin admin;
    Billing billing;
    std::mutex mtx;

public:
    ParkingSystem(int totalSpaces) {
        for (int i = 1; i <= totalSpaces; ++i) {
            spaces.emplace_back(i);
        }
    }

    void userInterface() {
        while (true) {
            std::cout << "1. View available spaces\n2. Reserve a space\n3. Exit\n";
            int choice;
            std::cin >> choice;

            if (choice == 1) {
                user.viewAvailableSpaces(spaces);
            } else if (choice == 2) {
                int spaceId;
                std::cout << "Enter space ID: ";
                std::cin >> spaceId;
                user.reserveSpace(spaces, spaceId);
            } else if (choice == 3) {
                break;
            } else {
                std::cout << "Invalid choice.\n";
            }
        }
    }

    void adminInterface() {
        while (true) {
            std::cout << "1. View statistics\n2. Manage spaces\n3. Exit\n";
            int choice;
            std::cin >> choice;

            if (choice == 1) {
                admin.viewStatistics(spaces);
            } else if (choice == 2) {
                int spaceId;
                bool action;
                std::cout << "Enter space ID: ";
                std::cin >> spaceId;
                std::cout << "Enter 1 to occupy, 0 to vacate: ";
                std::cin >> action;
                admin.manageSpaces(spaces, spaceId, action);
            } else if (choice == 3) {
                break;
            } else {
                std::cout << "Invalid choice.\n";
            }
        }
    }

    void billingInterface() {
        int spaceId;
        std::cout << "Enter space ID to generate bill: ";
        std::cin >> spaceId;

        for (auto& space : spaces) {
            if (space.getId() == spaceId) {
                billing.generateBill(space);
                return;
            }
        }
        std::cout << "Space not found.\n";
    }

    void startSystem() {
        std::thread userThread(&ParkingSystem::userInterface, this);
        std::thread adminThread(&ParkingSystem::adminInterface, this);

        userThread.join();
        adminThread.join();
    }
};

// Main function to start the system
int main() {
    ParkingSystem system(10); // 10 parking spaces

    system.startSystem();

    return 0;
}
