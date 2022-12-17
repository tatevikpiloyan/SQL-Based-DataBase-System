#include <iostream>
#include </Users/tatev/Desktop/TMS/headerFiles/connect.h>


// Connect constructor
Connect::Connect()
{
    this->init();
}


// Init function
void Connect::init()
{
    // Map initialization for selection 
    this->for_select["name"] = false;
    this->for_select["surname"] = false;
    this->for_select["position"] = false;
    this->for_select["email"] = false;
    this->for_select["phone_number"] = false;
    this->for_select["user"] = false;
    this->for_select["created"] = false;
    this->for_select["deadline"] = false;
}


// Date constructor
Date::Date() : year{}, month{}, day{} {}


// Date parametrized constructor
Date::Date(const std::string& rhs)
{
    auto str = parser(rhs, "/");
    if (str.size() < 3 || str.size() > 3) { throw std::invalid_argument("Invalid Date!"); }
    this->day = std::stoi(str[0]);
    this->month = std::stoi(str[1]);
    this->year = std::stoi(str[2]);
}


// Bool function to return empty
bool Date::empty() const
{
    return (this->year == 0) && (this->month == 0) && (this->day == 0);
}


// Operator overloading
bool Date::operator!=(const Date& rhs) const
{
    return (this->year != rhs.year || this->month != rhs.month || this->day != rhs.day);
}


// Operator overloading
std::ostream& operator<<(std::ostream& os, const Date& rhs)
{
    os << rhs.day << "/" << rhs.month << "/" << rhs.year;
    return os;
}