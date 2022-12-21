#include </Users/tatev/Desktop/TMS/headerFiles/project.h>
#include <iostream>
#include <chrono>


std::size_t ProjectDB::ID = 0;


// Project copy constructor
Project::Project(const Project& rhs)
{
    this->name = rhs.name;
    this->user = rhs.user;
    this->task = rhs.task;
    this->created = rhs.created;
    this->deadline = rhs.deadline;
    this->description = rhs.description;
}


// Initializing function
void ProjectDB::init()
{
    keyword_ht["name"] = Project_Keyword::NAME;
    keyword_ht["user"] = Project_Keyword::USER;
    keyword_ht["task"] = Project_Keyword::TASK;
    keyword_ht["deadline"] = Project_Keyword::DEADLINE;
    keyword_ht["created"] = Project_Keyword::CREATED;
    keyword_ht["description"] = Project_Keyword::DESCRIPTION;
    keyword_ht["where"] = Project_Keyword::WHERE;
    keyword_ht["set"] = Project_Keyword::SET;
    keyword_ht["all"] = Project_Keyword::ALL;
    keyword_ht["add"] = Project_Keyword::ADD;
    keyword_ht["delete"] = Project_Keyword::DELETE;

    this->set_all_false();
}


// ProjectDB parametrized constructor
ProjectDB::ProjectDB(UserDB* user_DB, TaskDB* task_DB) : user{user_DB}, task{task_DB}
{
    // Do init() function
    init();

    // Opening Project.data file
    project_file.open("/Users/tatev/Desktop/TMS/DB/project.data");
    if (!project_file.is_open())
    {
        throw std::runtime_error("Failed to open Project DB!");
    }

    // Analizing datas
    std::string line;
    std::string tmp;
    while (!project_file.eof())
    {
        std::getline(project_file, line);
        if (!(line.empty()) && (line.find("ID") != std::string::npos))
        {
            tmp += pars_data(line);
            ID_pos.insert(std::make_pair(std::stoi(tmp), project_file.tellg()));
            project_ID[std::stoi(tmp)] = new Project;
        }
        tmp.clear();
        line.clear();
    }
    std::string object_data;
    for (auto& it : ID_pos)
    {
        // Project name
        getline(project_file.seekg(it.second), object_data);
        project_ID[it.first]->name = pars_data(object_data);
        // Project users
        getline(project_file, object_data);
        project_ID[it.first]->user = parser(pars_data(object_data), " |");
        // Project tasks
        getline(project_file, object_data);
        auto tmp = parser(pars_data(object_data), " |");
        for (auto& i : tmp)
        {
            if (i.empty()) { continue; }
            project_ID[it.first]->task.push_back(std::stoi(i));
        }
        // Project created date
        getline(project_file, object_data);
        project_ID[it.first]->created = pars_data(object_data);
        // Project deadline
        getline(project_file, object_data);
        project_ID[it.first]->deadline = pars_data(object_data);
        // Project description
        getline(project_file, object_data);
        project_ID[it.first]->description = pars_data(object_data);
    }

    // Setting ID value
    if (project_ID.empty())
    {
        project_file.close();
        if (project_file.is_open())
        {
            std::cout << "Failed to close the file" << std::endl;
        }
        return;
    }
    auto it = project_ID.end();
    ID = (--it)->first;
    ++ID;

    // Closing project.data file
    project_file.close();
    if (project_file.is_open())
    {
        std::cout << "Failed to close the file" << std::endl;
    }
}


// Insert functionc implementation
void ProjectDB::insert(std::vector<std::string>& rhs)
{
    // Analize input
    std::string spliter = " =";
    std::string to_parse = to_string(rhs);
    auto half_parsed = parser(to_parse, spliter);
    
    Project project_tmp;

    // Find task keyword
    auto task_it = std::find_if(half_parsed.begin(), half_parsed.end(), [&](const std::string& rhs) { return keyword_ht[rhs] == Project_Keyword::TASK; });
    std::string task_to_parse;

    if (task_it != half_parsed.end())
    {
        for (auto it = task_it + 1; (it < half_parsed.end() - 1) && (!static_cast<bool>(keyword_ht[*it])); ++it)
        {
            task_to_parse += *(it);
            while (!((*it).find('\"')))
            {
                task_to_parse += ' ' + *(it + 1);
                ++it;
            }
            task_to_parse += ' ';
        }

        // Setting task
        auto tasks = parser(task_to_parse, "\"");
        for (auto& it : tasks)
        {
            if (it == " ") { continue; }
            project_tmp.task.push_back(task->find_ID(it));
        }
    }
    
    // Setting rest of values
    auto parsed = parser(to_parse, "\"");
    for (auto vec_it = parsed.begin(); vec_it < parsed.end(); vec_it += 2)
    {
        auto tmp = parser((*vec_it), " =,");
        for (auto string_it = tmp.begin(); string_it < tmp.end(); ++string_it)
        {
            if (keyword_ht[*string_it] == Project_Keyword::USER)
            {
                while ((!static_cast<bool>((keyword_ht[*(string_it + 1)]))) && (string_it != tmp.end() - 2))
                {
                    if (!user->find_email(*(string_it + 1))) { throw std::invalid_argument("No such user!"); }
                    project_tmp.user.push_back(*(string_it + 1));
                    ++string_it;
                }
            }
            if (keyword_ht[*string_it] == Project_Keyword::TASK)
            {
                break;
            }
            project_tmp.name = (keyword_ht[*string_it] == Project_Keyword::NAME) ? *(vec_it + 1) : project_tmp.name;
            project_tmp.deadline = (keyword_ht[*string_it] == Project_Keyword::DEADLINE) ? *(string_it + 1) : project_tmp.deadline;
            project_tmp.description = (keyword_ht[*string_it] == Project_Keyword::DESCRIPTION) ? *(vec_it + 1) : project_tmp.description;
        }
    }
    project_tmp.created = created_date();

    // Check data
    project_data_check(&project_tmp);
    project_ID[ID] = new Project(project_tmp);
    ++ID;
}


// Select function implementation
void ProjectDB::select(std::vector<std::string>& rhs)
{
    //Analizing input
    std::string spliter = " =";
    std::string to_parse = to_string(rhs);
    auto parsed = parser(to_parse, spliter);

    // Finding keywords
    auto all_it = std::find_if(parsed.begin(), parsed.end(), [&](const std::string& rhs) { return keyword_ht[rhs] == Project_Keyword::ALL; });
    auto where_it = std::find_if(parsed.begin(), parsed.end(), [&](const std::string& rhs) { return keyword_ht[rhs] == Project_Keyword::WHERE; });

    if (where_it == parsed.end()) { throw std::invalid_argument("Invalid syntax!"); }

    // Setting flags
    for (auto it = parsed.begin(); it < where_it; ++it)
    {
        if (static_cast<bool>(keyword_ht[*it]))
        {
            this->flag_name = (keyword_ht[*it] == Project_Keyword::NAME) ? true : flag_name;
            this->flag_user = (keyword_ht[*it] == Project_Keyword::USER) ? true : flag_user;
            this->flag_task = (keyword_ht[*it] == Project_Keyword::TASK) ? true : flag_task;
            this->flag_created = (keyword_ht[*it] == Project_Keyword::CREATED) ? true : flag_created;
            this->flag_deadline = (keyword_ht[*it] == Project_Keyword::DEADLINE) ? true : flag_deadline;
            this->flag_description = (keyword_ht[*it] == Project_Keyword::DESCRIPTION) ? true : flag_description;
        }
        if (keyword_ht[*it] == Project_Keyword::ALL)
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

    Project project;

    // Parsing after where keyword
    std::string tmp;

    for (auto it = where_it + 1; it < parsed.end(); ++it)
    {
        tmp += *it + ' ';
    }
    auto to_analize_where = parser(tmp, "\"");
    for (auto vec_it = to_analize_where.begin(); vec_it < to_analize_where.end(); vec_it += 2)
    {
        auto tmp = parser(*vec_it, " =,");
        for (auto string_it = tmp.begin(); string_it < tmp.end(); ++string_it)
        {
            if (keyword_ht[*string_it] == Project_Keyword::USER)
            {
                while ((!static_cast<bool>((keyword_ht[*(string_it + 1)]))) && (string_it != tmp.end() - 2))
                {
                    if (!user->find_email(*(string_it + 1))) { throw std::invalid_argument("No such user!"); }
                    project.user.push_back(*(string_it + 1));
                    ++string_it;
                }
            }
            project.name = (keyword_ht[*string_it] == Project_Keyword::NAME) ? *(vec_it + 1) : project.name;
            project.deadline = (keyword_ht[*string_it] == Project_Keyword::DEADLINE) ? *(string_it + 1) : project.deadline;
            project.description = (keyword_ht[*string_it] == Project_Keyword::DESCRIPTION) ? *(vec_it + 1) : project.description;
        }
    }

    // Implementing main selection
    this->set_for_select(project);
    for (auto& it : project_ID)
    {
        if (for_selection_unique(project, it.second))
        {
            if (flag_name)
            {
                std::cout << "name : " << it.second->name << std::endl;
            }
            if (flag_user)
            {
                std::cout << "users : ";
                std::copy(it.second->user.begin(), it.second->user.end(), std::ostream_iterator<std::string>(std::cout, " "));
                std::cout << std::endl;
            }
            if (flag_task)
            {
                std::cout << "tasks : ";
                std::copy(it.second->task.begin(), it.second->task.end(), std::ostream_iterator<std::size_t>(std::cout, " "));
                std::cout << std::endl;
            }
            if (flag_created)
            {
                std::cout << "created : " << it.second->created << std::endl;
            }
            if (flag_deadline)
            {
                std::cout << "deadline : " << it.second->deadline << std::endl;
            }
            if (flag_description)
            {
                std::cout << "description : " << it.second->description << std::endl;
            }
            std::cout << std::endl;
        }
    }
    this->set_all_false();
}


// Delete function implementation
void ProjectDB::delete_(std::vector<std::string>& rhs)
{
    // Analizing input
    std::string spliter = " =";
    std::string to_parse = to_string(rhs);
    auto parsed = parser(to_parse, spliter);

    // Finding keyword
    auto where_it = std::find_if(parsed.begin(), parsed.end(), [&](const std::string& rhs) { return keyword_ht[rhs] == Project_Keyword::WHERE; });
    
    if (where_it == parsed.end())
    {
        throw std::invalid_argument("Invalid syntax!");
    }

    Project project;

    // Parsing after where keyword
    std::string tmp;
    
    for (auto it = where_it + 1; it < parsed.end(); ++it)
    {
        tmp += *it + ' ';
    }
    auto to_analize = parser(tmp, "\"");
    for (auto vec_it = to_analize.begin(); vec_it < to_analize.end(); vec_it += 2)
    {
        auto tmp = parser(*vec_it, " =,");
        for (auto string_it = tmp.begin(); string_it < tmp.end(); ++string_it)
        {
            if (keyword_ht[*string_it] == Project_Keyword::USER)
            {
                while ((!static_cast<bool>((keyword_ht[*(string_it + 1)]))) && (string_it != tmp.end() - 2))
                {
                    if (!user->find_email(*(string_it + 1))) { throw std::invalid_argument("No such user!"); }
                    project.user.push_back(*(string_it + 1));
                    ++string_it;
                }
            }
            project.name = (keyword_ht[*string_it] == Project_Keyword::NAME) ? *(vec_it + 1) : project.name;
            project.deadline = (keyword_ht[*string_it] == Project_Keyword::DEADLINE) ? *(string_it + 1) : project.deadline;
            project.description = (keyword_ht[*string_it] == Project_Keyword::DESCRIPTION) ? *(vec_it + 1) : project.description;  
        }
    }

    // Finding projects to delete
    std::size_t count_for_deletion = 0;
    std::vector<std::size_t> tmp_deletion;

    this->set_for_select(project);

    for (auto& it : project_ID)
    {
        if (for_selection_unique(project, it.second))
        {
            std::cout << "name : " << it.second->name << std::endl;
            std::cout << "users : ";
            for (auto& it_tmp : it.second->user)
            {
                std::cout << it_tmp << ' ';
            }
            std::cout << std::endl;
            std::cout << "tasks : ";
            for (auto& it_tmp : it.second->task)
            {
                std::cout << it_tmp << ' ';
            }
            std::cout << std::endl;
            std::cout << "created : " << it.second->created << std::endl;
            std::cout << "deadline : " << it.second->deadline << std::endl;
            std::cout << "description : " << it.second->description << std::endl;
            std::cout << std::endl;
            ++count_for_deletion;
            tmp_deletion.push_back(it.first);
        }
    }
    std::cout << count_for_deletion << " projects found to delete!" << std::endl;
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

    // Delete projects from data
    this->delete_from_map(tmp_deletion);
    this->set_all_false();
}


// Update function implementation
void ProjectDB::update(std::vector<std::string>& rhs)
{
    // Analizing input
    std::string spliter = " =";
    std::string to_parse = to_string(rhs);
    auto parsed = parser(to_parse, spliter);

    // Finding keywords' iterators
    auto set_it = std::find_if(parsed.begin(), parsed.end(), [&](const std::string& rhs) { return keyword_ht[rhs] == Project_Keyword::SET; });
    auto where_it = std::find_if(parsed.begin(), parsed.end(), [&](const std::string& rhs) { return keyword_ht[rhs] == Project_Keyword::WHERE; });

    if ((std::distance(set_it, where_it) < 3) && (where_it != parsed.end()))
    {
        throw std::invalid_argument("Invalid syntax!");
    }

    ++where_it;
    ++set_it;

    Project where_tmp;

    // Finding projects to update
    std::string string_where_tmp;

    for (auto it = where_it; it < parsed.end(); ++it)
    {
        string_where_tmp += *it + ' ';
    }
    auto to_analize_where = parser(string_where_tmp, "\"");

    for (auto vec_it = to_analize_where.begin(); vec_it < to_analize_where.end(); vec_it += 2)
    {
        auto tmp = parser(*vec_it, " =,");
        for (auto string_it = tmp.begin(); string_it < tmp.end(); ++string_it)
        {
            if (keyword_ht[*string_it] == Project_Keyword::USER)
            {
                while ((!static_cast<bool>((keyword_ht[*(string_it + 1)]))) && (string_it != tmp.end() - 2))
                {
                    if (!user->find_email(*(string_it + 1))) { throw std::invalid_argument("No such user!"); }
                    where_tmp.user.push_back(*(string_it + 1));
                    ++string_it;
                }
            }
            where_tmp.name = (keyword_ht[*string_it] == Project_Keyword::NAME) ? *(vec_it + 1) : where_tmp.name;
            where_tmp.deadline = (keyword_ht[*string_it] == Project_Keyword::DEADLINE) ? *(string_it + 1) : where_tmp.deadline;
            where_tmp.description = (keyword_ht[*string_it] == Project_Keyword::DESCRIPTION) ? *(vec_it + 1) : where_tmp.description;
        }
    }

    std::vector<std::size_t> ID_for_update;

    this->set_for_select(where_tmp);

    for (auto& it : project_ID)
    {
        if (for_selection_unique(where_tmp, it.second))
        {
            ID_for_update.push_back(it.first);
        }
    }

    // Set data to update
    Project set_tmp;
    std::string string_set_tmp;

    for (auto it = set_it; it < where_it - 1; ++it)
    {
        string_set_tmp += *it + ' ';
    }

    auto to_analize_set = parser(string_set_tmp, "\"");
    for (auto vec_it = to_analize_set.begin(); vec_it < to_analize_set.end(); vec_it += 2)
    {
        auto tmp = parser(*vec_it, " =,");
        for (auto string_it = tmp.begin(); string_it < tmp.end(); ++string_it)
        {
            if (keyword_ht[*string_it] == Project_Keyword::USER)
            {
                while ((!static_cast<bool>((keyword_ht[*(string_it + 1)]))) && (string_it != tmp.end() - 2))
                {
                    if (!user->find_email(*(string_it + 1))) { throw std::invalid_argument("No such user!"); }
                    set_tmp.user.push_back(*(string_it + 1));
                    ++string_it;
                }
            }
            if (keyword_ht[*string_it] == Project_Keyword::CREATED)
            {
                throw std::invalid_argument("Syntax error!");
            }
            set_tmp.name = (keyword_ht[*string_it] == Project_Keyword::NAME) ? *(vec_it + 1) : set_tmp.name;
            set_tmp.deadline = (keyword_ht[*string_it] == Project_Keyword::DEADLINE) ? *(string_it + 1) : set_tmp.deadline;
            set_tmp.description = (keyword_ht[*string_it] == Project_Keyword::DESCRIPTION) ? *(vec_it + 1) : set_tmp.description;
        }
    }

    for (auto& it : ID_for_update)
    {
        if (!set_tmp.name.empty())
        {
            project_ID[it]->name = set_tmp.name;
        }
        if (!set_tmp.user.empty())
        {
            for (auto& tmp : set_tmp.user)
            {
                if (!this->user->find_email(tmp))
                {
                    throw std::invalid_argument("No such user!");
                }
                project_ID[it]->user.push_back(tmp);
            }
        }
        if (!set_tmp.deadline.empty())
        {
            if (!is_valid_date(set_tmp.deadline, project_ID[it]->created))
            {
                throw std::invalid_argument("Invalid date!");
            }
            project_ID[it]->deadline = set_tmp.deadline;
        }
        if (!set_tmp.deadline.empty())
        {
            project_ID[it]->deadline = set_tmp.deadline;
        }
    }
}


// Modify function implementation
void ProjectDB::modify(std::vector<std::string>& rhs)
{
    // Analizing input
    std::string spliter = " =";
    std::string to_parse = to_string(rhs);
    auto parsed = parser(to_parse, spliter);

    // Finding keywords' iterators
    auto where_it = std::find_if(parsed.begin(), parsed.end(), [&](const std::string& rhs) { return keyword_ht[rhs] == Project_Keyword::WHERE; });
    auto task_it = std::find_if(parsed.begin(), parsed.end(), [&](const std::string& rhs) { return keyword_ht[rhs] == Project_Keyword::TASK; });
    auto add_it = std::find_if(parsed.begin(), parsed.end(), [&](const std::string& rhs) { return keyword_ht[rhs] == Project_Keyword::ADD; });
    auto delete_it = std::find_if(parsed.begin(), parsed.end(), [&](const std::string& rhs) { return keyword_ht[rhs] == Project_Keyword::DELETE; });

    if (where_it == parsed.end()) { throw std::invalid_argument("Invalid syntax!"); }
    if ((add_it == parsed.end() && delete_it == parsed.end()) || (add_it != parsed.end() && delete_it != parsed.end()))
    {
        throw std::invalid_argument("Invalid syntax!");
    }

    Project project;

    // Parsing after where keyword
    std::string tmp;

    for (auto it = where_it + 1; ((it < add_it) && (it < delete_it)); ++it)
    {
        tmp += *it + ' ';
    }
    auto to_analize_where = parser(tmp, "\"");
    for (auto vec_it = to_analize_where.begin(); vec_it < to_analize_where.end(); vec_it += 2)
    {
        auto tmp = parser(*vec_it, " =,");
        for (auto string_it = tmp.begin(); string_it < tmp.end(); ++string_it)
        {
            if (keyword_ht[*string_it] == Project_Keyword::USER)
            {
                while ((!static_cast<bool>((keyword_ht[*(string_it + 1)]))) && (string_it != tmp.end() - 2))
                {
                    if (!user->find_email(*(string_it + 1))) { throw std::invalid_argument("No such user!"); }
                    project.user.push_back(*(string_it + 1));
                    ++string_it;
                }
            }
            project.name = (keyword_ht[*string_it] == Project_Keyword::NAME) ? *(vec_it + 1) : project.name;
            project.deadline = (keyword_ht[*string_it] == Project_Keyword::DEADLINE) ? *(string_it + 1) : project.deadline;
            project.description = (keyword_ht[*string_it] == Project_Keyword::DESCRIPTION) ? *(vec_it + 1) : project.description;
        }
    }

    std::vector<std::size_t> ID_for_modification;

    this->set_for_select(project);

    for (auto& it : project_ID)
    {
        if (for_selection_unique(project, it.second))
        {
            ID_for_modification.push_back(it.first);
        }
    }
    if (ID_for_modification.empty()) { throw std::invalid_argument("No such project!"); }

    // Analize for task
    std::string task_to_parse;

    if (task_it != parsed.end())
    {
        for (auto it = task_it + 1; (it < parsed.end() - 1) && (!static_cast<bool>(keyword_ht[*it])); ++it)
        {
            task_to_parse += *(it);
            while (!((*it).find('\"')))
            {
                task_to_parse += ' ' + *(it + 1);
                ++it;
            }
            task_to_parse += ' ';
        }
    }

    auto task_values = parser(task_to_parse, "\"");

    // Do deletion
    if (delete_it != parsed.end())
    {
        // Delete tasks
        for (auto& it_task : task_values)
        {
            if (it_task.empty()) { break; }
            if (it_task == " ") { continue; }
            for (auto it : ID_for_modification)
            {
                project_ID[it]->task.erase(std::remove(project_ID[it]->task.begin(), project_ID[it]->task.end(), task->find_ID(it_task)), project_ID[it]->task.end());
            }
        }

        // Delete users
        for (auto string_it = delete_it + 1; string_it < parsed.end(); ++string_it)
        {
            if (keyword_ht[*string_it] == Project_Keyword::USER)
            {
                while ((!static_cast<bool>((keyword_ht[*(string_it + 1)]))) && (string_it != parsed.end() - 2))
                {
                    for (auto it : ID_for_modification)
                    {
                        project_ID[it]->user.erase(std::remove(project_ID[it]->user.begin(), project_ID[it]->user.end(), *(string_it + 1)), project_ID[it]->user.end());
                    }
                    ++string_it;
                }
            }
        }
    }

    // Do addition
    if (add_it != parsed.end())
    {
        // Add tasks
        for (auto& it_task : task_values)
        {
            if (it_task.empty()) { break; }
            if (it_task == " ") { continue; }
            for (auto it : ID_for_modification)
            {
                for (auto& task_name : project_ID[it]->task)
                {
                    if (task_name == it) { throw std::invalid_argument("Invalid task name!"); }
                }
                project_ID[it]->task.push_back(task->find_ID(it_task));
            }
        }

        // Add users
        for (auto string_it = add_it + 1; string_it < parsed.end(); ++string_it)
        {
            if (keyword_ht[*string_it] == Project_Keyword::USER)
            {
                while ((!static_cast<bool>((keyword_ht[*(string_it + 1)]))) && (string_it != parsed.end() - 2))
                {
                    for (auto it : ID_for_modification)
                    {
                        if (!user->find_email(*(string_it + 1))) { throw std::invalid_argument("No such user!"); }
                        for (auto& user_name : project_ID[it]->user)
                        {
                            if (user_name == *(string_it + 1)) { throw std::invalid_argument("Invalid user name!"); }
                        }
                        project_ID[it]->user.push_back(*(string_it + 1));
                    }
                    ++string_it;
                }
            }
        }
    }
}


// Check for validity
void ProjectDB::project_data_check(Project* project)
{
    if (project->name.empty())
    {
        throw std::invalid_argument("Syntax Error!");
    }
    if (project->user.empty())
    {
        throw std::invalid_argument("Syntax Error!");
    }
    if (!is_valid_date(project->deadline, project->created))
    {
        throw std::invalid_argument("Invalid Date!");
    }
    if (project->description.empty())
    {
        throw std::invalid_argument("Syntax Error!");
    }
}


// Check for Date validity
bool ProjectDB::is_valid_date(const Date& deadline, const Date& created)
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
Date ProjectDB::created_date()
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
void ProjectDB::set_all_true()
{
    this->flag_name = true;
    this->flag_user = true;
    this->flag_task = true;
    this->flag_created = true;
    this->flag_deadline = true;
    this->flag_description = true;
}


// Set all flags false
void ProjectDB::set_all_false()
{
    this->flag_name = false;
    this->flag_user = false;
    this->flag_task = false;
    this->flag_created = false;
    this->flag_deadline = false;
    this->flag_description = false;
}


// Set selection flags
void ProjectDB::set_for_select(const Project& tmp)
{
    this->for_select["name"] = tmp.name.empty() ? false : true;
    this->for_select["user"] = tmp.user.empty() ? false : true;
    this->for_select["created"] = tmp.created.empty() ? false : true;
    this->for_select["deadline"] = tmp.deadline.empty() ? false : true;
    this->for_select["description"] = tmp.description.empty() ? false : true;
}


// Select Tasks
bool ProjectDB::for_selection_unique(const Project& lhs, const Project* rhs)
{
    if (for_select["name"])
    {
        if (lhs.name != rhs->name) { return false; }
    }
    if (for_select["user"])
    {
        bool flag = false;
        for (auto& it : lhs.user)
        {
            if (std::find_if(rhs->user.begin(), rhs->user.end(), [&](const std::string& user) { return user == it; }) != rhs->user.end())
            {
                flag = true;
                break;
            }
        }
        if (!flag) { return false; }
    }
    if (for_select["created"])
    {
        if (lhs.created != rhs->created) { return false; }
    }
    if (for_select["deadline"])
    {
        if (lhs.deadline != rhs->deadline) { return false; }
    }
    if (for_select["description"])
    {
        if (lhs.description != rhs->description) { return false; }
    }
    return true;
}


// Delete tasks from data
void ProjectDB::delete_from_map(const std::vector<std::size_t>& deleted_ID)
{
    for (auto& it : deleted_ID)
    {
        if (project_ID.find(it) != project_ID.end())
        {
            project_ID.erase(it);
        }
    }
}


// Move project data to file
void ProjectDB::to_file()
{
    project_file.open("/Users/tatev/Desktop/TMS//DB/project.data", std::ios::out);
    if (!project_file.is_open())
    {
        throw std::invalid_argument("Failed to open Project DB!");
    }
    for (auto& it : project_ID)
    {
        project_file << std::endl << "{" << std::endl;
        project_file << "  ID : " << it.first << std::endl;
        project_file << "  name : " << it.second->name << std::endl;
        project_file << "  user : ";
        for (auto& user_it : it.second->user)
        {
            project_file << user_it << "|";
        }
        project_file << std::endl;
        project_file << "  task : ";
        for (auto& task_it : it.second->task)
        {
            project_file << task_it << "|";
        }
        project_file << std::endl;
        project_file << "  created : " << it.second->created << std::endl;
        project_file << "  deadline : " << it.second->deadline << std::endl;
        project_file << "  description : " << it.second->description << std::endl;
        project_file << "}";
    }
    project_file.close();
    if (project_file.is_open())
    {
        std::cout << "Failed to close the file" << std::endl;
    }
}


// ProjectDB destructor
ProjectDB::~ProjectDB()
{
    for (auto& it : project_ID)
    {
        delete it.second;
    }
    project_ID.clear();
}