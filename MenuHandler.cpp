#include "MenuHandler.h"
#include <iostream>
#include <regex>
#include <algorithm>
#include "FamilyEvent.h"
#include "LoggerObserver.h"

bool MenuHandler::isValidDate(const std::string& date) {
    std::regex datePattern(R"((0[1-9]|[12][0-9]|3[01])\.(0[1-9]|1[012])\.\d{4})");
    return std::regex_match(date, datePattern);
}

bool MenuHandler::isValidGender(const std::string& gender) {
    return gender == "мужской" || gender == "женский";
}

void MenuHandler::printMenu() {
    std::cout << "\n=== Меню ===\n"
        << "1. Все люди\n"
        << "2. Информация о человеке\n"
        << "3. Генеалогическое дерево\n"
        << "4. Все связи\n"
        << "5. Добавить человека\n"
        << "6. Удалить человека\n"
        << "7. Добавить связь вручную\n"
        << "8. Удалить связь вручную\n"
        << "9. Вызвать событие (рождение, смерть, брак, развод)\n"
        << "10. Сохранить в файл\n"
        << "0. Выход\nВыберите: ";
}

void MenuHandler::printRelationshipTypes() {
    std::cout << "Типы связей:\n"
        << "1. Родитель-ребенок\n"
        << "2. Супруг/супруга\n"
        << "3. Брат/сестра\n"
        << "4. Бывший супруг/супруга\n"
        << "Выберите тип связи: ";
}

std::shared_ptr<Person> MenuHandler::selectPerson(const GenealogicalTree& tree) {
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

void MenuHandler::triggerEvent(std::shared_ptr<GenealogicalTree> tree) {
    std::cout << "\nВыберите событие:\n"
        << "1. Рождение\n2. Смерть\n3. Брак\n4. Развод\nВыбор: ";
    int ch;
    std::cin >> ch;
    std::cin.ignore();

    if (ch == 1) {
        auto mother = selectPerson(*tree);
        std::cout << "\n";
        auto father = selectPerson(*tree);
        std::cout << "\n";
        if (!mother || !father) return;

        std::string fn, ln, mn, g, bPlace, bDate, occ, bio;
        std::cout << "Имя ребёнка: "; std::getline(std::cin, fn);
        std::cout << "Фамилия: "; std::getline(std::cin, ln);
        std::cout << "Отчество: "; std::getline(std::cin, mn);
        do {
            std::cout << "Введите пол (мужской/женский): ";
            std::getline(std::cin, g);
        } while (!isValidGender(g));

        std::cout << "Место рождения: "; std::getline(std::cin, bPlace);
        do {
            std::cout << "Введите дату рождения (DD.MM.YYYY): ";
            std::getline(std::cin, bDate);
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
        std::cout << "Место смерти: "; std::getline(std::cin, place);
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

void MenuHandler::addPerson(GenealogicalTree& tree) {
    std::string firstName, lastName, middleName, gender, birthPlace, birthDate, occupation, bio;
    std::cout << "Введите имя: "; std::getline(std::cin, firstName);
    std::cout << "Введите фамилию: "; std::getline(std::cin, lastName);
    std::cout << "Введите отчество: "; std::getline(std::cin, middleName);
    do {
        std::cout << "Введите пол (мужской/женский): ";
        std::getline(std::cin, gender);
    } while (!isValidGender(gender));

    std::cout << "Введите место рождения: "; std::getline(std::cin, birthPlace);
    do {
        std::cout << "Введите дату рождения (DD.MM.YYYY): ";
        std::getline(std::cin, birthDate);
    } while (!isValidDate(birthDate));
    std::cout << "Введите профессию: "; std::getline(std::cin, occupation);
    std::cout << "Введите биографию (необязательно): "; std::getline(std::cin, bio);

    auto person = std::make_shared<Person>(firstName, lastName, middleName, gender, birthPlace, birthDate, occupation, bio);
    tree.addFamilyMember(person);
    std::cout << "Человек успешно добавлен: " << person->getFullName() << "\n\n";
}

void MenuHandler::removePerson(GenealogicalTree& tree) {
    auto person = selectPerson(tree);
    if (!person) return;
    std::cout << "\n";
    auto relSystem = tree.getRelationshipSystem();
    auto relationships = relSystem->getAllRelationships();
    for (const auto& rel : relationships) {
        if (std::get<0>(rel) == person || std::get<1>(rel) == person) {
            relSystem->removeRelationship(std::get<0>(rel), std::get<1>(rel), std::get<2>(rel));
        }
    }
    std::cout << "\n";
    auto& members = const_cast<std::vector<std::shared_ptr<Person>>&>(tree.getFamilyMembers());
    members.erase(std::remove(members.begin(), members.end(), person), members.end());

    std::cout << "Человек успешно удален: " << person->getFullName() << "\n\n";
}

void MenuHandler::addRelationship(GenealogicalTree& tree) {

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
    default: std::cout << "Неверный выбор.\n"; return;
    }
    std::cout << "\n";
    auto person1 = selectPerson(tree);
    if (!person1) return;
    std::cout << "\n";
    auto person2 = selectPerson(tree);
    if (!person2) return;
    std::cout << "\n";
    

    tree.getRelationshipSystem()->addRelationship(person1, person2, relationType);
    std::cout << "Связь успешно добавлена: " << person1->getFullName() << " -> "
        << person2->getFullName() << " (" << relationType << ")\n";
}

void MenuHandler::removeRelationship(GenealogicalTree& tree) {
    auto person1 = selectPerson(tree);
    if (!person1) return;
    std::cout << "\n";
    auto person2 = selectPerson(tree);
    if (!person2) return;
    std::cout << "\n";
    auto relationships = tree.getRelationshipSystem()->getRelationshipsFor(person1);
    std::vector<std::string> availableRelations;

    for (const auto& rel : relationships) {
        if (std::get<0>(rel) == person2) {
            availableRelations.push_back(std::get<1>(rel));
        }
    }

    if (availableRelations.empty()) {
        std::cout << "Между этими людьми нет связей.\n";
        return;
    }
    std::cout << "\n";
    std::cout << "Доступные связи:\n";
    for (size_t i = 0; i < availableRelations.size(); ++i) {
        std::cout << i + 1 << ". " << availableRelations[i] << "\n";
    }

    std::cout << "Выберите связь для удаления: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore();

    if (choice > 0 && choice <= static_cast<int>(availableRelations.size())) {
        tree.getRelationshipSystem()->removeRelationship(person1, person2, availableRelations[choice - 1]);
        std::cout << "Связь успешно удалена.\n";
    }
    else {
        std::cout << "Неверный выбор.\n";
    }
}
