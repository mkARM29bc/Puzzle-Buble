#include <Gl/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <FreeImage.h>
//#include <vector>
#include "mmsystem.h"
#include "display.h"
#include "main.h"

#define SPACEBAR 32

#define PI 3.14159265f



int cellshading=0;
bool scaled=false;
bool started=false;
GLfloat ship[10]={11.0f,10.0f,-150.0f, 45.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f};
bool loop=false;
GLfloat limitPoints[2][2]={{31.0f,-31.0f},{31.0f,-31.0f}};		// limites para onde a nave se pode mexer(para não sair do ecrã) - 4 cantos cada um com um x e um y
GLfloat actualPoints[4][2]={{11.0f,11.0f},{-11.0f,-11.0f},{5.0f,11.0f},{0.0f,5.0f}};		// ponto de onde começa uma curva, ponto de destino, pontos das duas extremidades que vao definir as curvas e x e y
bool modeWall=true;				// define se a nave esta a movimentar-se no plano ao fundo do ecrã ou em profundidade
GLfloat interpolation=0.0f;  // valor da actual interpolação (0.0-1.0)
GLfloat interpolationTax=0.0007f;  // taxa de variação da interpolação
GLfloat shipdepth=-200.0f;
bool pauseship=false;
GLfloat pointInterpolation[5][2];




void newBezierPoints(){
	GLfloat temp;
	int signTemp;
	interpolation=0.0f;

	// guarda o ponto onde está como ponto de partida
	actualPoints[0][0]=actualPoints[1][0];
	actualPoints[0][1]=actualPoints[1][1];

	actualPoints[1][0]=(GLfloat) (rand()%100) -50.0f;
	actualPoints[1][1]=(GLfloat) (rand()%100)-50.0f;


	actualPoints[2][0]=actualPoints[0][0]+(actualPoints[0][0]-actualPoints[3][0]);
	actualPoints[2][1]=actualPoints[0][1]+(actualPoints[0][1]-actualPoints[3][1]);

	actualPoints[3][1]=(GLfloat) (rand()%100) -50.0f;
	actualPoints[3][0]=(GLfloat) (rand()%100) -50.0f;





}




GLfloat Interpolate(GLfloat a,GLfloat b){
	return ((1-interpolation)*a   + interpolation*b) ;
}


void Bezier(){

	interpolation+=interpolationTax;


	pointInterpolation[0][0]=Interpolate(actualPoints[0][0],actualPoints[2][0]);
	pointInterpolation[0][1]=Interpolate(actualPoints[0][1],actualPoints[2][1]);

	pointInterpolation[1][0]=Interpolate(actualPoints[2][0],actualPoints[3][0]);
	pointInterpolation[1][1]=Interpolate(actualPoints[2][1],actualPoints[3][1]);

	pointInterpolation[2][0]=Interpolate(actualPoints[3][0],actualPoints[1][0]);
	pointInterpolation[2][1]=Interpolate(actualPoints[3][1],actualPoints[1][1]);

	pointInterpolation[3][0]=Interpolate(pointInterpolation[0][0],pointInterpolation[1][0]);
	pointInterpolation[3][1]=Interpolate(pointInterpolation[0][1],pointInterpolation[1][1]);

	pointInterpolation[4][0]=Interpolate(pointInterpolation[1][0],pointInterpolation[2][0]);
	pointInterpolation[4][1]=Interpolate(pointInterpolation[1][1],pointInterpolation[2][1]);

	ship[0]=Interpolate(pointInterpolation[3][0],pointInterpolation[4][0]);
	ship[1]=Interpolate(pointInterpolation[3][1],pointInterpolation[4][1]);

	if (interpolation>=1.0f)
	{newBezierPoints();}

}



GLfloat blink=1.0f;
GLfloat blinkaux=0.05f;
bool incrementblink=true;
int explodestage=0; //0 -> not exploding, 1 -> imploding, 2-> exploding
bool galaxyON=true;
bool particleShift=false;
bool populateParticlesShiftDistance=false;
const int transitionChangeTime=100;
int transitionTime=0;

bool debug=true;
const int lines = 10;
const int column  = 8;
GLfloat scaleBall[lines][column]={0.0f};
//Rui
GLfloat anglestart=0.0;
GLfloat anglefinish=360.0;
GLfloat sizespiral=0.0;
const GLfloat sizespiralmax=1000.0;
GLfloat actualfragment;
int particlesState=0;


int visitedBalls[lines][column];
GLfloat toDestroyBalls[lines][column];
bool visiting=false;
int sumBalls=0;
GLfloat toDestroy[20][6]={0.0f} ; 
//

//int colorBalls[NUMBER_OBJECTS][NUMBER_OBJECTS];
bool inited = false; //Have we done initialization?
int ballAtX=0;
int ballAtY=0;
int color=rand()%3+1;;
int colorActive=color;
/* Rui testing colors variable & more

c changes camera (
1,2,3 changes color

*/

//int colors[500];

float positionbackground=0.05f;
float POINTER = 180.0f;
float dispx=-35.0f;
/*end of test*/

/*
vao: will hold the VAO identifier (usually one per object)
geomId: will hold the VBO identifier (one per attribute: position, normal, etc.)
*/



OBJLoader object[objloader] = {("../models/esfera1.obj"),("../models/cube2.obj"),("../models/esfera1.obj"),("../models/textured_cube.obj"),("../models/textured_cube.obj"),("../models/triangle.obj"),("../models/pointer.obj")};



GLuint vertexShaderId;
GLuint fragShaderId;
GLuint programId;

GLuint baseTextureId[objloader];
GLuint specularTextureId;


GLuint vao[objloader];
GLuint geomId;
GLuint normalsId;
GLuint texUVId;


glm::mat4 perspectiveMatrix;
glm::mat4 cameraMatrix;


// can have three values. 0 means that no camera change is active. 1 means that a camera change is active and "move" was 0 before camera change, 2 camera change active but move was 1.
int cameraChange=0;

GLfloat cameraPosRatio[4];
GLfloat cameraViewRatio[4];
GLfloat cameraUpRatio[4];




glm::vec3 cameraPos[5]={glm::vec3(10.0f, 120.0f, -120.0f),glm::vec3(0.0f, -30.0f, 25.0f),glm::vec3(0.0f, 0.0f, 10.0f),glm::vec3(0.0f, 35.0f, 150.0f),glm::vec3(0.0f,-100.0f,-100.0f)};
glm::vec3 cameraView[5]={glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(1.0f,1.0f,1.0f),glm::vec3(0.0f, 0.0f, -10.0f),glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(0.0f,0.0f,-100.0f)};
glm::vec3 cameraUp[5]={glm::vec3(0.0f, 1.0f, 1.0f),glm::vec3(0.0f, 1.0f, 1.0f),glm::vec3(0.0f, 1.0f, 1.0f),glm::vec3(0.0f, 1.0f, 1.0f),glm::vec3(0.0f, 1.0f, 1.0f)};



glm::vec3 cameraPosCameraChanger=glm::vec3(10.0f, 120.0f, -120.0f);
glm::vec3 cameraViewCameraChanger=glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUpCameraChanger=glm::vec3(0.0f, 1.0f, 1.0f);

GLfloat angle = -1.57079633f;
GLfloat angle2 = 0.0f;

//translation, rotation,scale

const GLfloat velocity = 0.25f;

int cameraMode = 0;
int move = 0;
int frame=0,time,timebase=0;
int speed = 10;
int restart = 1;
int level = 0;
const int max_levels = 2;
int players = 1;

//Rui - controls light movements
bool increment=false;
bool incrementy=true;
bool incrementDiffuseBall=true;
bool incrDiffuseExplo=false;
bool destructionOngoing=false;

int destroyCounter=0;
GLfloat ambientDestroyer=1.0f;

GLfloat lightDir[] = {1.0f,  -0.5f, 1.0f};
GLfloat lightIntensity[] = {0.9f, 0.9f, 0.9f, 0.0f};

GLfloat lightIntensityBallNormal[] = {1.0f,  -0.5f, 1.0f, 0.0f};
GLfloat lightIntensityBallDestroy[] = {1.0f,  -0.5f, 1.0f, 0.0f};

GLfloat ambientComponent[] = {1.0f, 1.0f, 1.0f, 1.0f};


GLfloat diffuseColorBallNormal[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat diffuseColor[] = {1.0f, 1.0f, 1.0f, 1.0f};

GLfloat ambient;

bool exploding;
GLfloat diffused=1.5f;
GLfloat diffusedExplosion=1.5f;

GLfloat PLAYER_ORIGINAL[1][7] = {0.0f, -10.0f, 0.0f,0.0f,1.0f, 0.0f, 0.0f};
GLfloat PLAYER[1][7] = {0.0f, -10.0f, 0.0f,0.0f,1.0f, 0.0f, 0.0f};
GLfloat MOVE_PLAYER_ORIGINAL[1][2] = {0.0f,1.0f};
GLfloat MOVE_PLAYER_TRANSLATE[1][2] = {0.0f,1.0f};
GLfloat MOVE_PLAYER[1][2] = {0.0f,1.0f};

// LEVELS [number of level] [lines][column][0 relate of the ball , 1 relate of the color]
GLint LEVELS[max_levels][lines][column][2] = {
	//LEVEL 0
	{{
		{1,1},{1,1},{1,2},{1,2},{1,3},{1,3},{1,1},{1,1} 
	},{
		{-1,0},{1,1},{1,1},{1,2},{1,2},{1,3},{1,3},{1,1}
	},{
		{1,2},{1,2},{1,3},{1,3},{1,1},{1,1},{1,2},{1,2}
	},{
		{-1,0},{1,2},{1,3},{1,3},{1,1},{1,1},{1,2},{1,2}
	},{
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
	},{
		{-1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
	},{
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
	},{
		{-1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
	},{
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
	},{
		{-1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
	}},
		//LEVEL 1
	{{
		{1,1},{1,1},{1,2},{1,2},{1,3},{1,3},{1,1},{1,1} 
	},{
		{-1,0},{1,1},{1,1},{1,2},{1,2},{1,3},{1,3},{1,1}
	},{
		{1,2},{1,2},{1,3},{1,3},{1,1},{1,1},{1,2},{1,2}
	},{
		{-1,0},{1,2},{1,2},{1,3},{1,3},{1,1},{1,1},{1,2}
	},{
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
	},{
		{-1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
	},{
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
	},{
		{-1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
	},{
		{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
	},{
		{-1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
	}}
};

// GAMEPLAY [player1 = 0] [lines][column] [0 relate of the ball , 1 relate of the color]
GLint GAMEPLAY[1][lines][column][2] = {{{
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
},{
	{-1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
},{
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
},{
	{-1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
},{
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
},{
	{-1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
},{
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
},{
	{-1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
},{
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
},{
	{-1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
}
}};

GLint TEST_SPHERE[1][lines][column][1] = {{{
	{0},{0},{0},{0},{0},{0},{0},{0}
},{
	{-1},{0},{0},{0},{0},{0},{0},{0}
},{
	{0},{0},{0},{0},{0},{0},{0},{0}
},{
	{-1},{0},{0},{0},{0},{0},{0},{0}
},{
	{0},{0},{0},{0},{0},{0},{0},{0}
},{
	{-1},{0},{0},{0},{0},{0},{0},{0}
},{
	{0},{0},{0},{0},{0},{0},{0},{0}
},{
	{-1},{0},{0},{0},{0},{0},{0},{0}
},{
	{0},{0},{0},{0},{0},{0},{0},{0}
},{
	{-1},{0},{0},{0},{0},{0},{0},{0}
}
}};

GLint RESET_SPHERE[lines][column][1] = {{
	{0},{0},{0},{0},{0},{0},{0},{0}
},{
	{-1},{0},{0},{0},{0},{0},{0},{0}
},{
	{0},{0},{0},{0},{0},{0},{0},{0}
},{
	{-1},{0},{0},{0},{0},{0},{0},{0}
},{
	{0},{0},{0},{0},{0},{0},{0},{0}
},{
	{-1},{0},{0},{0},{0},{0},{0},{0}
},{
	{0},{0},{0},{0},{0},{0},{0},{0}
},{
	{-1},{0},{0},{0},{0},{0},{0},{0}
},{
	{0},{0},{0},{0},{0},{0},{0},{0}
},{
	{-1},{0},{0},{0},{0},{0},{0},{0}
}
};

GLfloat POSITION[1][2][column][2] = {{{
	{-28.0f,70.0f},{-20.0f,70.0f},{-12.0f,70.0f},{-4.0f,70.0f},{4.0f,70.0f},{12.0f,70.0f},{20.0f,70.0f},{28.0f,70.0f}
},{
	{0.0f,0.0f},{-24.0f,63.0f},{-16.0f,63.0f},{-8.0f,63.0f},{0.0f,63.0f},{8.0f,63.0f},{16.0f,63.0f},{24.0f,63.0f}
}}};

GLfloat SIDE_BORDER[1][2] = {{POSITION[0][0][0][0],POSITION[0][0][7][0]}};

int end_line = 9;
int end_game = 0;

int NUMBER_OF_BALLS = 0;


/*
FALL_BALL fall_ball;

fall_ball.x = 0.0f;
fall_ball.y = 0.0f;
fall_ball.speed = 0.0f;
*/

/* 
Error checking function:

It checks for error in the OpenGL pipeline;
*/
void soundcamera(){		
			int error2;
		LPCWSTR d = L"close camera.mp3";
		error2 = mciSendString(d, NULL, 0, 0);
		d = L"open camera.mp3";
		error2 = mciSendString(d, NULL, 0, 0);
		d = L"play camera.mp3";
		error2 = mciSendString(d, NULL, 0, 0);}

void soundballout(){		
			int error2;
		LPCWSTR d = L"close ballout.mp3";
		error2 = mciSendString(d, NULL, 0, 0);
		d = L"open ballout.mp3";
		error2 = mciSendString(d, NULL, 0, 0);
		d = L"play ballout.mp3";
		error2 = mciSendString(d, NULL, 0, 0);}


void checkError(const char *functionName)
{
	GLenum error;
	while (( error = glGetError() ) != GL_NO_ERROR) {
		fprintf (stderr, "GL error 0x%X detected in %s\n", error, functionName);
	}
}

/*
Prints the information regarding the OpenGL context
*/

void dumpInfo(void)
{
	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
	checkError ("dumpInfo");
}

/*
creates the program
1. Generate the program's id
2. Attach shader
3. Repeat 2. until all shaders attached
4. Link the program
5. Check for errors

*/
void setBallColor(void){

	if (destructionOngoing){
		if (color==0){

			//diffuseColor[0] = 1.0f;
			diffuseColor[0] = 0.0f;
			diffuseColor[1] = 0.0f;
			diffuseColor[2] = 0.0f;
		}
		if (color==1){

			//diffuseColor[0] = 1.0f;
			diffuseColor[0] = diffusedExplosion;
			diffuseColor[1] = 0.0f;
			diffuseColor[2] = 0.0f;
		}
		if (color==2){
			diffuseColor[0] = 0.0f;
			//diffuseColor[1] = 1.0f;
			diffuseColor[1] = diffusedExplosion;
			diffuseColor[2] = 0.0f;

		}
		if (color==3){

			diffuseColor[0] = 0.0f;
			diffuseColor[1] = 0.0f;
			//diffuseColor[2] = 1.0f;
			diffuseColor[2] = diffusedExplosion;
		}
	}
	else{
		/*
		if (color==0){

		//diffuseColor[0] = 1.0f;
		diffuseColor[0] = 0.0f;
		diffuseColor[1] = 0.0f;
		diffuseColor[2] = 0.0f;
		}
		*/
		if (color==1){

			//diffuseColor[0] = 1.0f;
			diffuseColor[0] = diffused;
			diffuseColor[1] = 0.0f;
			diffuseColor[2] = 0.0f;
		}
		if (color==2){
			diffuseColor[0] = 0.0f;
			//diffuseColor[1] = 1.0f;
			diffuseColor[1] = diffused;
			diffuseColor[2] = 0.0f;

		}
		if (color==3){

			diffuseColor[0] = 0.0f;
			diffuseColor[1] = 0.0f;
			//diffuseColor[2] = 1.0f;
			diffuseColor[2] = diffused;
		}
	}
}

//[number of particles][0-x,1-y,2-z,3-actualx,4-actualy]
GLfloat particles[1000][7];


GLfloat degreesToRadian(GLfloat anglei){
	return anglei*PI/180.0f;
}

GLfloat involuteX(GLfloat radius, GLfloat angle){
	return radius*(cos(angle)+angle*sin(angle));
}

GLfloat involuteY(GLfloat radius, GLfloat angle){
	return radius*(sin(angle)-angle*sin(angle));
}

GLfloat ArchimedeanX(GLfloat angle){
	return (1+angle)*(cos(angle));
}

GLfloat ArchimedeanY(GLfloat angle){
	return (1+angle)*(sin(angle));
}

GLfloat CircleX(GLfloat angle){
	return 10*cos(angle);
}

GLfloat CircleY(GLfloat angle){
	return 10*sin(angle);
}


void drawSpiral(){
	GLfloat extrafragz;
	for (int i=0;i<120;i++)
	{
		particles[i][0]=particles[i][0]+0.001f;
		/*	if (rand()%2==0)
		{
		if(particles[i][2]<100)
		particles[i][2]+=0.01;
		}
		else
		{
		if(particles[i][2]>-100)
		particles[i][2]-=0.01;
		}*/
		actualfragment=particles[i][0];
		particles[i][4]=ArchimedeanY(particles[i][0]);
		particles[i][3]=ArchimedeanX(particles[i][0])-100;
		display_at(5,particles[i][4], particles[i][2],  particles[i][3],particles[i][1]-particles[i][3],0.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f);

		if(i<50){
			extrafragz=0,36-(i*0.03);
			display_at(5,particles[i][4], particles[i][2]-(extrafragz), particles[i][3],particles[i][1]+particles[i][3],0.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f);
			display_at(5,particles[i][4], particles[i][2]+(extrafragz), particles[i][3],-particles[i][1]+particles[i][4],0.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f);
		} 
		if(particles[i][0]>120*0.2)
			particles[i][0]=0.01f*1.0f;

	}
}

void drawCircle(){
	GLfloat extrafragz;
	for (int i=0;i<120;i++)
	{
		particles[i][0]=particles[i][0]+0.001f;
		/*	if (rand()%2==0)
		{
		if(particles[i][2]<100)
		particles[i][2]+=0.01;
		}
		else
		{
		if(particles[i][2]>-100)
		particles[i][2]-=0.01;
		}*/
		actualfragment=particles[i][0];
		particles[i][4]=CircleY(particles[i][0]);
		particles[i][3]=CircleX(particles[i][0])-100.0f;
		display_at(5,particles[i][4], particles[i][2],  particles[i][3],particles[i][1]-particles[i][3],0.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f);

		if(i<50){
			extrafragz=0.36f-(i*0.03f);
			display_at(5,particles[i][4], particles[i][2]-(extrafragz), particles[i][3],particles[i][1]+particles[i][3],0.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f);
			display_at(5,particles[i][4], particles[i][2]+(extrafragz), particles[i][3],-particles[i][1]+particles[i][4],0.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f);
		} 
		if(particles[i][0]>120.0f*0.2f)
			particles[i][0]=0.01f*1.0f;

	}
}

void populateTransitionDisplacement(){

	if (particlesState==0)
	{
		for (int i=0;i<120;i++)
		{
			actualfragment=particles[i][0];
			particles[i][5]= (CircleX(particles[i][0])-100.0f-particles[i][3])/transitionChangeTime;
			particles[i][6]= (CircleY(particles[i][0])-particles[i][4])/transitionChangeTime;

			display_at(5,particles[i][4], particles[i][2],  particles[i][3],particles[i][1]+particles[i][4],0.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f);

		}

	}
	else
	{
		for (int i=0;i<120;i++)
		{

			actualfragment=particles[i][0];
			// -1     3   
			particles[i][5]= (CircleX(particles[i][0])-100.0f-particles[i][3])/transitionChangeTime;
			particles[i][6]= (CircleY(particles[i][0])-particles[i][4])/transitionChangeTime;

			display_at(5,particles[i][4], particles[i][2],  particles[i][3],particles[i][1]+particles[i][4],0.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f);
		}

	}
	populateParticlesShiftDistance=false;
	transitionTime=0;
}

void changeParticles(){
	bool complete=true;
	GLfloat extrafragz;
	if (populateParticlesShiftDistance)
	{
		populateTransitionDisplacement();
	}

	else
	{
		if(transitionTime<transitionChangeTime)
		{

			for (int i=0;i<120;i++)
			{
				actualfragment=particles[i][0];
				//particles[i][3]+=particles[i][5];
				//particles[i][4]+=particles[i][6];
				if (particlesState==0)
				{
					if (transitionTime==0){
						particles[i][5]=abs(abs(particles[i][3])-abs(ArchimedeanX(particles[i][0])-100))/transitionChangeTime;
						particles[i][6]=abs(abs(particles[i][4])-abs(ArchimedeanY(particles[i][0])))/transitionChangeTime;
					}
					if (particles[i][3]>ArchimedeanX(particles[i][0])-100){particles[i][3]-=particles[i][5];complete=false;}
					if (particles[i][3]<ArchimedeanX(particles[i][0])-100){particles[i][3]+=particles[i][5];complete=false;}
					if (particles[i][4]>ArchimedeanY(particles[i][0])){particles[i][4]-=particles[i][6];complete=false;}
					if (particles[i][4]<ArchimedeanY(particles[i][0])){particles[i][4]+=particles[i][6];complete=false;}
					display_at(5,particles[i][4], particles[i][2],  particles[i][3],particles[i][1]-particles[i][3],0.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f);

				}
				else 
				{
					if (transitionTime==0){
						particles[i][5]=abs(abs(particles[i][3])-abs(CircleX(particles[i][0])-100))/transitionChangeTime;
						particles[i][6]=abs(abs(particles[i][4])-abs(CircleY(particles[i][0])))/transitionChangeTime;
					}
					if (particles[i][3]>CircleX(particles[i][0])-100){particles[i][3]-=particles[i][5];complete=false;}
					if (particles[i][3]<CircleX(particles[i][0])-100){particles[i][3]+=particles[i][5];complete=false;}
					if (particles[i][4]>CircleY(particles[i][0])){particles[i][4]-=particles[i][6];complete=false;}
					if (particles[i][4]<CircleY(particles[i][0])){particles[i][4]+=particles[i][6];complete=false;}
					display_at(5,particles[i][4], particles[i][2],  particles[i][3],particles[i][1]-particles[i][3],0.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f);
				}
				if(i<50){
					extrafragz=0,36-(i*0.03);

					display_at(5,particles[i][4], particles[i][2]-(extrafragz), particles[i][3],particles[i][1]+particles[i][3],0.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f);
					display_at(5,particles[i][4], particles[i][2]+(extrafragz), particles[i][3],-particles[i][1]+particles[i][4],0.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f);
				} 
			}
			//display_at(5,particles[i][4], particles[i][2],  particles[i][3],particles[i][1]+particles[i][4],0.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f);
			transitionTime++;
			if (complete) transitionTime=transitionChangeTime;
			if (transitionTime+1>=transitionChangeTime) particleShift=false;
		}



		else
		{
			printf("teste");
			particleShift=false;

		}
	}
}

void createParticles(){

	GLfloat radius=60.0f;
	GLfloat anglehere=0.1f;
	GLfloat ztemp=0.0f;
	for (int i=0;i<120;i++)
	{
		anglehere = 0.2f * i;
		particles[i][0]=anglehere;
		particles[i][1]=GLfloat (rand() % 360);
		particles[i][2]=0;
		particles[i][3]=0;
		particles[i][5]=0;
		particles[i][6]=0;
		//particles[i][1]=ArchimedeanY(anglehere);
		if (rand()%2==0)
		{
			if(ztemp<100)
				ztemp+=0.1f;
		}
		else
		{
			if(ztemp>-100)
				ztemp-=0.1f;
		}
		particles[i][2]=ztemp;
		anglehere+=2.0f;

	}
	//changeParticles();	
}

void drawParticles(){
	float actualy,actualx;

	actualfragment=0.0f;		// primeiro o não transparente
	color=0;
	setBallColor();
	display_at(0, 0.0f, 0.0f, -100.0f,45.0f,1.0f,1.0f, 1.0f,blink-0.1f*blink,blink-0.1f*blink,blink-0.1f*blink);
	actualfragment=-1;
	display_at(0, 0.0f, 0.0f, -100.0f,45.0f,1.0f,1.0f, 1.0f,blink+blinkaux*blink,blink+blinkaux*blink,blink+blinkaux*blink);
	if(!particleShift)
	{

		if (particlesState==0)
			drawSpiral();
		else
			drawCircle(); 
	}
	else
		changeParticles();
}




void shipControl(void){
	/*
	if (increment)
	{ship[1]-=0.05f;
	ship[0]+=0.05f;}
	else
	{ship[1]+=0.05f;
	ship[0]-=0.05f;}
	*/
	
	Bezier();

}

void createAndCompileProgram(GLuint vertexId, GLuint fragId, GLuint *programId)
{
	*programId = glCreateProgram(); //1.
	glAttachShader(*programId, vertexId); //2. Attach the shader vertexId to the program programId
	glAttachShader(*programId, fragId); //2. Attach the shader fragId to the program programId

	glLinkProgram (*programId);//4.

	//5. Until the end of the if clause, is to check for COMPILE errors, and only for these. *not* related with the checkError procedure
	GLint status;
	glGetProgramiv (*programId, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		char *infoLog;
		glGetProgramiv (*programId, GL_INFO_LOG_LENGTH, &infoLogLength);
		infoLog = new char[infoLogLength];
		glGetProgramInfoLog (*programId, infoLogLength, NULL, infoLog);
		fprintf (stderr, "link log: %s\n", infoLog);
		delete [] infoLog;
	}
	checkError ("createAndCompileProgram");
}

/*
creates the shader in OpenGL and compiles it.

1. Generate the shader id
2. Set the shader's source
3. Compile the shader
4. Check for errors
*/

void createAndCompileShader(GLuint* id, GLenum type, GLsizei count, const char **shaderSource)
{
	*id = glCreateShader(type); //1. create the shader with type. (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER. others will follow in the future)

	glShaderSource(*id, count, shaderSource, NULL); //2. the shader's source. *id: shader's id; count: memory size of the contents; shaderSource: shader contents; NULL ;)

	glCompileShader(*id); //3.

	//4. Until the end of the if clause, is to check for COMPILE errors, and only for these. *not* related with the checkError procedure
	GLint status;
	glGetShaderiv (*id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		char *infoLog;
		glGetShaderiv (*id, GL_INFO_LOG_LENGTH, &infoLogLength);
		infoLog = new char[infoLogLength];
		glGetShaderInfoLog (*id, infoLogLength, NULL, infoLog);
		fprintf (stderr, "compile log: %s\n", infoLog);
		delete [] infoLog;
	}

	checkError ("createAndCompileShader");

}
/*
loads the shader in file.
1. load the file from disk into a memory block
2. create the shader

*/

void loadShader (char *file, GLuint *id, GLenum type) 
{
	//Open e load  shader file
	FILE* f = fopen(file, "r");
	if (!f) {
		fprintf(stderr, "Unable to open shader file: %s", file);
		return;
	}

	fseek(f, 0, SEEK_END);

	int fileSize = ftell(f);
	rewind(f);

	char *fileContents = new char[fileSize];
	memset(fileContents, 0, fileSize);
	fread(fileContents, sizeof(char), fileSize, f);
	//up until here is to load the contents of the file

	const char* t = fileContents;

	//2. create the shader. arguments (future) shader id, type of shader, memory size of the file contents, file contents
	createAndCompileShader(id, type, sizeof(fileContents)/sizeof(char*), &t);

	//cleanup
	fclose(f);
	delete [] fileContents;
}
/*
loads two shader files, one vertex shader and one fragment shader, and creates the program.
the arguments are the path to the files, including file name.
*/

void loadShaderProgram(char* vertexFile, char* fragmentFile)
{
	//load each shader seperately. arguments: the file, (future) shader id, shader type

	loadShader(vertexFile, &vertexShaderId, GL_VERTEX_SHADER); 
	loadShader(fragmentFile, &fragShaderId, GL_FRAGMENT_SHADER);

	//one the shaders loaded, create the program with the two shaders. arguments: vertex shader id, fragment shader id, (future) program id
	createAndCompileProgram(vertexShaderId, fragShaderId, &programId);
}

/*
Geometry initialization routine.

1. Generate a VAO that holds that matchs the *ATTRIBUTES* (vertex position, normal, etc) to vertex buffer objects (VBO)(which hold the actual information)
2. Active the VAO
3. Active attribute 0 (first attribute in the vertex shader)
4. Generate the VBO
5. Activate the VBO
6. Assign the geometry (the vertices variable previously defined) to the VBO (copying it to the graphics card's memory)
7. Assign this VBO to the attribute
8. Repeat steps 3-7 for remaining attributes
9. Reset OpenGL's state
*/

void initGeometry(int i)
{
	const float *vertices = object[i].getVerticesArray();
	const float *textureCoords = object[i].getTextureCoordinatesArray();
	const float *normals = object[i].getNormalsArray();

	glGenVertexArrays(1, &vao[i]); //1.
	glBindVertexArray(vao[i]); //2.

	glEnableVertexAttribArray(0); //3.
	glGenBuffers(1, &geomId); //4.
	glBindBuffer(GL_ARRAY_BUFFER, geomId); //5.
	glBufferData(GL_ARRAY_BUFFER, object[i].getNVertices() * 3 * sizeof(float), vertices, GL_STATIC_DRAW); //6. GL_ARRAY_BUFFER: the type of buffer; sizeof(vertices): the memory size; vertices: the pointer to data; GL_STATIC_DRAW: data will remain on the graphics card's memory and will not be changed
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //7. 0: the *ATTRIBUTE* number; 3: the number of coordinates; GL_FLOAT: the type of data; GL_FALSE: is the data normalized? (usually it isn't), 0: stride (forget for now); 0: data position (forget for now)

	glEnableVertexAttribArray(1);
	glGenBuffers(1, &texUVId);
	glBindBuffer(GL_ARRAY_BUFFER, texUVId);
	glBufferData(GL_ARRAY_BUFFER, object[i].getNVertices() * 2 * sizeof(float), textureCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(2);
	glGenBuffers(1, &normalsId);
	glBindBuffer(GL_ARRAY_BUFFER, normalsId);
	glBufferData(GL_ARRAY_BUFFER, object[i].getNVertices() * 3 * sizeof(float), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	checkError("initBuffer");
	glBindBuffer(GL_ARRAY_BUFFER, 0); //9.
	glBindVertexArray(0); //9.

}


GLuint loadTexture(char* textureFile)
{
	GLuint tId;

	FIBITMAP *tf = FreeImage_Load(FIF_DDS, textureFile);
	//FIBITMAP *tf = FreeImage_Load(FIF_PNG, textureFile, PNG_DEFAULT);

	if (tf) {

		fprintf(stderr, "Texture: %s loaded\n", textureFile);

		glGenTextures(1, &tId);
		glBindTexture(GL_TEXTURE_2D, tId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FreeImage_GetWidth(tf), FreeImage_GetHeight(tf), 0, GL_BGRA, GL_UNSIGNED_BYTE, FreeImage_GetBits(tf));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

		checkError("loadTexture");
		glBindTexture(GL_TEXTURE_2D, 0);

		FreeImage_Unload(tf);

		return tId;
	}
	return 0;
}

/*
Initialization function

All initialization procedures should be performed here.
*/


void resetVisited(void){
	for (int i=0;i<lines;i++)
		for (int j=0;j<column;j++)
		{visitedBalls[i][j]=0;
	toDestroyBalls[i][j]=0;
	}
}
void init(void) 
{

	createParticles();
	LPCWSTR a = L"open ballout.mp3 type mpegvideo";
		int error = 99;
		error = mciSendString(a, NULL,0,0);
		LPCWSTR c = L"open camera.mp3 type mpegvideo";
		 error = 99;
		error = mciSendString(c, NULL,0,0);
				 a = L"open star.mp3 type mpegvideo";

error = mciSendString(a, NULL,0,0);

	/*
	GLEW initialization.
	activate GLEW experimental features to have access to the most recent OpenGL, and then call glewInit.
	it is important to know that this must be done only when a OpenGL context already exists, meaning, in this case, glutCreateWindow has already been called.
	*/



	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (err != GLEW_OK) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
	} else {
		if (GLEW_VERSION_3_3)
		{
			fprintf(stderr, "Driver supports OpenGL 3.3\n");
		}
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	FreeImage_Initialise();

	loadShaderProgram("../shaders/vertex_shader.vs", "../shaders/frag_shader.fs");

	for (int i=0;i<objloader;i++){
		object[i].init();
		initGeometry(i);
	}
	//object[0].init();
	//initGeometry();
	//object.print();


	baseTextureId[0] = loadTexture("../models/textures/sky.dds");
	baseTextureId[1]=loadTexture("../models/textures/sky.dds");
	baseTextureId[2]=loadTexture("../models/textures/sky.dds");
	baseTextureId[3]=loadTexture("../models/textures/info.dds");
	baseTextureId[4]=loadTexture("../models/textures/base.dds");
	baseTextureId[5]=loadTexture("../models/textures/sky.dds");
	baseTextureId[6]=loadTexture("../models/textures/base.dds");
	//Rui edit
	glClearColor(0.8f, 0.8f, 1.0f, 1.0f); //Defines the clear color, i.e., the color used to wipe the display
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Defines the clear color, i.e., the color used to wipe the display

	glEnable(GL_DEPTH_TEST);

	// Rui




	//	glTexEnv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//glEnable (GL_BLEND); glBlendFunc (GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);


	float specReflection[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);
	glMateriali(GL_FRONT, GL_SHININESS, 56);

	for (int i=0;i<lines;i++){
		for (int j=0;j<column;j++)
			//colorBalls[i][j]=0;
				GAMEPLAY[0][i][j][1] = 0;
		//END Rui
		checkError("init");
	}
	
	 a = L"open po.mp3 type mpegvideo";
 error = 99;
error = mciSendString(a, NULL,0,0);
int error2;
LPCWSTR b = L"play po.mp3 repeat";
error2 = mciSendString(b, NULL, 0, 0); 


}


/*
[FREEGLUT]

display function;
This function is called to paint the display. This is where the drawing functions should be placed.
Why is the init inside display? Because some drivers require that the display window to be visible to work correctly; therefore, the initialization is done once, but on the first call to 
display, ensuring that the window is visible;
*/

//float t = 0;




/* 

Check surrounding balls of the given as argument, as long as they are not tagged as visited and different color
the surrounding indexes of a ball and change their value them to toDestroyBalls vector as 1
l-- c--		l--	c	
l c--						l c++
l++	c		l++ c++
*/
void checkSurroundingBalls(int l, int c){
	visitedBalls[l][c]=1;
	toDestroyBalls[l][c]=1;

	sumBalls++;
	//printf("\n \n SUMBALLS: %d",sumBalls);
	int linha,coluna,ctemp;
	for (int ltemp=-1;ltemp<=1;ltemp++){
		for (int ctemp2=-1;ctemp2<1;ctemp2++)
		{
			ctemp=ctemp2;
			if (l%2==0 && ltemp!=0 && ctemp==-1)
				ctemp=1;
			if (ltemp==0 && ctemp==0)
				ctemp=1;

			//aqui para corrigir
			linha=l+ltemp;
			coluna=c+ctemp;

			printf("Verificar bola linha %d e coluna %d\n",l+ltemp, c+ctemp);
			if ((l+ltemp)>=0 && (l+ltemp)<=7 && (c+ctemp)>=0 && (c+ctemp)<=7){
				printf("passou teste1");
				//printf(" teste linha:%d coluna:%d\n GAMEPLAY[0][linha][coluna][0] :%d\nGAMEPLAY[0][linha][coluna][1]:%d\ncolorActive%d\n",linha,coluna,GAMEPLAY[0][linha][coluna][0],GAMEPLAY[0][linha][coluna][1],colorActive);
				if ((ltemp != ctemp || ((ltemp==ctemp) && ltemp!=0)) && (GAMEPLAY[0][linha][coluna][0]==1) && (GAMEPLAY[0][linha][coluna][1]==colorActive) && visitedBalls[linha][coluna]==0)
				{
					printf("passou teste2");
					checkSurroundingBalls(l+ltemp, c+ctemp);
					
				}

				else
					printf("Verificada bola linha %d e coluna %d e nada.\n",l+ltemp, c+ctemp);
					if ((visitedBalls[l+ltemp][c+ctemp]==0) && GAMEPLAY[0][l+ltemp][c+ctemp][1]!=colorActive )
					{
						visitedBalls[l+ltemp][c+ctemp]=1;
					}
			}
		}
	}

}

// for all surroundig balls, checks if there are some of the same color. If >3 connected, returns true

bool checkLine(int l,int c){
	checkSurroundingBalls(l,c);
	if (sumBalls>=3)
		return true;
	else 
		return false;
}



// adds ball given as argument to toDestroy vector in a empty position - toDestroy[positiont][0]==0.0f

void queueDestruction(int linha,int coluna){
	int positiont=0;

	while ( toDestroy[positiont][0]!=0.0f)
	{positiont++;}

	toDestroy[positiont][0]=1.0f;												//check To Destroy
	toDestroy[positiont][1]=POSITION[0][linha%2][coluna][0];					//x
	toDestroy[positiont][2]=GLfloat (POSITION[0][linha%2][coluna][1] -14.0*(linha/2));	//y
	toDestroy[positiont][3]=0.0f;												//z
	toDestroy[positiont][4]=GLfloat (GAMEPLAY[0][linha][coluna][1]);						//color
	toDestroy[positiont][5]=1.0f;												//possible variation of destruction animation
	GAMEPLAY[0][linha][coluna][1]=0;
	GAMEPLAY[0][linha][coluna][0]=0;

}

// draws and animates balls to be destroyed

void proceedDestruction(void){
	for (int i=0;i < 20;i++){
		if (toDestroy[i][0]==1.0f){

			color = GLint(toDestroy[i][4]);
			destructionOngoing=true;

			destroyCounter++;
			if (destroyCounter<1000 && destroyCounter!=0)
			{
				ambientDestroyer+=1.001f;
				setBallColor();
				move=0;

				//ambientComponent = {1.0f, 1.0f, 1.0f, 1.0f};
			}
			else{

				setBallColor();
				//printf("COLOR %d\n",color);
				toDestroy[i][3]=toDestroy[i][3]+0.1f;
			}
			//
			ambientComponent[0]=1.0f;
			ambientComponent[1]=1.0f;
			ambientComponent[2]=1.0f;
			ambientComponent[3]=1.0f;
			display_at(0,toDestroy[i][1],toDestroy[i][2],toDestroy[i][3], 45.0f,0.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f);
			exploding=true;
			if (ambient >0.1f)
				ambient-=0.004f;
			destructionOngoing=false;
			if (toDestroy[i][3]>=150.0f){
				toDestroy[i][0]=0.0f;
				destroyCounter=1000;}


		}

	}

}

// checks which balls are tagged to destruction and queue them in a new vector where they will be placed until destroyed animation finishes

void destroy(){
	for (int i=0;i<lines;i++){
		for (int j=0;j<column;j++){
			if (toDestroyBalls[i][j]==1)
			{
				queueDestruction(i,j);
				GAMEPLAY[0][i][j][0]=0;
				GAMEPLAY[0][i][j][1]=0; 
				toDestroyBalls[i][j]=0;
				NUMBER_OF_BALLS = NUMBER_OF_BALLS - 1;

			}


		}

	}
}



void find_empty_place(int i,int j){

	int foundX=1,foundY=1;



	if (foundX == 1 && foundY == 1){

		GAMEPLAY[0][i][j][0] = 1;
		NUMBER_OF_BALLS = NUMBER_OF_BALLS + 1;
		printf("\n colisao2  i = %d and j = %d \n",i,j);



		//colorBalls[i][j]=colorActive;
		GAMEPLAY[0][i][j][1] = colorActive;


		if (checkLine(i,j)){
			if (lightIntensity[color]<10.0f)
			{
				lightIntensity[color]+=0.2f;

			}
			 soundballout();
			destroy();

			//teste123

			for (int i=0;i<lines;i++){
				for(int j=0;j<column;j++){
					if(GAMEPLAY[0][i][j][0] == 1){
						if(i==0){
							TEST_SPHERE[0][i][j][0] = 1;
						}
						else{

							if(TEST_SPHERE[0][i-1][j][0] == 1)
								TEST_SPHERE[0][i][j][0] = 1;


							if(j-1>=0 && i%2==1)
								if(TEST_SPHERE[0][i][j-1][0] == 1)
									TEST_SPHERE[0][i][j][0] = 1;

							if(j+1<column){
								if(TEST_SPHERE[0][i-1][j+1][0] == 1)
									TEST_SPHERE[0][i][j][0] = 1;

								if(i%2 == 0 && TEST_SPHERE[0][i][j+1][0] == 1) 
									TEST_SPHERE[0][i][j][0] = 1;
							}									
						}
					}
				}
			}

			for (int i=0;i<lines;i++){
				for(int j=0;j<column;j++){
					if(TEST_SPHERE[0][i][j][0] == 0 && GAMEPLAY[0][i][j][0] == 1){
						GAMEPLAY[0][i][j][0] = 0;
						GAMEPLAY[0][i][j][1] = 0; 

						NUMBER_OF_BALLS = NUMBER_OF_BALLS - 1;

					}
				}
			}

			printf("TEST_SPHERE\n");
			for (int j=0;j<lines;j++){
				if(j%2==1){
					printf(" ");
				}
				for (int k=0;k<column;k++){
					if(TEST_SPHERE[0][j][k][0] != -1){
						printf("%d ",TEST_SPHERE[0][j][k][0]);
					}
				}
				printf("\n");
			}
			for (int i=0;i<lines;i++){
				for(int j=0;j<column;j++){
					TEST_SPHERE[0][i][j][0] = RESET_SPHERE[i][j][0];
				}
			}


			for (int i=0;i<lines;i++)
				for(int j=0;j<column;j++){
					if(GAMEPLAY[0][i][j][0] == 2){
						GAMEPLAY[0][i][j][0] = 0;
					}
				}


				for (int i=0;i<lines;i++)
					for(int j=0;j<column;j++){

						if(i==0 && GAMEPLAY[0][i][j][0] == 0){
							GAMEPLAY[0][i][j][0] = 2;
						}

						if(GAMEPLAY[0][i][j][0] == 1){
							if(i%2 ==0){


								if(j==0)
									if(GAMEPLAY[0][i+1][j+1][0] == 0){
										GAMEPLAY[0][i+1][j+1][0] = 2;

									}

									if(i-1>=0){

										if(GAMEPLAY[0][i-1][j][0] == 0)
											GAMEPLAY[0][i-1][j][0] = 2;

										if(j+1<column)
											if(GAMEPLAY[0][i-1][j+1][0] == 0)
												GAMEPLAY[0][i-1][j+1][0] = 2;
									}

									if(j-1>=0)
										if(GAMEPLAY[0][i][j-1][0] == 0)
											GAMEPLAY[0][i][j-1][0] = 2;

									if(j+1<column)
										if(GAMEPLAY[0][i][j+1][0] == 0)
											GAMEPLAY[0][i][j+1][0] = 2;

									if(i+1<lines){
										if(GAMEPLAY[0][i+1][j][0] == 0)
											GAMEPLAY[0][i+1][j][0] = 2;

										if(j+1<column)
											if(GAMEPLAY[0][i+1][j+1][0] == 0)
												GAMEPLAY[0][i+1][j+1][0] = 2;
									}

							}

							else{
								if(j==column-1)
									if(GAMEPLAY[0][i+1][j-1][0] == 0)
										GAMEPLAY[0][i+1][j-1][0] = 2;

								if(i-1>=0){
									if(GAMEPLAY[0][i-1][j][0] == 0)
										GAMEPLAY[0][i-1][j][0] = 2;

									if(j-1>=0)
										if(GAMEPLAY[0][i-1][j-1][0] == 0)
											GAMEPLAY[0][i-1][j-1][0] = 2;
								}

								if(j-1>=0)
									if(GAMEPLAY[0][i][j-1][0] == 0)
										GAMEPLAY[0][i][j-1][0] = 2;

								if(j+1<column)
									if(GAMEPLAY[0][i][j+1][0] == 0)
										GAMEPLAY[0][i][j+1][0] = 2;

								if(i+1<lines){
									if(GAMEPLAY[0][i+1][j][0] == 0)
										GAMEPLAY[0][i+1][j][0] = 2;

									if(j-1>=0)
										if(GAMEPLAY[0][i+1][j-1][0] == 0)
											GAMEPLAY[0][i+1][j-1][0] = 2;
								}

							}

						}

					}

		}

		else{

			if(i%2 ==0){

				if(j==0)
					if(GAMEPLAY[0][i+1][j+1][0] == 0)
						GAMEPLAY[0][i+1][j+1][0] = 2;


				if(i-1>=0){
					if(GAMEPLAY[0][i-1][j][0] == 0)
						GAMEPLAY[0][i-1][j][0] = 2;

					if(j+1<column)
						if(GAMEPLAY[0][i-1][j+1][0] == 0)
							GAMEPLAY[0][i-1][j+1][0] = 2;
				}

				if(j-1>=0)
					if(GAMEPLAY[0][i][j-1][0] == 0)
						GAMEPLAY[0][i][j-1][0] = 2;

				if(j+1<column)
					if(GAMEPLAY[0][i][j+1][0] == 0)
						GAMEPLAY[0][i][j+1][0] = 2;

				if(i+1<lines){
					if(GAMEPLAY[0][i+1][j][0] == 0)
						GAMEPLAY[0][i+1][j][0] = 2;

					if(j+1<column)
						if(GAMEPLAY[0][i+1][j+1][0] == 0)
							GAMEPLAY[0][i+1][j+1][0] = 2;
				}

			}

			else{
				if(j==column-1)
					if(GAMEPLAY[0][i+1][j-1][0] == 0)
						GAMEPLAY[0][i+1][j-1][0] = 2;

				if(i-1>=0){
					if(GAMEPLAY[0][i-1][j][0] == 0)
						GAMEPLAY[0][i-1][j][0] = 2;

					if(j-1>=0)
						if(GAMEPLAY[0][i-1][j-1][0] == 0)
							GAMEPLAY[0][i-1][j-1][0] = 2;
				}

				if(j-1>=0)
					if(GAMEPLAY[0][i][j-1][0] == 0)
						GAMEPLAY[0][i][j-1][0] = 2;

				if(j+1<column)
					if(GAMEPLAY[0][i][j+1][0] == 0)
						GAMEPLAY[0][i][j+1][0] = 2;

				if(i+1<lines){
					if(GAMEPLAY[0][i+1][j][0] == 0)
						GAMEPLAY[0][i+1][j][0] = 2;

					if(j-1>=0)
						if(GAMEPLAY[0][i+1][j-1][0] == 0)
							GAMEPLAY[0][i+1][j-1][0] = 2;
				}

			}

		}

		sumBalls=0;
		resetVisited();
		colorActive=rand()%3+1;

		for(int j=0;j<7;j++){
			PLAYER[0][j] = PLAYER_ORIGINAL[0][j];
		}
		//ARMINDO
		MOVE_PLAYER[0][1] = MOVE_PLAYER_ORIGINAL[0][1];
		for(int j=0;j<2;j++){
			MOVE_PLAYER_TRANSLATE[0][j] = MOVE_PLAYER_ORIGINAL[0][j];
		}
		move = 0;

		for (int j=0;j<lines;j++){
			if(j%2==1){
				printf(" ");
			}
			for (int k=0;k<column;k++){
				if(GAMEPLAY[0][j][k][0] != -1){
					printf("%d ",GAMEPLAY[0][j][k][0]);
				}
			}
			printf("\n");
		}

		printf("NUMBER_OF_BALLS = %d\n",NUMBER_OF_BALLS);

		if(NUMBER_OF_BALLS == 0){
			restart = 1;
			level = level + 1;

			if (level == max_levels){
				level = 0;
			}

		}


		for(int j=0;j<column;j++){
			if(GAMEPLAY[0][end_line][j][0] == 1){
				end_game = 1;
				break;
			}
		}

		if(end_game==1){
			for(int i=0;i<lines;i++){
				for(int j=0;j<column;j++){
					if(GAMEPLAY[0][i][j][0] == 1){
						GAMEPLAY[0][i][j][1] = 0;
						//color = GAMEPLAY[0][i][j][1];
						//setBallColor();

					}
				}
			}
		}

	}

}
void scaleBalls(){
	if (!scaled)
	{scaled=true;
	for (int i=0;i<lines;i++)
	{if (!scaled) break;
	for(int j=0;j<column;j++){
		if (!scaled) break;
		if(scaleBall[i][j]<=1.0f && scaled){
			scaleBall[i][j]+=0.01f;scaled=false;}
	}
	}

	}
}
void display(void){
	if (!inited) {
		init();
		inited = true;
	}


	if ((cameraPos[0][0]!=0.0f ||cameraPos[0][1]!= 30.0f|| cameraPos[0][2]!= 150.0f) && cameraChange==0)
	{
		if (cameraPos[0][0] > 0.0f) cameraPos[0][0]-=0.1f;
		if (cameraPos[0][0] < 0.0f) cameraPos[0][0]+=0.1f;
		if (cameraPos[0][1] > 30.0f) cameraPos[0][1]-=0.1f;
		if (cameraPos[0][1] < 30.0f) cameraPos[0][1]+=0.1f;
		if (cameraPos[0][2] < 150.0f) cameraPos[0][2]+=0.1f;
		if (cameraPos[0][2] > 150.0f) cameraPos[0][2]-=0.1f;
		if ((abs(cameraPos[0][2]-150.0f)) <0.4f) cameraPos[0][2] = 150.0f;

	}
	if (cameraPos[0][0]==0.0f ||cameraPos[0][1]== 30.0f|| cameraPos[0][2]== 150.0f)
	{
		started=true;
	}

	if (started) scaleBalls();


	if (move==0 && (PLAYER[0][0] == PLAYER_ORIGINAL[0][0]) && (PLAYER[0][1] == PLAYER_ORIGINAL[0][1]) && (angle!=-1.57079633f || angle2!=0.0f) ){

		if (angle> -1.57079633f)
		{angle-=0.0001f;}
		if (angle< -1.57079633f)
		{angle+=0.0001f;}

		if (angle2> 0.0f)
		{angle2-=0.0001f;}
		if (angle2< 0.0f)
		{angle2+=0.0001f;}


	}
	//BALL_MOVEMENT
	//3rd person
	cameraView[1][0]=PLAYER[0][0]+MOVE_PLAYER_TRANSLATE[0][0];
	cameraView[1][1]=PLAYER[0][1]+MOVE_PLAYER_TRANSLATE[0][1];
	cameraPos[1][0]=cameraView[1][0];
	cameraPos[1][1]=cameraView[1][1]-70.0f;
	cameraView[1][1]=cameraView[1][1]+100.0f;
	//1st person
	cameraView[2][0]=PLAYER[0][0]+MOVE_PLAYER_TRANSLATE[0][0];
	cameraView[2][1]=PLAYER[0][1]+MOVE_PLAYER_TRANSLATE[0][1];
	cameraPos[2][0]=cameraView[2][0];

	cameraPos[2][1]=cameraView[2][1]-10.0f;
	cameraView[2][1]=cameraView[2][1]+100.0f;

	//cameraPos[1][2]=PLAYER[0][2];

	if (move == 1){

		// Rui camera
		angle += MOVE_PLAYER_TRANSLATE[0][0]/7000;		//x
		angle2+=MOVE_PLAYER_TRANSLATE[0][1] /10000;	//y


		//end camera

		time=glutGet(GLUT_ELAPSED_TIME);

		if (time - timebase >speed) {
			PLAYER[0][3]-=10.0;
			PLAYER[0][1] += MOVE_PLAYER_TRANSLATE[0][1];
			PLAYER[0][0] += MOVE_PLAYER_TRANSLATE[0][0];
			PLAYER[0][4] = MOVE_PLAYER_TRANSLATE[0][1];


			if(MOVE_PLAYER_TRANSLATE[0][0] == 0.0f){
				PLAYER[0][5] = 0.0f;
			}

			else{
				if(MOVE_PLAYER_TRANSLATE[0][0] > 0.0){
					PLAYER[0][5] = abs(MOVE_PLAYER_TRANSLATE[0][1]) - 1;
				}

				else{
					PLAYER[0][5] = 1- abs(MOVE_PLAYER_TRANSLATE[0][1]) ;
				}
			}

			timebase = time;

			// COLISION_SYSTEM

			if (PLAYER[0][0] < SIDE_BORDER[0][0] || PLAYER[0][0] > SIDE_BORDER[0][1]){
				MOVE_PLAYER_TRANSLATE[0][0] =  MOVE_PLAYER_TRANSLATE[0][0] * -1;
							mciSendString( L"close shot.wav", NULL, 0, 0);
							LPCWSTR c = L"open shot.wav type mpegvideo";
							int error = 99;
							error = mciSendString(c, NULL,0,0);
							int error2;
							LPCWSTR d = L"play shot.wav";
							error2 = mciSendString(d, NULL, 0, 0); 
			}

			if(PLAYER[0][1] >= 140.0 || PLAYER[0][1] <=-20.0 || PLAYER[0][0] >= 60.0 || PLAYER[0][0] <= -60.0){
				move = 0;
				for(int j=0;j<7;j++){
					PLAYER[0][j] = PLAYER_ORIGINAL[0][j];
				}
			}


			//int i=0,j=0, foundX=0,foundY=0;

			int i=lines-1,j=column-1,found = 0,foundX=0,foundY = 0;

			while(true){

				if(i==-1 || j==-1){
					break;
				}

				if(foundX == 1 && foundY == 1 && i>=0 && j>=0){ 
					if(GAMEPLAY[0][i][j][0] == 2){
						found = 1;
						break;
					}
					else{
						break;
					}
				}

				if(foundX==0 && i>=0){
					if(PLAYER[0][1] <= (70.0-i*7.0)+5.25   && PLAYER[0][1] >= (70.0-i*7.0)-5.25){ //5.25
						foundX=1;
					}
					else{
						i = i -1;
					}
				}

				if(foundY==0 && j>=0){
					if(PLAYER[0][0] <= (-28.0+j*8.0)+6.0 && PLAYER[0][0] >= (-28.0+j*8.0)-6.0){ //6.0

						foundY=1;
					}
					else{
						j=j-1;
					}
				}
			}

			//if ((GAMEPLAY[0][i][j][0] == 1 && foundX == 1 && foundY == 1) || (i==0 && foundY == 1 && GAMEPLAY[0][i][j][0] == 2)){
			if(found==1){
				find_empty_place(i,j);
				//move=0;
				printf("\n colisao1  i = %d and j = %d \n",i,j);
				printf("Encontrou colisao");
			}


		}
	}


	GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat black[] = {0.0f, 0.0f, 0.0f, 1.0f};

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clears the display with the defined clear color
	glUseProgram(programId);

	//TEST Rui
	setBallColor();

	if (increment){
		lightDir[0]=lightDir[0]+0.003f;
		blinkaux+=0.0001f;
		//diffused+=0.1f;
	}


	if (lightDir[0]>=1.0)
	{increment=false;}

	if (!increment)
	{		
		lightDir[0]=lightDir[0]-0.003f;
		blinkaux-=0.0001f;
		//	diffused-=0.1f;	
	}

	if (lightDir[0]<=-1.0)
	{increment=true;}


	if (incrementy){
		lightDir[1]=lightDir[1]+0.003f;

	}

	if (lightDir[1]>=1.0)
	{incrementy=false;}

	if (!incrementy)
	{		
		lightDir[1]=lightDir[1]-0.003f;

	}
	if (lightDir[1]<=-1.0)
	{incrementy=true;}

	if (incrementDiffuseBall){
		diffused+=0.0001f;}
	else{diffused-=0.0001f;}

	if (diffused>=1.6f){incrementDiffuseBall=false;}
	if (diffused<=1.0f){incrementDiffuseBall=true;}

	//blinking exploding ball

	if (incrDiffuseExplo){diffusedExplosion+=0.05f;}
	else{diffusedExplosion-=0.05f;}

	if (diffusedExplosion>=2.0f){incrDiffuseExplo=false;}
	if (diffusedExplosion<=0.5f){incrDiffuseExplo=true;}



	// blink

	if (incrementblink){
		blink=blink+0.001f;

		//diffused+=0.1f;
	}


	if (blink>=1.13f)
	{incrementblink=false;}

	if (!incrementblink)
	{		
		blink=blink-0.0003f;

		//	diffused-=0.1f;	
	}

	if (blink<=1.05f)
	{incrementblink=true;}



	// RESTART_THE_GAME_SET
	if (restart == 1){
	
	
		

		end_game = 0;
		NUMBER_OF_BALLS = 0;
		POINTER = 180.0f;

		for (int i=0;i<players;i++){
			for (int j=0;j<lines;j++){
				for (int k=0;k<column;k++){
					for(int l=0;l<2;l++){
						GAMEPLAY[0][j][k][l] = 0;
					}
				}
			}
		}

		for (int i=0;i<players;i++){
			for (int j=0;j<lines;j++){
				for (int k=0;k<column;k++){
					for(int l=0;l<2;l++){

						if((GAMEPLAY[0][j][k][0]== 2 &&  LEVELS[level][j][k][0] == 0) == 0)
							GAMEPLAY[0][j][k][l] = LEVELS[level][j][k][l];

					}

					if(j==0 && GAMEPLAY[0][j][k][0] == 0){
						GAMEPLAY[0][j][k][0] = 2;
					}

					if(GAMEPLAY[0][j][k][0] == 1){

						NUMBER_OF_BALLS = NUMBER_OF_BALLS + 1;


						if(j%2 ==0){
							if(k==0)
								if(GAMEPLAY[i][j+1][k+1][0] == 0){
									GAMEPLAY[i][j+1][k+1][0] = 2;

								}

								if(j-1>=0){

									if(GAMEPLAY[i][j-1][k][0] == 0)
										GAMEPLAY[i][j-1][k][0] = 2;

									if(k+1<column)
										if(GAMEPLAY[i][j-1][k+1][0] == 0)
											GAMEPLAY[i][j-1][k+1][0] = 2;
								}

								if(k-1>=0)
									if(GAMEPLAY[i][j][k-1][0] == 0)
										GAMEPLAY[i][j][k-1][0] = 2;

								if(k+1<column)
									if(GAMEPLAY[i][j][k+1][0] == 0)
										GAMEPLAY[i][j][k+1][0] = 2;

								if(j+1<lines){
									if(GAMEPLAY[i][j+1][k][0] == 0)
										GAMEPLAY[i][j+1][k][0] = 2;

									if(k+1<column)
										if(GAMEPLAY[i][j+1][k+1][0] == 0)
											GAMEPLAY[i][j+1][k+1][0] = 2;
								}
						}




						else{
							if(k==column-1)
								if(GAMEPLAY[i][j+1][k-1][0] == 0)
									GAMEPLAY[i][j+1][k-1][0] = 2;

							if(j-1>=0){
								if(GAMEPLAY[i][j-1][k][0] == 0)
									GAMEPLAY[i][j-1][k][0] = 2;

								if(k-1>=0)
									if(GAMEPLAY[i][j-1][k-1][0] == 0)
										GAMEPLAY[i][j-1][k-1][0] = 2;
							}

							if(k-1>=0)
								if(GAMEPLAY[i][j][k-1][0] == 0)
									GAMEPLAY[i][j][k-1][0] = 2;

							if(k+1<column)
								if(GAMEPLAY[i][j][k+1][0] == 0)
									GAMEPLAY[i][j][k+1][0] = 2;

							if(j+1<lines){
								if(GAMEPLAY[i][j+1][k][0] == 0)
									GAMEPLAY[i][j+1][k][0] = 2;

								if(k-1>=0)
									if(GAMEPLAY[i][j+1][k-1][0] == 0)
										GAMEPLAY[i][j+1][k-1][0] = 2;
							}
						}


					}
				}




			}

			for(int j=0;j<7;j++){
				PLAYER[i][j] = PLAYER_ORIGINAL[i][j];
			}
			for(int j=0;j<2;j++){
				MOVE_PLAYER[i][j] = MOVE_PLAYER_ORIGINAL[i][j];
				MOVE_PLAYER_TRANSLATE[i][j] = MOVE_PLAYER_ORIGINAL[i][j];
			}

		}

		move = 0;
		restart = 0;

		for (int j=0;j<lines;j++){
			if(j%2==1){
				printf(" ");
			}
			for (int k=0;k<column;k++){
				if(GAMEPLAY[0][j][k][0] != -1)
					printf("%d ",GAMEPLAY[0][j][k][0]);
			}
			printf("\n");
		}
			scaled=false;
	
		for  (int i=0;i<lines;i++ )
			for  (int j=0;j<column;j++ )
			scaleBall[i][j]=0.0f;
	}

	//if (cameraMode!=1)

	//RUI
	color=colorActive;
	setBallColor();
	//END RUI 



	if (galaxyON)
		drawParticles();


	display_at(0, PLAYER[0][0], PLAYER[0][1], PLAYER[0][2],PLAYER[0][3],PLAYER[0][4], PLAYER[0][5], PLAYER[0][6],blink,blink,blink);
	for(int i=0;i<players;i++){
		for(int j=0;j<lines;j++){
			for(int k=0;k<column;k++){
				if((j%2 == 1 && k ==0) == 0 && GAMEPLAY[i][j][k][0] == 1){
					//RUI
					//color=colorBalls[j][k];
					color = GAMEPLAY[0][j][k][1];
					setBallColor();
					//
					actualfragment=1000;
					display_at(0,POSITION[0][j%2][k][0], POSITION[0][j%2][k][1] -14.0f*(j/2), 0.0f, 45.0f,0.0f, 1.0f, 0.0f,scaleBall[i][j],scaleBall[i][j],scaleBall[i][j]);

				}
			}
		}
	}
	actualfragment=1000;
	exploding=false;
	
	proceedDestruction();
	color = 0;
	setBallColor();

	display_at(6, 0.0f,-10.0f, 0.0f,POINTER,0.0f, 0.0f, 1.0f,7.0f,7.0f,7.0f);

	cellshading=1;
	display_at(6, 0.0f,-10.0f, 0.0f,POINTER,0.0f, 0.0f, 1.0f,7.5f+blinkaux,7.5f+blinkaux,7.5f+blinkaux);
	cellshading=0;

	//RUI barra cima
	display_at(1, -35.0f, 75.0f, 0.0f,0.0f,0.0f, -1.0f, 1.0f,70.0f,2.0f,2.0f);

	//RUI barra esq
	display_at(1, -35.0f, -10.0f, 0.0f,0.0f,0.0f, -1.0f, 1.0f,2.0f,85.0f,2.0f);
	//RUI barra dir
	display_at(1, 33.0f, -10.0f, 0.0f,0.0f,0.0f, -1.0f, 1.0f,2.0f,87.0f,2.0f);



	//dispx+=0.00035f;
	//RUI - GLOBE
	display_at(2, dispx, -dispx, -100.0f, 1.0f,1.0f, 1.0f, 1.0f,100.0f,100.0f,100.0f);
	//END 
	if (cameraMode==0)
	display_at(3, cameraPos[0][0]-5.0f,cameraPos[0][1]-4.0f,cameraPos[0][2]-10.0f,0.0f,0.0f, 1.0f, 1.0f,2.0f,2.0f,0.0f);

	//ship
	//shipControl();
	if (!pauseship)
	Bezier();

	ship[4]=lightDir[0];
	display_at(4, ship[0], ship[1],ship[2],ship[3],ship[4],ship[4],ship[6],1.5f,1.5f,1.5f);

	cameraPos[3][0]=ship[0];
	cameraPos[3][1]=ship[1];
	cameraPos[3][2]=ship[2]-10.0f;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0); //4.
	glUseProgram(0); //4.

	glFlush(); //Instructes OpenGL to finish all rendering operations
	glutSwapBuffers(); //Swaps the display in a GLfloat buffering scenario. In GLfloat buffering, rendering is done in a offline buffer (not directly on the screen); this avoid flickering 
	checkError ("display");
}

/*
[FREEGLUT]
This function is called whenever the user changes the size of the display window;

The size of the OpenGL's viewport is mapped to the same size of the window.
*/

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);

	/* 
	This sets the perspective matrix. There's no need to compute this matrix at each frame. Only when the window changes size
	*/

	float aspect = (float)w/(float)h; //aspect ratio.
	
	perspectiveMatrix = glm::perspective(45.0f, aspect, 0.001f, 1000.0f); //field of view; aspect ratio; z near; z far;

	checkError ("reshape");
}




/*
[FREEGLUT]
Keyboard handling function
*/

void keyboard(unsigned char key, int x, int y)
{if (!exploding && started)
switch (key) {
case 27:
	exit(0);
	break;

case 'c':

	if (cameraMode == 0 ||cameraMode==4 ){
		//move=1;
		cameraChange=0;
		cameraMode +=1;
		LPCWSTR b = L"pause star.mp3";
		mciSendString(b, NULL, 0, 0); 
	}



	else{
		cameraPosCameraChanger[0]=cameraPos[cameraMode][0];
		cameraPosCameraChanger[1]=cameraPos[cameraMode][1];
		cameraPosCameraChanger[2]=cameraPos[cameraMode][2];

		cameraViewCameraChanger[0]=cameraView[cameraMode][0];
		cameraViewCameraChanger[1]=cameraView[cameraMode][1];
		cameraViewCameraChanger[2]=cameraView[cameraMode][2];

		cameraUpCameraChanger[0]=cameraUp[cameraMode][0];
		cameraUpCameraChanger[1]=cameraUp[cameraMode][1];
		cameraUpCameraChanger[2]=cameraUp[cameraMode][2];


		if (move==0){
			cameraChange=1;
		}
		else {
			cameraChange=2;
		}

		move=0;
		cameraMode +=1;


		if(cameraMode == 5) {
			cameraMode = 0;
		}
		cameraPosRatio[0]=(abs(cameraPosCameraChanger[0]-cameraPos[cameraMode][0]))/200;
		cameraPosRatio[1]=(abs(cameraPosCameraChanger[1]-cameraPos[cameraMode][1]))/200;
		cameraPosRatio[2]=(abs(cameraPosCameraChanger[2]-cameraPos[cameraMode][2]))/200;

		cameraViewRatio[0]=(abs(cameraViewCameraChanger[0]-cameraView[cameraMode][0]))/200;
		cameraViewRatio[1]=(abs(cameraViewCameraChanger[1]-cameraView[cameraMode][1]))/200;
		cameraViewRatio[2]=(abs(cameraViewCameraChanger[2]-cameraView[cameraMode][2]))/200;

		cameraUpRatio[0]=(abs(cameraUpCameraChanger[0]-cameraUp[cameraMode][0]))/200;
		cameraUpRatio[1]=(abs(cameraUpCameraChanger[1]-cameraUp[cameraMode][1]))/200;
		cameraUpRatio[2]=(abs(cameraUpCameraChanger[2]-cameraUp[cameraMode][2]))/200;

		//dynamic camera change only happens from 3rd to 1st person
	}

	if(cameraMode == 5) {
		cameraMode = 0;
	}
	if (cameraMode==3)
		pauseship=true;
		if (cameraMode!=1 && cameraMode!=0  )
	soundcamera();

	if (cameraMode==4 ||cameraMode==3){

	int error2;
	LPCWSTR b = L"close star.mp3 ";
	error2 = mciSendString(b, NULL, 0, 0);	
	b = L"open star.mp3 ";
	error2 = mciSendString(b, NULL, 0, 0);

	b = L"play star.mp3 repeat";
	error2 = mciSendString(b, NULL, 0, 0); }

	break;
	case 'a':
	cameraPos[cameraMode].x += sin(angle) * velocity;
	cameraPos[cameraMode].z -= cos(angle) * velocity;
	break;
case 'd':
	cameraPos[cameraMode].x -= sin(angle) * velocity;
	cameraPos[cameraMode].z += cos(angle) * velocity;
	break;
case 'w':
	cameraPos[cameraMode].y += velocity;
	break;
case 's':
	cameraPos[cameraMode].y -= velocity;

	break;

case 'x':
	angle -= velocity;
	break;
case 'v':
	angle += velocity;
	break;
case 'q':
	cameraPos[cameraMode].x += cos(angle) * velocity;
	cameraPos[cameraMode].z += sin(angle) * velocity;
	break;
case 'z':
	cameraPos[cameraMode].x -= cos(angle) * velocity;
	cameraPos[cameraMode].z -= sin(angle) * velocity;
	break;
	/*
	case 'u':
	t++;
	break;
	*/
case 'm':
	restart = 1;

	level = 0;

	/*
	level = level + 1;

	if (level == max_levels){
	level = 0;
	}
	*/
	break;
case SPACEBAR:
	if(end_game==0){
		if (move == 0){
			
			mciSendString( L"close shot.wav", NULL, 0, 0);
				LPCWSTR c = L"open shot.wav type mpegvideo";
int error = 99;
error = mciSendString(c, NULL,0,0);
int error2;
LPCWSTR d = L"play shot.wav";
error2 = mciSendString(d, NULL, 0, 0); 

			move = 1;
			MOVE_PLAYER_TRANSLATE[0][0] = MOVE_PLAYER[0][0];
			MOVE_PLAYER_TRANSLATE[0][1] = MOVE_PLAYER[0][1];
			PLAYER[0][0] += MOVE_PLAYER_TRANSLATE[0][0];
			PLAYER[0][1] += MOVE_PLAYER_TRANSLATE[0][0];

			timebase=glutGet(GLUT_ELAPSED_TIME);
		}
		else{
			move = 0;
		}
	}
	break;
	case 'o':
	if (particlesState==0)
	{particlesState=1;}
	else
		particlesState=0;
	printf("%d.",particlesState);
	particleShift=true;
	transitionTime=0;
	//populateParticlesShiftDistance=true;
	break;
	/*Rui test colors*/
case '1':
	color=1;
	break;
case '2':
	color=2;
	break;
case '3':
	color=3;
	break;
	/*END Rui test colors*/
	//particle system ON/OFF
case 'p':
	galaxyON=!galaxyON;
	break;
}
}



/*
[FREEGLUT]
Special keys handling function (for "normal" keys, use the previously defined function)

This function not only handles the special keys, but also controls the camera's position and rotation angle based on the input.	
*/


void keyboardSpecialKeys(int key, int x, int y)
{
	if (!exploding)
		switch (key) {
		case GLUT_KEY_LEFT:
			//angle -= velocity;
			//PLAYER[0][0]-=1;
			if(end_game == 0){
				if (MOVE_PLAYER[0][0] >= -0.9f){
					MOVE_PLAYER[0][0] -= 0.01f;
					MOVE_PLAYER[0][1] = 1 - abs(MOVE_PLAYER[0][0]);
					POINTER +=1.0f;
				}
			}
			break;

		case GLUT_KEY_RIGHT:
			//angle += velocity;
			//PLAYER[0][0]+=1;
			if(end_game == 0){
				if (MOVE_PLAYER[0][0] <= 0.9f){
					MOVE_PLAYER[0][0] += 0.01f;
					MOVE_PLAYER[0][1] = 1 - abs(MOVE_PLAYER[0][0]);
					POINTER -=1.0f;
				}
			}
			break;
		case GLUT_KEY_UP:
			//cameraPos.x += cos(angle) * velocity;
			//cameraPos.z += sin(angle) * velocity;
			/*
			PLAYER[0][3]-=10.0;
			PLAYER[0][6] = 1.0f;
			PLAYER[0][4] = 1.0f;
			PLAYER[0][5] = 0.0f;
			PLAYER[0][1]+=1;
			*/

			if (MOVE_PLAYER_TRANSLATE[0][0] >= -0.9f){
				MOVE_PLAYER_TRANSLATE[0][0] -= 0.1f;
				MOVE_PLAYER_TRANSLATE[0][1] = 1 - abs(MOVE_PLAYER_TRANSLATE[0][0]);
			}
			break;
		case GLUT_KEY_DOWN:
			//cameraPos.x -= cos(angle) * velocity;
			//cameraPos.z -= sin(angle) * velocity;
			/*
			PLAYER[0][3]+=10.0;
			PLAYER[0][6] = 1.0f;
			PLAYER[0][4] = 1.0f;
			PLAYER[0][5] = 0.0f;
			PLAYER[0][1]-=1;
			*/
			if (MOVE_PLAYER_TRANSLATE[0][0] <= 0.9f){
				MOVE_PLAYER_TRANSLATE[0][0] += 0.1f;
				MOVE_PLAYER_TRANSLATE[0][1] = 1 - abs(MOVE_PLAYER_TRANSLATE[0][0]);
			}
			break;
	}
}

void ChangePos(void){

	if (cameraPosCameraChanger[0]>cameraPos[cameraMode][0]){cameraPosCameraChanger[0]=cameraPosCameraChanger[0]-cameraPosRatio[0];}
	if (cameraPosCameraChanger[0]<cameraPos[cameraMode][0]){cameraPosCameraChanger[0]=cameraPosCameraChanger[0]+cameraPosRatio[0];}

	if (cameraPosCameraChanger[1]>cameraPos[cameraMode][1]){cameraPosCameraChanger[1]=cameraPosCameraChanger[1]-cameraPosRatio[1];}
	if (cameraPosCameraChanger[1]<cameraPos[cameraMode][1]){cameraPosCameraChanger[1]=cameraPosCameraChanger[1]+cameraPosRatio[1];}

	if (cameraPosCameraChanger[2]>cameraPos[cameraMode][2]){cameraPosCameraChanger[2]=cameraPosCameraChanger[2]-cameraPosRatio[2];}
	if (cameraPosCameraChanger[2]<cameraPos[cameraMode][2]){cameraPosCameraChanger[2]=cameraPosCameraChanger[2]+cameraPosRatio[2];}



	if (cameraViewCameraChanger[0]>cameraView[cameraMode][0]){cameraViewCameraChanger[0]=cameraViewCameraChanger[0]-cameraViewRatio[0];}
	if (cameraViewCameraChanger[0]<cameraView[cameraMode][0]){cameraViewCameraChanger[0]=cameraViewCameraChanger[0]+cameraViewRatio[0];}

	if (cameraViewCameraChanger[1]>cameraView[cameraMode][1]){cameraViewCameraChanger[1]=cameraViewCameraChanger[1]-cameraViewRatio[1];}
	if (cameraViewCameraChanger[1]<cameraView[cameraMode][1]){cameraViewCameraChanger[1]=cameraViewCameraChanger[1]+cameraViewRatio[1];}

	if (cameraViewCameraChanger[2]>cameraView[cameraMode][2]){cameraViewCameraChanger[2]=cameraViewCameraChanger[2]-cameraViewRatio[2];}
	if (cameraViewCameraChanger[2]<cameraView[cameraMode][2]){cameraViewCameraChanger[2]=cameraViewCameraChanger[2]+cameraViewRatio[2];}



	if (cameraUpCameraChanger[0]>cameraUp[cameraMode][0]){cameraUpCameraChanger[0]=cameraUpCameraChanger[0]-cameraUpRatio[0];}
	if (cameraUpCameraChanger[0]<cameraUp[cameraMode][0]){cameraUpCameraChanger[0]=cameraUpCameraChanger[0]+cameraUpRatio[0];}

	if (cameraUpCameraChanger[1]>cameraUp[cameraMode][1]){cameraUpCameraChanger[1]=cameraUpCameraChanger[1]-cameraUpRatio[1];}
	if (cameraUpCameraChanger[1]<cameraUp[cameraMode][1]){cameraUpCameraChanger[1]=cameraUpCameraChanger[1]+cameraUpRatio[1];}

	if (cameraUpCameraChanger[2]>cameraUp[cameraMode][2]){cameraUpCameraChanger[2]=cameraUpCameraChanger[2]-cameraUpRatio[2];}
	if (cameraUpCameraChanger[2]<cameraUp[cameraMode][2]){cameraUpCameraChanger[2]=cameraUpCameraChanger[2]+cameraUpRatio[2];}

}

void ChangeView(void){
}
void ChangeUp(void){
}

void setupCamera(void)
{
	// reajusts ambient light after line done 		
	if (ambient<1.0f)
	{	ambient+=0.01f;
	ambientComponent[0]=ambient;
	ambientComponent[1]=ambient;
	ambientComponent[2]=ambient;
	}




	cameraView[3][0]=PLAYER[0][0];
	cameraView[3][1]=PLAYER[0][1];
	cameraView[3][2]=0.0f;



	//Define the view direction based on the camera's position
	cameraView[0].x = cameraPos[0].x + cos(angle);

	// Rui edit
	cameraView[0].y = cameraPos[0].y + sin(angle2);
	//

	cameraView[0].z = cameraPos[0].z + sin(angle);



	//Creates the view matrix based on the position, the view direction and the up vector

	//tests if a change to the camera is being made. If so, draws what is on the aux camera. 
	if (cameraChange !=0 )
	{
		//tests if there is a significative distance of the camera that is animating the change and the destiny camera coordinates (for now only Postition)


		/*if (cameraMode==0){


		if (((abs(cameraUpCameraChanger[0]-cameraUp[cameraMode][0]))>=cameraUpRatio[0]) && ((abs(cameraUpCameraChanger[1]-cameraUp[cameraMode][1]))>=cameraUpRatio[1]) &&((abs(cameraUpCameraChanger[2]-cameraUp[cameraMode][2]))>=cameraUpRatio[2]))
		{ChangeUp();}
		else {
		if (((abs(cameraViewCameraChanger[0]-cameraView[cameraMode][0]))>=cameraViewRatio[0]) && ((abs(cameraViewCameraChanger[1]-cameraView[cameraMode][1]))>=cameraViewRatio[1]) &&((abs(cameraViewCameraChanger[2]-cameraView[cameraMode][2]))>=cameraViewRatio[2]))
		{ChangeView();}

		else
		{	if ( ((abs(cameraPosCameraChanger[0]-cameraPos[cameraMode][0]))>=cameraPosRatio[0]) && ((abs(cameraPosCameraChanger[2]-cameraPos[cameraMode][2]))>=cameraPosRatio[2]) && ((abs(cameraPosCameraChanger[1]-cameraPos[cameraMode][1]))>=cameraPosRatio[1]))
		{
		ChangePos();}

		else { 
		if (cameraChange==1)
		move=0;

		if (cameraChange==2)
		move=1;
		cameraChange=0;
		cameraPosCameraChanger=cameraPos[cameraMode];
		}
		}}}
		else{*/


		if ( ((abs(cameraPosCameraChanger[0]-cameraPos[cameraMode][0]))>cameraPosRatio[0]) || ((abs(cameraPosCameraChanger[2]-cameraPos[cameraMode][2]))>cameraPosRatio[2]) || ((abs(cameraPosCameraChanger[1]-cameraPos[cameraMode][1]))>cameraPosRatio[1]) 
			|| abs(cameraViewCameraChanger[0]-cameraView[cameraMode][0])>cameraViewRatio[0] || ((abs(cameraViewCameraChanger[1]-cameraView[cameraMode][1]))>cameraViewRatio[1]) ||((abs(cameraViewCameraChanger[2]-cameraView[cameraMode][2]))>cameraViewRatio[2]) || ((abs(cameraUpCameraChanger[0]-cameraUp[cameraMode][0]))>cameraUpRatio[0]) || ((abs(cameraUpCameraChanger[1]-cameraUp[cameraMode][1]))>cameraUpRatio[1]) || ((abs(cameraUpCameraChanger[2]-cameraUp[cameraMode][2]))>cameraUpRatio[2]) )
		{
			ChangePos();

			ChangeUp();
			ChangeView();
		}
		else { 
			if (cameraChange==1)
				move=0;

			if (cameraChange==2)
				move=1;
			cameraChange=0;
			cameraPosCameraChanger=cameraPos[cameraMode];
			if (cameraMode==3)
				pauseship=false;
		}
		//}		
	}
	//printf("camerachange %d.",cameraChange);
	//	cameraView[4][0]=PLAYER[0][0];
	//	cameraView[4][1]=PLAYER[0][1]+30.0f;
	//Define the view direction based on the camera's position

	//if (cameraChange==0){
	if (cameraChange==0){
		cameraMatrix = glm::lookAt(cameraPos[cameraMode],
			cameraView[cameraMode],
			cameraUp[cameraMode]);}

	else {
		cameraMatrix = glm::lookAt(cameraPosCameraChanger,
			cameraViewCameraChanger,
			cameraUpCameraChanger);}

}

/* 
This is a function to be issued on every cycle.

It performs all that is needed for an interactive simulation.
*/

void mainLoop(void)
{
	setupCamera();
	display();
}
int main(int argc, char** argv)
{

	LPCWSTR a = L"open po.mp3 type mpegvideo";
int error = 99;
error = mciSendString(a, NULL,0,0);
int error2;
LPCWSTR b = L"play po.mp3 repeat";
error2 = mciSendString(b, NULL, 0, 0); 

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); //GLfloat buffer; Color display

	glutInitContextVersion(3, 3); //OpenGL 3.3
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG); //Forward compatible profile (Old Opengl (pre-3.x) functions can be used, but SHOULDN'T) 

	glutInitWindowSize(800, 600); 
	glutCreateWindow(argv[0]);

	dumpInfo();

	/*
	Sets the specific functions
	*/
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboardSpecialKeys);
	glutIdleFunc(mainLoop);

	checkError ("main");
	glutMainLoop(); //starts processing


	return 0;
}


