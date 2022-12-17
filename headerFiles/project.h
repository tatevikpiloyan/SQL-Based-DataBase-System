#ifndef PROJECT_H
#define PROJECT_H
#include "connect.h"
#include "/Users/tatev/Desktop/TMS/headerFiles/user.h"
#include "/Users/tatev/Desktop/TMS/headerFiles/task.h"
#include <fstream>


struct Project
{
    std::string name;
    std::vector<std::string> user;
    std::vector<std::size_t> task;
    Date created;
    Date deadline;
    std::string description;
    Project() = default;
    Project(const Project&);
};

class ProjectDB : public Connect
{
public:
    ProjectDB() = default;
    ProjectDB(UserDB*, TaskDB*);
    ~ProjectDB();
    virtual void insert(std::vector<std::string>&) override;
    virtual void select(std::vector<std::string>&) override;
    virtual void delete_(std::vector<std::string>&) override;
    virtual void update(std::vector<std::string>&) override;
    virtual void to_file() override;

private:
    void init();
    void project_data_check(Project*);
    bool is_valid_date(const Date&, const Date&);
    Date created_date();
    void set_all_true();
    void set_all_false();
    void set_for_select(const Project&);
    bool for_selection_unique(const Project&, const Project*);
    void delete_from_map(const std::vector<std::size_t>&);

private:
    // Static ID count
    static std::size_t ID;

    // Connect project ID with project
    std::map<std::size_t, Project*> project_ID;

    // To open file
    std::fstream project_file;

    // Connect project ID with project file position
    std::map<std::size_t, std::streampos> ID_pos;

    // Keyworda to use for ProjectDB
    std::map<std::string, Project_Keyword> keyword_ht;

    // UserDB pointer
    UserDB* user;

    // TaskDB pointer
    TaskDB* task;

private:
    // Flags for select and update
    bool flag_name;
    bool flag_user;
    bool flag_task;
    bool flag_created;
    bool flag_deadline;
    bool flag_description;
};

#endif // PROJECT_H