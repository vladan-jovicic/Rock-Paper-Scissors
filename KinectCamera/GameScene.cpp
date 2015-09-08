

#include "GameScene.h"

GLuint			noveteksture[4];
bool			ucitan[4] = {0}, goingon = true, povecano = false;
char			naziv[4][20];
int				score1 = 0, score2 = 0;
int				rnd, currentTime, prevTime = -1, brojac = 0, lastRecognized = -1, fing;
int				pobjednik[4][4] = { {-1, -1, -1, -1},
									{-1,  0,  1,  2},
									{-1,  2,  0,  1},
									{-1,  1,  2,  0}};


void resetAll(){
	score1 = 0; score2 = 0;
	prevTime=-1; brojac = 0; lastRecognized = -1;
	povecano = false;
}
void writeText(char slovo, float x, float y){
	glRasterPos2f(x,y);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,slovo);
}

bool getSecond(IplImage *slika){
	if(prevTime < 0){
		prevTime = glutGet(GLUT_ELAPSED_TIME);
	}
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	if(currentTime - prevTime > 1000){
		//ako je prosla jedna sekunda
		prevTime = currentTime;
		return true;
	}
	if(currentTime-prevTime > 30){
		cvShowImage("window1",slika);
	}
	return false;
}

void writeInt(int p, float x, float y){
	char text[10];
	int bb = 0;
	if(p==0){
		text[bb++] = '0';
	}else {
		while(p>0){
			text[bb++] = (p%10)+'0';
			p/=10;
		}
	}
	glRasterPos2f(x,y);
	for(int i=bb-1;i>=0;i--){
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,(int)text[i]);
	}
}
void writeString(char *p, float x, float y, int size){
	glRasterPos2f(x,y);
	for(int i=0;i<size;i++){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,(int)p[i]);
	}
}
void nacrtaj_nesto(Kinect *kinect, IplImage *img){
	strcpy(naziv[0],"thirdscene.bmp");
	strcpy(naziv[1],"fist.bmp");
	strcpy(naziv[2],"dvaprsta.bmp");
	strcpy(naziv[3],"petprsta.bmp");
	int font = (int)GLUT_BITMAP_HELVETICA_18;
	Bitmap *image;
	image = new Bitmap();
	for(int i=0;i<4;i++){
		if(!ucitan[i]){
			if(image->loadBMP(naziv[i])){
				glGenTextures(1,&noveteksture[i]);
				glBindTexture(GL_TEXTURE_2D, noveteksture[i]);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
				glTexImage2D(GL_TEXTURE_2D, 0, 3, image->width, image->height, 0,
				GL_RGB, GL_UNSIGNED_BYTE, image->data);
				ucitan[i] = true;
			}
		}
	}
	delete image;
	//postavi glavnu sliku
	glBindTexture(GL_TEXTURE_2D,noveteksture[0]);
		glBegin(GL_QUADS);
			glTexCoord2d(0.0,0.0); glVertex2d(-1.0,-1.0);
			glTexCoord2d(1.0,0.0); glVertex2d(+1.0,-1.0);
			glTexCoord2d(1.0,1.0); glVertex2d(+1.0,+1.0);
			glTexCoord2d(0.0,1.0); glVertex2d(-1.0,+1.0);
		glEnd();
	//ispisi rezultat
	writeInt(score1, -0.2,0.7);
	writeInt(score2,  0.2,0.7);
	if(brojac < 5){
	rnd = rand()%3+1;
	}
	glBindTexture(GL_TEXTURE_2D, noveteksture[rnd]);
		glBegin(GL_QUADS);
		glTexCoord2d(0.0,0.0); glVertex2d( 0.25,-1.00);
		glTexCoord2d(1.0,0.0); glVertex2d( 1.00,-1.00);
		glTexCoord2d(1.0,1.0); glVertex2d( 1.00, 0.65);
		glTexCoord2d(0.0,1.0); glVertex2d( 0.25, 0.65);
		glEnd();
	if(brojac < 5) {
		fing = -1;
		//prepoznaj
		if(kinect->getDepthImage(img,&fing)){
			//printf("%d\n",fing);
			if(fing == 0){
				lastRecognized = 1;
			}else if(fing == 2){
				lastRecognized = 2;
			}else if(fing == 5){
				lastRecognized = 3;
			}
		}

	}
	if(lastRecognized > 0){
		glBindTexture(GL_TEXTURE_2D,noveteksture[lastRecognized]);
		glBegin(GL_QUADS);
			glTexCoord2d(0.0,0.0); glVertex2d(-1.00,-1.00);
			glTexCoord2d(1.0,0.0); glVertex2d(-0.25,-1.00);
			glTexCoord2d(1.0,1.0); glVertex2d(-0.25, 0.65);
			glTexCoord2d(0.0,1.0); glVertex2d(-1.00, 0.65);
		glEnd();
	}
	if(brojac >= 5){
		//pauziraj
		//writeInt(brojac-5,-0.02,0.3);
		//ispisi ko je pobjednik
		if(lastRecognized > 0){
			if(pobjednik[rnd][lastRecognized] == 1){
				writeString("Comp",-0.09,0.3,4);
				if(!povecano){	
					score2++;
					povecano = true;
				}
			}
			else if(pobjednik[rnd][lastRecognized] == 2){
				writeString("Youu",-0.09,0.3,4);
				if(!povecano){
					score1++;
					povecano = true;
				}
			}else if(pobjednik[rnd][lastRecognized] == 0){
				writeString("Draw",-0.09,0.3,4);
			}
		}else {
			writeString("None!!!",-0.09,0.3,7);
		}
	}else {
		writeInt(brojac,-0.02,0.3);
	}
	//iscrtaj ako si prepoznao nesto novo, ako ne, ostavi staru
	if(brojac == 15){
		lastRecognized = -1;
		brojac = 0;
		povecano = false;
	}
	if(getSecond(img)){
		brojac ++;
	}
}