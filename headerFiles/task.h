#ifndef TASK_H
#define TASK_H
#include "connect.h"
#include "/Users/tatev/Desktop/TMS/headerFiles/pars.h"
#include "/Users/tatev/Desktop/TMS/headerFiles/user.h"
#include <fstream>


// Task
struct Task
{
    std::string name;
    std::string user;
    Date created;
    Date deadline;
    Task() = default;
    Task(const Task&);
};


// TaskDB
class TaskDB : public Connect
{
public:
    TaskDB() = default;
    TaskDB(UserDB*);
    ~TaskDB();
    virtual void insert(std::vector<std::string>&) override;
    virtual void select(std::vector<std::string>&) override;
    virtual void delete_(std::vector<std::string>&) override;
    virtual void update(std::vector<std::string>&) override;
    virtual void to_file() override;
    std::size_t find_ID(const std::string&);

private:
    void init();
    void task_data_check(Task*);
    bool is_valid_date(const Date&, const Date&);
    Date created_date();
    void set_all_true();
    void set_all_false();
    void set_for_select(const Task&);
    bool for_selection_unique(const Task&, const Task*);
    void delete_from_map(const std::vector<std::size_t>&);

private:
    // Static ID count
    static std::size_t ID;

    // Connect task ID with task
    std::map<std::size_t, Task*> task_ID;

    // To open file
    std::fstream task_file;

    // Connect user ID with user file position
    std::map<std::size_t, std::streampos> ID_pos;

    // Keywords to use for TaskDB
    std::map<std::string, Task_Keyword> keyword_ht;
    
    // UserDB pointer
    UserDB* user;

private:
    // Flags for select and update
    bool flag_name;
    bool flag_user;
    bool flag_created;
    bool flag_deadline;
};

#endif // TASK_H