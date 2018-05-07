#include "label.h"


/*************************************************
Function:  readGBKLabel
Description: ��ȡGBK��ǩ
���룺
1.fileName����ǩ�ļ�·��
�����
1.labelGBK:��ǩmapͼ
Date��2017/02/16
*************************************************/
std::map<int, int> readGBKLabel(std::string fileName)
{
	std::map<int, int> labelGBK;
	std::ifstream inFile;
	std::string line;
	int label = 0;
	inFile.open(fileName.c_str(), std::ios::in);
	if (inFile) // �и��ļ�  
	{
		while (getline(inFile, line)) // line�в�����ÿ�еĻ��з�  
		{
			int number = std::atoi(line.c_str());
			labelGBK.insert(std::pair<int, int>(label, number));
			label++;			
		}
	}
	else // û�и��ļ�  
	{
		std::cout << "no such file" << std::endl;
	}
	return labelGBK;

}

/*************************************************
Function:  readChoiceLabel
Description: ��ȡChoice��ǩ
���룺
1.fileName����ǩ�ļ�·��
�����
1.ChoiceLabel:��ǩmapͼ
Date��2017/02/16
*************************************************/
std::map<int, std::string> readChoiceLabel(std::string fileName)
{
	std::map<int, std::string> ChoiceLabel;
	std::ifstream inFile;
	std::string line;
	int label = 0;
	inFile.open(fileName.c_str(), std::ios::in);
	if (inFile) // �и��ļ�  
	{
		while (getline(inFile, line)) // line�в�����ÿ�еĻ��з�  
		{
			ChoiceLabel.insert(std::pair<int, std::string>(label, line));
			label++;			
		}
	}
	else // û�и��ļ�  
	{
		std::cout << "no such file" << std::endl;
	}
	return ChoiceLabel;

}


