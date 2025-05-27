#include "observer.h"
#include <iostream>

void LoggerObserver::onPersonChanged(const Person& person, const std::string& changeType) {
    std::cout << "[Лог] Изменение в Person: " << person.getFullName()
        << ", тип изменения: " << changeType << std::endl;
}

void LoggerObserver::onRelationshipChanged(const Relationship& rel,
    const std::shared_ptr<Person>& p1,
    const std::shared_ptr<Person>& p2,
    const std::string& changeType) {
    std::cout << "[Лог] Изменение в Relationship: "
        << p1->getFullName() << " -> " << p2->getFullName()
        << ", тип изменения: " << changeType << std::endl;
}