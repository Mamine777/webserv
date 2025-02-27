#ifndef PARSECONFIG_HPP
#define PARSECONFIG_HPP
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cctype>
#include <cstdlib> 
#include "Config.hpp"

class ParseConfig
{
private:
    std::string _filename;
    Config &config;
public:
    ParseConfig(std::string filename, Config & config);
    ~ParseConfig();
    std::string get_filename() const{return this->_filename;}
    void parse();
    size_t parseSize(const std::string &sizeStr);
};

#endif