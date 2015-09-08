#include "SecondScene.h"

bool			loadING = false, kInit = false, inImage = false;
GLuint			loadTex;
int				fingers, counter=0;

int waitForPlayer(Kinect *kinect, IplImage *image){
	counter ++;
	if(!loadING){
		Bitmap *image;
		image = new Bitmap;
		if(!image->loadBMP("loading.bmp")){
			printf("game error");
			exit(0);
		}
		glGenTextures(1,&loadTex);
		glBindTexture(GL_TEXTURE_2D, loadTex);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, image->width, image->height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image->data);
		loadING = true;
		delete image;
	}
	glBindTexture(GL_TEXTURE_2D, loadTex);
	glBegin(GL_QUADS);
		glTexCoord2d(0.0,0.0); glVertex2d(-1.0,-1.0);
		glTexCoord2d(1.0,0.0); glVertex2d(+1.0,-1.0);
		glTexCoord2d(1.0,1.0); glVertex2d(+1.0,+1.0);
		glTexCoord2d(0.0,1.0); glVertex2d(-1.0,+1.0);
	glEnd();
	//sad ide dio za kinect
	//if(counter > 1000) return 1;
	if(!kInit){
		if(kinect->initializeKinect(NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_SKELETON) ) {
			kInit = true;
		}
	}
	if(!kInit) return 0;

	if(!kinect->openDepthStream(NUI_IMAGE_RESOLUTION_640x480)){
			return 0;
	}

	/*if(!inImage){
		image = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,4);
		inImage = true;
	}*/
	fingers = -1;
	if(!kinect->getDepthImage(image,&fingers)){
		return 0;
	}else {
		cvShowImage("window1",image);
		cvMoveWindow("window1",645,0);
	}
	//printf("%d\n",fingers);
	if(fingers < 0) return 0;

	return 1;


}