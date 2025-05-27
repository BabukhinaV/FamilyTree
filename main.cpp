#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <windows.h>
#include <unordered_set>
#include <functional> 
#include <unordered_map> 
#include <memory> 
#include <tuple>

// Предварительное объявление
class GenealogicalTree;
using namespace std;

// Предварительные объявления
class Person;
class Relationship;

// Базовый класс наблюдателя
class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void onPersonChanged(const Person& person, const string& changeType) = 0;
    virtual void onRelationshipChanged(const Relationship& rel,
        const shared_ptr<Person>& p1,
        const shared_ptr<Person>& p2,
        const string& changeType) = 0;
};

// Базовый класс наблюдаемого объекта
class Observable {
protected:
    unordered_set<shared_ptr<IObserver>> observers;

public:
    void addObserver(const shared_ptr<IObserver>& observer) {
        observers.insert(observer);
    }

    void removeObserver(const shared_ptr<IObserver>& observer) {
        observers.erase(observer);
    }

    void notifyPersonChanged(const Person& person, const string& changeType);
    void notifyRelationshipChanged(const Relationship& rel,
        const shared_ptr<Person>& p1,
        const shared_ptr<Person>& p2,
        const string& changeType);
};

// Класс человека
class Person : public Observable {
private:
    string firstName;
    string lastName;
    string middleName;
    string gender;
    string birthPlace;
    string deathPlace;
    string occupation;
    bool alive;

public:
    Person(string fName, string lName, string mName, string gender,
        string bPlace, string occupation)
        : firstName(fName), lastName(lName), middleName(mName), gender(gender),
        birthPlace(bPlace), deathPlace(""), occupation(occupation), alive(true) {
    }

    // Геттеры
    string getFullName() const { return lastName + " " + firstName + " " + middleName; }
    string getFirstName() const { return firstName; }
    string getLastName() const { return lastName; }
    string getMiddleName() const { return middleName; }
    string getGender() const { return gender; }
    string getBirthPlace() const { return birthPlace; }
    string getDeathPlace() const { return deathPlace; }
    string getOccupation() const { return occupation; }
    bool isAlive() const { return alive; }

    string getShortName() const {
        return lastName + " " + firstName[0] + "." + middleName[0] + ".";
    }

    // Сеттеры
    void setDeath(string place) {
        deathPlace = place;
        alive = false;
        notifyPersonChanged(*this, "death");
    }

    void printInfo() const {
        cout << "ФИО: " << getFullName() << endl;
        cout << "Пол: " << gender << endl;
        cout << "Место рождения: " << birthPlace << endl;
        if (!alive) {
            cout << "Место смерти: " << deathPlace << endl;
        }
        cout << "Профессия: " << occupation << endl;
        cout << "Статус: " << (alive ? "Жив" : "Умер") << endl;
    }
};

// Реализация методов Observable
void Observable::notifyPersonChanged(const Person& person, const string& changeType) {
    for (const auto& observer : observers) {
        observer->onPersonChanged(person, changeType);
    }
}

void Observable::notifyRelationshipChanged(const Relationship& rel,
    const shared_ptr<Person>& p1,
    const shared_ptr<Person>& p2,
    const string& changeType) {
    for (const auto& observer : observers) {
        observer->onRelationshipChanged(rel, p1, p2, changeType);
    }
}

// Класс отношений
class Relationship : public Observable {
private:
    vector<tuple<shared_ptr<Person>, shared_ptr<Person>, string>> relationships;

public:
    // Добавление отношения
    void addRelationship(shared_ptr<Person> person1, shared_ptr<Person> person2, string relationType) {
        relationships.emplace_back(person1, person2, relationType);
        notifyRelationshipChanged(*this, person1, person2, "add:" + relationType);

        // Автоматически добавляем обратное отношение
        if (relationType == "parent-child") {
            relationships.emplace_back(person2, person1, "child-parent");
            notifyRelationshipChanged(*this, person2, person1, "add:child-parent");
        }
        else if (relationType == "sibling") {
            relationships.emplace_back(person2, person1, "sibling");
            notifyRelationshipChanged(*this, person2, person1, "add:sibling");
        }
        else if (relationType == "spouse") {
            relationships.emplace_back(person2, person1, "spouse");
            notifyRelationshipChanged(*this, person2, person1, "add:spouse");
        }
    }

    // Удаление отношения
    void removeRelationship(shared_ptr<Person> person1, shared_ptr<Person> person2, string relationType) {
        relationships.erase(
            remove_if(relationships.begin(), relationships.end(),
                [&](const auto& rel) {
                    return get<0>(rel) == person1 && get<1>(rel) == person2 && get<2>(rel) == relationType;
                }),
            relationships.end());
        notifyRelationshipChanged(*this, person1, person2, "remove:" + relationType);

        // Удаляем обратное отношение
        if (relationType == "spouse") {
            relationships.erase(
                remove_if(relationships.begin(), relationships.end(),
                    [&](const auto& rel) {
                        return get<0>(rel) == person2 && get<1>(rel) == person1 && get<2>(rel) == "spouse";
                    }),
                relationships.end());
            notifyRelationshipChanged(*this, person2, person1, "remove:spouse");
        }
    }

    // Получение всех отношений для человека
    vector<tuple<shared_ptr<Person>, string>> getRelationshipsFor(shared_ptr<Person> person) const {
        vector<tuple<shared_ptr<Person>, string>> result;
        for (const auto& rel : relationships) {
            if (get<0>(rel) == person) {
                result.emplace_back(get<1>(rel), get<2>(rel));
            }
        }
        return result;
    }
};

// Класс узла дерева для отображения (Компоновщик)
class TreeNode {
public:
    virtual ~TreeNode() = default;
    virtual void print(int level = 0) const = 0;
    virtual void addChild(std::shared_ptr<TreeNode> child) {}
    virtual std::vector<std::shared_ptr<TreeNode>> getChildren() const { return {}; }
};

// Класс для отображения человека
class PersonNode : public TreeNode {
private:
    shared_ptr<Person> person;
    vector<shared_ptr<TreeNode>> children;

public:
    PersonNode(shared_ptr<Person> p) : person(p) {}

    void print(int level = 0) const override {
        cout << string(level * 4, ' ') << person->getLastName() << " "
            << person->getFirstName()[0] << "." << person->getMiddleName()[0] << ".";
    }

    void addChild(shared_ptr<TreeNode> child) override {
        children.push_back(child);
    }

    vector<shared_ptr<TreeNode>> getChildren() const override {
        return children;
    }

    shared_ptr<Person> getPerson() const { return person; }
};

// Класс для отображения пары (супруги)
class CoupleNode : public TreeNode {
private:
    shared_ptr<PersonNode> person1;
    shared_ptr<PersonNode> person2;
    vector<shared_ptr<TreeNode>> children;

public:
    CoupleNode(shared_ptr<PersonNode> p1, shared_ptr<PersonNode> p2)
        : person1(p1), person2(p2) {
    }

    void print(int level = 0) const override {
        person1->print(level);
        cout << " + ";
        person2->print(0);
    }

    void addChild(shared_ptr<TreeNode> child) override {
        children.push_back(child);
    }

    vector<shared_ptr<TreeNode>> getChildren() const override {
        return children;
    }
};

// Класс для построения дерева отношений
class TreeBuilder {
private:
    std::shared_ptr<GenealogicalTree> tree;
    std::unordered_map<std::shared_ptr<Person>, std::shared_ptr<PersonNode>> personToNode;

    std::shared_ptr<PersonNode> getOrCreateNode(std::shared_ptr<Person> person) {
        if (personToNode.find(person) == personToNode.end()) {
            personToNode[person] = std::make_shared<PersonNode>(person);
        }
        return personToNode[person];
    }

    std::shared_ptr<TreeNode> buildSubtree(std::shared_ptr<Person> person, std::unordered_set<std::shared_ptr<Person>>& visited) {
        if (visited.count(person)) return nullptr;
        visited.insert(person);

        auto relSystem = tree->getRelationshipSystem();
        auto relationships = relSystem->getRelationshipsFor(person);

        // Находим супруга
        std::shared_ptr<Person> spouse;
        for (const auto& rel : relationships) {
            if (std::get<1>(rel) == "spouse") {
                spouse = std::get<0>(rel);
                break;
            }
        }

        std::shared_ptr<TreeNode> node;
        if (spouse) {
            auto personNode = getOrCreateNode(person);
            auto spouseNode = getOrCreateNode(spouse);
            auto coupleNode = std::make_shared<CoupleNode>(personNode, spouseNode);
            node = coupleNode;
        }
        else {
            node = getOrCreateNode(person);
        }

        // Находим детей
        std::vector<std::shared_ptr<Person>> children;
        for (const auto& rel : relationships) {
            if (std::get<1>(rel) == "child-parent") {
                children.push_back(std::get<0>(rel));
            }
        }

        // Сортируем детей по имени
        std::sort(children.begin(), children.end(),
            [](const auto& a, const auto& b) {
                return a->getFullName() < b->getFullName();
            });

        // Рекурсивно строим поддеревья для детей
        for (const auto& child : children) {
            auto childNode = buildSubtree(child, visited);
            if (childNode) {
                node->addChild(childNode);
            }
        }

        return node;
    }

public:
    TreeBuilder(std::shared_ptr<GenealogicalTree> t) : tree(t) {}

    std::shared_ptr<TreeNode> buildTree() {
        // Находим корни (людей без родителей)
        std::unordered_set<std::shared_ptr<Person>> roots;
        for (const auto& person : tree->familyMembers) {
            bool hasParents = false;
            auto relationships = tree->getRelationshipSystem()->getRelationshipsFor(person);
            for (const auto& rel : relationships) {
                if (std::get<1>(rel) == "parent-child") {
                    hasParents = true;
                    break;
                }
            }
            if (!hasParents) {
                roots.insert(person);
            }
        }

        std::unordered_set<std::shared_ptr<Person>> visited;
        if (roots.size() == 1) {
            return buildSubtree(*roots.begin(), visited);
        }
        else if (roots.size() > 1) {
            auto root = std::make_shared<PersonNode>(nullptr);
            for (const auto& person : roots) {
                auto subtree = buildSubtree(person, visited);
                if (subtree) {
                    root->addChild(subtree);
                }
            }
            return root;
        }
        return nullptr;
    }
};
// Класс генеалогического древа (только хранение и вывод информации)
class GenealogicalTree : public enable_shared_from_this<GenealogicalTree> {
private:
    vector<shared_ptr<Person>> familyMembers;
    shared_ptr<Relationship> relationshipSystem;

public:
    GenealogicalTree() : relationshipSystem(make_shared<Relationship>()) {}
    GenealogicalTree() : relationshipSystem(std::make_shared<Relationship>()) {}

    friend class TreeBuilder;

    // Добавление нового члена семьи
    void addFamilyMember(shared_ptr<Person> person) {
        familyMembers.push_back(person);
    }

    // Получение системы отношений
    shared_ptr<Relationship> getRelationshipSystem() const {
        return relationshipSystem;
    }

    // Поиск человека по ФИО
    shared_ptr<Person> findPerson(const string& fullName) const {
        for (const auto& person : familyMembers) {
            if (person->getFullName() == fullName) {
                return person;
            }
        }
        return nullptr;
    }

    // Вывод информации о человеке
    void printPersonInfo(const string& fullName) const {
        auto person = findPerson(fullName);
        if (person) {
            person->printInfo();
        }
        else {
            cout << "Человек с именем " << fullName << " не найден в древе." << endl;
        }
    }

    // Вывод всех членов семьи
    void printAllMembers() const {
        cout << "=== Все члены семьи ===" << endl;
        for (const auto& person : familyMembers) {
            cout << "- " << person->getFullName() << endl;
        }
    }

    // Вывод родственных связей для человека
    void printRelationships(const string& fullName) const {
        auto person = findPerson(fullName);
        if (!person) {
            cout << "Человек не найден." << endl;
            return;
        }

        cout << "=== Родственные связи для " << fullName << " ===" << endl;
        auto relationships = relationshipSystem->getRelationshipsFor(person);

        if (relationships.empty()) {
            cout << "Связи не найдены." << endl;
            return;
        }

        for (const auto& rel : relationships) {
            string relationType;
            if (get<1>(rel) == "parent-child") {
                relationType = "родитель";
            }
            else if (get<1>(rel) == "child-parent") {
                relationType = "ребенок";
            }
            else if (get<1>(rel) == "sibling") {
                relationType = "брат/сестра";
            }
            else if (get<1>(rel) == "spouse") {
                relationType = "супруг(а)";
            }
            else if (get<1>(rel) == "ex-spouse") {
                relationType = "бывший(ая) супруг(а)";
            }
            else {
                relationType = get<1>(rel);
            }

            cout << "- " << get<0>(rel)->getFullName() << " (" << relationType << ")" << endl;
        }

    }

    void printTree() const {
        std::cout << "=== Генеалогическое древо ===" << std::endl;
        auto self = const_cast<GenealogicalTree*>(this)->shared_from_this();
        TreeBuilder builder(self);
        auto root = builder.buildTree();

        if (!root) {
            std::cout << "Дерево пустое." << std::endl;
            return;
        }

        // Лямбда для рекурсивного вывода
        std::function<void(const std::shared_ptr<TreeNode>&, int, std::vector<bool>)> printNode =
            [&](const std::shared_ptr<TreeNode>& node, int level, std::vector<bool> lastFlags) {
            // Вывод префикса
            for (int i = 0; i < level - 1; ++i) {
                if (i < lastFlags.size() && lastFlags[i]) {
                    std::cout << "    ";
                }
                else {
                    std::cout << "|   ";
                }
            }

            if (level > 0) {
                if (lastFlags.empty() || lastFlags.back()) {
                    std::cout << "\\--- ";
                }
                else {
                    std::cout << "|--- ";
                }
            }

            // Вывод узла
            node->print(0);
            std::cout << std::endl;

            // Рекурсивный вывод детей
            auto children = node->getChildren();
            for (size_t i = 0; i < children.size(); ++i) {
                auto newFlags = lastFlags;
                newFlags.push_back(i == children.size() - 1);
                printNode(children[i], level + 1, newFlags);
            }
            };

        printNode(root, 0, {});
    }
};

// Базовый класс семейного события
class FamilyEvent {
protected:
    shared_ptr<GenealogicalTree> tree;

public:
    FamilyEvent(shared_ptr<GenealogicalTree> tree) : tree(tree) {}
    virtual ~FamilyEvent() = default;
    virtual void execute() = 0;
};

// Событие рождения
class BirthEvent : public FamilyEvent {
private:
    shared_ptr<Person> mother;
    shared_ptr<Person> father;
    shared_ptr<Person> child;

public:
    BirthEvent(shared_ptr<GenealogicalTree> tree,
        shared_ptr<Person> mother,
        shared_ptr<Person> father,
        const string& fName, const string& lName, const string& mName,
        const string& gender, const string& bPlace, const string& occupation)
        : FamilyEvent(tree), mother(mother), father(father),
        child(make_shared<Person>(fName, lName, mName, gender, bPlace, occupation)) {
    }

    void execute() override {
        auto relSystem = tree->getRelationshipSystem();

        // Добавляем ребенка в древо
        tree->addFamilyMember(child);

        // Устанавливаем отношения родитель-ребенок
        relSystem->addRelationship(mother, child, "parent-child");
        relSystem->addRelationship(father, child, "parent-child");

        // Добавляем братские отношения с другими детьми
        auto motherChildren = relSystem->getRelationshipsFor(mother);
        for (const auto& rel : motherChildren) {
            if (get<1>(rel) == "parent-child" && get<0>(rel) != child) {
                relSystem->addRelationship(get<0>(rel), child, "sibling");
            }
        }

        cout << "Зарегистрировано рождение: " << child->getFullName() << endl;
    }
};

// Событие смерти
class DeathEvent : public FamilyEvent {
private:
    shared_ptr<Person> person;
    string deathPlace;

public:
    DeathEvent(shared_ptr<GenealogicalTree> tree,
        shared_ptr<Person> person,
        const string& deathPlace)
        : FamilyEvent(tree), person(person), deathPlace(deathPlace) {
    }

    void execute() override {
        person->setDeath(deathPlace);
        cout << "Зарегистрирована смерть: " << person->getFullName()
            << ", место: " << deathPlace << endl;
    }
};

// Событие брака
class MarriageEvent : public FamilyEvent {
private:
    shared_ptr<Person> person1;
    shared_ptr<Person> person2;

public:
    MarriageEvent(shared_ptr<GenealogicalTree> tree,
        shared_ptr<Person> person1,
        shared_ptr<Person> person2)
        : FamilyEvent(tree), person1(person1), person2(person2) {
    }

    void execute() override {
        auto relSystem = tree->getRelationshipSystem();
        relSystem->addRelationship(person1, person2, "spouse");
        relSystem->addRelationship(person2, person1, "spouse");
        cout << "Зарегистрирован брак между: " << person1->getFullName()
            << " и " << person2->getFullName() << endl;
    }
};

// Событие развода
class DivorceEvent : public FamilyEvent {
private:
    shared_ptr<Person> person1;
    shared_ptr<Person> person2;

public:
    DivorceEvent(shared_ptr<GenealogicalTree> tree,
        shared_ptr<Person> person1,
        shared_ptr<Person> person2)
        : FamilyEvent(tree), person1(person1), person2(person2) {
    }

    void execute() override {
        auto relSystem = tree->getRelationshipSystem();
        relSystem->removeRelationship(person1, person2, "spouse");
        relSystem->removeRelationship(person2, person1, "spouse");

        // Добавляем отношения "бывший супруг"
        relSystem->addRelationship(person1, person2, "ex-spouse");
        relSystem->addRelationship(person2, person1, "ex-spouse");

        cout << "Зарегистрирован развод между: " << person1->getFullName()
            << " и " << person2->getFullName() << endl;
    }
};

// Класс наблюдателя для логирования
class LoggerObserver : public IObserver {
public:
    void onPersonChanged(const Person& person, const string& changeType) override {
        cout << "[Лог] Изменение в Person: " << person.getFullName()
            << ", тип изменения: " << changeType << endl;
    }

    void onRelationshipChanged(const Relationship& rel,
        const shared_ptr<Person>& p1,
        const shared_ptr<Person>& p2,
        const string& changeType) override {
        cout << "[Лог] Изменение в Relationship: "
            << p1->getFullName() << " -> " << p2->getFullName()
            << ", тип изменения: " << changeType << endl;
    }
};

int main() {
    SetConsoleOutputCP(1251); // Настраиваем консоль для русского текста
    // Создаем древо и наблюдателя
    auto tree = make_shared<GenealogicalTree>();
    auto logger = make_shared<LoggerObserver>();

    // 1. Создаем основателей семьи
    cout << "\n=== Создаем основателей семьи ===" << endl;
    auto ivan = make_shared<Person>("Иван", "Иванов", "Петрович", "мужской",
        "Москва, 1950", "Инженер");
    ivan->addObserver(logger);
    tree->addFamilyMember(ivan);

    auto maria = make_shared<Person>("Мария", "Иванова", "Сергеевна", "женский",
        "Санкт-Петербург, 1952", "Учитель");
    maria->addObserver(logger);
    tree->addFamilyMember(maria);

    // 2. Регистрируем их брак
    cout << "\n=== Регистрируем брак ===" << endl;
    MarriageEvent marriage(tree, ivan, maria);
    marriage.execute();

    // 3. Рождение детей
    cout << "\n=== Рождение детей ===" << endl;
    BirthEvent birth1(tree, maria, ivan, "Алексей", "Иванов", "Иванович",
        "мужской", "Москва, 1975", "Программист");
    birth1.execute();

    BirthEvent birth2(tree, maria, ivan, "Ольга", "Иванова", "Ивановна",
        "женский", "Москва, 1978", "Врач");
    birth2.execute();

    // 4. Алексей женится
    cout << "\n=== Алексей создает свою семью ===" << endl;
    auto alexey = tree->findPerson("Иванов Алексей Иванович");
    auto ekaterina = make_shared<Person>("Екатерина", "Иванова", "Дмитриевна", "женский",
        "Москва, 1980", "Дизайнер");
    ekaterina->addObserver(logger);
    tree->addFamilyMember(ekaterina);

    MarriageEvent marriage2(tree, alexey, ekaterina);
    marriage2.execute();

    // 5. Рождение ребенка у Алексея и Екатерины
    cout << "\n=== Рождение ребенка у Алексея ===" << endl;
    BirthEvent birth3(tree, ekaterina, alexey, "Дмитрий", "Иванов", "Алексеевич",
        "мужской", "Москва, 2005", "Школьник");
    birth3.execute();

    // 6. Выводим информацию
    cout << "\n=== Информация о семье ===" << endl;
    tree->printAllMembers();
    cout << endl;

    cout << "=== Информация об Иване ===" << endl;
    tree->printPersonInfo("Иванов Иван Петрович");
    cout << endl;

    cout << "=== Связи Марии ===" << endl;
    tree->printRelationships("Иванова Мария Сергеевна");
    cout << endl;

    cout << "=== Связи Алексея ===" << endl;
    tree->printRelationships("Иванов Алексей Иванович");
    cout << endl;

    // 7. Регистрируем смерть Ивана
    cout << "\n=== Регистрируем смерть Ивана ===" << endl;
    DeathEvent death(tree, ivan, "Сочи, 2020");
    death.execute();

    // 8. Выводим обновленную информацию
    cout << "\n=== Обновленная информация об Иване ===" << endl;
    tree->printPersonInfo("Иванов Иван Петрович");

    // 9. Развод Алексея и Екатерины
    cout << "\n=== Развод Алексея и Екатерины ===" << endl;
    DivorceEvent divorce(tree, alexey, ekaterina);
    divorce.execute();

    // 10. Проверяем связи после развода
    cout << "\n=== Связи Алексея после развода ===" << endl;
    tree->printRelationships("Иванов Алексей Иванович");

    cout << "\n=== Генеалогическое древо ===" << endl;
    tree->printTree();
    return 0;
}