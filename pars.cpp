#include </Users/tatev/Desktop/TMS/headerFiles/pars.h>
#include <iostream>


// Parse string by spliter to make vector of strings
std::vector<std::string> parser(const std::string& str, const std::string& spliter)
{
	std::vector<std::string> nstr;
	std::string tstr;
	for (int i = 0; i < str.size(); ++i)
    {
		if (spliter.find(str[i]) != std::string::npos)
        {   
            if (tstr != "") {
			    nstr.push_back(tstr);
			    tstr.clear();
			    continue;
            }
            continue;
		}
		tstr.push_back(str[i]);
	}
	nstr.push_back(tstr);
	return nstr;
}


// Make string lowercase
std::string lowercase(std::string& rhs)
{
    std::size_t size = rhs.size();
    for (int i = 0; i < size; ++i)
    {
        if (rhs[i] >= 'A' && rhs[i] <= 'Z')
        {
            rhs[i] += 32;
        }
    }
    return rhs;
}


// Converts vector type to string type
std::string to_string(std::vector<std::string>& rhs)
{
    std::size_t size = rhs.size();
    std::string to_pars;
    for (auto& iter : rhs)
    {
        to_pars += iter + " ";
    }
    return to_pars;
}


// Parse every line from file
std::string pars_data(const std::string& line)
{
    int i = 0;
    while (line[i] != ':')
    {
        ++i;
    }
    return line.substr(i + 2, line.size() - 1);
}