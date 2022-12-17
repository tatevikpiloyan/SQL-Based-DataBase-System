#ifndef PARS_H
#define PARS_H
#include <vector>
#include <string>

std::string lowercase(std::string&);
std::vector<std::string> parser(const std::string& , const std::string&);
std::string to_string(std::vector<std::string>&);
std::string pars_data(const std::string&);

#endif //PARS_H