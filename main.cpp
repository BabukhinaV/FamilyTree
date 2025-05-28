#include <iostream>
#include <Windows.h>
#include <memory>
#include "GenealogicalTree.h"
#include "LoggerObserver.h"
#include "MenuHandler.h"

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    auto tree = std::make_shared<GenealogicalTree>();
    auto logger = std::make_shared<LoggerObserver>();

    tree->loadFromFile("family_data.txt");

    std::cout << R"(
           # #### ####
         ### /#|### |/####
        ##/#/ \||/##/_/##/_#
      ###  \/###|/ \/ # ###
    ##_\_#\_\## | #/###_/_####
   ## #### # \ #| /  #### ##/##
    __#_--###  |{,###---###-~
              \ }{
               }}{
               }}{
          ejm  {{}}     Древо Жизни
         , -=-~{ .-^- _ 
               }        Эта программа создаёт 
                {       простейшее генеалогическое древо.
    )" << std::endl;

    int choice;
    do {
        MenuHandler::printMenu();
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
        case 1:
            tree->printAllMembers();
            std::cout << std::endl;
            break;
        case 2: {
            auto p = MenuHandler::selectPerson(*tree);
            if (p) p->printInfo();
            std::cout << std::endl;
            break;
        }
        case 3: {
            auto p = MenuHandler::selectPerson(*tree);
            if (p) tree->printRelationships(p->getFullName());
            std::cout << std::endl;
            break;
        }
        case 4:
            tree->printAllRelationships();
            std::cout << std::endl;
            break;
        case 5:
            MenuHandler::addPerson(*tree);
            tree->saveToFile("family_data.txt");
            std::cout << std::endl;
            break;
        case 6:
            MenuHandler::removePerson(*tree);
            tree->saveToFile("family_data.txt");
            std::cout << std::endl;
            break;
        case 7:
            MenuHandler::addRelationship(*tree);
            tree->saveToFile("family_data.txt");
            std::cout << std::endl;
            break;
        case 8:
            MenuHandler::removeRelationship(*tree);
            tree->saveToFile("family_data.txt");
            std::cout << std::endl;
            break;
        case 9:
            MenuHandler::triggerEvent(tree);
            tree->saveToFile("family_data.txt");
            std::cout << std::endl;
            break;
        case 10:
            tree->saveToFile("family_data.txt");
            std::cout << "Данные сохранены.\n";
            break;
        case 0:
            break;
        default:
            std::cout << "Неверный выбор.\n";
        }

    } while (choice != 0);

    std::cout << R"(
                _
                \`*-.                   
                 )  _`-.                
                .  : `. .               
                : _   '  \              
                ; *` _.   `*-._         
                `-.-'          `-.      
                  ;       `       `.    
                  :.       .        \   
                  . \  .   :   .-'   .  
                  '  `+.;  ;  '      :  
                  :  '  |    ;       ;-.
                  ; '   : :`-:     _.`* ;
         [bug] .*' /  .*' ; .*`- +'  `*'
               `*-*   `*-*  `*-*'       

          Доброго пути!
    )" << std::endl;

    return 0;
}
