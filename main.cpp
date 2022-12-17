#include <iostream>
#include </Users/tatev/Desktop/TMS/headerFiles/MySQL.h>

int main()
{
    std::string str;
    UserDB user;
    TaskDB task(&user);
    ProjectDB project(&user, &task);
    MySQL SQL(&user, &task, &project);
    while (true)
    {
        try
        {
            std::cout << "Enter command : ";
            std::getline(std::cin, str);
            if (str == "exit") { 
                break;
            }
            SQL.execute(str);
        }
        catch(std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            continue;
        }
    }
    return 0;
}