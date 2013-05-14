#ifndef _MAIN_H
#define _MAIN_H

#include "objloader.h"

const int objloader = 7;

struct FALL_BALL{
	GLfloat x;
	GLfloat y;
	double speed;
};

std::vector<FALL_BALL>; 

extern GLuint vao[objloader];
extern OBJLoader object[objloader];

#endif