#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
//#include <Eigen>
//#include <Dense>
//#include <opencv2/core/eigen.hpp>
#include <vector>
#include <numeric>
#include "classification.h"

namespace cv
{
	using std::vector;
}
//ȥ���׶�����ȥ��С���򣬲�������������ʵ��
//srcΪ��ֵͼ��ǰ��Ϊ1������Ϊ0
//checkMode: 0����ȥ��������1����ȥ��������; NeihborMode��0����4����1����8����; 
void RemoveSmallRegion(cv::Mat& src, cv::Mat& dst, int areaLimit, int checkMode, int neihborMode);

cv::Mat SegmentValidParts2(cv::Mat& src, int continuousNum);

//Description: ȥ���ַ�֮���ո񣬽�ͼ���и��ͼ��鼯��
void RemoveBigSpaces(cv::Mat src, std::vector<cv::Mat> &blockImgs, int bigSpaceThreshold);

//Description: ����ճ����һ��Ĺ�����ַ�������ƽ����
std::vector<cv::Mat> SplitTooWideCharacters(cv::Mat& blockImg, float scale);

std::vector<cv::Mat> SplitTooWideCharacters2(cv::Mat& blockImg, float scale, int rangeValue);

//Description: �Ӵ����ַ���ͼ���зָ�������ַ�
void SingleCharacterSegmentation(cv::Mat& img, std::vector<cv::Mat>& characterImgs);


//Description: ȥ�����߿򣬻�ȡ�ַ�ͼ�����Ч����
void RemoveBorder(cv::Mat& src, int continuousNum, float pixelRatio);


//Description: ����ˮƽͶӰ��������дʱ����б�벻ˮƽ������
cv::Mat MultiColProjection(cv::Mat& src, int colNum);

//Description: ȥ��ͶӰ��������ɢ�ĵ�
void RemoveDiscretePoint(cv::Mat& src, cv::Mat& dst, int thresValue);

//Description: ����ͶӰ���ʺϵķָ�λ��
std::vector<std::vector<int>> FindSegPosition(cv::Mat& src, float ratio);

//Description: ���ÿ��ͶӰ�εķָ��
std::vector<int> DetectSegPoint(std::vector<int> proSection, int startPoint, int meanValue, float ratio);

//Description: �ɵõ��ķָ��Զ����ı��������ı��ָ�
std::vector<cv::Mat> SegLineText(cv::Mat& img, std::vector<std::vector<int>>& proVector, int colNum);



//Description: ������ĸ��ǵ�
cv::Rect DetectCornerRect(cv::Mat &image);

//Description: ��ͷ��������ѧ�ŵȱ�־����ƥ��
double LandmarkMatch(cv::Mat &src, cv::Mat &modelImg);

//Description: ����
void drawDetectedLines(cv::Mat &image, std::vector<cv::Vec4i> lines);

//Description: �ӻ���任�м�⵽��ֱ������ȡ��ֱ�߶�
void VerticalLineDectect(std::vector<cv::Vec4i> &lines, std::vector<cv::Vec4i> &verticalLines, int thres);


//Description: ���������е�ֱ�ߵĵ㶼�Ǵ��ϵ�������
std::vector<cv::Vec4i> UpDownSort(std::vector<cv::Vec4i> verticalLines);

//Description: ���������е�ֱ�ߴ���������
std::vector<cv::Vec4i> LeftRightSort(std::vector<cv::Vec4i> verticalLines);

//Description: ����ֱ�߶κϲ�
std::vector<cv::Vec4i> VerticalLinesMerge(std::vector<cv::Vec4i> verticalLines, int thres);

//Description: ��ˮƽ�߶κϲ�
std::vector<cv::Vec4i> HorizontalLinesMerge(std::vector<cv::Vec4i> horizontalLines, int thres);

//Description: ����ֱ�߶κϲ�
int RegionRecognition(std::vector<cv::Vec4i> mergedLines);

//Description: �������༶��ѧ����Ч������ȡ
void ValidHeaderExtraction(cv::Mat &image, cv::Mat &validImg, std::vector<cv::Vec4i> sortedLines);

//Description: ��ͷ�ָ�
void HeaderSegmentation(cv::Mat &image, std::vector<cv::Mat>& headerImgs, int thres);

//Description: ѡ����ָ�
void ChooseSegmentation(cv::Mat &image, std::vector<cv::Mat> &orderImgs, std::vector<cv::Mat> &chooseImgs, int thres);


//Description: �����ָ�
void GapSegmentation(cv::Mat &image, std::vector<cv::Mat> &orderImgs, std::vector<cv::Mat> &gapImgs, int thres);


//Description: ͨ���������ָ�
void GeneralGapSegmentation(cv::Mat &image, std::vector<cv::Mat> &orderImgs, std::vector<cv::Mat> &gapImgs, std::vector<cv::Vec4i> &upDownSortLines, std::vector<cv::Vec4i> &leftRightSortLines);

//Description: �ʴ���ָ�
void QuestionSegmentation(cv::Mat &image, std::vector<cv::Mat> &questionImgs, int thres);

//Description: ͼ�����, ͨ���������ȷ�������Ǳ�־���Σ�Ȼ�������ת�Ƕȣ�Ȼ����з���任
void ImgCorrection2(cv::Mat& src, cv::Mat& warpImage);

//Description: ȥ�����߿򣬻�ȡ��Ч�ַ�����
void GetValidMat(cv::Mat& img, cv::Mat& validImg, int N = 5);

//Description: ���������ַ�ʶ��
int SingleCharaRecognition(Classifier classifier, cv::Mat& characterImg);


int NumberModelMatch(cv::Mat& characterImg);


void SingleNumberLetterCrop(cv::Mat& src, cv::Mat& dst);

//Description: ѧ�ŷָ�
void NumberSegmentation(cv::Mat& image, std::vector<cv::Mat> &numberImgs, int num);

//Description: ȥ���߿򣬴����ң����ϵ��£���ȡ�ַ�ͼ�����Ч����;
void RemoveBorder2(cv::Mat& src, float pixelRatio);


void SingleCharacterSegmentation2(cv::Mat& img, std::vector<cv::Mat>& characterImgs);

//Description: ɾ������Ԫ�أ�����ƽ��ֵ
void RemoveContinuousElements(std::vector<int>& inputVector, std::vector<int>& outputVector);

//Description: ��ȡ�ַ�ͼ�����Ч�߶�;
int ImgValidHeight(cv::Mat& src, int continuousNum);

//Description: �������ַ�ͼ��ֳ�colNum��ƽ����Ч���
int ComputeMeanLineHeight(cv::Mat& src, int colNum);

//Description: ���ַ�����Χ��ӿհ�
cv::Mat PeripheralProcess(cv::Mat& curMat, int ratioValue = 8);


//Description: ��ȡ�ַ�ͼ�����Ч���;
int ImgValidWidth(cv::Mat& src, int continuousNum);