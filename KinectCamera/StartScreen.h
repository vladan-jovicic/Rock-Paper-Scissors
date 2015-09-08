#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include "Bitmap.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <glut.h>

void LoadAllTextures(GLuint *textures, bool read[], int num, char names[][20], bool selected[]);
void draw(GLuint *textures, int num, bool selected[]);

#endif