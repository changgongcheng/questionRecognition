/*
//////////////////
// File name:  segmentation.cpp
// Author��    ZhangCheng
// Date:       2016/12/14
// Function:   �ַ�ͼ���и�����صĺ���
//////////////
*/
#include "segmentation.h"

/*************************************************
Function:  removeSmallRegion
Description: ��ȡ�ַ�ͼ�����Ч����; //�Ժ��������ͨ��ķ�ʽ����
//ȥ���׶�����ȥ��С���򣬲�������������ʵ��
//srcΪ��ֵͼ��ǰ��Ϊ1������Ϊ0
//checkMode: 0����ȥ��������1����ȥ��������; NeihborMode��0����4����1����8����;
Date��2016/09/14
*************************************************/
void RemoveSmallRegion(cv::Mat& src, cv::Mat& dst, int areaLimit, int checkMode, int neihborMode)
{

	//��¼ÿ�����ص����״̬�ı�ǩ��0����δ��飬1�������ڼ��,2�����鲻�ϸ���Ҫ��ת��ɫ����3������ϸ������
	cv::Mat pointlabel = cv::Mat::zeros(src.size(), CV_8UC1);
	if (checkMode == 1)
	{
		//cout << "Mode: ȥ��С����." << endl;
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
		//cout << "Mode: ȥ���׶�." << endl;
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

	std::vector<cv::Point2i> neihborPos;  //��¼�����λ��  
	neihborPos.push_back(cv::Point2i(-1, 0));
	neihborPos.push_back(cv::Point2i(1, 0));
	neihborPos.push_back(cv::Point2i(0, -1));
	neihborPos.push_back(cv::Point2i(0, 1));
	if (neihborMode == 1)
	{
		//cout << "Neighbor mode: 8����." << endl;
		neihborPos.push_back(cv::Point2i(-1, -1));
		neihborPos.push_back(cv::Point2i(-1, 1));
		neihborPos.push_back(cv::Point2i(1, -1));
		neihborPos.push_back(cv::Point2i(1, 1));
	}
	else
	{
		//cout << "Neighbor mode: 4����." << endl;
	}
	int neihborCount = 4 + 4 * neihborMode;
	int currX = 0;
	int currY = 0;
	//��ʼ���  
	for (int i = 0; i < src.rows; i++)
	{
		uchar* iLabel = pointlabel.ptr<uchar>(i);
		for (int j = 0; j < src.cols; j++)
		{
			if (iLabel[j] == 0)
			{
				//********��ʼ�õ㴦�ļ��********** 				
				std::vector<cv::Point2i> growBuffer; //��ջ�����ڴ洢������  
				growBuffer.push_back(cv::Point2i(j, i));
				pointlabel.at<uchar>(i, j) = 1;
				int checkResult = 0; //�����жϽ�����Ƿ񳬳���С����0Ϊδ������1Ϊ����
				for (int z = 0; z < growBuffer.size(); z++)
				{

					for (int q = 0; q < neihborCount; q++) //����ĸ������  
					{
						currX = growBuffer.at(z).x + neihborPos.at(q).x;
						currY = growBuffer.at(z).y + neihborPos.at(q).y;
						if (currX >= 0 && currX < src.cols &&currY >= 0 && currY < src.rows)  //��ֹԽ��  
						{
							if (pointlabel.at<uchar>(currY, currX) == 0)
							{
								growBuffer.push_back(cv::Point2i(currX, currY)); //��������buffer  
								pointlabel.at<uchar>(currY, currX) = 1; //���������ļ���ǩ�������ظ����  
							}
						}
					}

				}
				if (growBuffer.size() > areaLimit)
				{
					checkResult = 2; //�жϽ�����Ƿ񳬳��޶��Ĵ�С����1Ϊδ������2Ϊ����  
				}

				else
				{
					checkResult = 1;

				}
				for (int z = 0; z < growBuffer.size(); z++)                         //����Label��¼  
				{
					currX = growBuffer.at(z).x;
					currY = growBuffer.at(z).y;
					pointlabel.at<uchar>(currY, currX) += checkResult;
				}
				/********�����õ㴦�ļ��**********/
			}
		}
	}

	checkMode = 1 - checkMode;
	//��ʼ��ת�����С������  
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
Description: ��ͼ��ÿ�еĺ�;
Input��
1.src������ͼ��
Output:
1. colSum:�������
Date��2017/04/27
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
Description: ��ͼ��ÿ�еĺ�;
Input��
1.src������ͼ��
Output:
1. rowSum:�������
Date��2017/04/27
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
Description: ��ȡ�ַ�ͼ�����Ч����;
Input��
1.src����ֵͼ��ǰ��Ϊ1������Ϊ0�����ְ׵׺�
2.continuousNum: �������ֵ���Ч���ص�����������
Output:
1. segmentedImg:�ָ���ͼ��
Date��2016/09/14
*************************************************/
cv::Mat SegmentValidParts2(cv::Mat& src, int continuousNum)
{
	std::vector<int> eachRowSum = GetRowSum(src);//ÿ�����

	int thres = 1;
	int cnt = 0;//������������

	//�ϱ�
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
	//�±�
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

	std::vector<int> eachColSum = GetColSum(src);//ÿ�����
	//���
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
	//�ұ�
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
Description: ȥ���ַ�֮���ո񣬽�ͼ���и��ͼ��鼯��
Parameter��
1.src������ͼ��ǰ��Ϊ1������Ϊ0�����ְ׵׺�
2.blockImgs�����ͼ��鼯��
3.bigSpaceThreshold�������ֵ
Date��2016/12/20
*************************************************/

void RemoveBigSpaces(cv::Mat src, std::vector<cv::Mat> &blockImgs, int bigSpaceThreshold)
{

	std::vector<int> eachColSum = GetColSum(src);//ÿ�����
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
Description: ȥ���߿��ĸ��ܱ�ȥ��һЩ����ȡ�ַ�ͼ�����Ч����;;
Input��
1.src����ֵͼ�񣬺ڵװ���
2.continuousNum: ����������Ӧ�������ص�������������
3.pixelRatio:��ɫ����ռ��
Output:

Date��2017/04/28
*************************************************/
void RemoveBorder(cv::Mat& src, int continuousNum, float pixelRatio)
{
	//����
	std::vector<int> eachRowSum = GetRowSum(src);//ÿ�����
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

	//����
	std::vector<int> eachColSum = GetColSum(src);//ÿ�����	
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
Description: ȥ���߿򣬴����ң����ϵ��£���ȡ�ַ�ͼ�����Ч����;
Input��
1.src����ֵͼ�񣬺ڵװ���
2.pixelRatio:��ɫ����ռ��
Output:

Date��2017/04/28
*************************************************/
void RemoveBorder2(cv::Mat& src, float pixelRatio)
{
	//����
	std::vector<int> eachRowSum = GetRowSum(src);//ÿ�����
	cv::Mat blackRow(1, src.cols, CV_8UC1, cv::Scalar(0));
	for (int i = 0; i < src.rows; i++)
	{
		if (eachRowSum[i] > pixelRatio * src.cols)
		{
			blackRow.copyTo(src(cv::Rect(0, i, src.cols, 1)));
		}
	}



	//����
	std::vector<int> eachColSum = GetColSum(src);//ÿ�����	
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
Description: ����ˮƽͶӰ��������дʱ����б�벻ˮƽ������
Input��
1.src����������ͼ��,ǰ��Ϊ1
2.colNum:���е�����
output:
1.proMat:ͶӰ����
Date��2017/02/21
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
Description: ȥ��ͶӰ��������ɢ�ĵ�
Input��
1.src������ͶӰͼ��
2.thresValue: ͶӰ�鳤����ֵ
output:
1.dst:���ȥ����ɢ���ľ���
Date��2017/02/22
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
Description: ����ͶӰ���ʺϵķָ�λ��
Input��
1.src������ͶӰͼ��
2.ratio :��ѡ�㳤�ȱ�����ֵ
output:
1.dst:�ָ�λ����������
Date��2017/02/23
*************************************************/
std::vector<std::vector<int>> FindSegPosition(cv::Mat& src, float ratio)
{

	std::vector<std::vector<int>> proVector;//ͶӰ������
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
					if (meanValue > meanThres) //ͶӰ�ξ�ֵ̫Сֱ����ȥ
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
				if (meanValue > meanThres) //ͶӰ�ξ�ֵ̫Сֱ����ȥ
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
Description: ���ÿ��ͶӰ�εķָ��
Input��
1.proSection��ͶӰ����
2.startPoint: ��ʼ��
2.meanValue: ƽ��ֵ
output:
1.afterProcessPoints:�ָ������
Date��2017/02/23
*************************************************/
std::vector<int> DetectSegPoint(std::vector<int> proSection, int startPoint, int meanValue, float ratio)
{
	std::vector<int> segPoints;
	segPoints.push_back(0);

	int lastPoint = 0;//��¼��һ�εĵ�λ��
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
Description: �ɵõ��ķָ��Զ����ı��������ı��ָ�
Input��
1.img����������ı�ͼ�񣬶�ֵͼ���ֺڵװ�
2.proVector: �ָ������
output:
1.lineImgs:���ı�ͼ�񼯺�
Date��2017/02/28
*************************************************/
std::vector<cv::Mat> SegLineText(cv::Mat& img, std::vector<std::vector<int>>& proVector, int colNum)
{
	std::vector<int> topLocation;
	std::vector<int> bottomLocation;
	for (int m = 0; m < proVector.size(); m++)
	{
		std::vector<int> segPos = proVector.at(m);

		//m����0ʱ�������ֵ
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
			int yStartPoint = segPos.at(n) - topLocation.at(n / 2); //���ı���ߵ��ȥƬ���ı���ߵ�
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
Description: ������ĸ��ǵ�
Input��
1.img������ͼ��
output:
1.result:���ͼ��
Date��2017/04/07
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
Description: ����
Input��
1.src������ͼ��
2.lines:ֱ��
Date��2017/04/11
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
Description: �ӻ���任�м�⵽��ֱ������ȡ��ֱ�߶�
Input��
1.lines�������߼���
2.verticalLines:��ֱ�߼���
3.thres:��ֵ
Date��2017/04/11
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
Description: �ӻ���任�м�⵽��ֱ������ȡˮƽֱ��
Input��
1.lines�������߼���
2.verticalLines:��ֱ�߼���
3.thres:��ֵ
Date��2017/04/11
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
Description: ���������е�ֱ�ߵĵ㶼�Ǵ��ϵ�������,����x����������
Input��
2.verticalLines:��ֱ�߼���
Date��2017/04/13
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

	//��x��������
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
Description: ���������е�ֱ�ߴ���������
Input��
2.verticalLines:��ֱ�߼���
Date��2017/04/13
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
Description: ����ֱ�߶κϲ�
Input��
1.verticalLines:��ֱ�߼���
2.thres:��ֵ
Date��2017/04/11
*************************************************/
std::vector<cv::Vec4i> VerticalLinesMerge(std::vector<cv::Vec4i> verticalLines, int thres)
{

	std::vector<cv::Vec4i> mergedLines;
	mergedLines = UpDownSort(verticalLines);

	//�ϲ�
	for (int i = 0; i < mergedLines.size() - 1; i++)
	{
		for (int j = i + 1; j < mergedLines.size(); j++)
		{
			cv::Vec4i curLine = mergedLines[i];
			cv::Vec4i unmergedLine = mergedLines[j];
			int dis = std::abs(curLine[0] - unmergedLine[0]);

			if (dis < thres)
			{
				//��������Ƚ�
				if (curLine[1] > unmergedLine[1])
				{
					curLine[0] = unmergedLine[0];
					curLine[1] = unmergedLine[1];
				}
				//�յ����յ�Ƚ�
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
Description: ��ˮƽ�߶κϲ�
Input��
1.verticalLines:ˮƽ�߼���
2.thres:��ֵ
Date��2017/04/13
*************************************************/
std::vector<cv::Vec4i> HorizontalLinesMerge(std::vector<cv::Vec4i> horizontalLines, int thres)
{

	std::vector<cv::Vec4i> mergedLines;
	mergedLines = LeftRightSort(horizontalLines);

	//�ϲ�
	for (int i = 0; i < mergedLines.size() - 1; i++)
	{
		for (int j = i + 1; j < mergedLines.size(); j++)
		{
			cv::Vec4i curLine = mergedLines[i];
			cv::Vec4i unmergedLine = mergedLines[j];
			int dis = std::abs(curLine[1] - unmergedLine[1]);

			if (dis < thres)
			{
				//��������Ƚ�
				if (curLine[0] > unmergedLine[0])
				{
					curLine[0] = unmergedLine[0];
					curLine[1] = unmergedLine[1];
				}
				//�յ����յ�Ƚ�
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
Description: ����ֱ�߶κϲ�
Input��
2.verticalLines:��ֱ�߼���

�����
1.regionFlag�������������
Date��2017/04/11
*************************************************/
int RegionRecognition(std::vector<cv::Vec4i> mergedLines)
{
	int regionFlag = -1;

	int lineNum = mergedLines.size();
	switch (lineNum)
	{
	case 2:
		regionFlag = 3;//�����
		break;
	case 3:
	case 5:
		regionFlag = 2;//�����
		break;
	case 11:
		regionFlag = 1;//ѡ����
		break;
	case 4:
		regionFlag = 0;//��ͷ
		break;
	default:
		break;
	}
	return regionFlag;
}



/*************************************************
Function:  ValidHeaderExtraction
Description: �������༶��ѧ����Ч������ȡ
Input��
1.image:����ͼ��
2.verticalLines:��ֱ�߼���

�����
1.validImg����Чͼ������
Date��2017/04/13
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
Description: ��ͷ�ָ�
Input��
1.image:����ͼ��

�����
1.headerImgs�������������
Date��2017/04/13
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

	//ȥ��һЩ���е�ˮƽ�����
	for (int i = 0; i < leftRightSortLines.size(); i++)
	{
		cv::Vec4i curLine = leftRightSortLines[i];
		if (curLine[2] - curLine[0] < image.cols / 2)
		{
			leftRightSortLines.erase(leftRightSortLines.begin() + i);
			i--;
		}
	}
	//����������ȡ
	//ȡ��С��Ч���򣬾����̶ȷ�ֹ�α估��������
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

	//�༶������ȡ
	cv::Rect classRect;

	maxY = std::max(leftRightSortLines[1][1], leftRightSortLines[1][3]);
	classRect.x = maxX;
	classRect.y = maxY;
	classRect.width = image.cols - maxX;
	classRect.height = std::min(leftRightSortLines[2][1], leftRightSortLines[2][3]) - maxY;

	cv::Mat classMat;
	classMat = image(classRect);
	headerImgs.push_back(classMat);

	//ѧ��������ȡ
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
Description: ѧ�ŷָ�,����ƽ���ָ�ķ���
Input��
1.image:����ͼ��
2.num:ѧ��λ��
�����
1.numberImgs��ѧ��ͼ�񼯺�
Date��2017/04/28
*************************************************/
void NumberSegmentation(cv::Mat& image, std::vector<cv::Mat> &numberImgs, int num)
{
	//ѧ�ŵ�ƽ�����
	int meanWidth = image.cols / num;

	for (int i = 0; i < num; i++)
	{
		cv::Mat numberImg = image(cv::Rect(i*meanWidth, 0, meanWidth, image.rows));
		numberImgs.push_back(numberImg);
	}

}

/*************************************************
Function:  ChooseSegmentation
Description: ѡ����ָ�
Input��
1.image:����ͼ��

�����
1.orderImgs�����
2.chooseImgs�������������
Date��2017/04/13
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

	//ȥ��һЩ���е�ˮƽ�����
	for (int i = 0; i < leftRightSortLines.size(); i++)
	{
		cv::Vec4i curLine = leftRightSortLines[i];
		if (curLine[2] - curLine[0] < image.cols / 2)
		{
			leftRightSortLines.erase(leftRightSortLines.begin() + i);
			i--;
		}
	}

	//�Ȼ�ȡ���
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

	//��ȡ����
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
Description: ͨ���������ָ�
Input��
1.image:����ͼ��

�����
1.orderImgs�����
2.gapImgs����������
Date��2017/04/13
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
Description: �����ָ�
Input��
1.image:����ͼ��

�����
1.orderImgs�����
2.gapImgs����������
Date��2017/04/13
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

	//ȥ��һЩ���еı����
	for (int i = 0; i < leftRightSortLines.size(); i++)
	{
		cv::Vec4i curLine = leftRightSortLines[i];
		if (curLine[2] - curLine[0] < image.cols * 0.7)
		{
			leftRightSortLines.erase(leftRightSortLines.begin() + i);
			i--;

		}
	}


	//��ֻ�г�����⣬��������ⶼ�������������
	if (upDownSortLines.size() == 3)
	{
		GeneralGapSegmentation(image, orderImgs, gapImgs, upDownSortLines, leftRightSortLines);
	}

	//��5������
	if (upDownSortLines.size() == 5)
	{

		int totalHeight = leftRightSortLines[leftRightSortLines.size() - 1][1] - leftRightSortLines[0][1];
		int rowHeight = totalHeight / (leftRightSortLines.size() - 1);

		int maxLength = std::max(upDownSortLines[2][3], upDownSortLines[3][3]);//����������ĸ���ֱ���е���󳤶�
		int rowNum = std::round((double)maxLength / (double)rowHeight + 0.5);

		//std::cout << "row =" << rowNum << std::endl;
		std::vector<cv::Vec4i> firstLeftRightSortLines;//ǰ���
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

		//��ʼ����,����ǰ���
		GeneralGapSegmentation(image, orderImgs, gapImgs, upDownSortLines, firstLeftRightSortLines);

		//����


		if (latterLeftRightSortLines.size() > 0)
		{

			std::vector<cv::Vec4i> latterUpDownSortLines;
			latterUpDownSortLines = upDownSortLines;
			//ȥ������������ĸ�
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
Description: �ʴ���ָ�
Input��
1.image:����ͼ��

�����
1.orderImgs�����
2.gapImgs����������
Date��2017/04/13
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

	//ȥ��һЩ���еı����
	for (int i = 0; i < leftRightSortLines.size(); i++)
	{
		cv::Vec4i curLine = leftRightSortLines[i];
		if (curLine[2] - curLine[0] < image.cols / 2)
		{
			leftRightSortLines.erase(leftRightSortLines.begin() + i);
			i--;

		}
	}

	//��ȡ����
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
Description: ͼ�����,ͨ���������ȷ�������Ǳ�־���Σ�Ȼ�������ת�Ƕȣ�Ȼ����з���任
1.img�����뿼��ͼ��Դͼ��
2.warpImage���������ͼ��
Date��2017/04/18
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

	//ͨ������Ƕȣ�Ȼ����ݽǶȼ�����ת����Ч�����ã������ǽǶȼ��㲻��׼��ɣ���ɽǶȲ���׼��ԭ����Ҫ�Ǽ���Ķ�λ��ƫ��
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
Description: ȥ�����߿򣬻�ȡ��Ч�ַ�����
1.img����ȡ���ַ�ͼ���ͼ���
2.validImg���ַ���Ч����ͼ��
Date��2017/04/18
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
Description: �����ַ�ʶ��
���룺
1.��������
2.label����
3.��ʶ����ַ�ͼ��
�����
1.order�����ֵ
Date��2017/04/19
*************************************************/
int SingleCharaRecognition(Classifier classifier, cv::Mat& characterImg)
{
	std::vector<Prediction> predictions = classifier.Classify(characterImg);

	Prediction p = predictions[0];
	int order = std::atoi(p.first.c_str());
	//std::cout << p.second << std::endl;�������
	return order;
}

/*************************************************
Function:  SingleNumberLetterCrop
Description: �������ֻ���ĸ��Ч����ü�,
���룺
1.src������ͼ��Ϊ��ֵ��ͼ��,ǰ��Ϊ1������Ϊ0
�����
1.dst�����ͼ��
Date��2017/04/27
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
Description: ����ճ����һ��Ĺ�����ַ�������ƽ����
���룺
1.img�������ַ���ͼ��,��ֵͼ��ǰ��Ϊ1,����Ϊ0�����ְ׵׺�
2. scale������ȡƽ���ַ����ı�������
2.characterImgs:��������ַ�ͼ��������
Date��2017/07/05
*************************************************/
std::vector<cv::Mat> SplitTooWideCharacters(cv::Mat& blockImg, float scale)
{
	std::vector<cv::Mat> splitImgs;

	float lengthWidthRatio = (float)blockImg.rows / blockImg.cols;
	//�ַ��Ĵ��¸���
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
	//�ַ��Ĵ��¸���
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
Description: �Ӵ����ַ���ͼ���зָ�������ַ�,���ڸ���ԭ�����е�һЩ©���Ĳ���
1.img�������ַ���ͼ��,��ֵͼ��ǰ��Ϊ1,����Ϊ0�����ְ׵׺�
2.characterImgs:��������ַ�ͼ��������
Date��2017/05/02
Amend:2017/06/05
*************************************************/

void SingleCharacterSegmentation2(cv::Mat& img, std::vector<cv::Mat>& characterImgs)
{
	//�ü���Ч����,��ʽ2
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
	float upScale = 0.7;//ȡͼ�����沿�ֱ���ֵ
	for (int block = 0; block < blockImgs.size(); block++)
	{
		cv::Mat segmentedBlockImg = blockImgs[block];
		//upMat ָ��ͼ�����沿�ֱ�����ͼ���ɱ���ֵȷ��
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

		//ȥ���ظ�Ԫ��
		std::sort(segPoints.begin(), segPoints.end());
		segPoints.erase(std::unique(segPoints.begin(), segPoints.end()), segPoints.end());

		int minWidth = 30;//����ֵ
		
		float lowerThreshold = 0.9;//��������ޣ���Թ����,����ֵ

		int n = 1;//����ͼ���ʼ�зֵ����Ŀ����

		float scaleValue = 1.8;//�ָ�ճ���ַ���ʱƽ��ȡֵ�ı�������
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
								//�ڲ������������£����ַ�ͼ�����һ�������Թ���һ���ַ��ģ���ǰ��ĺϲ�
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
Description: ɾ������Ԫ�أ�����ƽ��ֵ
���룺
1.inputVector����������
�����
1.outputVector���������
Date��2017/05/02
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
Description: ��ȡ�ַ�ͼ�����Ч�߶�;
Input��
1.src����ֵͼ��ǰ��Ϊ1������Ϊ0�����ְ׵׺�
2.continuousNum: �������ֵ���Ч���ص�����������
Output:
1. validHeight:�ַ�ͼ����Ч�߶�
Date��2017/07/10
*************************************************/
int ImgValidHeight(cv::Mat& src, int continuousNum)
{
	std::vector<int> eachRowSum = GetRowSum(src);//ÿ�����

	int thres = 1;
	int cnt = 0;//������������

	//�ϱ�
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
	//�±�
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
Description: �������ַ�ͼ��ֳ�colNum��ƽ����Ч���
Input��
1.src����ֵͼ��,ǰ��Ϊ1
2.colNum:���е�����
output:
1.meanHeight:ƽ���߶�
Date��2017/07/10
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
Description: ���ַ�����Χ��ӿհ�
Input��
1.curMat����ֵͼ��,ǰ��Ϊ0������255
2.ratio:��������ֵ
output:
1.filledMat:��ӿհ׺��ͼ��
Date��2017/07/17
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
Description: ��ȡ�ַ�ͼ�����Ч���;
Input��
1.src����ֵͼ��ǰ��Ϊ1������Ϊ0�����ְ׵׺�
2.continuousNum: �������ֵ���Ч���ص�����������
Output:
1. validHeight:�ַ�ͼ����Ч���
Date��2017/07/10
*************************************************/
int ImgValidWidth(cv::Mat& src, int continuousNum)
{

	int thres = 1;
	int cnt = 0;//������������

	std::vector<int> eachColSum = GetColSum(src);//ÿ�����
	//���
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
	//�ұ�
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
