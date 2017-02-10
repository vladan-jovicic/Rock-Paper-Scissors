#include "Kinect.h"
#include "Bitmap.h"
#include "Recognition.h"
#include "StartScreen.h"
#include "SecondScene.h"
#include "GameScene.h"
#include <cmath>
#define PI 3.14159265

Kinect						kinect(0);
bool						isKinectInitialized = false, initImg = false;
pixel						hand;
float						WIDTH = 640, HEIGHT = 480;
IplImage					*img;
int							scene = 1;
GLuint						teksture[7];
bool						inited[7] = {0}, selected[3] = {0};
char						names[7][20];

void displayStartScene();
void displaySecondScene();
void displayThirdScene();
void specialKeys(int key, int x, int y);
void normalKeys(unsigned char key, int x, int y);


void display()
{
    // clear the draw buffer .
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_BLEND);
	if(scene == 1) {
		displayStartScene();
	}else if(scene == 2){
		displaySecondScene();
	}else if(scene == 3){
		displayThirdScene();
	}
} 

void change_size(int w, int h){
	if(!h) h = 1;
	float ratio = 1.0f * w / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0,0,w,h);
	gluPerspective(45,ratio,1,10000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


int main(int argc, char **argv){

	
	strcpy(names[0],"resource/scene.bmp");
	strcpy(names[1],"resource/start.bmp");
	strcpy(names[2],"resource/startr.bmp");
	strcpy(names[3],"resource/instr.bmp");
	strcpy(names[4],"resource/instrr.bmp");
	strcpy(names[5],"resource/exiit.bmp");
	strcpy(names[6],"resource/exiitr.bmp");
	img = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,4);
	selected[0] = true;
	glutInit(&argc,argv);
	glutInitWindowSize(640,480);
	glEnable(GL_DEPTH_TEST);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	//glEnable( GL_TEXTURE_2D );
	glutCreateWindow("Opengl ena");
	glutKeyboardFunc(normalKeys);
	glutSpecialFunc(specialKeys);
	glutDisplayFunc(display);
	//glutReshapeFunc(change_size);
	glutIdleFunc(display);
	glutMainLoop();
	return (EXIT_SUCCESS);

}
void displayStartScene(){
	glClearColor(0.0,0.0,0.0,0.0);
	LoadAllTextures(teksture, inited, 4, names, selected);
	//glDisable(GL_TEXTURE_2D);
	glutSwapBuffers();
}
void displaySecondScene(){

	glClearColor(0,0,0,1.0);
	if(waitForPlayer(&kinect,img)>0){
		scene = 3;
	}
	glutSwapBuffers();
}

void displayThirdScene(){
	glClearColor(0.0,0.0,0.0,1.0);
	nacrtaj_nesto(&kinect, img);
	glutSwapBuffers();
}

void specialKeys(int key, int x, int y){
	if(key == GLUT_KEY_RIGHT){
		if(scene == 1){
			int			priv;
			for(int i=0;i<3;i++){
				if(selected[i] == true){
					selected[i] = false;
					selected[(i+1)%3] = true;
					break;
				}
			}
		}
	}
	else if(key == GLUT_KEY_LEFT){
		if(scene == 1){
			int			priv;
			for(int i=0;i<3;i++){
				if(selected[i] == true){
					priv = (i>0)?i-1:2;
					selected[priv] = true;
					selected[i] = false;
					break;
				}
			}
		}
	}
}

void normalKeys(unsigned char key, int x, int y){
	switch(key){
		case 13:
			if(scene == 1 && selected[2]){
				kinect.shutdown();
				exit(0);
			}else if(scene == 1 && selected[0]){
				scene = 2;
			}
			break;
		case 27:
			if(scene == 3){
				memset(selected,0,sizeof(selected));
				selected[0] = true;
				resetAll();
				scene = 1;
			}
			break;
		default:
			break;
	}
}
