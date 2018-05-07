/*
//////////////////
// File name:  segmentation.cpp
// Author：    ZhangCheng
// Date:       2016/12/14
// Function:   字符图像切割中相关的函数
//////////////
*/
#include "segmentation.h"

/*************************************************
Function:  removeSmallRegion
Description: 获取字符图像的有效区域; //以后可用求连通域的方式代替
//去除孔洞或者去除小区域，采用区域生长法实现
//src为二值图像，前景为1，背景为0
//checkMode: 0代表去除黑区域，1代表去除白区域; NeihborMode：0代表4邻域，1代表8邻域;
Date：2016/09/14
*************************************************/
void RemoveSmallRegion(cv::Mat& src, cv::Mat& dst, int areaLimit, int checkMode, int neihborMode)
{

	//记录每个像素点检验状态的标签，0代表未检查，1代表正在检查,2代表检查不合格（需要反转颜色），3代表检查合格或不需检查
	cv::Mat pointlabel = cv::Mat::zeros(src.size(), CV_8UC1);
	if (checkMode == 1)
	{
		//cout << "Mode: 去除小区域." << endl;
		for (int i = 0; i < src.rows; i++)
		{
			uchar* iData = src.ptr<uchar>(i);
			uchar* iLabel = pointlabel.ptr<uchar>(i);
			for (int j = 0; j < src.cols; j++)
			{
				if (iData[j] == 0)
				{
					iLabel[j] = 3;
				}
			}
		}
	}
	else
	{
		//cout << "Mode: 去除孔洞." << endl;
		for (int i = 0; i < src.rows; i++)
		{
			uchar* iData = src.ptr<uchar>(i);
			uchar* iLabel = pointlabel.ptr<uchar>(i);
			for (int j = 0; j < src.cols; j++)
			{
				if (iData[j] == 1)
				{
					iLabel[j] = 3;
				}
			}
		}
	}

	std::vector<cv::Point2i> neihborPos;  //记录邻域点位置  
	neihborPos.push_back(cv::Point2i(-1, 0));
	neihborPos.push_back(cv::Point2i(1, 0));
	neihborPos.push_back(cv::Point2i(0, -1));
	neihborPos.push_back(cv::Point2i(0, 1));
	if (neihborMode == 1)
	{
		//cout << "Neighbor mode: 8邻域." << endl;
		neihborPos.push_back(cv::Point2i(-1, -1));
		neihborPos.push_back(cv::Point2i(-1, 1));
		neihborPos.push_back(cv::Point2i(1, -1));
		neihborPos.push_back(cv::Point2i(1, 1));
	}
	else
	{
		//cout << "Neighbor mode: 4邻域." << endl;
	}
	int neihborCount = 4 + 4 * neihborMode;
	int currX = 0;
	int currY = 0;
	//开始检测  
	for (int i = 0; i < src.rows; i++)
	{
		uchar* iLabel = pointlabel.ptr<uchar>(i);
		for (int j = 0; j < src.cols; j++)
		{
			if (iLabel[j] == 0)
			{
				//********开始该点处的检查********** 				
				std::vector<cv::Point2i> growBuffer; //堆栈，用于存储生长点  
				growBuffer.push_back(cv::Point2i(j, i));
				pointlabel.at<uchar>(i, j) = 1;
				int checkResult = 0; //用于判断结果（是否超出大小），0为未超出，1为超出
				for (int z = 0; z < growBuffer.size(); z++)
				{

					for (int q = 0; q < neihborCount; q++) //检查四个邻域点  
					{
						currX = growBuffer.at(z).x + neihborPos.at(q).x;
						currY = growBuffer.at(z).y + neihborPos.at(q).y;
						if (currX >= 0 && currX < src.cols &&currY >= 0 && currY < src.rows)  //防止越界  
						{
							if (pointlabel.at<uchar>(currY, currX) == 0)
							{
								growBuffer.push_back(cv::Point2i(currX, currY)); //邻域点加入buffer  
								pointlabel.at<uchar>(currY, currX) = 1; //更新邻域点的检查标签，避免重复检查  
							}
						}
					}

				}
				if (growBuffer.size() > areaLimit)
				{
					checkResult = 2; //判断结果（是否超出限定的大小），1为未超出，2为超出  
				}

				else
				{
					checkResult = 1;

				}
				for (int z = 0; z < growBuffer.size(); z++)                         //更新Label记录  
				{
					currX = growBuffer.at(z).x;
					currY = growBuffer.at(z).y;
					pointlabel.at<uchar>(currY, currX) += checkResult;
				}
				/********结束该点处的检查**********/
			}
		}
	}

	checkMode = 1 - checkMode;
	//开始反转面积过小的区域  
	for (int i = 0; i < src.rows; i++)
	{
		uchar* iData = src.ptr<uchar>(i);
		uchar* iDstData = dst.ptr<uchar>(i);
		uchar* iLabel = pointlabel.ptr<uchar>(i);
		for (int j = 0; j < src.cols; j++)
		{
			if (iLabel[j] == 2)
			{
				iDstData[j] = checkMode;
			}
			else if (iLabel[j] == 3)
			{
				iDstData[j] = iData[j];
			}
		}
	}
}
/*************************************************
Function:  GetColSum
Description: 求图像每列的和;
Input：
1.src：输入图像
Output:
1. colSum:求和向量
Date：2017/04/27
*************************************************/
std::vector<int> GetColSum(cv::Mat& src)
{
	std::vector<int> colSum;
	for (int i = 0; i < src.cols; i++)
	{
		cv::Mat colMat = src.col(i);
		cv::Scalar sumValue = cv::sum(colMat);
		colSum.push_back(sumValue[0]);
	}
	return colSum;
}

/*************************************************
Function:  GetRowSum
Description: 求图像每行的和;
Input：
1.src：输入图像
Output:
1. rowSum:求和向量
Date：2017/04/27
*************************************************/
std::vector<int> GetRowSum(cv::Mat& src)
{
	std::vector<int> rowSum;
	for (int i = 0; i < src.rows; i++)
	{
		cv::Mat rowMat = src.row(i);
		cv::Scalar sumValue = cv::sum(rowMat);
		rowSum.push_back(sumValue[0]);
	}
	return rowSum;
}

/*************************************************
Function:  segmentValidParts2
Description: 获取字符图像的有效区域;
Input：
1.src：二值图像，前景为1，背景为0，即字白底黑
2.continuousNum: 连续出现的有效像素的行数或列数
Output:
1. segmentedImg:分割后的图像
Date：2016/09/14
*************************************************/
cv::Mat SegmentValidParts2(cv::Mat& src, int continuousNum)
{
	std::vector<int> eachRowSum = GetRowSum(src);//每行求和

	int thres = 1;
	int cnt = 0;//连续个数计数

	//上边
	int upPos = 0;
	for (int i = 0; i < eachRowSum.size(); i++)
	{
		if (eachRowSum[i] > thres)
		{
			cnt = cnt + 1;
		}
		else
		{
			cnt = 0;
		}
		if (cnt >= continuousNum)
		{
			upPos = i - continuousNum + 1;
			break;
		}
	}
	//下边
	int downPos = 0;
	cnt = 0;
	for (int i = eachRowSum.size() - 1; i >= 0; i--)
	{
		if (eachRowSum[i] > thres)
		{
			cnt = cnt + 1;
		}
		else
		{
			cnt = 0;
		}
		if (cnt >= continuousNum)
		{
			downPos = i + continuousNum - 1;
			break;
		}
	}

	std::vector<int> eachColSum = GetColSum(src);//每列求和
	//左边
	int leftPos = 0;
	cnt = 0;
	for (int i = 0; i < eachColSum.size(); i++)
	{


		if (eachColSum[i] > thres)
		{
			cnt = cnt + 1;
		}
		else
		{
			cnt = 0;
		}
		if (cnt >= continuousNum)
		{
			leftPos = i - continuousNum + 1;
			break;
		}
	}
	//右边
	int rightPos = 0;
	cnt = 0;
	for (int i = eachColSum.size() - 1; i >= 0; i--)
	{
		if (eachColSum[i] > thres)
		{
			cnt = cnt + 1;
		}
		else
		{
			cnt = 0;
		}
		if (cnt >= continuousNum)
		{
			rightPos = i + continuousNum - 1;
			break;
		}
	}

	int imgWidth = rightPos - leftPos + 1;
	int imgHeight = downPos - upPos + 1;
	cv::Mat segmentedImg;
	src(cv::Rect(leftPos, upPos, imgWidth, imgHeight)).copyTo(segmentedImg);
	return segmentedImg;
}

/*************************************************
Function:  RemoveBigSpaces
Description: 去除字符之间大空格，将图像切割成图像块集合
Parameter：
1.src：输入图像，前景为1，背景为0，即字白底黑
2.blockImgs：输出图像块集合
3.bigSpaceThreshold：宽度阈值
Date：2016/12/20
*************************************************/

void RemoveBigSpaces(cv::Mat src, std::vector<cv::Mat> &blockImgs, int bigSpaceThreshold)
{

	std::vector<int> eachColSum = GetColSum(src);//每列求和
	std::vector<int> zeroPoint;

	for (int i = 0; i < eachColSum.size(); i++)
	{
		if (eachColSum[i] == 0)
		{
			zeroPoint.push_back(i);

		}
	}
	int nextStartPoint = 0;

	int cnt = 0;
	for (int i = 1; i < zeroPoint.size(); i++)
	{
		int dif = zeroPoint.at(i) - zeroPoint.at(i - 1);
		if (dif == 1)
		{
			cnt++;
		}
		else
		{
			if (cnt > bigSpaceThreshold)
			{
				if (blockImgs.size() < 1)
				{
					int blockWidth = zeroPoint.at(i - 1) - cnt;
					nextStartPoint = zeroPoint.at(i - 1);
					cv::Mat blockImg = src(cv::Rect(0, 0, blockWidth, src.rows));
					blockImgs.push_back(blockImg);
				}
				else
				{
					int blockWidth = zeroPoint.at(i - 1) - cnt - nextStartPoint;
					cv::Mat blockImg = src(cv::Rect(nextStartPoint, 0, blockWidth, src.rows));
					nextStartPoint = zeroPoint.at(i - 1);
					blockImgs.push_back(blockImg);

				}

			}
			cnt = 0;
		}
		if (i == zeroPoint.size() - 1)
		{
			if (cnt > bigSpaceThreshold)
			{
				int blockWidth = zeroPoint.at(i - 1) - cnt - nextStartPoint;
				cv::Mat blockImg = src(cv::Rect(nextStartPoint, 0, blockWidth, src.rows));
				nextStartPoint = zeroPoint.at(i - 1);
				blockImgs.push_back(blockImg);
			}
			int blockWidth = src.cols - nextStartPoint;
			cv::Mat blockImg = src(cv::Rect(nextStartPoint, 0, blockWidth, src.rows));
			blockImgs.push_back(blockImg);
		}
	}
	if (blockImgs.size() == 0)
	{
		blockImgs.push_back(src);
	}

}

/*************************************************
Function:  RemoveBorder
Description: 去除边框，四个周边去除一些，获取字符图像的有效区域;;
Input：
1.src：二值图像，黑底白字
2.continuousNum: 连续出现相应比例像素的行数或者列数
3.pixelRatio:白色像素占比
Output:

Date：2017/04/28
*************************************************/
void RemoveBorder(cv::Mat& src, int continuousNum, float pixelRatio)
{
	//上下
	std::vector<int> eachRowSum = GetRowSum(src);//每行求和
	cv::Mat blackRow(1, src.cols, CV_8UC1, cv::Scalar(0));
	for (int i = 0; i < continuousNum; i++)
	{
		if (eachRowSum[i] > pixelRatio * src.cols)
		{
			blackRow.copyTo(src(cv::Rect(0, i, src.cols, 1)));
		}
	}

	for (int i = src.rows - 1; i > src.rows - 1 - continuousNum; i--)
	{
		if (eachRowSum[i] > pixelRatio * src.cols)
		{
			blackRow.copyTo(src(cv::Rect(0, i, src.cols, 1)));
		}
	}

	//左右
	std::vector<int> eachColSum = GetColSum(src);//每列求和	
	cv::Mat blackCol(src.rows, 1, CV_8UC1, cv::Scalar(0));
	for (int i = 0; i < continuousNum; i++)
	{
		if (eachColSum[i] > pixelRatio * src.rows)
		{
			blackCol.copyTo(src(cv::Rect(i, 0, 1, src.rows)));
		}
	}

	for (int i = src.cols - 1; i > src.cols - 1 - continuousNum; i--)
	{
		if (eachColSum[i] > pixelRatio * src.rows)
		{
			blackCol.copyTo(src(cv::Rect(i, 0, 1, src.rows)));
		}
	}

}

/*************************************************
Function:  RemoveBorder
Description: 去除边框，从左到右，从上到下，获取字符图像的有效区域;
Input：
1.src：二值图像，黑底白字
2.pixelRatio:白色像素占比
Output:

Date：2017/04/28
*************************************************/
void RemoveBorder2(cv::Mat& src, float pixelRatio)
{
	//上下
	std::vector<int> eachRowSum = GetRowSum(src);//每行求和
	cv::Mat blackRow(1, src.cols, CV_8UC1, cv::Scalar(0));
	for (int i = 0; i < src.rows; i++)
	{
		if (eachRowSum[i] > pixelRatio * src.cols)
		{
			blackRow.copyTo(src(cv::Rect(0, i, src.cols, 1)));
		}
	}



	//左右
	std::vector<int> eachColSum = GetColSum(src);//每列求和	
	cv::Mat blackCol(src.rows, 1, CV_8UC1, cv::Scalar(0));
	for (int i = 0; i < src.cols; i++)
	{
		if (eachColSum[i] > pixelRatio * src.rows)
		{
			blackCol.copyTo(src(cv::Rect(i, 0, 1, src.rows)));
		}
	}

}

/*************************************************
Function:  multiColProjection
Description: 分列水平投影，降低书写时的倾斜与不水平的现象
Input：
1.src：输入区域图像,前景为1
2.colNum:分列的列数
output:
1.proMat:投影矩阵
Date：2017/02/21
*************************************************/
cv::Mat MultiColProjection(cv::Mat& src, int colNum)
{
	cv::Mat proMat(src.rows, colNum, CV_8UC1, cv::Scalar(0));
	int meanValue = src.cols / colNum;

	for (int i = 0; i < src.rows; i++)
	{
		cv::Mat rowMat = src.row(i);

		for (int j = 0; j < colNum; j++)
		{
			cv::Mat sectionMat;
			if (j < colNum - 1)
			{
				sectionMat = rowMat.colRange(j * meanValue, (j + 1) * meanValue);
			}
			else
			{
				sectionMat = rowMat.colRange(j * meanValue, src.cols);
			}

			cv::Scalar sumPixel = cv::sum(sectionMat);
			proMat.at<uchar>(i, j) = sumPixel[0];
		}

	}
	return proMat;

}


/*************************************************
Function:  removeDiscretePoint
Description: 去除投影矩阵中离散的点
Input：
1.src：输入投影图像
2.thresValue: 投影块长度阈值
output:
1.dst:输出去除离散点后的矩阵
Date：2017/02/22
*************************************************/
void RemoveDiscretePoint(cv::Mat& src, cv::Mat& dst, int thresValue)
{
	src.copyTo(dst);
	for (int j = 0; j < src.cols; j++)
	{
		int flag = 0;
		int startPoint = 0;
		int endPoint = 0;
		for (int i = 0; i < src.rows; i++)
		{
			int eleValue = src.at<uchar>(i, j);
			if (eleValue >= 1)
			{
				if (flag == 0)
				{
					flag = 1;
					startPoint = i;
				}
			}
			else
			{
				if (flag == 1)
				{
					flag = 0;
					endPoint = i;

					if ((endPoint - startPoint) < thresValue)
					{
						for (int k = startPoint; k < endPoint; k++)
						{
							dst.at<uchar>(k, j) = 0;
						}
					}
				}
			}
			if (i == src.rows - 1)
			{
				if (flag == 1)
				{
					flag = 0;
					endPoint = i;

					if ((endPoint - startPoint) < thresValue)
					{
						for (int k = startPoint; k <= endPoint; k++)
						{
							dst.at<uchar>(k, j) = 0;
						}
					}
				}
			}
		}
	}
}


/*************************************************
Function:  findSegPosition
Description: 查找投影段适合的分割位置
Input：
1.src：输入投影图像
2.ratio :候选点长度比例阈值
output:
1.dst:分割位置向量集合
Date：2017/02/23
*************************************************/
std::vector<std::vector<int>> FindSegPosition(cv::Mat& src, float ratio)
{

	std::vector<std::vector<int>> proVector;//投影段向量
	std::vector<int> segPosVector;
	int meanThres = 3;
	for (int j = 0; j < src.cols; j++)
	{
		//std::cout << j << std::endl;
		int num = 0;
		int sumValue = 0;
		int startPoint = 0;
		int endPoint = 0;

		std::vector<int> proSection;
		for (int i = 0; i < src.rows; i++)
		{
			int eleValue = src.at<uchar>(i, j);
			if (eleValue > 0)
			{
				proSection.push_back(eleValue);

				sumValue = sumValue + eleValue;
				num++;

				if (num == 1)
				{
					startPoint = i;
				}
			}
			else
			{
				if (num > 0)
				{
					int meanValue = sumValue / num;
					endPoint = i - 1;
					sumValue = 0;
					num = 0;
					if (meanValue > meanThres) //投影段均值太小直接舍去
					{
						std::vector<int> sectionVector = DetectSegPoint(proSection, startPoint, meanValue, ratio);
						segPosVector.insert(segPosVector.end(), sectionVector.begin(), sectionVector.end());
					}

					proSection.clear();
				}
			}
			if (i == src.rows - 1 && num > 0)
			{
				int meanValue = sumValue / num;
				endPoint = i;
				sumValue = 0;
				num = 0;
				if (meanValue > meanThres) //投影段均值太小直接舍去
				{
					std::vector<int> sectionVector = DetectSegPoint(proSection, startPoint, meanValue, ratio);
					segPosVector.insert(segPosVector.end(), sectionVector.begin(), sectionVector.end());
				}
				proSection.clear();
			}


		}
		proVector.push_back(segPosVector);
		segPosVector.clear();
	}

	return proVector;
}

/*************************************************
Function:  detectSegPoint
Description: 检测每个投影段的分割点
Input：
1.proSection：投影向量
2.startPoint: 起始点
2.meanValue: 平均值
output:
1.afterProcessPoints:分割点向量
Date：2017/02/23
*************************************************/
std::vector<int> DetectSegPoint(std::vector<int> proSection, int startPoint, int meanValue, float ratio)
{
	std::vector<int> segPoints;
	segPoints.push_back(0);

	int lastPoint = 0;//记录上一次的点位置
	int disThres = 20;
	int num = 0;
	int midPoint = 0;
	for (int i = 1; i < proSection.size(); i++)
	{
		if (proSection.at(i) < ratio * meanValue)
		{
			//int currentPoint = i;
			if (i - lastPoint < disThres)
			{
				midPoint = midPoint + i;
				num++;
			}
			else
			{
				if (num > 1)
				{
					midPoint = midPoint / num;
					segPoints.push_back(midPoint);

				}
				num = 1;
				midPoint = i;

			}
			lastPoint = i;
		}
		else
		{
			if (i == proSection.size() - 1 && num > 1)
			{
				midPoint = midPoint / num;
				segPoints.push_back(midPoint);
			}
		}
	}
	segPoints.push_back(proSection.size() - 1);

	int intervalValue = 20;

	if (segPoints.at(1) - segPoints.at(0) < intervalValue)
	{
		segPoints.erase(segPoints.begin() + 1);
	}

	if (segPoints.at(segPoints.size() - 1) - segPoints.at(segPoints.size() - 2) < intervalValue)
	{
		segPoints.erase(segPoints.begin() + segPoints.size() - 2);

	}



	for (int i = 0; i < segPoints.size(); i++)
	{
		segPoints.at(i) = segPoints.at(i) + startPoint;
	}

	std::vector<int> afterProcessPoints;
	afterProcessPoints = segPoints;
	if (segPoints.size() >= 3)
	{
		for (int i = 1; i < segPoints.size() - 1; i++)
		{
			afterProcessPoints.insert(afterProcessPoints.begin() + 2 * i, 1, segPoints.at(i));
		}
	}
	return afterProcessPoints;

}


/*************************************************
Function:  segLineText
Description: 由得到的分割点对段落文本进行行文本分割
Input：
1.img：输入段落文本图像，二值图像，字黑底白
2.proVector: 分割点向量
output:
1.lineImgs:行文本图像集合
Date：2017/02/28
*************************************************/
std::vector<cv::Mat> SegLineText(cv::Mat& img, std::vector<std::vector<int>>& proVector, int colNum)
{
	std::vector<int> topLocation;
	std::vector<int> bottomLocation;
	for (int m = 0; m < proVector.size(); m++)
	{
		std::vector<int> segPos = proVector.at(m);

		//m等于0时，赋予初值
		if (m == 0)
		{
			for (int n = 0; n < segPos.size(); n = n + 2)
			{
				topLocation.push_back(segPos.at(n));
			}

			for (int n = 1; n < segPos.size(); n = n + 2)
			{
				bottomLocation.push_back(segPos.at(n));
			}
		}
		else
		{
			for (int n = 0; n < segPos.size(); n = n + 2)
			{

				if (segPos.at(n) < topLocation.at(n / 2))
				{
					topLocation.at(n / 2) = segPos.at(n);
				}

			}

			for (int n = 1; n < segPos.size(); n = n + 2)
			{
				if (segPos.at(n) > bottomLocation.at((n - 1) / 2))
				{
					bottomLocation.at((n - 1) / 2) = segPos.at(n);
				}
			}
		}
	}

	std::vector<cv::Mat> lineImgs;

	for (int k = 0; k < topLocation.size(); k++)
	{
		cv::Mat lineMat(bottomLocation.at(k) - topLocation.at(k), img.cols, CV_8UC1, cv::Scalar(1));

		//cv::Mat lineMat = img(cv::Rect(0, topLocation.at(k), img.cols, bottomLocation.at(k) - topLocation.at(k)));
		lineImgs.push_back(lineMat);
	}

	int widthValue = img.cols / colNum;
	for (int m = 0; m < proVector.size(); m++)
	{
		std::vector<int> segPos = proVector.at(m);

		for (int n = 0; n < segPos.size(); n = n + 2)
		{
			int heightValue = segPos.at(n + 1) - segPos.at(n);
			cv::Mat midMat = lineImgs.at(n / 2);
			int yStartPoint = segPos.at(n) - topLocation.at(n / 2); //行文本最高点减去片段文本最高点
			if (m == proVector.size() - 1)
			{
				widthValue = img.cols - m*img.cols / colNum - 1;
			}
			cv::Mat subMat = img(cv::Rect(widthValue * m, segPos.at(n), widthValue, heightValue));
			subMat.copyTo(midMat(cv::Rect(widthValue * m, yStartPoint, widthValue, heightValue)));
			lineImgs.at(n / 2) = midMat;
		}
	}
	return lineImgs;
}

/*************************************************
Function:  DetectCornerRect
Description: 检测答卷四个角点
Input：
1.img：输入图像
output:
1.result:输出图像
Date：2017/04/07
*************************************************/
cv::Rect DetectCornerRect(cv::Mat &image)
{
	cv::Mat thresImg;
	cv::threshold(image, thresImg, 0, 1, CV_THRESH_OTSU);
	thresImg = 1 - thresImg;

	double maxArea = 0;
	std::vector<cv::Point> maxContour;

	cv::vector<cv::vector<cv::Point>> contours;
	findContours(thresImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	for (int i = 0; i < contours.size(); i++)
	{
		double area = cv::contourArea(contours[i]);
		if (area > maxArea)
		{
			maxArea = area;
			maxContour = contours[i];
		}
	}
	cv::Rect maxRect = cv::boundingRect(maxContour);
	return maxRect;
}


/*************************************************
Function:  drawDetectedLines
Description: 画线
Input：
1.src：输入图像
2.lines:直线
Date：2017/04/11
*************************************************/

void drawDetectedLines(cv::Mat &image, std::vector<cv::Vec4i> lines)
{
	// Draw the lines
	for (int i = 0; i < lines.size(); i++)
	{
		cv::Vec4i curLine = lines[i];
		cv::Point pt1(curLine[0], curLine[1]);
		cv::Point pt2(curLine[2], curLine[3]);
		cv::line(image, pt1, pt2, cv::Scalar(255), 2);
	}
}

/*************************************************
Function:  VerticalLineDectect
Description: 从霍夫变换中检测到的直线中提取垂直线段
Input：
1.lines：输入线集合
2.verticalLines:垂直线集合
3.thres:阈值
Date：2017/04/11
*************************************************/

void VerticalLineDectect(std::vector<cv::Vec4i> &lines, std::vector<cv::Vec4i> &verticalLines, int thres)
{
	for (int i = 0; i < lines.size(); i++)
	{
		int dis = std::abs(lines[i][0] - lines[i][2]);
		if (dis < thres)
		{
			verticalLines.push_back(lines[i]);
		}
	}
}

/*************************************************
Function:  HorizontalLineDectect
Description: 从霍夫变换中检测到的直线中提取水平直线
Input：
1.lines：输入线集合
2.verticalLines:垂直线集合
3.thres:阈值
Date：2017/04/11
*************************************************/

void HorizontalLineDectect(std::vector<cv::Vec4i> &lines, std::vector<cv::Vec4i> &horizontalLines, int thres)
{
	for (int i = 0; i < lines.size(); i++)
	{
		int dis = std::abs(lines[i][1] - lines[i][3]);
		if (dis < thres)
		{
			horizontalLines.push_back(lines[i]);
		}
	}
}

/*************************************************
Function:  UpDownSort
Description: 排序，让所有的直线的点都是从上到下排列,并按x的升序排列
Input：
2.verticalLines:垂直线集合
Date：2017/04/13
*************************************************/
std::vector<cv::Vec4i> UpDownSort(std::vector<cv::Vec4i> verticalLines)
{

	for (int i = 0; i < verticalLines.size(); i++)
	{
		cv::Vec4i curLine = verticalLines[i];
		if (curLine[1] > curLine[3])
		{
			int tmpX = curLine[0];
			int tmpY = curLine[1];

			curLine[0] = curLine[2];
			curLine[1] = curLine[3];

			curLine[2] = tmpX;
			curLine[3] = tmpY;

			verticalLines[i] = curLine;
		}
	}

	//按x升序排列
	for (int i = 0; i < verticalLines.size(); i++)
	{
		for (int j = i + 1; j < verticalLines.size(); j++)
		{
			if (verticalLines[i][0] > verticalLines[j][0])
			{

				std::swap(verticalLines[i], verticalLines[j]);
			}
		}
	}
	return verticalLines;
}

/*************************************************
Function:  LeftRightSort
Description: 排序，让所有的直线从左到右排列
Input：
2.verticalLines:垂直线集合
Date：2017/04/13
*************************************************/
std::vector<cv::Vec4i> LeftRightSort(std::vector<cv::Vec4i> horizontalLines)
{

	for (int i = 0; i < horizontalLines.size(); i++)
	{
		cv::Vec4i curLine = horizontalLines[i];
		if (curLine[0] > curLine[2])
		{
			int tmpX = curLine[0];
			int tmpY = curLine[1];

			curLine[0] = curLine[2];
			curLine[1] = curLine[3];

			curLine[2] = tmpX;
			curLine[3] = tmpY;

			horizontalLines[i] = curLine;
		}
	}


	for (int i = 0; i < horizontalLines.size(); i++)
	{
		for (int j = i + 1; j < horizontalLines.size(); j++)
		{
			if (horizontalLines[i][1] > horizontalLines[j][1])
			{

				std::swap(horizontalLines[i], horizontalLines[j]);
			}
		}
	}
	return horizontalLines;
}

/*************************************************
Function:  VerticalLinesMerge
Description: 将垂直线段合并
Input：
1.verticalLines:垂直线集合
2.thres:阈值
Date：2017/04/11
*************************************************/
std::vector<cv::Vec4i> VerticalLinesMerge(std::vector<cv::Vec4i> verticalLines, int thres)
{

	std::vector<cv::Vec4i> mergedLines;
	mergedLines = UpDownSort(verticalLines);

	//合并
	for (int i = 0; i < mergedLines.size() - 1; i++)
	{
		for (int j = i + 1; j < mergedLines.size(); j++)
		{
			cv::Vec4i curLine = mergedLines[i];
			cv::Vec4i unmergedLine = mergedLines[j];
			int dis = std::abs(curLine[0] - unmergedLine[0]);

			if (dis < thres)
			{
				//起点与起点比较
				if (curLine[1] > unmergedLine[1])
				{
					curLine[0] = unmergedLine[0];
					curLine[1] = unmergedLine[1];
				}
				//终点与终点比较
				if (curLine[3] < unmergedLine[3])
				{

					curLine[2] = unmergedLine[2];
					curLine[3] = unmergedLine[3];
				}


				mergedLines[i] = curLine;
				mergedLines.erase(mergedLines.begin() + j);
				j = j - 1;
			}
		}
	}


	return mergedLines;
}


/*************************************************
Function:  HorizontalLinesMerge
Description: 将水平线段合并
Input：
1.verticalLines:水平线集合
2.thres:阈值
Date：2017/04/13
*************************************************/
std::vector<cv::Vec4i> HorizontalLinesMerge(std::vector<cv::Vec4i> horizontalLines, int thres)
{

	std::vector<cv::Vec4i> mergedLines;
	mergedLines = LeftRightSort(horizontalLines);

	//合并
	for (int i = 0; i < mergedLines.size() - 1; i++)
	{
		for (int j = i + 1; j < mergedLines.size(); j++)
		{
			cv::Vec4i curLine = mergedLines[i];
			cv::Vec4i unmergedLine = mergedLines[j];
			int dis = std::abs(curLine[1] - unmergedLine[1]);

			if (dis < thres)
			{
				//起点与起点比较
				if (curLine[0] > unmergedLine[0])
				{
					curLine[0] = unmergedLine[0];
					curLine[1] = unmergedLine[1];
				}
				//终点与终点比较
				if (curLine[2] < unmergedLine[2])
				{

					curLine[2] = unmergedLine[2];
					curLine[3] = unmergedLine[3];
				}


				mergedLines[i] = curLine;
				mergedLines.erase(mergedLines.begin() + j);
				j = j - 1;
			}
		}
	}


	return mergedLines;
}



/*************************************************
Function:  RegionRecognition
Description: 将垂直线段合并
Input：
2.verticalLines:垂直线集合

输出：
1.regionFlag：答卷区域类型
Date：2017/04/11
*************************************************/
int RegionRecognition(std::vector<cv::Vec4i> mergedLines)
{
	int regionFlag = -1;

	int lineNum = mergedLines.size();
	switch (lineNum)
	{
	case 2:
		regionFlag = 3;//解答题
		break;
	case 3:
	case 5:
		regionFlag = 2;//填空题
		break;
	case 11:
		regionFlag = 1;//选择题
		break;
	case 4:
		regionFlag = 0;//卷头
		break;
	default:
		break;
	}
	return regionFlag;
}



/*************************************************
Function:  ValidHeaderExtraction
Description: 姓名，班级，学号有效区域提取
Input：
1.image:输入图像
2.verticalLines:垂直线集合

输出：
1.validImg：有效图像区域
Date：2017/04/13
*************************************************/
void ValidHeaderExtraction(cv::Mat &image, cv::Mat &validImg, std::vector<cv::Vec4i> sortedLines)
{
	int x = std::max(sortedLines[1][0], sortedLines[1][2]);
	int imgWidth = std::min(sortedLines[2][0], sortedLines[2][2]) - x;
	//if (sortedLines[sortedLines.size() - 2][0] > sortedLines[sortedLines.size() - 2][2])
	//{
	//	imgWidth = sortedLines[sortedLines.size() - 2][0];
	//}
	//else
	//{
	//	imgWidth = sortedLines[sortedLines.size() - 2][2];
	//}
	//std::cout << imgWidth << std::endl;
	validImg = image(cv::Rect(x, 0, imgWidth, image.rows));
	//cv::imwrite("header.jpg", validImg);

}

/*************************************************
Function:  HeaderSegmentation
Description: 卷头分割
Input：
1.image:输入图像

输出：
1.headerImgs：答卷区域类型
Date：2017/04/13
*************************************************/
void HeaderSegmentation(cv::Mat &image, std::vector<cv::Mat>& headerImgs, int thres)
{

	cv::Mat thresBlockImg;
	cv::threshold(image, thresBlockImg, 0, 1, CV_THRESH_OTSU);
	thresBlockImg = 1 - thresBlockImg;

	std::vector<cv::Vec4i> lines;
	int accuThres = image.rows / 5;
	cv::HoughLinesP(thresBlockImg, lines, 1, CV_PI / 180, accuThres, accuThres, 10);

	std::vector<cv::Vec4i> horizontalLines;
	HorizontalLineDectect(lines, horizontalLines, thres);

	std::vector<cv::Vec4i> leftRightSortLines;
	leftRightSortLines = HorizontalLinesMerge(horizontalLines, thres);

	//去掉一些误判的水平表格线
	for (int i = 0; i < leftRightSortLines.size(); i++)
	{
		cv::Vec4i curLine = leftRightSortLines[i];
		if (curLine[2] - curLine[0] < image.cols / 2)
		{
			leftRightSortLines.erase(leftRightSortLines.begin() + i);
			i--;
		}
	}
	//姓名区域提取
	//取最小有效区域，尽最大程度防止形变及其他干扰
	cv::Rect nameRect;
	int maxX = 1;
	int maxY = std::max(leftRightSortLines[0][1], leftRightSortLines[0][3]);
	nameRect.x = maxX;
	nameRect.y = maxY;
	nameRect.width = image.cols - maxX;
	nameRect.height = std::min(leftRightSortLines[1][1], leftRightSortLines[1][3]) - maxY;

	cv::Mat nameMat;
	nameMat = image(nameRect);
	headerImgs.push_back(nameMat);

	//班级区域提取
	cv::Rect classRect;

	maxY = std::max(leftRightSortLines[1][1], leftRightSortLines[1][3]);
	classRect.x = maxX;
	classRect.y = maxY;
	classRect.width = image.cols - maxX;
	classRect.height = std::min(leftRightSortLines[2][1], leftRightSortLines[2][3]) - maxY;

	cv::Mat classMat;
	classMat = image(classRect);
	headerImgs.push_back(classMat);

	//学号区域提取
	cv::Rect numRect;
	maxY = std::max(leftRightSortLines[2][1], leftRightSortLines[2][3]);
	numRect.x = maxX;
	numRect.y = maxY;
	numRect.width = image.cols - maxX;
	numRect.height = std::min(leftRightSortLines[3][1], leftRightSortLines[3][3]) - maxY;

	cv::Mat numMat;
	numMat = image(numRect);
	headerImgs.push_back(numMat);
	cv::imwrite("number.jpg", numMat);
}

/*************************************************
Function:  NumberSegmentation
Description: 学号分割,采用平均分割的方法
Input：
1.image:输入图像
2.num:学号位数
输出：
1.numberImgs：学号图像集合
Date：2017/04/28
*************************************************/
void NumberSegmentation(cv::Mat& image, std::vector<cv::Mat> &numberImgs, int num)
{
	//学号的平均宽度
	int meanWidth = image.cols / num;

	for (int i = 0; i < num; i++)
	{
		cv::Mat numberImg = image(cv::Rect(i*meanWidth, 0, meanWidth, image.rows));
		numberImgs.push_back(numberImg);
	}

}

/*************************************************
Function:  ChooseSegmentation
Description: 选择题分割
Input：
1.image:输入图像

输出：
1.orderImgs：题号
2.chooseImgs：答卷区域类型
Date：2017/04/13
*************************************************/
void ChooseSegmentation(cv::Mat &image, std::vector<cv::Mat> &orderImgs, std::vector<cv::Mat> &chooseImgs, int thres)
{

	cv::Mat thresBlockImg;
	cv::threshold(image, thresBlockImg, 0, 1, CV_THRESH_OTSU);
	thresBlockImg = 1 - thresBlockImg;

	std::vector<cv::Vec4i> lines;
	int accuThres = image.cols / 27;
	cv::HoughLinesP(thresBlockImg, lines, 1, CV_PI / 180, accuThres, accuThres, 10);
	std::vector<cv::Vec4i> verticalLines;

	VerticalLineDectect(lines, verticalLines, thres);


	std::vector<cv::Vec4i> upDownSortLines;
	upDownSortLines = VerticalLinesMerge(verticalLines, thres);

	for (int i = 0; i < upDownSortLines.size(); i++)
	{
		cv::Vec4i curLine = upDownSortLines[i];
		if (curLine[1]  > image.cols / 80)
		{
			upDownSortLines.erase(upDownSortLines.begin() + i);
			i--;
		}

	}
	drawDetectedLines(image, upDownSortLines);
	cv::imwrite("choose.jpg", image);

	std::vector<cv::Vec4i> horizontalLines;
	HorizontalLineDectect(lines, horizontalLines, thres);

	std::vector<cv::Vec4i> leftRightSortLines;
	leftRightSortLines = HorizontalLinesMerge(horizontalLines, thres);

	//去掉一些误判的水平表格线
	for (int i = 0; i < leftRightSortLines.size(); i++)
	{
		cv::Vec4i curLine = leftRightSortLines[i];
		if (curLine[2] - curLine[0] < image.cols / 2)
		{
			leftRightSortLines.erase(leftRightSortLines.begin() + i);
			i--;
		}
	}

	//先获取题号
	for (int i = 0; i < leftRightSortLines.size() - 1; i = i + 2)
	{
		int maxY = std::max(leftRightSortLines[i][1], leftRightSortLines[i][3]);
		int heightVal = std::min(leftRightSortLines[i + 1][1], leftRightSortLines[i + 1][3]) - maxY;
		for (int j = 0; j < upDownSortLines.size() - 1; j++)
		{
			cv::Rect orderRect;
			int maxX = std::max(upDownSortLines[j][0], upDownSortLines[j][2]);

			orderRect.x = maxX;
			orderRect.y = maxY;
			orderRect.width = std::min(upDownSortLines[j + 1][0], upDownSortLines[j + 1][2]) - maxX;
			orderRect.height = heightVal;
			cv::Mat orderMat;
			orderMat = image(orderRect);
			orderImgs.push_back(orderMat);
		}
	}

	//获取内容
	for (int i = 1; i < leftRightSortLines.size() - 1; i = i + 2)
	{
		int maxY = std::max(leftRightSortLines[i][1], leftRightSortLines[i][3]);
		int heightVal = std::min(leftRightSortLines[i + 1][1], leftRightSortLines[i + 1][3]) - maxY;
		for (int j = 0; j < upDownSortLines.size() - 1; j++)
		{
			cv::Rect charaRect;
			int maxX = std::max(upDownSortLines[j][0], upDownSortLines[j][2]);

			charaRect.x = maxX;
			charaRect.y = maxY;
			charaRect.width = std::min(upDownSortLines[j + 1][0], upDownSortLines[j + 1][2]) - maxX;
			charaRect.height = heightVal;
			cv::Mat charaMat;
			charaMat = image(charaRect);
			chooseImgs.push_back(charaMat);
		}
	}

}


/*************************************************
Function:  GeneralGapSegmentation
Description: 通用填空题表格分割
Input：
1.image:输入图像

输出：
1.orderImgs：题号
2.gapImgs：答题内容
Date：2017/04/13
*************************************************/
void GeneralGapSegmentation(cv::Mat &image, std::vector<cv::Mat> &orderImgs, std::vector<cv::Mat> &gapImgs, std::vector<cv::Vec4i> &upDownSortLines, std::vector<cv::Vec4i> &leftRightSortLines)
{

	for (int i = 0; i < leftRightSortLines.size() - 1; i = i + 1)
	{
		int maxY = std::max(leftRightSortLines[i][1], leftRightSortLines[i][3]);
		int heightVal = std::min(leftRightSortLines[i + 1][1], leftRightSortLines[i + 1][3]) - maxY;
		for (int j = 0; j < upDownSortLines.size() - 1; j = j + 2)
		{
			cv::Rect orderRect;
			int maxX = std::max(upDownSortLines[j][0], upDownSortLines[j][2]);

			orderRect.x = maxX;
			orderRect.y = maxY;
			orderRect.width = std::min(upDownSortLines[j + 1][0], upDownSortLines[j + 1][2]) - maxX;
			orderRect.height = heightVal;

			cv::Mat orderMat;
			orderMat = image(orderRect);
			orderImgs.push_back(orderMat);
		}

		for (int j = 1; j < upDownSortLines.size() - 1; j = j + 2)
		{
			cv::Rect gapRect;
			int maxX = std::max(upDownSortLines[j][0], upDownSortLines[j][2]);

			gapRect.x = maxX;
			gapRect.y = maxY;
			gapRect.width = std::min(upDownSortLines[j + 1][0], upDownSortLines[j + 1][2]) - maxX;
			gapRect.height = heightVal;
			cv::Mat gapMat;
			gapMat = image(gapRect);
			gapImgs.push_back(gapMat);
		}
	}
}


/*************************************************
Function:  GapSegmentation
Description: 填空题分割
Input：
1.image:输入图像

输出：
1.orderImgs：题号
2.gapImgs：答题内容
Date：2017/04/13
*************************************************/
void GapSegmentation(cv::Mat &image, std::vector<cv::Mat> &orderImgs, std::vector<cv::Mat> &gapImgs, int thres)
{

	cv::Mat thresBlockImg;
	cv::threshold(image, thresBlockImg, 0, 1, CV_THRESH_OTSU);
	thresBlockImg = 1 - thresBlockImg;

	std::vector<cv::Vec4i> lines;
	int accuThres = image.cols / 27;
	cv::HoughLinesP(thresBlockImg, lines, 1, CV_PI / 180, accuThres, accuThres, 10);
	std::vector<cv::Vec4i> verticalLines;

	VerticalLineDectect(lines, verticalLines, thres);


	std::vector<cv::Vec4i> upDownSortLines;
	upDownSortLines = VerticalLinesMerge(verticalLines, thres);

	for (int i = 0; i < upDownSortLines.size(); i++)
	{
		cv::Vec4i curLine = upDownSortLines[i];

		if (curLine[1]  > image.cols / 80 && curLine[3]  < image.rows - image.cols / 80)
		{
			upDownSortLines.erase(upDownSortLines.begin() + i);
			i--;
		}

	}
	drawDetectedLines(image, upDownSortLines);
	cv::imwrite("gap.jpg", image);
	//std::cout << "upDownSortLines.size()  = " << upDownSortLines.size() << std::endl;

	std::vector<cv::Vec4i> horizontalLines;
	HorizontalLineDectect(lines, horizontalLines, thres);

	std::vector<cv::Vec4i> leftRightSortLines;
	leftRightSortLines = HorizontalLinesMerge(horizontalLines, thres);

	//去掉一些误判的表格线
	for (int i = 0; i < leftRightSortLines.size(); i++)
	{
		cv::Vec4i curLine = leftRightSortLines[i];
		if (curLine[2] - curLine[0] < image.cols * 0.7)
		{
			leftRightSortLines.erase(leftRightSortLines.begin() + i);
			i--;

		}
	}


	//分只有长填空题，长短填空题都有两种情况处理
	if (upDownSortLines.size() == 3)
	{
		GeneralGapSegmentation(image, orderImgs, gapImgs, upDownSortLines, leftRightSortLines);
	}

	//有5格的情况
	if (upDownSortLines.size() == 5)
	{

		int totalHeight = leftRightSortLines[leftRightSortLines.size() - 1][1] - leftRightSortLines[0][1];
		int rowHeight = totalHeight / (leftRightSortLines.size() - 1);

		int maxLength = std::max(upDownSortLines[2][3], upDownSortLines[3][3]);//第三根与第四根垂直线中的最大长度
		int rowNum = std::round((double)maxLength / (double)rowHeight + 0.5);

		//std::cout << "row =" << rowNum << std::endl;
		std::vector<cv::Vec4i> firstLeftRightSortLines;//前半段
		std::vector<cv::Vec4i> latterLeftRightSortLines;

		for (int i = 0; i < leftRightSortLines.size(); i++)
		{
			if (i < rowNum)
			{
				firstLeftRightSortLines.push_back(leftRightSortLines[i]);
			}
			if (i >= rowNum - 1)
			{
				latterLeftRightSortLines.push_back(leftRightSortLines[i]);
			}
		}

		//开始处理,先是前半段
		GeneralGapSegmentation(image, orderImgs, gapImgs, upDownSortLines, firstLeftRightSortLines);

		//后半段


		if (latterLeftRightSortLines.size() > 0)
		{

			std::vector<cv::Vec4i> latterUpDownSortLines;
			latterUpDownSortLines = upDownSortLines;
			//去除第三个与第四个
			latterUpDownSortLines.erase(latterUpDownSortLines.end() - 2);
			latterUpDownSortLines.erase(latterUpDownSortLines.end() - 2);


			std::vector<cv::Mat> latterOrderImgs;
			std::vector<cv::Mat> latterGapImgs;
			GeneralGapSegmentation(image, latterOrderImgs, latterGapImgs, latterUpDownSortLines, latterLeftRightSortLines);

			//std::cout << latterLeftRightSortLines.size() << std::endl;
			for (int i = 0; i < latterOrderImgs.size(); i++)
			{
				orderImgs.push_back(latterOrderImgs[i]);
			}

			for (int i = 0; i < latterGapImgs.size(); i++)
			{
				gapImgs.push_back(latterGapImgs[i]);
			}

		}



	}


}



/*************************************************
Function:  QuestionSegmentation
Description: 问答题分割
Input：
1.image:输入图像

输出：
1.orderImgs：题号
2.gapImgs：答题内容
Date：2017/04/13
*************************************************/
void QuestionSegmentation(cv::Mat &image, std::vector<cv::Mat> &questionImgs, int thres)
{

	cv::Mat thresBlockImg;
	cv::threshold(image, thresBlockImg, 0, 1, CV_THRESH_OTSU);
	thresBlockImg = 1 - thresBlockImg;

	std::vector<cv::Vec4i> lines;
	int accuThres = image.cols / 10;
	//cv::HoughLinesP(thresBlockImg, lines, 1, CV_PI / 180, 80, 100, 10);
	cv::HoughLinesP(thresBlockImg, lines, 1, CV_PI / 180, accuThres, accuThres, 10);
	std::vector<cv::Vec4i> verticalLines;

	VerticalLineDectect(lines, verticalLines, thres);


	std::vector<cv::Vec4i> upDownSortLines;
	upDownSortLines = VerticalLinesMerge(verticalLines, thres);


	std::vector<cv::Vec4i> horizontalLines;
	HorizontalLineDectect(lines, horizontalLines, thres);

	std::vector<cv::Vec4i> leftRightSortLines;
	leftRightSortLines = HorizontalLinesMerge(horizontalLines, thres);

	//去掉一些误判的表格线
	for (int i = 0; i < leftRightSortLines.size(); i++)
	{
		cv::Vec4i curLine = leftRightSortLines[i];
		if (curLine[2] - curLine[0] < image.cols / 2)
		{
			leftRightSortLines.erase(leftRightSortLines.begin() + i);
			i--;

		}
	}

	//获取内容
	for (int i = 0; i < leftRightSortLines.size() - 1; i = i + 1)
	{
		int maxY = std::max(leftRightSortLines[i][1], leftRightSortLines[i][3]);
		int heightVal = std::min(leftRightSortLines[i + 1][1], leftRightSortLines[i + 1][3]) - maxY;
		for (int j = 0; j < upDownSortLines.size() - 1; j++)
		{
			cv::Rect questionRect;
			int maxX = std::max(upDownSortLines[j][0], upDownSortLines[j][2]);

			questionRect.x = maxX;
			questionRect.y = maxY;
			questionRect.width = std::min(upDownSortLines[j + 1][0], upDownSortLines[j + 1][2]) - maxX;
			questionRect.height = heightVal;
			cv::Mat questionMat;
			questionMat = image(questionRect);
			questionImgs.push_back(questionMat);
		}
	}

}

/*************************************************
Function:  ImgCorrection2
Description: 图像矫正,通过轮廓检测确定三个角标志矩形，然后计算旋转角度，然后进行仿射变换
1.img：输入考卷图像，源图像
2.warpImage：矫正后的图像
Date：2017/04/18
*************************************************/
void ImgCorrection2(cv::Mat& img, cv::Mat& warpImage)
{
	int subWidth = img.cols / 8;
	int subHeight = img.rows / 8;

	cv::Mat leftTopImg = img(cv::Rect(0, 0, subWidth, subHeight));
	cv::Rect detectedRect = DetectCornerRect(leftTopImg);
	cv::Point leftTopPos;
	leftTopPos.y = detectedRect.y;
	leftTopPos.x = detectedRect.x;

	cv::Mat rightTopImg = img(cv::Rect(img.cols - subWidth, 0, subWidth, subHeight));
	detectedRect = DetectCornerRect(rightTopImg);
	cv::Point rightTopPos;
	rightTopPos.y = detectedRect.y;
	rightTopPos.x = detectedRect.x + img.cols - subWidth;

	cv::Mat leftDownImg = img(cv::Rect(0, img.rows - subHeight, subWidth, subHeight));
	detectedRect = DetectCornerRect(leftDownImg);
	cv::Point leftDownPos;
	leftDownPos.y = detectedRect.y + img.rows - subHeight;
	leftDownPos.x = detectedRect.x;

	//通过先求角度，然后根据角度计算旋转矩阵，效果不好，可能是角度计算不精准造成，造成角度不精准的原因主要是检测点的定位有偏差
	float ang = cv::fastAtan2(rightTopPos.y - leftTopPos.y, rightTopPos.x - leftTopPos.x);
	if (ang > 90)
	{
		ang = ang - 360;
	}

	cv::Point center;
	center = cv::Point(img.cols / 2, img.rows / 2);

	cv::Mat rotMat = cv::getRotationMatrix2D(center, ang, 1);
	warpAffine(img, warpImage, rotMat, cv::Size(img.size()), 1, 0, cv::Scalar(255));
}


/*************************************************
Function:  GetValidMat
Description: 去掉表格边框，获取有效字符区域
1.img：获取的字符图像或图像块
2.validImg：字符有效区域图像
Date：2017/04/18
*************************************************/
void GetValidMat(cv::Mat& img, cv::Mat& validImg, int N)
{
	int x = N - 1;
	int y = N - 1;
	int validWidth = img.cols - N * 2;
	int vaildHeight = img.rows - N * 2;
	validImg = img(cv::Rect(x, y, validWidth, vaildHeight));
}

/*************************************************
Function:  SingleChiCharaRecognition
Description: 单个字符识别
输入：
1.分类器：
2.label容器
3.待识别的字符图像
输出：
1.order：序号值
Date：2017/04/19
*************************************************/
int SingleCharaRecognition(Classifier classifier, cv::Mat& characterImg)
{
	std::vector<Prediction> predictions = classifier.Classify(characterImg);

	Prediction p = predictions[0];
	int order = std::atoi(p.first.c_str());
	//std::cout << p.second << std::endl;概率输出
	return order;
}

/*************************************************
Function:  SingleNumberLetterCrop
Description: 单个数字或字母有效区域裁剪,
输入：
1.src：输入图像，为二值化图像,前景为1，背景为0
输出：
1.dst：输出图像
Date：2017/04/27
*************************************************/
void SingleNumberLetterCrop(cv::Mat& src, cv::Mat& dst)
{
	cv::vector<cv::vector<cv::Point>> contours;
	cv::findContours(src, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	std::vector<cv::Rect> boundRects;
	cv::Rect maxRect;
	double maxArea = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		double area = cv::contourArea(contours[i]);
		cv::Rect subRect = cv::boundingRect(contours[i]);
		boundRects.push_back(subRect);
		if (area > maxArea)
		{
			maxRect = subRect;
			maxArea = area;

		}
	}

	cv::Rect tmpRect = maxRect;
	for (int i = 0; i < boundRects.size(); i++)
	{
		cv::Rect curRect = boundRects[i];
		if (curRect.area() != maxRect.area())
		{
			cv::Rect andRect = curRect & maxRect;
			cv::Rect orRect = curRect | maxRect;
			if (andRect.area() > 0 || orRect.area() < 1.5*maxRect.area())
			{
				tmpRect = tmpRect | orRect;

			}
		}
	}

	maxRect = tmpRect;
	if (maxRect.area() > 0)
	{
		dst = src(maxRect);
	}
	else
	{
		dst = src;
	}

}

/*************************************************
Function:  SplitTooWideCharacters
Description: 分离粘连在一起的过宽的字符串，简单平均法
输入：
1.img：输入字符串图像,二值图像，前景为1,背景为0，即字白底黑
2. scale：用来取平均字符数的比例倍数
2.characterImgs:输出单个字符图像向量组
Date：2017/07/05
*************************************************/
std::vector<cv::Mat> SplitTooWideCharacters(cv::Mat& blockImg, float scale)
{
	std::vector<cv::Mat> splitImgs;

	float lengthWidthRatio = (float)blockImg.rows / blockImg.cols;
	//字符的大致个数
	int num = (int)(1 / lengthWidthRatio * scale);
	for (int k = 0; k < num; k++)
	{

		int leftcolPosition = (int)((float)blockImg.cols / num * k);
		int rightcolPosition = (int)((float)blockImg.cols / num * (k + 1));
		cv::Mat segCharacterImg = blockImg(cv::Rect(leftcolPosition, 0, rightcolPosition - leftcolPosition, blockImg.rows));
		cv::Scalar sumPixel = sum(segCharacterImg);
		if (sumPixel[0] > 0)
		{
			if (segCharacterImg.rows > 30 && segCharacterImg.cols > 30)
			{
				segCharacterImg = SegmentValidParts2(segCharacterImg, 1);
			}
			segCharacterImg = 1 - segCharacterImg;
			splitImgs.push_back(segCharacterImg * 255);
		}
	}
	return splitImgs;
}

std::vector<cv::Mat> SplitTooWideCharacters2(cv::Mat& blockImg, float scale, int rangeValue)
{
	std::vector<cv::Mat> splitImgs;

	float lengthWidthRatio = (float)blockImg.rows / blockImg.cols;
	//字符的大致个数
	int num = (int)(1 / lengthWidthRatio * scale);

	std::vector<int> meanColPosition;
	for (int k = 1; k < num; k++)
	{
		int colPosition = (int)((float)blockImg.cols / num * k);
		meanColPosition.push_back(colPosition);
	}

	std::vector<int> segPosition;
	segPosition.push_back(0);
	for (int i = 0; i < meanColPosition.size(); i++)
	{
		int startCol = meanColPosition[i] - rangeValue;
		int endCol = meanColPosition[i] + rangeValue;
		
		cv::Scalar sumPixel = sum(blockImg.col(startCol));
		int minValue = sumPixel[0];
		int labelPos = startCol;
		for (int j = startCol+1; j < endCol+1; j++)
		{
			sumPixel = sum(blockImg.col(j));
			if (minValue > sumPixel[0])
			{
				minValue = sumPixel[0];
				labelPos = j;
			}
		}
		segPosition.push_back(labelPos);
	}
	segPosition.push_back(blockImg.cols - 1);

	for (int k = 0; k < segPosition.size() - 1; k++)
	{

		int leftcolPosition = segPosition[k];
		int rightcolPosition = segPosition[k+1];
		cv::Mat segCharacterImg = blockImg(cv::Rect(leftcolPosition, 0, rightcolPosition - leftcolPosition, blockImg.rows));
		cv::Scalar sumPixel = sum(segCharacterImg);
		if (sumPixel[0] > 0)
		{
			if (segCharacterImg.rows > 30 && segCharacterImg.cols > 30)
			{
				segCharacterImg = SegmentValidParts2(segCharacterImg, 1);
			}
			segCharacterImg = 1 - segCharacterImg;
			splitImgs.push_back(segCharacterImg * 255);
		}
	}
	return splitImgs;
}
/*************************************************
Function:  SingleCharacterSegmentation2
Description: 从词组字符串图像中分割出单个字符,用于改善原问题中的一些漏掉的部分
1.img：输入字符串图像,二值图像，前景为1,背景为0，即字白底黑
2.characterImgs:输出单个字符图像向量组
Date：2017/05/02
Amend:2017/06/05
*************************************************/

void SingleCharacterSegmentation2(cv::Mat& img, std::vector<cv::Mat>& characterImgs)
{
	//裁剪有效区域,方式2
	int continuousNum = 5;
	cv::Mat segmentedImg = SegmentValidParts2(img, continuousNum);

	float fixHeight = 64;
	if (segmentedImg.cols > 320)
	{
		int meanHeight = ComputeMeanLineHeight(segmentedImg, 5);
		fixHeight = fixHeight*segmentedImg.rows / meanHeight;
	}

	float zoomScale = fixHeight / segmentedImg.rows;
	cv::resize(segmentedImg, segmentedImg, cv::Size(), zoomScale, zoomScale);

	std::vector<cv::Mat> blockImgs;
	int bigSpaceThreshold = (int)(15 / zoomScale);
	RemoveBigSpaces(segmentedImg, blockImgs, bigSpaceThreshold);

	char inttoChar[64];
	float upScale = 0.7;//取图像上面部分比例值
	for (int block = 0; block < blockImgs.size(); block++)
	{
		cv::Mat segmentedBlockImg = blockImgs[block];
		//upMat 指代图像上面部分比例的图像，由比例值确定
		//int rowsVal = (int)(segmentedBlockImg.rows * upScale);
		//cv::Mat upMat = segmentedBlockImg.rowRange(0, rowsVal);
		//std::vector<int> eachColSum = GetColSum(upMat);

		std::vector<int> eachColSum = GetColSum(segmentedBlockImg);
		std::vector<int> zeroPoint;
		zeroPoint.clear();
		for (int i = 0; i < eachColSum.size(); i++)
		{
			if (eachColSum[i] == 0)
			{
				zeroPoint.push_back(i);

			}
		}

		std::vector<int> segPoints;
		segPoints.clear();
		segPoints.push_back(0);

		std::vector<int> difPoint;
		if (zeroPoint.size() > 0)
		{
			RemoveContinuousElements(zeroPoint, difPoint);
		}

		segPoints.insert(segPoints.end(), difPoint.begin(), difPoint.end());
		segPoints.push_back(segmentedBlockImg.cols - 1);

		//去除重复元素
		std::sort(segPoints.begin(), segPoints.end());
		segPoints.erase(std::unique(segPoints.begin(), segPoints.end()), segPoints.end());

		int minWidth = 30;//经验值
		
		float lowerThreshold = 0.9;//长宽比下限，针对过宽的,经验值

		int n = 1;//整个图像初始切分点的数目计数

		float scaleValue = 1.8;//分割粘连字符串时平均取值的倍数比例
		while (n < segPoints.size())
		{
			if (segPoints.size() > 2)
			{

				int blockWidth = segPoints.at(n) - segPoints.at(n - 1);
				if (blockWidth == 1)
				{
					segPoints.erase(segPoints.begin() + n);
					continue;
				}
				else if (blockWidth > 1)
				{
					cv::Mat blockImg;
					segmentedBlockImg(cv::Rect(segPoints.at(n - 1), 0, blockWidth, segmentedBlockImg.rows)).copyTo(blockImg);

					cv::Scalar sumPixel = sum(blockImg);
					if (sumPixel[0] > 0)
					{
						float lengthWidthRatio = (float)blockImg.rows / blockImg.cols;
						//std::cout << "lengthWidthRatio = " << lengthWidthRatio << std::endl;

						if (blockImg.cols < minWidth)
						{
							if (n < segPoints.size() - 1)
							{
								segPoints.erase(segPoints.begin() + n);
								continue;
							}
							else
							{
								//在不处理标点的情况下，将字符图像最后一个不足以构成一个字符的，与前面的合并
								int lastBlockWidth = segPoints.at(n - 1) - segPoints.at(n - 2);	
								cv::Mat lastBlockImg;
								segmentedBlockImg(cv::Rect(segPoints.at(n - 2), 0, lastBlockWidth, segmentedBlockImg.rows)).copyTo(lastBlockImg);

								float  lastLengthWidthRatio = (float)lastBlockImg.rows / lastBlockImg.cols;
								
								int startPoint = segPoints.at(n - 2) ;
								if (lastLengthWidthRatio < lowerThreshold)
								{
									int numValue = (int)(1 / lastLengthWidthRatio * scaleValue);				
									blockWidth = lastBlockWidth / numValue + segPoints.at(n) - segPoints.at(n - 1);
									startPoint = segPoints.at(n - 1) - lastBlockWidth / numValue;
								}
								else
								{
									blockWidth = segPoints.at(n) - segPoints.at(n - 2);
								}
								cv::Mat newBlockImg;
								segmentedBlockImg(cv::Rect(startPoint, 0, blockWidth, segmentedBlockImg.rows)).copyTo(newBlockImg);

								characterImgs.pop_back();
								newBlockImg = SegmentValidParts2(newBlockImg, 1);
								newBlockImg = 1 - newBlockImg;
								characterImgs.push_back(newBlockImg * 255);
							}

						}
						else if (lengthWidthRatio < lowerThreshold)
						{
							std::vector<cv::Mat> splitImgs;
							//splitImgs = SplitTooWideCharacters(blockImg, scaleValue);
							splitImgs = SplitTooWideCharacters2(blockImg, scaleValue, 5);
							characterImgs.insert(characterImgs.end(), splitImgs.begin(), splitImgs.end());
						}
						else
						{
							blockImg = SegmentValidParts2(blockImg, 1);
							blockImg = 1 - blockImg;
							characterImgs.push_back(blockImg * 255);
						}

					}
				}

				n++;
			}
			else
			{
				float lengthWidthRatio = (float)segmentedBlockImg.rows / segmentedBlockImg.cols;
				if (lengthWidthRatio < lowerThreshold)
				{
					std::vector<cv::Mat> splitImgs;
					//splitImgs = SplitTooWideCharacters(segmentedBlockImg, scaleValue);
					splitImgs = SplitTooWideCharacters2(segmentedBlockImg, scaleValue, 5);
					characterImgs.insert(characterImgs.end(), splitImgs.begin(), splitImgs.end());

					break;
				}
				else
				{
					segmentedBlockImg = SegmentValidParts2(segmentedBlockImg, 1);
					segmentedBlockImg = 1 - segmentedBlockImg;
					characterImgs.push_back(segmentedBlockImg * 255);
					break;
				}
			}
		}
	}
}


/*************************************************
Function:  RemoveContinuousElements
Description: 删除连续元素，保留平均值
输入：
1.inputVector：输入向量
输出：
1.outputVector：输出向量
Date：2017/05/02
*************************************************/
void RemoveContinuousElements(std::vector<int>& inputVector, std::vector<int>& outputVector)
{
	int sumValue = inputVector[0];
	int cnt = 1;
	int meanValue = 0;
	for (int i = 0; i < inputVector.size() - 1; i++)
	{
		if (inputVector[i + 1] - inputVector[i] <= 2)
		{
			sumValue = sumValue + inputVector[i + 1];
			cnt++;
		}
		else
		{
			meanValue = sumValue / cnt;
			outputVector.push_back(meanValue);
			sumValue = inputVector[i + 1];
			cnt = 1;
		}

		if (i == inputVector.size() - 2)
		{
			meanValue = sumValue / cnt;
			outputVector.push_back(meanValue);
		}
	}
}


/*************************************************
Function:  ImgValidHeight
Description: 获取字符图像的有效高度;
Input：
1.src：二值图像，前景为1，背景为0，即字白底黑
2.continuousNum: 连续出现的有效像素的行数或列数
Output:
1. validHeight:字符图像有效高度
Date：2017/07/10
*************************************************/
int ImgValidHeight(cv::Mat& src, int continuousNum)
{
	std::vector<int> eachRowSum = GetRowSum(src);//每行求和

	int thres = 1;
	int cnt = 0;//连续个数计数

	//上边
	int upPos = 0;
	for (int i = 0; i < eachRowSum.size(); i++)
	{
		if (eachRowSum[i] > thres)
		{
			cnt = cnt + 1;
		}
		else
		{
			cnt = 0;
		}
		if (cnt >= continuousNum)
		{
			upPos = i - continuousNum + 1;
			break;
		}
	}
	//下边
	int downPos = 0;
	cnt = 0;
	for (int i = eachRowSum.size() - 1; i >= 0; i--)
	{
		if (eachRowSum[i] > thres)
		{
			cnt = cnt + 1;
		}
		else
		{
			cnt = 0;
		}
		if (cnt >= continuousNum)
		{
			downPos = i + continuousNum - 1;
			break;
		}
	}

	int validHeight = downPos - upPos + 1;	
	return validHeight;
}

/*************************************************
Function:  ComputeMeanLineHeight
Description: 计算行字符图像分成colNum的平均有效宽度
Input：
1.src：二值图像,前景为1
2.colNum:分列的列数
output:
1.meanHeight:平均高度
Date：2017/07/10
*************************************************/
int ComputeMeanLineHeight(cv::Mat& src, int colNum)
{
	int meanValue = src.cols / colNum;
	int continuousNum = 2;
	int meanHeight = 0;
	for (int i = 0; i < colNum; i++)
	{
		cv::Mat subMat;

		if (i < colNum - 1)
		{
			subMat = src.colRange(i * meanValue, (i + 1) * meanValue);
			meanHeight = meanHeight + ImgValidHeight(subMat, continuousNum);
		}
		else
		{
			subMat = src.colRange(i * meanValue, src.cols);
			meanHeight = meanHeight + ImgValidHeight(subMat, continuousNum);
		}
	}
	meanHeight = meanHeight / colNum;
	return meanHeight;
}


/*************************************************
Function:  PeripheralProcess
Description: 在字符四周围添加空白
Input：
1.curMat：二值图像,前景为0，背景255
2.ratio:被除比例值
output:
1.filledMat:添加空白后的图像
Date：2017/07/17
*************************************************/
cv::Mat PeripheralProcess(cv::Mat& curMat, int ratioValue)
{
	int maxLength = std::max(curMat.cols, curMat.rows);
	int expandValue = maxLength / ratioValue;
	//std::cout << maxLength << std::endl;

	maxLength = maxLength + expandValue;
	cv::Mat filledMat(maxLength, maxLength, CV_8UC1, cv::Scalar(255));

	if (curMat.cols < curMat.rows)
	{
		int x = (maxLength - curMat.cols) / 2;
		int y = expandValue / 2;
		curMat.copyTo(filledMat(cv::Rect(x, y, curMat.cols, curMat.rows)));
	}
	else
	{
		int x = expandValue / 2;
		int y = (maxLength - curMat.rows) / 2;
		curMat.copyTo(filledMat(cv::Rect(x, y, curMat.cols, curMat.rows)));
	}
	return filledMat;
}


/*************************************************
Function:  ImgValidWidth
Description: 获取字符图像的有效宽度;
Input：
1.src：二值图像，前景为1，背景为0，即字白底黑
2.continuousNum: 连续出现的有效像素的行数或列数
Output:
1. validHeight:字符图像有效宽度
Date：2017/07/10
*************************************************/
int ImgValidWidth(cv::Mat& src, int continuousNum)
{

	int thres = 1;
	int cnt = 0;//连续个数计数

	std::vector<int> eachColSum = GetColSum(src);//每列求和
	//左边
	int leftPos = 0;
	
	for (int i = 0; i < eachColSum.size(); i++)
	{


		if (eachColSum[i] > thres)
		{
			cnt = cnt + 1;
		}
		else
		{
			cnt = 0;
		}
		if (cnt >= continuousNum)
		{
			leftPos = i - continuousNum + 1;
			break;
		}
	}
	//右边
	int rightPos = 0;
	cnt = 0;
	for (int i = eachColSum.size() - 1; i >= 0; i--)
	{
		if (eachColSum[i] > thres)
		{
			cnt = cnt + 1;
		}
		else
		{
			cnt = 0;
		}
		if (cnt >= continuousNum)
		{
			rightPos = i + continuousNum - 1;
			break;
		}
	}

	int validWidth = rightPos - leftPos + 1;
	return validWidth;
}
