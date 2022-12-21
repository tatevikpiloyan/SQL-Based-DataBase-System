#include </Users/tatev/Desktop/TMS/headerFiles/task.h>
#include <iostream>
#include <chrono>


std::size_t TaskDB::ID = 0;


// Initializing function
void TaskDB::init()
{
    keyword_ht["name"] = Task_Keyword::NAME;
    keyword_ht["user"] = Task_Keyword::USER;
    keyword_ht["created"] = Task_Keyword::CREATED;
    keyword_ht["deadline"] = Task_Keyword::DEADLINE;
    keyword_ht["where"] = Task_Keyword::WHERE;
    keyword_ht["set"] = Task_Keyword::SET;
    keyword_ht["all"] = Task_Keyword::ALL;

    this->set_all_false();
}


// Task copy constructor
Task::Task(const Task& rhs) : name(), user()
{
    this->name = rhs.name;
    this->user = rhs.user;
    this->created = rhs.created;
    this->deadline = rhs.deadline;
}


// TaskDB parametrized constructor
TaskDB::TaskDB(UserDB* user_DB) : user{user_DB}
{
    // Do init() function
    init();

    // Opening task.data file
    task_file.open("/Users/tatev/Desktop/TMS/DB/task.data");
    if (!task_file.is_open())
    {
        throw std::runtime_error("Failed to open Task DB!");
    }

    // Analizing datas
    std::string line = "";
    std::string tmp = "";
    while (!task_file.eof())
    {
        std::getline(task_file, line);
        if (!(line.empty()) && (line.find("ID") != std::string::npos))
        {
            tmp += pars_data(line);
            ID_pos.insert(std::make_pair(std::stoi(tmp), task_file.tellg()));
            task_ID[std::stoi(tmp)] = new Task;
        }
        tmp.clear();
        line.clear();
    }
    std::string object_data = "";
    for (auto& it : ID_pos)
    {
        // Task name
        getline(task_file.seekg(it.second), object_data);
        task_ID[it.first]->name = pars_data(object_data);
        // Task user
        getline(task_file, object_data);
        task_ID[it.first]->user = pars_data(object_data);
        // Task created
        getline(task_file, object_data);
        task_ID[it.first]->created = Date(pars_data(object_data));
        // Task deadline
        getline(task_file, object_data);
        task_ID[it.first]->deadline = Date(pars_data(object_data));
    }

    // Setting ID value
    if (task_ID.empty())
    {
        task_file.close();
        if (task_file.is_open())
        {
            std::cout << "Failed to close the file" << std::endl;
        }
        return;
    }
    auto it = task_ID.end();
    ID = (--it)->first;
    ++ID;

    // Closing task.data file
    task_file.close();
    if (task_file.is_open())
    {
        std::cout << "Failed to close the file" << std::endl;
    }
}


// Insert functionc implementation
void TaskDB::insert(std::vector<std::string>& rhs)
{
    // Analizing input
    std::string to_parse = to_string(rhs);
    std::string spliter = "\"";
    auto half_parsed = parser(to_parse, spliter);
    Task tmp;

    // Setting values
    tmp.name = half_parsed[1];
    auto parsed = parser(half_parsed[2], " =,");
    for (auto it = parsed.begin(); it < parsed.end(); ++it)
    {
        if (static_cast<bool>(keyword_ht[*it]))
        {
            tmp.deadline = (keyword_ht[*it] == Task_Keyword::DEADLINE) ? *(it + 1) : tmp.deadline;
            tmp.user = (keyword_ht[*it] == Task_Keyword::USER) ? *(it + 1) : tmp.user;
        }
    }
    tmp.created = created_date();

    // Check data
    task_data_check(&tmp);
    task_ID[ID] = new Task(tmp);
    ++ID;
}


// Select function implementation
void TaskDB::select(std::vector<std::string>& rhs)
{
    // Analizing input
    std::string spliter = " =,";
    std::string to_parse = to_string(rhs);
    auto parsed = parser(to_parse, spliter);

    // Finding keywords' iterators
    auto all_it = std::find_if(parsed.begin(), parsed.end(), [&](const std::string& rhs) { return keyword_ht[rhs] == Task_Keyword::ALL; });
    auto where_it = std::find_if(parsed.begin(), parsed.end(), [&] (const std::string rhs) { return keyword_ht[rhs] == Task_Keyword::WHERE; });

    // Setting flags
    for (auto it = parsed.begin(); it < where_it; ++it)
    {
        if (static_cast<bool>(keyword_ht[*it]))
        {
            this->flag_name = (keyword_ht[*it] == Task_Keyword::NAME) ? true : flag_name;
            this->flag_user = (keyword_ht[*it] == Task_Keyword::USER) ? true : flag_user;
            this->flag_created = (keyword_ht[*it] == Task_Keyword::CREATED) ? true : flag_created;
            this->flag_deadline = (keyword_ht[*it] == Task_Keyword::DEADLINE) ? true : flag_deadline;
        }
        if (keyword_ht[*it] == Task_Keyword::ALL)
        {
            this->set_all_true();
            break;
        }
    }
    
    // Throw exception if invalid syntax
    if (std::distance(all_it, where_it) != 1 && all_it != parsed.end())
    {
        throw std::invalid_argument("Invalid argument!");
    }

    Task task;

    // Parsing after where keyword
    std::string tmp;
    for (auto it = where_it + 1; it < parsed.end(); ++it)
    {
        tmp += *it + ' ';
    }
    auto to_analize = parser(tmp, "\"");
    for (auto vec_it = to_analize.begin(); vec_it < to_analize.end(); vec_it +=2)
    {
        auto tmp = parser(*vec_it, " =,");
        for (auto string_it = tmp.begin(); string_it < tmp.end(); string_it += 2)
        {
            if (keyword_ht[*string_it] == Task_Keyword::NAME)
            {
                task.name = *(vec_it + 1);
                break;
            }
            task.user = (keyword_ht[*string_it] == Task_Keyword::USER) ? *(string_it + 1) : task.user;
            task.created = (keyword_ht[*string_it] == Task_Keyword::CREATED) ? *(string_it + 1) : task.created;
            task.deadline = (keyword_ht[*string_it] == Task_Keyword::DEADLINE) ? *(string_it + 1) : task.deadline;
        }
    }

    // Implemeting main selection
    this->set_for_select(task);
    for (auto& it : task_ID)
    {
        if (for_selection_unique(task, it.second))
        {
            if (flag_name)
            {
                std::cout << "name : " << it.second->name << std::endl;
            }
            if (flag_user)
            {
                std::cout << "user : " << it.second->user << std::endl;
            }
            if (flag_created)
            {
                std::cout << "created : " << it.second->created << std::endl;
            }
            if (flag_deadline)
            {
                std::cout << "deadline : " << it.second->deadline << std::endl;
            }
            std::cout << std::endl;
        }
    }
    this->set_all_false();
}


// Delete function implementation
void TaskDB::delete_(std::vector<std::string>& rhs)
{
    // Analizing input
    std::string spliter = " =,";
    std::string to_pars = to_string(rhs);
    auto parsed = parser(to_pars, spliter);

    // Finding keyword's iterator
    auto where_it = std::find_if(parsed.begin(), parsed.end(), [&](const std::string& rhs) { return (keyword_ht[rhs] == Task_Keyword::WHERE); });
    
    if (where_it == parsed.end())
    {
        throw std::invalid_argument("Syntax Error!");
    }

    Task tmp_task;

    // Parsing after where keyword
    std::string tmp_string;

    for (auto it = where_it + 1; it < parsed.end(); ++it)
    {
        tmp_string += *it + ' ';
    }
    auto to_analize = parser(tmp_string, "\"");
    for (auto vec_it = to_analize.begin(); vec_it < to_analize.end(); vec_it +=2)
    {
        auto tmp = parser(*vec_it, " =,");
        for (auto string_it = tmp.begin(); string_it < tmp.end(); string_it += 2)
        {

            if (keyword_ht[*string_it] == Task_Keyword::NAME)
            {
                tmp_task.name = *(vec_it + 1);
                break;
            }
            tmp_task.user = (keyword_ht[*string_it] == Task_Keyword::USER) ? *(string_it + 1) : tmp_task.user;
            tmp_task.created = (keyword_ht[*string_it] == Task_Keyword::CREATED) ? *(string_it + 1) : tmp_task.created;
            tmp_task.deadline = (keyword_ht[*string_it] == Task_Keyword::DEADLINE) ? *(string_it + 1) : tmp_task.deadline;
        }
    }

    // Finding tasks to delete
    std::size_t count_for_deletion = 0;
    std::vector<std::size_t> tmp_deletion;

    this->set_for_select(tmp_task);
   
    for (auto& it : task_ID)
    {
        if (for_selection_unique(tmp_task, it.second))
        {
            std::cout << "name : " << it.second->name << std::endl;
            std::cout << "user : " << it.second->user << std::endl;
            std::cout << "created : " << it.second->created << std::endl;
            std::cout << "deadline : " << it.second->deadline << std::endl;
            std::cout << std::endl;
            ++count_for_deletion;
            tmp_deletion.push_back(it.first);
        }
    }
    std::cout << count_for_deletion << " tasks found to delete!" << std::endl;
    char question;
    std::cout << "Are you shure? (y/n) : ";
    do 
    {
        std::cin >> question;

        // Ignore one character from input buffer (puts '\n')
        std::cin.ignore(1);
    }
    while (question != 'y' && question != 'n');
    if (question == 'n') { return; }

    // Delete tasks from data
    this->delete_from_map(tmp_deletion);
    this->set_all_false();
}


// Update function implementation
void TaskDB::update(std::vector<std::string>& rhs)
{
    // Analizing input
    std::string spliter = " =";
    std::string to_pars = to_string(rhs);
    auto parsed = parser(to_pars, spliter);

    // Finding keywords' iterators
    auto set_it = std::find_if(parsed.begin(), parsed.end(), [&](const std::string& rhs) { return keyword_ht[rhs] == Task_Keyword::SET; });
    auto where_it = std::find_if(parsed.begin(), parsed.end(), [&](const std::string& rhs) { return keyword_ht[rhs] == Task_Keyword::WHERE; });

    if ((std::distance(set_it, where_it) < 3) && (where_it != parsed.end()))
    {
        throw std::invalid_argument("Invalid syntax!");
    }

    ++where_it;
    ++set_it;

    Task where_tmp;

    // Find tasks to update
    std::string string_where_tmp;

    for (auto it = where_it; it < parsed.end(); ++it)
    {
        string_where_tmp += *it + ' ';
    }
    auto to_analize_where = parser(string_where_tmp, "\"");

    for (auto vec_it = to_analize_where.begin(); vec_it < to_analize_where.end(); vec_it += 2)
    {
        auto tmp = parser(*vec_it, " =,");
        for (auto string_it = tmp.begin();string_it < tmp.end(); string_it += 2)
        {
            if (keyword_ht[*string_it] == Task_Keyword::NAME)
            {
                where_tmp.name = *(vec_it + 1);
                break;
            }
            where_tmp.user = (keyword_ht[*string_it] == Task_Keyword::USER) ? *(string_it + 1) : where_tmp.user;
            where_tmp.created = (keyword_ht[*string_it] == Task_Keyword::CREATED) ? *(string_it + 1) : where_tmp.created;
            where_tmp.deadline = (keyword_ht[*string_it] == Task_Keyword::DEADLINE) ? *(string_it + 1) : where_tmp.deadline;
        }
    }

    std::vector<std::size_t> ID_for_update;

    this->set_for_select(where_tmp);

    for (auto& it : task_ID)
    {
        if (for_selection_unique(where_tmp, it.second))
        {
            ID_for_update.push_back(it.first);
        }
    }

    // Set data to update
    Task set_tmp;
    std::string string_set_tmp;

    for (auto it = set_it; it < where_it - 1; ++it)
    {
        string_set_tmp += *it + ' ';
    }

    auto to_analize_set = parser(string_set_tmp, "\"");
    for (auto vec_it = to_analize_set.begin(); vec_it < to_analize_set.end(); vec_it += 2)
    {
        auto tmp = parser(*vec_it, " =,");
        for (auto string_it = tmp.begin(); string_it < tmp.end(); string_it += 2)
        {
            if (keyword_ht[*string_it] == Task_Keyword::NAME)
            {
                set_tmp.name = *(vec_it + 1);
                break;
            }
            if (keyword_ht[*string_it] == Task_Keyword::CREATED)
            {
                throw std::invalid_argument("Syntax error!");
            }
            set_tmp.user = (keyword_ht[*string_it] == Task_Keyword::USER) ? *(string_it + 1) : set_tmp.user;
            set_tmp.deadline = (keyword_ht[*string_it] == Task_Keyword::DEADLINE) ? *(string_it + 1) : set_tmp.deadline;
        }
    }

    for (auto& it : ID_for_update)
    {
        if (!set_tmp.name.empty())
        {
            task_ID[it]->name = set_tmp.name;
        }
        if (!set_tmp.user.empty())
        {
            if (!this->user->find_email(set_tmp.user))
            {
                throw std::invalid_argument("No such user!");
            }
            task_ID[it]->user = set_tmp.user;
        }
        if (!set_tmp.deadline.empty())
        {
            if (!is_valid_date(set_tmp.deadline, task_ID[it]->created))
            {
                throw std::invalid_argument("Invalid date!");
            }
            task_ID[it]->deadline = set_tmp.deadline;
        }
    }
}


// Check for validity
void TaskDB::task_data_check(Task* task)
{
    if (task->name.empty())
    {
        throw std::invalid_argument("Syntax Error!");
    }
    if (!user->find_email(task->user))
    {
        throw std::invalid_argument("No user found!");
    }
    if (!is_valid_date(task->deadline, task->created))
    {
        throw std::invalid_argument("Invalid Date!");
    }
}


// Check for Date validity
bool TaskDB::is_valid_date(const Date& deadline, const Date& created)
{
    if (deadline.day > 30 || deadline.month > 12)
    {
        return false;
    }
    if (deadline.year > created.year)
    {
        return true;
    }
    if (deadline.year == created.year)
    {
        if (deadline.month >= created.month)
        {
            if (deadline.day >= created.day)
            {
                return true;
            }
        }
    }
    return false;
}


// Date of creation
Date TaskDB::created_date()
{
    const std::chrono::time_point now{std::chrono::system_clock::now()};
    const std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(now)};
    Date tmp;
    tmp.year = static_cast<int>(ymd.year());
    tmp.month = static_cast<unsigned>(ymd.month());
    tmp.day = static_cast<unsigned>(ymd.day());
    return tmp;
}


// Set all flags true
void TaskDB::set_all_true()
{
    this->flag_name = true;
    this->flag_user = true;
    this->flag_created = true;
    this->flag_deadline = true;
}


// Set all flags false
void TaskDB::set_all_false()
{
    this->flag_name = false;
    this->flag_user = false;
    this->flag_created = false;
    this->flag_deadline = false;
}


// Set selection flags
void TaskDB::set_for_select(const Task& tmp)
{
    this->for_select["name"] = tmp.name.empty() ? false : true;
    this->for_select["user"] = tmp.user.empty() ? false : true;
    this->for_select["created"] = tmp.created.empty() ? false : true;
    this->for_select["deadline"] = tmp.deadline.empty() ? false : true;
}


// Select Tasks
bool TaskDB::for_selection_unique(const Task& lhs, const Task* rhs)
{
    if (for_select["name"])
    {
        if (lhs.name != rhs->name) { return false; }
    }
    if (for_select["user"])
    {
        if (lhs.user != rhs->user) { return false; }
    }
    if (for_select["created"])
    {
        if (lhs.created != rhs->created) { return false; }
    }
    if (for_select["deadline"])
    {
        if (lhs.deadline != rhs->deadline) { return false; }
    }
    return true;
}


// Delete tasks from data
void TaskDB::delete_from_map(const std::vector<std::size_t>& deleted_ID)
{
    for (auto& it : deleted_ID)
    {
        if (task_ID.find(it) != task_ID.end())
        {
            task_ID.erase(it);
        }
    }
}


// Return task ID
std::size_t TaskDB::find_ID(const std::string& rhs)
{
    for (auto& it : task_ID)
    {
        if (it.second->name == rhs) { return it.first; }
    }
    throw std::invalid_argument("No such task!");
}


// Inherited modify function
void TaskDB::modify(std::vector<std::string>&)
{
    throw std::invalid_argument("Syntax error!");
}


// Move user data to file
void TaskDB::to_file()
{
    task_file.open("/Users/tatev/Desktop/TMS//DB/task.data", std::ios::out);
    if (!task_file.is_open())
    {
        throw std::invalid_argument("Failed to open Task DB!");
    }
    for (auto& it : task_ID)
    {
        task_file << std::endl << "{" << std::endl;
        task_file << "  ID : " << it.first << std::endl;
        task_file << "  name : " << it.second->name << std::endl;
        task_file << "  user : " << it.second->user << std::endl;
        task_file << "  created : " << it.second->created << std::endl;
        task_file << "  deadline : " << it.second->deadline.day << "/" << it.second->deadline.month << "/" << it.second->deadline.year << std::endl;
        task_file << "}";
    }
    task_file.close();
    if (task_file.is_open())
    {
        std::cout << "Failed to close the file" << std::endl;
    }
}


// TaskDB destructor
TaskDB::~TaskDB()
{
    for (auto& it : task_ID)
    {
        delete it.second;
    }
    task_ID.clear();
}