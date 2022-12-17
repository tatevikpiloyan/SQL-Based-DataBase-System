#ifndef USER_H
#define USER_H
#include <iterator>
#include "connect.h"
#include "/Users/tatev/Desktop/TMS/headerFiles/pars.h"
#include <fstream>


// User 
struct User
{
    std::string m_name;
    std::string m_surname;
    std::string m_position;
    std::string m_email;
    std::string m_password;
    std::string m_phone_number;

    User() = default;
    User(const User&);
};


// UserDB
class UserDB : public Connect
{
public:
    UserDB();
    ~UserDB();
    virtual void insert(std::vector<std::string>& rhs) override;
    virtual void select(std::vector<std::string>&) override;
    virtual void delete_(std::vector<std::string>&) override; 
    virtual void update(std::vector<std::string>&) override; 
    virtual void to_file() override;
    bool find_email(const std::string&);

private:
    void init();
    void user_data_check(User*) ;
    bool for_selection_unique(const User&, User*);
    bool is_valid_email(std::string rhs);
    bool is_valid_phone_number(const std::string& rhs);
    void set_all_true();
    void set_all_false();
    void set_for_select(const User&);
    void delete_from_map(const std::vector<std::size_t>&);

private:
    // Static ID count
    static std::size_t ID;

    // Connect user ID with user DATA
    std::map<std::size_t, User*> user_ID;
        
    // Connect user ID with user FILE POSITION
    std::map<std::size_t, std::streampos> ID_pos;
    
    // To open FILE
    std::fstream user_file;

    // Keywords to use for UserDB
    std::map<std::string, User_Keyword> keyword_ht;

private:
    // Flags for select and update
    bool flag_name;
    bool flag_surname;
    bool flag_email;
    bool flag_phone_number;
    bool flag_position;
};

#endif // USER_H