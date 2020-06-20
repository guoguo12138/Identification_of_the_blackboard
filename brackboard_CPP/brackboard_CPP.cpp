// brackboard_CPP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//a change
//

int iLowH = 70;
int iHighH = 115;

int iLowS = 31;
int iHighS = 168;

int iLowV = 114;
int iHighV = 255;

int main()
{
	VideoCapture cap("G:/perk/brackboard/brackboard/1.mp4");//载入检测视频
	int n_frames = int(cap.get(CV_CAP_PROP_FRAME_COUNT));//获取视频帧数
	n_frames = n_frames - 5;
	Mat src, gray, dst, ROI;
	cap >> src;//读取第一帧图像

	for (int i = 0; i < n_frames; i++)
	{
		cap >> src;
		Mat imgHSV;
		vector<Mat> hsvSplit;
		cvtColor(src, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

		//因为我们读取的是彩色图，直方图均衡化需要在HSV空间做
		split(imgHSV, hsvSplit);
		equalizeHist(hsvSplit[2], hsvSplit[2]);
		merge(hsvSplit, imgHSV);
		Mat thr;

		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), dst); //Threshold the image

		//开操作 (去除一些噪点)
		Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
		morphologyEx(dst, thr, MORPH_OPEN, element);

		//闭操作 (连接一些连通域)
		morphologyEx(thr, thr, MORPH_CLOSE, element);
		imshow("黑板", thr);

		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		findContours(thr, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));//寻找轮廓函数
		
		if (contours.size() > 0)//轮廓不为0
		{
			for (int i = 0; i < contours.size(); i++)//对每个轮廓进行操作
			{
				double area = contourArea(contours[i], false);//计算轮廓面积
				Rect rect = boundingRect(contours[i]);//计算轮廓的外接矩形
			
				if (area > 50000.0 && area < 200000.0)//筛选
				{
					rectangle(src, rect.tl(), rect.br(), Scalar(0, 0, 255), 3, 8, 0);//对符合条件的轮廓画出外接矩形
				}
			}
		}


		imshow("src", src);
		waitKey(30);

	}
	cap.release();//释放cap
	destroyAllWindows();//关闭所有opencv窗口
	waitKey(0);
	return 0;
}
