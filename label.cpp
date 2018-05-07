#include "label.h"


/*************************************************
Function:  readGBKLabel
Description: 读取GBK标签
输入：
1.fileName：标签文件路径
输出：
1.labelGBK:标签map图
Date：2017/02/16
*************************************************/
std::map<int, int> readGBKLabel(std::string fileName)
{
	std::map<int, int> labelGBK;
	std::ifstream inFile;
	std::string line;
	int label = 0;
	inFile.open(fileName.c_str(), std::ios::in);
	if (inFile) // 有该文件  
	{
		while (getline(inFile, line)) // line中不包括每行的换行符  
		{
			int number = std::atoi(line.c_str());
			labelGBK.insert(std::pair<int, int>(label, number));
			label++;			
		}
	}
	else // 没有该文件  
	{
		std::cout << "no such file" << std::endl;
	}
	return labelGBK;

}

/*************************************************
Function:  readChoiceLabel
Description: 读取Choice标签
输入：
1.fileName：标签文件路径
输出：
1.ChoiceLabel:标签map图
Date：2017/02/16
*************************************************/
std::map<int, std::string> readChoiceLabel(std::string fileName)
{
	std::map<int, std::string> ChoiceLabel;
	std::ifstream inFile;
	std::string line;
	int label = 0;
	inFile.open(fileName.c_str(), std::ios::in);
	if (inFile) // 有该文件  
	{
		while (getline(inFile, line)) // line中不包括每行的换行符  
		{
			ChoiceLabel.insert(std::pair<int, std::string>(label, line));
			label++;			
		}
	}
	else // 没有该文件  
	{
		std::cout << "no such file" << std::endl;
	}
	return ChoiceLabel;

}


