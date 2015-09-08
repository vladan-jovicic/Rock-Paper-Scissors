/*#include "Kinect.h"
#include <Windows.h>
#include <iostream>
#include <cmath>
#include <queue>
#include <cstring>
using namespace std;

Kinect kinect(0);
bool display[2] = {0};


int dx[] = {1, 1, 1, -1, -1, -1, 0, 0};
int dy[] = {1, 0, -1, 1, 0, -1, 1, -1};

DWORD WINAPI thread1( LPVOID lpParam ) 
{
	
	while(!kinect.openDepthStream(NUI_IMAGE_RESOLUTION_640x480)){
	}
	IplImage		*rgb = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,4);
	IplImage		*depth = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,4);
	IplImage		*contour = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,4);
	display[0]=true;
	while(display[0]){
		if(kinect.getDepthImage(depth,contour)){
			cvShowImage("window2",depth);
			cvMoveWindow("window2",640,0);
			cvShowImage("window3",contour);
			cvMoveWindow("window3",0,0);
		}
		cvWaitKey(40);
	}
	return 0;

} 

DWORD WINAPI thread2(LPVOID lParam){
	int cnt = 0;
	while(!kinect.openDepthStream(NUI_IMAGE_RESOLUTION_320x240)){
		cout << "depth stream is not opened" << endl;
		cnt ++;
	}
	IplImage *img = cvCreateImageHeader(cvSize(320,240),8,4);
	display[1]=true;
	while(display[1]){
		/*if(kinect.getDepthImage(img)){
			if(img){
				cvShowImage("window2",img);
				cvMoveWindow("window2",640,0);
				cv::waitKey(30);
			}
		}
	}
	return 0;
}

DWORD WINAPI thread3(LPVOID lParam){
	char q;
	while(cin >> q){
		if(q=='q'){
			display[0]=false;
			display[1]=false;
			kinect.shutdown();
			return 0;
		}
	}
}

int main(){
	int data1,data2,data3;
	if(!kinect.initializeKinect(NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_SKELETON)){
		cout << "ma ne mogu da ga init" << endl;
		system("pause");
		return 0;
	}
	HANDLE threads[2];
	threads[0] = CreateThread(NULL,0,thread1,&data1,0,NULL);
	//threads[1] = CreateThread(NULL,0,thread2,&data2,0,NULL);
	threads[1] = CreateThread(NULL,0,thread3,&data3,0,NULL);

	WaitForMultipleObjects(2,threads,TRUE,INFINITE);
	CloseHandle(threads[0]);
	CloseHandle(threads[1]);
	//CloseHandle(threads[2]);
	return 0;
}*/