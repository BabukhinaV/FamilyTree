#include "Observable.h"
#include "Observer.h"
#include "Person.h"
#include "Relationship.h"

void Observable::addObserver(const std::shared_ptr<IObserver>& observer) {
    observers.insert(observer);
}

void Observable::removeObserver(const std::shared_ptr<IObserver>& observer) {
    observers.erase(observer);
}

void Observable::notifyPersonChanged(const Person& person, const std::string& changeType) {
    for (const auto& observer : observers) {
        observer->onPersonChanged(person, changeType);
    }
}

void Observable::notifyRelationshipChanged(const Relationship& rel,
    const std::shared_ptr<Person>& p1,
    const std::shared_ptr<Person>& p2,
    const std::string& changeType) {
    for (const auto& observer : observers) {
        observer->onRelationshipChanged(rel, p1, p2, changeType);
    }
}
