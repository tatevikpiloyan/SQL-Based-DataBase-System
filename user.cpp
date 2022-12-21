#include </Users/tatev/Desktop/TMS/headerFiles/user.h>
#include <iostream>


std::size_t UserDB::ID = 0;


// User copy constructor 
User::User(const User& rhs)
{
    this->m_name = rhs.m_name;
    this->m_surname = rhs.m_surname;
    this->m_position = rhs.m_position;
    this->m_email = rhs.m_email;
    this->m_password = rhs.m_password;
    this->m_phone_number = rhs.m_phone_number;
}


// Initialize keyword_ht map
void UserDB::init()
{
    this->keyword_ht["name"] = User_Keyword::NAME;
    this->keyword_ht["surname"] = User_Keyword::SURNAME;
    this->keyword_ht["position"] = User_Keyword::POSITION;
    this->keyword_ht["email"] = User_Keyword::EMAIL;
    this->keyword_ht["phone_number"] = User_Keyword::PHONE_NUMBER;
    this->keyword_ht["where"] = User_Keyword::WHERE;
    this->keyword_ht["set"] = User_Keyword::SET;
    this->keyword_ht["all"] = User_Keyword::ALL;

    this->set_all_false();
}


// User constructor
UserDB::UserDB()
{
    // Do init() function
    init();

    // Opening user.data file
    user_file.open("/Users/tatev/Desktop/TMS/DB/user.data");
    if (!user_file.is_open())
    {
       throw std::runtime_error("Failed to open User DB");
    }
    
    // Analizing datas
    std::string line;
    std::string tmp;
    while (!user_file.eof())
    {
        std::getline(user_file, line);
        if (!(line.empty()) && (line.find("ID") != std::string::npos))
        {
            tmp += pars_data(line);
            ID_pos.insert(std::make_pair(std::stoi(tmp), user_file.tellg()));
            user_ID[std::stoi(tmp)] = new User;
        }
        tmp.clear();
        line.clear();
    }
    std::string object_data = "";
    for (auto& it : ID_pos)
    {
        // User name
        getline(user_file.seekg(it.second), object_data);
        user_ID[it.first]->m_name = pars_data(object_data);
        // User surname
        getline(user_file, object_data);
        user_ID[it.first]->m_surname = pars_data(object_data);
        // User position
        getline(user_file, object_data);
        user_ID[it.first]->m_position = pars_data(object_data);
        // User email
        getline(user_file, object_data);
        user_ID[it.first]->m_email = pars_data(object_data);
        // User phone_number
        getline(user_file, object_data);
        user_ID[it.first]->m_phone_number = pars_data(object_data);
    }

    // Setting ID value
    if (user_ID.empty())
    {
        user_file.close();
        if (user_file.is_open())
        {
            std::cout << "Failed to close the file" << std::endl;
        }
        return;
    }
    auto it = user_ID.end();
    ID = (--it)->first;
    ++ID;

    // Closing user.data file
    user_file.close();
    if (user_file.is_open())
    {
        std::cout << "Failed to close the file" << std::endl;
    }
}


// Insert function implementation
void UserDB::insert(std::vector<std::string>& rhs)
{
    // Analizing input
    std::string split = " =";
    std::string to_pars = to_string(rhs);
    auto parsed = parser(to_pars, split);

    // Setting values
    User tmp;
    for (auto it = parsed.begin(); it != parsed.end(); ++it)
    {
        if (static_cast<bool>(keyword_ht[*it]))
        {
            tmp.m_name = (keyword_ht[*it] == User_Keyword::NAME) ? *(it + 1) : tmp.m_name;
            tmp.m_surname = (keyword_ht[*it] == User_Keyword::SURNAME) ? *(it + 1) : tmp.m_surname;
            tmp.m_position = (keyword_ht[*it] == User_Keyword::POSITION) ? *(it + 1) : tmp.m_position;
            tmp.m_email = (keyword_ht[*it] == User_Keyword::EMAIL) ? *(it + 1) : tmp.m_email;
            tmp.m_phone_number = (keyword_ht[*it] == User_Keyword::PHONE_NUMBER) ? *(it + 1) : tmp.m_phone_number;
            ++it;
        }
    }
    user_data_check(&tmp);
    user_ID[ID] = new User(tmp);
    ++ID;
}


// Select function implementation
void UserDB::select(std::vector<std::string>& rhs)
{
    // Analizing input
    std::string spliter = " =";
    std::string to_pars = to_string(rhs);
    auto parsed = parser(to_pars, spliter);
    
    // Finding keyword positions
    auto all_it = std::find_if(parsed.begin(), parsed.end(),[&](const std::string& rhs) { return keyword_ht[rhs] == User_Keyword::ALL; });
    auto where_it = std::find_if(parsed.begin(), parsed.end(),[&](const std::string& rhs) { return keyword_ht[rhs] == User_Keyword::WHERE; });
    
    // Setting flags
    for (auto it = parsed.begin(); it < where_it; ++it)
    {
        if (static_cast<bool>(keyword_ht[*it]))
        {
            this->flag_name = (keyword_ht[*it] == User_Keyword::NAME) ? true: flag_name;
            this->flag_surname = (keyword_ht[*it] == User_Keyword::SURNAME) ? true : flag_surname;
            this->flag_position = (keyword_ht[*it] == User_Keyword::POSITION) ? true: flag_position;
            this->flag_email = (keyword_ht[*it] == User_Keyword::EMAIL) ? true: flag_email;
            this->flag_phone_number = (keyword_ht[*it] == User_Keyword::PHONE_NUMBER) ? true: flag_phone_number;
        }
        if (keyword_ht[*it] == User_Keyword::ALL)
        {
            this->set_all_true();
            break;
        }
    }
    if (std::distance(all_it, where_it) != 1 && all_it != parsed.end())
    {
        throw std::invalid_argument("Invalid syntax!");
    }

    ++where_it;

    User tmp;
    while (where_it < parsed.end() - 1)
    {
        if (!static_cast<bool>(keyword_ht[*where_it]))
        { 
            throw std::invalid_argument("Invalid keyword!");
        }
        tmp.m_name = (keyword_ht[*where_it] == User_Keyword::NAME) ? *(where_it + 1) : tmp.m_name;
        tmp.m_surname = (keyword_ht[*where_it] == User_Keyword::SURNAME) ? *(where_it + 1) : tmp.m_surname;
        tmp.m_position = (keyword_ht[*where_it] == User_Keyword::POSITION) ? *(where_it + 1) : tmp.m_position;
        tmp.m_email = (keyword_ht[*where_it] == User_Keyword::EMAIL) ? (*(where_it + 1)) : tmp.m_email;
        tmp.m_phone_number = (keyword_ht[*where_it] == User_Keyword::PHONE_NUMBER) ? *(where_it + 1) : tmp.m_phone_number;
        where_it += 2;
    }

    this->set_for_select(tmp);    
    for (auto& it : user_ID)
    {
        if (for_selection_unique(tmp, it.second))
        {
            if (flag_name)
            {
                std::cout << "name : " << it.second->m_name << std::endl;
            }
            if (flag_surname)
            {
                std::cout << "surname : " << it.second->m_surname << std::endl;
            }
            if (flag_email)
            {
                std::cout << "email : " << it.second->m_email << std::endl;
            }
            if (flag_position)
            {
                std::cout << "position : " << it.second->m_position << std::endl;
            }
            if (flag_phone_number)
            {
                std::cout << "phone_number : " << it.second->m_phone_number << std::endl;
            }
            std::cout << std::endl;
        }
    }
    this->set_all_false();
}


// Delete function implementation
void UserDB::delete_(std::vector<std::string>& rhs)
{
    // Analizing input
    std::string split = " =";
    std::string to_pars = to_string(rhs);
    auto parsed = parser(to_pars, split);
    
    // Finding keyword position
    auto where_it = std::find_if(parsed.begin(), parsed.end(),[&](const std::string& rhs) { return keyword_ht[rhs] == User_Keyword::WHERE; });
    if (where_it == parsed.end())
    {
        throw std::invalid_argument("Invalid syntax!");
    }

    ++where_it;

    std::size_t count_for_deletion = 0;
    std::vector<std::size_t> tmp_deletion;
    User tmp;
    while (where_it < parsed.end() - 1)
    {
        if (!static_cast<bool>(keyword_ht[*where_it]))
        { 
            throw std::invalid_argument("Invalid keyword!");
        }
        tmp.m_name = (keyword_ht[*where_it] == User_Keyword::NAME) ? *(where_it + 1) : tmp.m_name;
        tmp.m_surname = (keyword_ht[*where_it] == User_Keyword::SURNAME) ? *(where_it + 1) : tmp.m_surname;
        tmp.m_position = (keyword_ht[*where_it] == User_Keyword::POSITION) ? *(where_it + 1) : tmp.m_position;
        tmp.m_email = (keyword_ht[*where_it] == User_Keyword::EMAIL) ? (*(where_it + 1)) : tmp.m_email;
        tmp.m_phone_number = (keyword_ht[*where_it] == User_Keyword::PHONE_NUMBER) ? *(where_it + 1) : tmp.m_phone_number;
        where_it += 2;
    }
    
    // Finding users to delete
    this->set_for_select(tmp);    
    for (auto& it : user_ID)
    {
        if (for_selection_unique(tmp, it.second))
        {
            std::cout << "name : " << it.second->m_name << std::endl;
            std::cout << "surname : " << it.second->m_surname << std::endl;
            std::cout << "email : " << it.second->m_email << std::endl;
            std::cout << "position : " << it.second->m_position << std::endl;
            std::cout << "phone_number : " << it.second->m_phone_number << std::endl;
            ++count_for_deletion;
            tmp_deletion.push_back(it.first);
        }
    }
    std::cout << count_for_deletion << " users found to delete!" << std::endl;
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

    // Delete users from data
    this->delete_from_map(tmp_deletion);
    this->set_all_false();
}


// Update function implementation
void UserDB::update(std::vector<std::string>& rhs)
{
    // Analizing input
    std::string spliter = " =,";
    std::string to_pars = to_string(rhs);
    auto parsed = parser(to_pars, spliter);
    
    // Finding keywords' iterators
    auto set_it = std::find_if(parsed.begin(), parsed.end(),[&](const std::string& rhs) { return keyword_ht[rhs] == User_Keyword::SET; });
    auto where_it = std::find_if(parsed.begin(), parsed.end(),[&](const std::string& rhs) { return keyword_ht[rhs] == User_Keyword::WHERE; });
    
    if (std::distance(set_it, where_it) < 3)
    {
        throw std::invalid_argument("Invalid syntax!");
    }

    ++where_it;
    ++set_it;
    std::vector<std::size_t> ID_for_update;
    User where_tmp;

    // Find users to update
    while (where_it < parsed.end() - 1)
    {
        if (!static_cast<bool>(keyword_ht[*where_it]))
        {
            throw std::invalid_argument("Invalid keyword!");
        }
        where_tmp.m_name = (keyword_ht[*where_it] == User_Keyword::NAME) ? *(where_it + 1) : where_tmp.m_name;
        where_tmp.m_surname = (keyword_ht[*where_it] == User_Keyword::SURNAME) ? *(where_it + 1) : where_tmp.m_surname;
        where_tmp.m_position = (keyword_ht[*where_it] == User_Keyword::POSITION) ? *(where_it + 1) : where_tmp.m_position;
        where_tmp.m_email = (keyword_ht[*where_it] == User_Keyword::EMAIL) ? (*(where_it + 1)) : where_tmp.m_email;
        where_tmp.m_phone_number = (keyword_ht[*where_it] == User_Keyword::PHONE_NUMBER) ? *(where_it + 1) : where_tmp.m_phone_number;
        where_it += 2;
    }

    // Finding users to update
    this->set_for_select(where_tmp);

    for (auto& it : user_ID)
    {
        if (for_selection_unique(where_tmp, it.second))
        {
            ID_for_update.push_back(it.first);
        }
    }
    
    // Set data to update
    User set_tmp;

    while (set_it < where_it - 1)
    {
        if (!static_cast<bool>(keyword_ht[*set_it]))
        {
            throw std::invalid_argument("Invalid keyword!");
        }
        set_tmp.m_name = (keyword_ht[*set_it] == User_Keyword::NAME) ? *(set_it + 1) : set_tmp.m_name;
        set_tmp.m_surname = (keyword_ht[*set_it] == User_Keyword::SURNAME) ? *(set_it + 1) : set_tmp.m_surname;
        set_tmp.m_position = (keyword_ht[*set_it] == User_Keyword::POSITION) ? *(set_it + 1) : set_tmp.m_position;
        set_tmp.m_email = (keyword_ht[*set_it] == User_Keyword::EMAIL) ? (*(set_it + 1)) : set_tmp.m_email;
        set_tmp.m_phone_number = (keyword_ht[*set_it] == User_Keyword::PHONE_NUMBER) ? *(set_it + 1) : set_tmp.m_phone_number;
        set_it += 2;
    }

    // Change user_ID users' data
    for (auto& i : ID_for_update)
    {
        if (!set_tmp.m_name.empty())
        {
            user_ID[i]->m_name = set_tmp.m_name;
        }
        if (!set_tmp.m_surname.empty())
        {
            user_ID[i]->m_surname = set_tmp.m_surname;
        }
        if (!set_tmp.m_position.empty())
        {
            user_ID[i]->m_position = set_tmp.m_position;
        }
        if (!set_tmp.m_email.empty())
        {
            if (!is_valid_email(set_tmp.m_email))
            {
                throw std::invalid_argument("Wrong email!"); 
            }
            user_ID[i]->m_email = set_tmp.m_email;
        }
        if (!set_tmp.m_phone_number.empty())
        {
            if (!is_valid_phone_number(set_tmp.m_phone_number))
            {
                throw std::invalid_argument("Wrong phone number!"); 
            }
            user_ID[i]->m_phone_number = set_tmp.m_phone_number;
        }
    }
}


// Check for data validation
void UserDB::user_data_check(User* user)
{
    if (user->m_name.empty())
    { 
        throw std::invalid_argument("Invalid syntax!"); 
    }
    if (user->m_surname.empty())
    {
        throw std::invalid_argument("Invalid syntax!"); 
    }
    if (user->m_position.empty())
    { 
        user->m_position = "NIL";
    }
    if (!is_valid_email(user->m_email))
    {
        throw std::invalid_argument("Invalid email!"); 
    }
    if (!is_valid_phone_number(user->m_phone_number))
    {
        throw std::invalid_argument("Invalid phone number!"); 
    }
}


// Check for email validity
bool UserDB::is_valid_email(std::string rhs)
{
    if (rhs.empty()) { return false; }
    auto unique = std::find_if(user_ID.begin(), user_ID.end(), [&](std::pair<std::size_t, User*> for_check) { return for_check.second->m_email == rhs; });
    if (unique != user_ID.end())
    { 
        throw std::invalid_argument("This email is used!");
    }
    std::size_t at_signe_ind = rhs.find('@');

    if (at_signe_ind == std::string::npos) { return false; }
    std::string to_check = rhs.substr(at_signe_ind, rhs.size() - at_signe_ind);
    
    for (int i = 0; i < at_signe_ind; ++i)
    {
        if (!((rhs[i] >= 'a' && rhs[i] <= 'z') || (rhs[i] >= 'A' && rhs[i] <= 'Z') || (rhs[i] == 46) || (rhs[i] == 95) || (rhs[i] >= '0' && rhs[i] <= '9')))
        {
            return false;
        }
    } 
    if ((to_check == "@gmail.com") || (to_check == "@mail.ru"))
    {
        return true;
    }
    return false;
}


// Check for phone_number validity
bool UserDB::is_valid_phone_number(const std::string& rhs)
{
    if (rhs.empty()) { return false;}
    auto unique = std::find_if(user_ID.begin(), user_ID.end(), [&](std::pair<std::size_t, User*> for_check) { return for_check.second->m_phone_number == rhs; });
    if (unique != user_ID.end())
    {
        throw std::invalid_argument("This phone number is used!");
    }
    for (auto& it : rhs)
    {
        if (!(it >= '0' && it <= '9')) { return false; }
    }
    return true;
}


// Select Tasks
bool UserDB::for_selection_unique(const User& lhs, User* rhs)
{
    if (for_select["name"]) {
        if (lhs.m_name != rhs->m_name) { return false; }
    }
    if (for_select["surname"]) {
        if (lhs.m_surname != rhs->m_surname) { return false; }
    }
    if (for_select["position"]) {
        if (lhs.m_position != rhs->m_position) { return false; }
    }
    if (for_select["email"]) {
        if (lhs.m_email != rhs->m_email) { return false; }
    }
    if (for_select["phone_number"]) {
        if (lhs.m_phone_number != rhs->m_phone_number) { return false; }
    }
    return true;
}


// Set all flags true
void UserDB::set_all_true()
{
    this->flag_name = true;
    this->flag_surname = true;
    this->flag_email = true;
    this->flag_phone_number = true;
    this->flag_position = true;
}


// Set all flags fasle
void UserDB::set_all_false()
{
    this->flag_name = false;
    this->flag_surname = false;
    this->flag_email = false;
    this->flag_phone_number = false;
    this->flag_position = false;
}


// Set flags to select
void UserDB::set_for_select(const User& tmp)
{
    this->for_select["name"] = tmp.m_name.empty() ? false : true;
    this->for_select["surname"] = tmp.m_surname.empty() ? false : true;
    this->for_select["position"] = tmp.m_position.empty() ? false : true;
    this->for_select["email"] = tmp.m_email.empty() ? false : true;
    this->for_select["phone_number"] = tmp.m_phone_number.empty() ? false : true;
}


// Delete tasks from data
void UserDB::delete_from_map(const std::vector<std::size_t>& deleted_ID)
{
    for (std::map<std::size_t, User*>::iterator it = user_ID.begin(); it != user_ID.end(); ++it)
    {
        if (std::find(deleted_ID.begin(), deleted_ID.end(), it->first) != deleted_ID.end())
        {
            user_ID.erase(it->first);
            it = user_ID.begin();
        }
    }
}


// Inherited modify function
void UserDB::modify(std::vector<std::string>&)
{
    throw std::invalid_argument("Syntax error!");
}


// Move user data to file
void UserDB::to_file()
{
    user_file.open("/Users/tatev/Desktop/TMS//DB/user.data", std::ios::out);
    if (!user_file.is_open())
    {
        throw std::invalid_argument("Failed to open User DB!");
    }
    for (auto it : user_ID)
    {
        user_file << std::endl << "{" << std::endl;
        user_file << "  ID : " << it.first << std::endl;
        user_file << "  name : " << it.second->m_name << std::endl;
        user_file << "  surname : " << it.second->m_surname << std::endl;
        user_file << "  position : " << it.second->m_position << std::endl;
        user_file << "  email : " << it.second->m_email  << std::endl;
        user_file << "  phone_number : " << it.second->m_phone_number << std::endl;
        user_file << "}";
    }
    user_file.close();
    if (user_file.is_open())
    {
        std::cout << "Failed to close the file" << std::endl;
    }
}


// Find user email
bool UserDB::find_email(const std::string& rhs)
{
    for (auto& it : user_ID)
    {
        if (it.second->m_email == rhs) { return true; }
    }
    return false;
}


// UserDB destructor
UserDB::~UserDB()
{
    for (auto& it : user_ID)
    {
        delete it.second;
    }
    user_ID.clear();
}