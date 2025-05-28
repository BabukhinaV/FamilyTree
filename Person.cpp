#include "Person.h"
#include <iostream>
#include <sstream>
#include <vector>

Person::Person(std::string fName, std::string lName, std::string mName, std::string gender,
    std::string bPlace, std::string bDate, std::string occupation, std::string bio)
    : firstName(fName), lastName(lName), middleName(mName), gender(gender),
    birthPlace(bPlace), birthDate(bDate), occupation(occupation), biography(bio),
    deathPlace(""), alive(true) {
}

std::string Person::getFullName() const {
    return lastName + " " + firstName + " " + middleName;
}

std::string Person::getFirstName() const { return firstName; }
std::string Person::getLastName() const { return lastName; }
std::string Person::getMiddleName() const { return middleName; }
std::string Person::getGender() const { return gender; }
std::string Person::getBirthPlace() const { return birthPlace; }
std::string Person::getDeathPlace() const { return deathPlace; }
std::string Person::getOccupation() const { return occupation; }
std::string Person::getBirthDate() const { return birthDate; }
std::string Person::getBiography() const { return biography; }
bool Person::isAlive() const { return alive; }

void Person::setDeath(std::string place) {
    deathPlace = place;
    alive = false;
    notifyPersonChanged(*this, "death");
}

void Person::printInfo() const {
    std::cout << "ФИО: " << getFullName() << std::endl;
    std::cout << "Пол: " << gender << std::endl;
    std::cout << "Место рождения: " << birthPlace << std::endl;
    if (!alive) {
        std::cout << "Место смерти: " << deathPlace << std::endl;
    }
    std::cout << "Профессия: " << occupation << std::endl;
    std::cout << "Статус: " << (alive ? "Жив" : "Умер") << std::endl;
    std::cout << "Дата рождения: " << birthDate << std::endl;
    if (!biography.empty())
        std::cout << "Биография: " << biography << std::endl;
}

std::string Person::serialize() const {
    return firstName + "|" + lastName + "|" + middleName + "|" + gender + "|" +
        birthPlace + "|" + birthDate + "|" + deathPlace + "|" + occupation + "|" +
        (alive ? "1" : "0") + "|" + biography;
}

std::shared_ptr<Person> Person::deserialize(const std::string& data) {
    std::vector<std::string> fields;
    std::stringstream ss(data);
    std::string field;

    while (std::getline(ss, field, '|')) {
        fields.push_back(field);
    }

    if (fields.size() < 8) return nullptr;

    auto person = std::make_shared<Person>(
        fields[0], // firstName
        fields[1], // lastName
        fields[2], // middleName
        fields[3], // gender
        fields[4], // birthPlace
        fields[5], // birthDate
        fields[7], // occupation
        fields.size() > 9 ? fields[9] : "" // biography
    );

    if (!fields[6].empty()) {
        person->deathPlace = fields[6];
    }
    if (fields.size() > 8) {
        person->alive = fields[8] == "1";
    }

    return person;
}
