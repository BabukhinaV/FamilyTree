#pragma once

#include "Observer.h"

class LoggerObserver : public IObserver {
public:
    void onPersonChanged(const Person& person, const std::string& changeType) override;
    void onRelationshipChanged(const Relationship& rel,
        const std::shared_ptr<Person>& p1,
        const std::shared_ptr<Person>& p2,
        const std::string& changeType) override;
};
