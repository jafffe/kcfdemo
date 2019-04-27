#include <stdio.h>
#include <tchar.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "kcftracker.hpp"

using namespace std;
using namespace cv;
bool drawing_box = false;
bool gotBB = false;
Rect box;
//�����ο�
void drawBox(Mat& image, CvRect box, Scalar color, int thick) {
	rectangle(image, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), color, thick);
}
//���׷�ٺ���
void mouseHandler(int event, int x, int y, int flags, void *param) {
	switch (event) {
	case CV_EVENT_MOUSEMOVE:
		if (drawing_box) {
			box.width = x - box.x;
			box.height = y - box.y;
		}
		break;
	case CV_EVENT_LBUTTONDOWN:
		drawing_box = true;
		box = Rect(x, y, 0, 0);
		break;
	case CV_EVENT_LBUTTONUP:
		drawing_box = false;
		if (box.width < 0) {
			box.x += box.width;
			box.width *= -1;
		}
		if (box.height < 0) {
			box.y += box.height;
			box.height *= -1;
		}
		gotBB = true;   //�Ѿ����bounding box  
		break;
	}
}
int main(int argc, char* argv[]) {
	VideoCapture capture;//������ͷ����
	capture.open(0);
	if (!capture.isOpened())
	{
		cout << "capture device failed to open!" << endl;
		return 1;
	}
	//Register mouse callback to draw the bounding box  
	cvNamedWindow("KCF", CV_WINDOW_AUTOSIZE);
	cout << "11111" << endl;
	cvSetMouseCallback("KCF", mouseHandler, NULL);  //�����ѡ�г�ʼĿ���bounding box  
	cout << "22222" << endl;

	bool HOG = true;//�Ƿ�ʹ��HOG��ɫֱ��ͼ����
	bool FIXEDWINDOW = false;//�Ƿ�ʹ�ù̶����ڴ�С
	bool MULTISCALE = true;//�Ƿ�ʹ�ö�߶�
	bool LAB = false;//�Ƿ�ʹ��labɫ��ɫ�ռ䣬���rgb��ɫ
	bool SILENT = false;
	// Create KCFTracker object
	KCFTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);
	// Frame readed
	Mat frame;
	// Tracker results
	Rect result;

	///Initialization  
	while (!gotBB)
	{
		cout << "aaaaa" << endl;
		capture >> frame;
		drawBox(frame, box, (0, 0, 255), 2);  //��bounding box ������  
		imshow("KCF", frame);
		if (cvWaitKey(33) == 'q')
			return 0;
	}
	//Remove callback  
	cvSetMouseCallback("KCF", NULL, NULL);  //����Ѿ���õ�һ֡�û��򶨵�box�ˣ���ȡ�������Ӧ  
	cout << "33333" << endl;
	printf("Initial Bounding Box = x:%d y:%d h:%d w:%d\n", box.x, box.y, box.width, box.height);
	tracker.init(box, frame);//��ʼ��׷�ٿ�/////////////////////////////////// tracker.init ��ʼ��Ŀ�����ѭ��������
	////ѭ����ʼѭ����ʼ////
	while (capture.read(frame))// һֱ��capture���ͼ��read�� frame����ȥ����ʼ����Ŀ��
	{
		result = tracker.update(frame);//׷��///////////////////////////////////// tracker.update 
		//�����ο�
		rectangle(frame, Point(result.x, result.y), Point(result.x + result.width, result.y + result.height), Scalar(0, 255, 255), 1, 8);////////////////////////////////////////////// ��result�Ľ�����ڵ�ǰͼ����
		if (!SILENT) {
			imshow("KCF", frame);
			waitKey(10);
		}
	}
	////ѭ����ʼѭ������////
}
