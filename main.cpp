#include <iostream>
#include <string>
#include <memory>
#include <Windows.h>
#include <regex>
#include "GenealogicalTree.h"
#include "FamilyEvent.h"
#include "LoggerObserver.h"

// === Date validation function ===
bool isValidDate(const std::string& date) {
    std::regex datePattern(R"((0[1-9]|[12][0-9]|3[01])\.(0[1-9]|1[012])\.\d{4})");
    return std::regex_match(date, datePattern);
}

// === Gender validation function ===
bool isValidGender(const std::string& gender) {
    return gender == "мужской" || gender == "женский";
}

// === Меню ===
void printMenu() {
    std::cout << "\n=== Меню ===" << std::endl;
    std::cout << "1. Все люди" << std::endl;
    std::cout << "2. Инфо о человеке" << std::endl;
    std::cout << "3. Связи человека" << std::endl;
    std::cout << "4. Все связи" << std::endl;
    std::cout << "5. Добавить человека" << std::endl;
    std::cout << "6. Удалить человека" << std::endl;
    std::cout << "7. Добавить связь вручную" << std::endl;
    std::cout << "8. Удалить связь вручную" << std::endl;
    std::cout << "9. Вызвать событие (рождение, смерть, брак, развод)" << std::endl;
    std::cout << "10. Сохранить в файл" << std::endl;
    std::cout << "0. Выход\nВыберите: ";
}

void printRelationshipTypes() {
    std::cout << "Типы связей:" << std::endl;
    std::cout << "1. Родитель-ребенок" << std::endl;
    std::cout << "2. Супруг/супруга" << std::endl;
    std::cout << "3. Брат/сестра" << std::endl;
    std::cout << "4. Бывший супруг/супруга" << std::endl;
    std::cout << "Выберите тип связи: ";
}

std::shared_ptr<Person> selectPerson(const GenealogicalTree& tree) {
    tree.printAllMembers();
    std::cout << "Введите номер человека (0 для отмены): ";
    int choice;
    std::cin >> choice;
    std::cin.ignore();

    if (choice == 0) return nullptr;

    const auto& members = tree.getFamilyMembers();
    if (choice > 0 && choice <= static_cast<int>(members.size())) {
        return members[choice - 1];
    }
    return nullptr;
}

void triggerEvent(std::shared_ptr<GenealogicalTree> tree) {
    std::cout << "\nВыберите событие:\n";
    std::cout << "1. Рождение\n2. Смерть\n3. Брак\n4. Развод\nВыбор: ";
    int ch;
    std::cin >> ch;
    std::cin.ignore();

    if (ch == 1) {
        auto mother = selectPerson(*tree);
        auto father = selectPerson(*tree);
        if (!mother || !father) return;

        std::string fn, ln, mn, g, bPlace, bDate, occ, bio;
        std::cout << "Имя ребёнка: "; std::getline(std::cin, fn);
        std::cout << "Фамилия: "; std::getline(std::cin, ln);
        std::cout << "Отчество: "; std::getline(std::cin, mn);
        do{
            std::cout << "Введите пол (мужской/женский): ";
            std::getline(std::cin, g);
            if (!isValidGender(g)) {
                std::cout << "Ошибка: пол должен быть 'мужской' или 'женский'.\n";
        }
        } while (!isValidGender(g));
        std::cout << "Место рождения: "; std::getline(std::cin, bPlace);
        std::cout << "Дата рождения: "; std::getline(std::cin, bDate);
        do {
            std::cout << "Введите дату рождения (DD.MM.YYYY): ";
            std::getline(std::cin, bDate);
            if (!isValidDate(bDate)) {
                std::cout << "Ошибка: дата должна быть в формате DD.MM.YYYY (например, 01.01.2000).\n";
            }
        } while (!isValidDate(bDate));
        std::cout << "Профессия: "; std::getline(std::cin, occ);
        std::cout << "Биография (необязательно): "; std::getline(std::cin, bio);

        BirthEvent event(tree, mother, father, fn, ln, mn, g, bPlace, bDate, occ, bio);
        event.execute();
    }
    else if (ch == 2) {
        auto person = selectPerson(*tree);
        if (!person) return;
        std::string place;
        std::cout << "Место смерти: ";
        std::getline(std::cin, place);
        DeathEvent event(tree, person, place);
        event.execute();
    }
    else if (ch == 3 || ch == 4) {
        auto p1 = selectPerson(*tree);
        auto p2 = selectPerson(*tree);
        if (!p1 || !p2) return;
        if (ch == 3) {
            MarriageEvent event(tree, p1, p2);
            event.execute();
        }
        else {
            DivorceEvent event(tree, p1, p2);
            event.execute();
        }
    }
}

void addPerson(GenealogicalTree& tree) {
    std::string firstName, lastName, middleName, gender, birthPlace, birthDate, occupation, bio;

    std::cout << "Введите имя: ";
    std::getline(std::cin, firstName);
    std::cout << "Введите фамилию: ";
    std::getline(std::cin, lastName);
    std::cout << "Введите отчество: ";
    std::getline(std::cin, middleName);
    do {
        std::cout << "Введите пол (мужской/женский): ";
        std::getline(std::cin, gender);
        if (!isValidGender(gender)) {
            std::cout << "Ошибка: пол должен быть 'мужской' или 'женский'.\n";
        }
    } while (!isValidGender(gender));
    std::cout << "Введите место рождения: ";
    std::getline(std::cin, birthPlace);
    do {
        std::cout << "Введите дату рождения (DD.MM.YYYY): ";
        std::getline(std::cin, birthDate);
        if (!isValidDate(birthDate)) {
            std::cout << "Ошибка: дата должна быть в формате DD.MM.YYYY (например, 01.01.2000).\n";
        }
    } while (!isValidDate(birthDate));
    std::cout << "Введите профессию: ";
    std::getline(std::cin, occupation);
    std::cout << "Введите биографию (необязательно): ";
    std::getline(std::cin, bio);

    auto person = std::make_shared<Person>(firstName, lastName, middleName, gender,
        birthPlace, birthDate, occupation, bio);
    tree.addFamilyMember(person);
    std::cout << "Человек успешно добавлен: " << person->getFullName() << std::endl;
}

void removePerson(GenealogicalTree& tree) {
    auto person = selectPerson(tree);
    if (!person) return;

    auto relSystem = tree.getRelationshipSystem();
    auto relationships = relSystem->getAllRelationships();

    for (const auto& rel : relationships) {
        if (std::get<0>(rel) == person || std::get<1>(rel) == person) {
            relSystem->removeRelationship(std::get<0>(rel), std::get<1>(rel), std::get<2>(rel));
        }
    }

    auto& members = const_cast<std::vector<std::shared_ptr<Person>>&>(tree.getFamilyMembers());
    members.erase(std::remove(members.begin(), members.end(), person), members.end());

    std::cout << "Человек успешно удален: " << person->getFullName() << std::endl;
}

void addRelationship(GenealogicalTree& tree) {
    auto person1 = selectPerson(tree);
    if (!person1) return;

    auto person2 = selectPerson(tree);
    if (!person2) return;

    printRelationshipTypes();
    int choice;
    std::cin >> choice;
    std::cin.ignore();

    std::string relationType;
    switch (choice) {
    case 1: relationType = "parent-child"; break;
    case 2: relationType = "spouse"; break;
    case 3: relationType = "sibling"; break;
    case 4: relationType = "ex-spouse"; break;
    default:
        std::cout << "Неверный выбор." << std::endl;
        return;
    }

    tree.getRelationshipSystem()->addRelationship(person1, person2, relationType);
    std::cout << "Связь успешно добавлена: " << person1->getFullName() << " -> "
        << person2->getFullName() << " (" << relationType << ")" << std::endl;
}

void removeRelationship(GenealogicalTree& tree) {
    auto person1 = selectPerson(tree);
    if (!person1) return;

    auto person2 = selectPerson(tree);
    if (!person2) return;

    auto relationships = tree.getRelationshipSystem()->getRelationshipsFor(person1);
    std::vector<std::string> availableRelations;

    for (const auto& rel : relationships) {
        if (std::get<0>(rel) == person2) {
            availableRelations.push_back(std::get<1>(rel));
        }
    }

    if (availableRelations.empty()) {
        std::cout << "Между этими людьми нет связей." << std::endl;
        return;
    }

    std::cout << "Доступные связи:" << std::endl;
    for (size_t i = 0; i < availableRelations.size(); ++i) {
        std::cout << i + 1 << ". " << availableRelations[i] << std::endl;
    }

    std::cout << "Выберите связь для удаления: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore();

    if (choice > 0 && choice <= static_cast<int>(availableRelations.size())) {
        tree.getRelationshipSystem()->removeRelationship(person1, person2, availableRelations[choice - 1]);
        std::cout << "Связь успешно удалена." << std::endl;
    }
    else {
        std::cout << "Неверный выбор." << std::endl;
    }
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    auto tree = std::make_shared<GenealogicalTree>();
    auto logger = std::make_shared<LoggerObserver>();

    tree->loadFromFile("family_data.txt");

    int choice;

    do {
        printMenu();
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
        case 1: tree->printAllMembers(); break;
        case 2: {
            auto p = selectPerson(*tree);
            if (p) p->printInfo();
            break;
        }
        case 3: {
            auto p = selectPerson(*tree);
            if (p) tree->printRelationships(p->getFullName());
            break;
        }
        case 4: tree->printAllRelationships(); break;
        case 5: addPerson(*tree); tree->saveToFile("family_data.txt"); break;
        case 6: removePerson(*tree); tree->saveToFile("family_data.txt"); break;
        case 7: addRelationship(*tree); tree->saveToFile("family_data.txt"); break;
        case 8: removeRelationship(*tree); tree->saveToFile("family_data.txt"); break;
        case 9: triggerEvent(tree); tree->saveToFile("family_data.txt"); break;
        case 10: tree->saveToFile("family_data.txt"); break;
        case 0: std::cout << "Пока!" << std::endl; break;
        default: std::cout << "Неверный выбор." << std::endl;
        }

    } while (choice != 0);

    return 0;
}
