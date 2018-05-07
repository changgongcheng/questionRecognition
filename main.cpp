#include "classification.h"
#include "segmentation.h"
#include "label.h"



int main(int argc, char** argv) 
{
	//��16�������ŵ�ģ��
	//std::string chiFileName = "E:/Chinese_punctuation/GBKCode.txt";
	//std::map<int, int> chiLabel = readGBKLabel(chiFileName);
	//std::string chiPrototxt = "e:/Chinese_punctuation/deploy_6.prototxt";
	//std::string chiModel = "e:/Chinese_punctuation/Inception_Vgg_A-LRN-6_iter_452880.caffemodel";
	//std::string chiMeanFile = "e:/Chinese_punctuation/mean.binaryproto";
	//std::string chiLabelFile = "e:/Chinese_punctuation/GBKlabel.txt";

	//extended hwdb data 300
	//std::string chiFileName = "E:/Chinese3755vgg/hwdbtrain/GBKCode.txt";
	//std::map<int, int> chiLabel = readGBKLabel(chiFileName);
	//std::string chiPrototxt = "e:/Chinese3755vgg/hwdbtrain/deploy_6.prototxt";
	//std::string chiModel = "e:/Chinese3755vgg/hwdbtrain/Inception_Vgg_A-LRN-6_iter_400000.caffemodel";
	//std::string chiMeanFile = "e:/Chinese3755vgg/hwdbtrain/mean.binaryproto";
	//std::string chiLabelFile = "e:/Chinese3755vgg/hwdbtrain/GBKlabel.txt";


	//hwdb300_inception_deploy5
	//std::string chiFileName = "E:/Chinese3755vgg/hwdb_inception_modify5/GBKCode.txt";
	//std::map<int, int> chiLabel = readGBKLabel(chiFileName);
	//std::string chiPrototxt = "e:/Chinese3755vgg/hwdb_inception_modify5/inception_deploy5.prototxt";
	//std::string chiModel = "e:/Chinese3755vgg/hwdb_inception_modify5/inception_modify5_iter_320000.caffemodel";
	//std::string chiMeanFile = "e:/Chinese3755vgg/hwdb_inception_modify5/mean.binaryproto";
	//std::string chiLabelFile = "e:/Chinese3755vgg/hwdb_inception_modify5/GBKlabel.txt";
	//Classifier chiClassifier(chiPrototxt, chiModel, chiMeanFile, chiLabelFile);

	//hwdb300_inception_deploy1
	//std::string chiFileName = "E:/Chinese3755vgg/hwdb_inception_modify1/GBKCode.txt";
	//std::map<int, int> chiLabel = readGBKLabel(chiFileName);
	//std::string chiPrototxt = "e:/Chinese3755vgg/hwdb_inception_modify1/inception_deploy1.prototxt";
	//std::string chiModel = "e:/Chinese3755vgg/hwdb_inception_modify1/inception_modify1_iter_400000.caffemodel";
	//std::string chiMeanFile = "e:/Chinese3755vgg/hwdb_inception_modify1/mean.binaryproto";
	//std::string chiLabelFile = "e:/Chinese3755vgg/hwdb_inception_modify1/GBKlabel.txt";
	//Classifier chiClassifier(chiPrototxt, chiModel, chiMeanFile, chiLabelFile);
	///////////////////////////////////////////////////////////////////

	//////��������ļ�
	std::ofstream outdata;
	outdata.open("questionResult.txt", std::ios::out);


	//////ͼƬ·���ļ���ȡ
	string imgListPath = "E:/cellphoneImg/question.txt";
	
	std::ifstream inFile;
	std::string imgFileName;
	inFile.open(imgListPath.c_str(), std::ios::in);

	int num = 0;

	//std::string savePath = "E:/cellphoneImg/header/";
	std::string savePath = "E:/cellphoneImg/question/single/";
	char inttoChar[64];
	int cnt = 0; //��¼ͼƬ���
	
	int thres = 30; //��ֵ

	if (inFile) // �и��ļ�  
	{
		while (getline(inFile, imgFileName)) // line�в�����ÿ�еĻ��з�  
		{
			cnt++;

			cv::Mat img = cv::imread(imgFileName, cv::IMREAD_GRAYSCALE);


			outdata << "�����:" << std::endl;
			std::cout << "�����" << std::endl;
			std::vector<cv::Mat> questionImgs;

			cv::Mat questionImg;
			//GetValidMat(img, questionImg);

	
			cv::threshold(img, questionImg, 0, 1, CV_THRESH_OTSU);

			int matWidth = questionImg.cols / 29;
			int matHeight = questionImg.rows / 9;


						
			questionImg = 1 - questionImg;//Ҫ����������
				
			//��������numberMat���򣬸��ǵ���������
			cv::Mat whiteMat(matHeight, matWidth, CV_8UC1, cv::Scalar(0));
			whiteMat.copyTo(questionImg(cv::Rect(0, 0, matWidth, matHeight)));
						
			cv::Mat proMat;
			int colNum = 4;
			proMat = MultiColProjection(questionImg, colNum);



			int thresValue = 20;
			cv::Mat dst;
			RemoveDiscretePoint(proMat, dst, thresValue);

			float ratio = 0.4;
			std::vector<std::vector<int>> proVector;

			proVector = FindSegPosition(dst, ratio);


			std::vector<cv::Mat> lineImgs;
			questionImg = 1 - questionImg;
			lineImgs = SegLineText(questionImg, proVector, colNum);



			for (int col = 0; col < lineImgs.size(); col++)
			{
				cv::Mat lineImg = lineImgs[col];


				sprintf_s(inttoChar, "quesImg%03d_%d", cnt, col);
				std::string str(inttoChar);
				std::string imgPath = savePath + str + ".jpg";
				cv::imwrite(imgPath, lineImg*255);

				std::vector<cv::Mat> characterImgs;
				lineImg = 1 - lineImg;
				SingleCharacterSegmentation2(lineImg, characterImgs);


				for (int k = 0; k < characterImgs.size(); k++)
				{
					cv::Mat filledMat = PeripheralProcess(characterImgs[k]);

					sprintf_s(inttoChar, "quesImg%03d_%d_%2d", cnt, col,k);
					std::string str(inttoChar);
					std::string imgPath = savePath + str + ".jpg";
					cv::imwrite(imgPath, filledMat);

					//int order = SingleCharaRecognition(chiClassifier, filledMat);
					//int gbkCode = chiLabel[order];
					//char ch[3];
					//ch[0] = gbkCode / 256;
					//ch[1] = gbkCode % 256;
					//ch[2] = '\0';
					//outdata << ch;
				}

				outdata << std::endl;

			}
					
			
		}
	}
	outdata.close();
	return 0;
	
}


