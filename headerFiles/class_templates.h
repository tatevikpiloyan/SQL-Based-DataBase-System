#include <vector>
#include <string>
#include <list>
#include <iterator>
#include <iostream>
#include <algorithm>

class Corporation;
class Project;
class Task;
class User;


class Corporation {
private:
    std::vector<User> users;
    std::list<Project> projects;
};

class Project
{
private:
    class DashBoard
    {
    public:
        DashBoard(const std::string& name);
        DashBoard(std::list<Task*>);
    private:
        std::string m_comment;
        std::string m_name;
    };
    struct Day {};
    struct Month {};
    struct Year {};
private:
    class DeadLine
    {
    private:
        Day m_day;
        Month m_month;
        Year m_year;
    };
private:
    std::string m_project_name;
    std::string m_description;
    std::vector<User> m_memeber_users;
    std::list<Task> tasks;
    DashBoard m_arr[3];
};

class User {
private:
    std::string m_name;
    std::string m_surname;
    std::string m_position;
    std::string m_phone_number;
    std::string m_email;
    std::string m_password;
    std::vector<Project*> m_project_ptr;
};