#include "StartScreen.h"

struct dot{
	GLfloat x,y;
	dot(){}
	dot(GLfloat a, GLfloat b){
		x = a;
		y = b;
	}
};

dot dots[] = {dot(-0.8,-0.4),dot(-0.4,-0.4),dot(-0.4,0),dot(-0.8,0),
			  dot(-0.2,-0.4),dot( 0.2,-0.4),dot( 0.2,0),dot(-0.2,0),
			  dot( 0.4,-0.4),dot( 0.8,-0.4),dot( 0.8,0),dot( 0.4,0)
};


void LoadAllTextures(GLuint *textures, bool read[], int num, char names[][20], bool selected[]){
	Bitmap *image;
	image = new Bitmap();
	for(int i=0;i<2*num - 1;i++){
		if(read[i]) continue;
		if(!image->loadBMP(names[i])){
			printf("%d -ta textura nije ucitana", i);
			continue;
		}
		glGenTextures(1,&textures[i]);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, image->width, image->height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image->data);
		read[i] = true;
	}
	delete image;
	draw(textures, num, selected);
}

void draw(GLuint *textures, int num, bool selected[]){
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glBegin(GL_QUADS);
		glTexCoord2d(0.0,0.0); glVertex2d(-1.0,-1.0);
		glTexCoord2d(1.0,0.0); glVertex2d(+1.0,-1.0);
		glTexCoord2d(1.0,1.0); glVertex2d(+1.0,+1.0);
		glTexCoord2d(0.0,1.0); glVertex2d(-1.0,+1.0);
	glEnd();

	for(int i = 1;i<num;i++){
		GLfloat fens;
		if(selected[i-1]==true){
			glBindTexture(GL_TEXTURE_2D,textures[2*i]);
			fens = 0.05;
		}
		else {
			glBindTexture(GL_TEXTURE_2D,textures[2*i-1]);
			fens = 0;
		}
		glBegin(GL_QUADS);
			glTexCoord2d(0.0,0.0); glVertex2d(dots[4*(i-1)].x,dots[4*(i-1)].y+fens);
			glTexCoord2d(1.0,0.0); glVertex2d(dots[4*(i-1)+1].x,dots[4*(i-1)+1].y+fens);
			glTexCoord2d(1.0,1.0); glVertex2d(dots[4*(i-1)+2].x,dots[4*(i-1)+2].y+fens);
			glTexCoord2d(0.0,1.0); glVertex2d(dots[4*(i-1)+3].x,dots[4*(i-1)+3].y+fens);
		glEnd();
	}
	//iscrtaj 3 dugmeta;

}
