#pragma once
#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <vector>

//Description: ��ȡGBK��ǩ
std::map<int, int> readGBKLabel(std::string fileName);

//Description: ��ȡChoice��ǩ
std::map<int, std::string> readChoiceLabel(std::string fileName);