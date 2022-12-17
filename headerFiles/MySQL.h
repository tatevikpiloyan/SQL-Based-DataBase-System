#ifndef MYSQL_H
#define MYSQL_H
#include <fstream>
#include "enumaration.h"
#include "connect.h"
#include </Users/tatev/Desktop/TMS/headerFiles/user.h>
#include </Users/tatev/Desktop/TMS/headerFiles/task.h>
#include </Users/tatev/Desktop/TMS/headerFiles/project.h>


class MySQL {
public:

    MySQL(UserDB*, TaskDB*, ProjectDB*);
    ~MySQL();

public:
    void execute(const std::string&);
    
private:
    void check_file(const std::string&);
    void insert(Connect*, std::vector<std::string>&);
    void select(Connect*, std::vector<std::string>&);
    void update(Connect*, std::vector<std::string>&);
    void delete_(Connect*, std::vector<std::string>&);

private:
    using Func_Point = void (MySQL::*)(Connect*, std::vector<std::string>&);
    std::map<std::string, Command> command;
    std::map<std::string, DBFile> DB;
    std::map<DBFile, Connect*> DB_Obj;
    std::map<Command, Func_Point> func_ht;
    std::fstream file;
};

#endif //MYSQL_H