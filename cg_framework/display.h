#ifndef _DISPLAY_H
#define _DISPLAY_H

extern GLuint baseTextureId[];
extern GLuint programId;
extern glm::mat4 perspectiveMatrix;
extern glm::mat4 cameraMatrix;
extern GLfloat lightDir[];
extern GLfloat lightIntensity[];
extern GLfloat ambientComponent[];
extern GLfloat diffuseColor[];

//GLuint vertexShaderId;
//GLuint fragShaderId;


//GLuint baseTextureId;
//GLuint specularTextureId;

void display_at(int va_pos,float tx,float ty,float tz, float ra,float rx,float ry,float rz,float sx,float sy,float sz);

#endif