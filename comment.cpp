// for (auto& it : user_ID) {
    //     std::cout << it.first << " ";
    //     print(it.second);
    //     std::cout << std::endl;
    // }

    // std::string line = "";
    // std::string tmp = "";
    // std::streampos index_ID = 2;
    // while (!user_file.eof()) {
    //     // user_file.seekg(index_ID + 8);
    //     getline(user_file.seekg(index_ID), line);
    //     int i = line.size() - 1;
    //     while (line[i] != ' ') {
    //         tmp += line[i];
    //         --i;
    //     }
    //     std::cout << "LINE = " << line << std::endl;
    //     std::cout << "TMP = " << tmp << std::endl;
    //     ID_pos.insert(std::make_pair(std::stoi(tmp), user_file.tellg()));
    //     tmp = "";
    //     index_ID += 8;
    // }
    // // while (!user_file.eof()) {
    // //     std::getline(user_file, line);
    // //     if (line.find("ID") != std::string::npos) {
    // //         ++ID;
    // //         
    // //         ID_pos.insert(std::stoi(tmp), user_file.tellg());
    // //         break; 
    // //     }
    // // }
    // // std::streampos i = ID_pos[0];
    // // while ()
    // std::string ps = "";
    // for (auto& it : ID_pos) {
    //     std::getline(user_file.seekg(it.second), ps);
    //     std::cout << it.first << " " << ps << std::endl;
    // }