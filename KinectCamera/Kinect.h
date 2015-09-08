#ifndef KINECT_H
#define KINECT_H

#include <Windows.h>
#include <opencv\cv.h>
#include <opencv\highgui.h>

#include <NuiApi.h>
#include <NuiSensor.h>
#include <NuiImageCamera.h>

#include <cstdio>
#include <cstdlib>

using namespace cv;
using namespace std;

struct point {
	double x,y,z;
	point(){};
	point(double _x, double _y, double _z){
		x = _x;
		y = _y;
		z = _z;
	}
};
struct pixel {
	int x, y;
	pixel(){}
	pixel(int _x, int _y){
		x=_x;
		y=_y;
	}
};


class Kinect {
private:
	INuiSensor			*sensor;
	HANDLE				freeEvent,depthEvent;
	HANDLE				rgbCamera;
	HANDLE				depthStream;
	int					index;
	BYTE				tmp[640*480*4];

public:
	int first;
	Kinect(int ind);

	bool initializeKinect(DWORD dwFlags);
	bool getRGBImage(IplImage *img);
	bool openImageStream(NUI_IMAGE_RESOLUTION resolution);
	bool openDepthStream(NUI_IMAGE_RESOLUTION resolution);
	bool getDepthImage(IplImage *img, int *numOfFing);
	bool getRightHand(Vector4 *p);
	bool getCoordinatesRightHand(pixel *p);
	void shutdown();
};

#endif

