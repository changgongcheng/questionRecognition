#pragma once
#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <vector>

//Description: 读取GBK标签
std::map<int, int> readGBKLabel(std::string fileName);

//Description: 读取Choice标签
std::map<int, std::string> readChoiceLabel(std::string fileName);