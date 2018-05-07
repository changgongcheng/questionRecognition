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
//去除孔洞或者去除小区域，采用区域生长法实现
//src为二值图像，前景为1，背景为0
//checkMode: 0代表去除黑区域，1代表去除白区域; NeihborMode：0代表4邻域，1代表8邻域; 
void RemoveSmallRegion(cv::Mat& src, cv::Mat& dst, int areaLimit, int checkMode, int neihborMode);

cv::Mat SegmentValidParts2(cv::Mat& src, int continuousNum);

//Description: 去除字符之间大空格，将图像切割成图像块集合
void RemoveBigSpaces(cv::Mat src, std::vector<cv::Mat> &blockImgs, int bigSpaceThreshold);

//Description: 分离粘连在一起的过宽的字符串，简单平均法
std::vector<cv::Mat> SplitTooWideCharacters(cv::Mat& blockImg, float scale);

std::vector<cv::Mat> SplitTooWideCharacters2(cv::Mat& blockImg, float scale, int rangeValue);

//Description: 从词组字符串图像中分割出单个字符
void SingleCharacterSegmentation(cv::Mat& img, std::vector<cv::Mat>& characterImgs);


//Description: 去除表格边框，获取字符图像的有效区域
void RemoveBorder(cv::Mat& src, int continuousNum, float pixelRatio);


//Description: 分列水平投影，降低书写时的倾斜与不水平的现象
cv::Mat MultiColProjection(cv::Mat& src, int colNum);

//Description: 去除投影矩阵中离散的点
void RemoveDiscretePoint(cv::Mat& src, cv::Mat& dst, int thresValue);

//Description: 查找投影段适合的分割位置
std::vector<std::vector<int>> FindSegPosition(cv::Mat& src, float ratio);

//Description: 检测每个投影段的分割点
std::vector<int> DetectSegPoint(std::vector<int> proSection, int startPoint, int meanValue, float ratio);

//Description: 由得到的分割点对段落文本进行行文本分割
std::vector<cv::Mat> SegLineText(cv::Mat& img, std::vector<std::vector<int>>& proVector, int colNum);



//Description: 检测答卷四个角点
cv::Rect DetectCornerRect(cv::Mat &image);

//Description: 卷头中姓名、学号等标志区域匹配
double LandmarkMatch(cv::Mat &src, cv::Mat &modelImg);

//Description: 画线
void drawDetectedLines(cv::Mat &image, std::vector<cv::Vec4i> lines);

//Description: 从霍夫变换中检测到的直线中提取垂直线段
void VerticalLineDectect(std::vector<cv::Vec4i> &lines, std::vector<cv::Vec4i> &verticalLines, int thres);


//Description: 排序，让所有的直线的点都是从上到下排列
std::vector<cv::Vec4i> UpDownSort(std::vector<cv::Vec4i> verticalLines);

//Description: 排序，让所有的直线从左到右排列
std::vector<cv::Vec4i> LeftRightSort(std::vector<cv::Vec4i> verticalLines);

//Description: 将垂直线段合并
std::vector<cv::Vec4i> VerticalLinesMerge(std::vector<cv::Vec4i> verticalLines, int thres);

//Description: 将水平线段合并
std::vector<cv::Vec4i> HorizontalLinesMerge(std::vector<cv::Vec4i> horizontalLines, int thres);

//Description: 将垂直线段合并
int RegionRecognition(std::vector<cv::Vec4i> mergedLines);

//Description: 姓名，班级，学号有效区域提取
void ValidHeaderExtraction(cv::Mat &image, cv::Mat &validImg, std::vector<cv::Vec4i> sortedLines);

//Description: 卷头分割
void HeaderSegmentation(cv::Mat &image, std::vector<cv::Mat>& headerImgs, int thres);

//Description: 选择题分割
void ChooseSegmentation(cv::Mat &image, std::vector<cv::Mat> &orderImgs, std::vector<cv::Mat> &chooseImgs, int thres);


//Description: 填空题分割
void GapSegmentation(cv::Mat &image, std::vector<cv::Mat> &orderImgs, std::vector<cv::Mat> &gapImgs, int thres);


//Description: 通用填空题表格分割
void GeneralGapSegmentation(cv::Mat &image, std::vector<cv::Mat> &orderImgs, std::vector<cv::Mat> &gapImgs, std::vector<cv::Vec4i> &upDownSortLines, std::vector<cv::Vec4i> &leftRightSortLines);

//Description: 问答题分割
void QuestionSegmentation(cv::Mat &image, std::vector<cv::Mat> &questionImgs, int thres);

//Description: 图像矫正, 通过轮廓检测确定三个角标志矩形，然后计算旋转角度，然后进行仿射变换
void ImgCorrection2(cv::Mat& src, cv::Mat& warpImage);

//Description: 去掉表格边框，获取有效字符区域
void GetValidMat(cv::Mat& img, cv::Mat& validImg, int N = 5);

//Description: 单个中文字符识别
int SingleCharaRecognition(Classifier classifier, cv::Mat& characterImg);


int NumberModelMatch(cv::Mat& characterImg);


void SingleNumberLetterCrop(cv::Mat& src, cv::Mat& dst);

//Description: 学号分割
void NumberSegmentation(cv::Mat& image, std::vector<cv::Mat> &numberImgs, int num);

//Description: 去除边框，从左到右，从上到下，获取字符图像的有效区域;
void RemoveBorder2(cv::Mat& src, float pixelRatio);


void SingleCharacterSegmentation2(cv::Mat& img, std::vector<cv::Mat>& characterImgs);

//Description: 删除连续元素，保留平均值
void RemoveContinuousElements(std::vector<int>& inputVector, std::vector<int>& outputVector);

//Description: 获取字符图像的有效高度;
int ImgValidHeight(cv::Mat& src, int continuousNum);

//Description: 计算行字符图像分成colNum的平均有效宽度
int ComputeMeanLineHeight(cv::Mat& src, int colNum);

//Description: 在字符四周围添加空白
cv::Mat PeripheralProcess(cv::Mat& curMat, int ratioValue = 8);


//Description: 获取字符图像的有效宽度;
int ImgValidWidth(cv::Mat& src, int continuousNum);