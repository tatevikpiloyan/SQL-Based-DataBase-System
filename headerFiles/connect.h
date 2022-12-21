#pragma once
#include <vector>
#include <string>
#include "enumaration.h"
#include "pars.h"


// Date
struct Date
{
    // Date variables
    std::size_t year;
    std::size_t month;
    std::size_t day;

    // Date constructors
    Date();
    Date(const std::string&);

    // Date functions
    bool empty() const;
    bool operator!=(const Date&) const;
};

std::ostream& operator<<(std::ostream&, const Date&);


// Connect 
class Connect
{
public:
    Connect();
    ~Connect() = default;
    virtual void insert(std::vector<std::string>&) = 0;
    virtual void select(std::vector<std::string>&) = 0;
    virtual void delete_(std::vector<std::string>&) = 0;
    virtual void update(std::vector<std::string>&) = 0;
    virtual void modify(std::vector<std::string>&) = 0;
    virtual void to_file() = 0;

public:
    std::map<std::string, bool> for_select;

private:
    void init();
};