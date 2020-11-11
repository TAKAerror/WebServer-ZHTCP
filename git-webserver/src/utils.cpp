//
// Created by marvinle on 2019/2/1 12:18 PM.
//



#include <string>
//删除一个string前后的空格、\t
std::string& ltrim(std::string &str) {
    if (str.empty()) {
        return str;
    }

    str.erase(0, str.find_first_not_of(" \t"));
    return str;
}

std::string&  rtrim(std::string &str) {
    if (str.empty()) {
        return str;
    }
    str.erase(str.find_last_not_of(" \t")+1);
    return str;
}

std::string& trim(std::string& str) {
    if (str.empty()) {
        return str;
    }

    ltrim(str);
    rtrim(str);
    return str;
}


