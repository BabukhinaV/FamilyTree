#ifndef PERSON_H
#define PERSON_H


#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <windows.h>
#include <unordered_set>
#include <string>
#include "observable.h"

class Person : public Observable {
private:
    std::string firstName;
    std::string lastName;
    std::string middleName;
    std::string gender;
    std::string birthPlace;
    std::string deathPlace;
    std::string occupation;
    bool alive;

public:
    Person(std::string fName, std::string lName, std::string mName,
        std::string gender, std::string bPlace, std::string occupation);

    // Геттеры
    std::string getFullName() const;
    std::string getShortName() const;
    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getMiddleName() const;
    std::string getGender() const;
    std::string getBirthPlace() const;
    std::string getDeathPlace() const;
    std::string getOccupation() const;
    bool isAlive() const;

    // Сеттеры
    void setDeath(std::string place);
    void printInfo() const;
};

#endif // PERSON_H