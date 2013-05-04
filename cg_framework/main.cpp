#include <Gl/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <FreeImage.h>
#include <iostream>

#include "display.h"
#include "main.h"

using namespace std;

#define SPACEBAR 32

bool inited = false; //Have we done initialization?

/*
	vao: will hold the VAO identifier (usually one per object)
	geomId: will hold the VBO identifier (one per attribute: position, normal, etc.)
*/

const int NUMBER_OBJECTS = 8;

OBJLoader object[objloader] = {("../models/esfera1.obj"),("../models/textured_cube.obj")};
GLuint vertexShaderId;
GLuint fragShaderId;
GLuint programId;

GLuint baseTextureId;
GLuint specularTextureId;


GLuint vao[objloader];
GLuint geomId;
GLuint normalsId;
GLuint texUVId;

glm::mat4 perspectiveMatrix;
glm::mat4 cameraMatrix;

glm::vec3 cameraPos(0.0f, 30.0f, 120.0f);
glm::vec3 cameraView(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

float angle = -1.57079633f;

const float velocity = 0.25f;

int cameraMode = 0;
int moveout = 0;
int frame=0,time,timebase=0;
int speed = 100;
int restart = 1;
int levels = 0;
int players = 1;

int lines = 8;
int rows = 8;

GLfloat lightDir[] = {1.0f, 1.0f, 1.0};
GLfloat lightIntensity[] = {0.9f, 0.9f, 0.9f, 1.0f};

GLfloat ambientComponent[] = {0.4f, 0.4f, 0.4f, 1.0f};
GLfloat diffuseColor[] = {1.0f, 1.0f, 1.0f, 1.0f};

//GLfloat object_trans[] = {0.0f, -10.0f, 0.0f}; 
//GLfloat object_rotate[] = {1.0f, 0.0f, 0.0f};
//float object_angle = 0.0f;
GLfloat PLAYER_ORIGINAL[1][7] = {0.0f, -10.0f, 0.0f,0.0f,1.0f, 0.0f, 0.0f};
GLfloat PLAYER[1][7] = {0.0f, -10.0f, 0.0f,0.0f,1.0f, 0.0f, 0.0f};
GLfloat MOVE_PLAYER_ORIGINAL[1][2] = {0.0f,1.0f};
GLfloat MOVE_PLAYER_TRANSLATE[1][2] = {0.0f,1.0f};
GLfloat MOVE_PLAYER[1][2] = {0.0f,1.0f};

// COLORS {Red,Green,Blue,Yellow}
GLfloat COLORS[4][3] = {
	{1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,0.0f,1.0f},{1.0f,1.0f,0.0f}
};

GLint LEVELS[2][8][8][2] = {
	//LEVEL 0
{{
	{0,' '},{0,' '},{0,' '},{1,' '},{0,' '},{0,' '},{0,' '},{0,' '}
},{
	{-1,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
},{
	{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
},{
	{-1,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
},{
	{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
},{
	{-1,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
},{
	{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
},{
	{-1,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
}},
	//LEVEL 1
{{
	{1,'r'},{1,'r'},{1,'y'},{1,'y'},{1,'b'},{1,'b'},{1,'g'},{1,'g'} 
},{
	{-1,' '},{1,'r'},{1,'r'},{1,'y'},{1,'y'},{1,'b'},{1,'b'},{1,'g'}
},{
	{1,'b'},{1,'b'},{1,'g'},{1,'g'},{1,'r'},{1,'r'},{1,'y'},{1,'y'}
},{
	{-1,' '},{1,'b'},{1,'b'},{1,'g'},{1,'g'},{1,'r'},{1,'r'},{1,'y'}
},{
	{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
},{
	{-1,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
},{
	{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
},{
	{-1,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
}}
};

GLint GAMEPLAY[1][8][8][2] = {{{
	{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
},{
	{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
},{
	{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
},{
	{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
},{
	{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
},{
	{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
},{
	{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
},{
	{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '},{0,' '}
}
}};

GLfloat POSITION[1][2][8][2] = {{{
	{-28.0f,70.0f},{-20.0f,70.0f},{-12.0f,70.0f},{-4.0f,70.0f},{4.0f,70.0f},{12.0f,70.0f},{20.0f,70.0f},{28.0f,70.0f}
},{
	{0.0f,0.0f},{-24.0f,63.0f},{-16.0f,63.0f},{-8.0f,63.0f},{0.0f,63.0f},{8.0f,63.0f},{16.0f,63.0f},{24.0f,63.0f}
}}};

//OBJLoader object("../models/haywagon/haywagon_tris.obj");
//OBJLoader object1("../models/esfera1.obj");

GLfloat SIDE_BORDER[1][2] = {{POSITION[0][0][0][0],POSITION[0][0][7][0]}};

//GLfloat TOP_BORDER = POSITION[0][0][0][1];
//GLfloat BALL_BORDER[1] = {};

/* 
	Error checking function:

	It checks for error in the OpenGL pipeline;
*/



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

void init(void) 
{
	
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

	baseTextureId = loadTexture("../models/textures/base.dds");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Defines the clear color, i.e., the color used to wipe the display
	glEnable(GL_DEPTH_TEST);
	checkError("init");
}


/*
	[FREEGLUT]
	
	display function;
	This function is called to paint the display. This is where the drawing functions should be placed.
	Why is the init inside display? Because some drivers require that the display window to be visible to work correctly; therefore, the initialization is done once, but on the first call to 
	display, ensuring that the window is visible;
*/

//float t = 0;

void display(void)
{
	if (!inited) {
		init();
		inited = true;
	}

	//BALL_MOVEMENT
	if (moveout == 1){
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
			}

			
			int i=0,j=0, foundX=0,foundY=0;
		
			while (true){

				if ((foundX == 1 && foundY == 1) || (i==8 && j == 8)){
					break;
				}
				
				if (PLAYER[0][0] <= (-28.0+j*8.0)+4.0 && PLAYER[0][0] >= (-28.0+j*8.0)-4.0){ //8
					foundY = 1;
				}
				else{
					j = j+1;
				}
				
				if (PLAYER[0][1] <= (70.0-i*7.0)+3.5   && PLAYER[0][1] >= (70.0-i*7.0)-3.5){ //7
					foundX = 1;
				}
				else{
					i = i+1;
				}

			}
			
			if (GAMEPLAY[0][i][j][0] == 2 && foundX == 1 && foundY == 1){
				
				GAMEPLAY[0][i][j][0] = 1;

				if(j-1>=0)
					if(GAMEPLAY[0][i][j-1][0] == 0)
						GAMEPLAY[0][i][j-1][0] = 2;

				if(j+1<8)
					if(GAMEPLAY[0][i][j+1][0] == 0)
						GAMEPLAY[0][i][j+1][0] = 2;
							
				if(i+1<lines){
					if(GAMEPLAY[0][i+1][j][0] == 0)
						GAMEPLAY[0][i+1][j][0] = 2;

					if(j+1<8)
						if(GAMEPLAY[0][i+1][j+1][0] == 0)
							GAMEPLAY[0][i+1][j+1][0] = 2;
				}
				
				for(int j=0;j<7;j++){
					PLAYER[0][j] = PLAYER_ORIGINAL[0][j];
				}
				MOVE_PLAYER[0][3] = MOVE_PLAYER_ORIGINAL[0][3];
				for(int j=0;j<2;j++){
					MOVE_PLAYER_TRANSLATE[0][j] = MOVE_PLAYER_ORIGINAL[0][j];
				}
				moveout = 0;

				for (int j=0;j<8;j++){
					for (int k=0;k<8;k++){
						if(GAMEPLAY[0][j][k][0] != -1)
							printf("%d ",GAMEPLAY[0][j][k][0]);
					}
				printf("\n");
				}

			}

			/*
			if(PLAYER[0][1] > TOP_BORDER){
				float interval = abs(POSITION[0][0][0][0] - PLAYER[0][0]);
				int j = 0;
				move = 0;

				for (int i=1;i<8;i++){
					if(((abs(POSITION[0][0][i][0] - PLAYER[0][0])) <= interval) && (GAMEPLAY[0][0][i][0] == 0)){
						j = i;
						interval = abs(POSITION[0][0][i][0] - PLAYER[0][0]);
					}
				}
				GAMEPLAY[0][0][j][0] = 1;
				for(int j=0;j<7;j++){
					PLAYER[0][j] = PLAYER_ORIGINAL[0][j];
				}
				MOVE_PLAYER[0][3] = MOVE_PLAYER_ORIGINAL[0][3];
				for(int j=0;j<2;j++){
					MOVE_PLAYER_TRANSLATE[0][j] = MOVE_PLAYER_ORIGINAL[0][j];
				}
			}
			*/
		}
	}
	

	GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat black[] = {0.0f, 0.0f, 0.0f, 1.0f};
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clears the display with the defined clear color
	glUseProgram(programId);


	// RESTART_THE_GAME_SET
	if (restart == 1){
		for (int i=0;i<players;i++){
			for (int j=0;j<8;j++){
				for (int k=0;k<8;k++){
					for(int l=0;l<2;l++){

						if((GAMEPLAY[0][j][k][0]== 2 &&  LEVELS[levels][j][k][0] == 0) == 0)
							GAMEPLAY[0][j][k][l] = LEVELS[levels][j][k][l];

					}
					
					if(j==0 && GAMEPLAY[0][j][k][0] == 0){
							GAMEPLAY[0][j][k][0] = 2;
					}

					if(GAMEPLAY[0][j][k][0] == 1){
						
						if(k-1>=0)
							if(GAMEPLAY[0][j][k-1][0] == 0)
								GAMEPLAY[0][j][k-1][0] = 2;
								

						if(k+1<8)
							if(GAMEPLAY[0][j][k+1][0] == 0){
								GAMEPLAY[0][j][k+1][0] = 2;
							}
						if(j+1<lines){
							if(GAMEPLAY[0][j+1][k][0] == 0){
								GAMEPLAY[0][j+1][k][0] = 2;
							}
							if(k-1>=0){
								if(GAMEPLAY[0][j+1][k-1][0] == 0){
									GAMEPLAY[0][j+1][k-1][0] = 2;

								}
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

		moveout = 0;
		restart = 0;

		for (int j=0;j<8;j++){
			for (int k=0;k<8;k++){
				if(GAMEPLAY[0][j][k][0] != -1)
					printf("%d ",GAMEPLAY[0][j][k][0]);
			}
			printf("\n");
		}
	}




	display_at(0, PLAYER[0][0], PLAYER[0][1], PLAYER[0][2],PLAYER[0][3],PLAYER[0][4], PLAYER[0][5], PLAYER[0][6],1.0f,1.0f,1.0f);
	
	for(int i=0;i<players;i++){
		for(int j=0;j<8;j++){
			for(int k=0;k<8;k++){
				if((j%2 == 1 && k ==0) == 0 && GAMEPLAY[i][j][k][0] == 1){
					display_at(0,POSITION[0][j%2][k][0], POSITION[0][j%2][k][1] -14.0*(j/2), 0.0f, 45.0f,0.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f);
				}
			}
		}
	}

	display_at(1, -15.0f, -15.0f, 0.0f,45.0f,0.0f, 1.0f, 0.0f,20.0f,3.0f,1.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0); //4.
	glUseProgram(0); //4.
	
	glFlush(); //Instructes OpenGL to finish all rendering operations
	glutSwapBuffers(); //Swaps the display in a double buffering scenario. In double buffering, rendering is done in a offline buffer (not directly on the screen); this avoid flickering 
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
{
	switch (key) {
		case 27:
			exit(0);
			break;
		case 'c':
			cameraMode +=1;
			if(cameraMode == 2) {
				cameraMode = 0;
			}
			break;
		case 'a':
			cameraPos.x += sin(angle) * velocity;
			cameraPos.z -= cos(angle) * velocity;
			break;
		case 'd':
			cameraPos.x -= sin(angle) * velocity;
			cameraPos.z += cos(angle) * velocity;
			break;
		case 'w':
			cameraPos.y += velocity;
			break;
		case 's':
			cameraPos.y -= velocity;
			break;

		case 'x':
			angle -= velocity;
			break;
		case 'v':
			angle += velocity;
			break;
		case 'q':
			cameraPos.x += cos(angle) * velocity;
			cameraPos.z += sin(angle) * velocity;
			break;
		case 'z':
			cameraPos.x -= cos(angle) * velocity;
			cameraPos.z -= sin(angle) * velocity;
			break;
			/*
		case 'u':
			t++;
			break;
			*/
		case 'm':
			restart = 1;

			if (levels == 1){
				levels = 0;
			}
			else{
				levels = 1;
			}
			break;
		case SPACEBAR:
			if (moveout == 0){
				moveout = 1;
				MOVE_PLAYER_TRANSLATE[0][0] = MOVE_PLAYER[0][0];
				MOVE_PLAYER_TRANSLATE[0][1] = MOVE_PLAYER[0][1];
				PLAYER[0][0] += MOVE_PLAYER_TRANSLATE[0][0];
				PLAYER[0][1] += MOVE_PLAYER_TRANSLATE[0][0];

				timebase=glutGet(GLUT_ELAPSED_TIME);
			}
			else{
				moveout = 0;
			}
			
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
	switch (key) {
		case GLUT_KEY_LEFT:
			//angle -= velocity;
			//PLAYER[0][0]-=1;

			if (MOVE_PLAYER[0][0] >= -0.9f){
				MOVE_PLAYER[0][0] -= 0.1;
				MOVE_PLAYER[0][1] = 1 - abs(MOVE_PLAYER[0][0]);
			}
			break;

		case GLUT_KEY_RIGHT:
			//angle += velocity;
			//PLAYER[0][0]+=1;
			if (MOVE_PLAYER[0][0] <= 0.9f){
				MOVE_PLAYER[0][0] += 0.1;
				MOVE_PLAYER[0][1] = 1 - abs(MOVE_PLAYER[0][0]);
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
				MOVE_PLAYER_TRANSLATE[0][0] -= 0.1;
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
				MOVE_PLAYER_TRANSLATE[0][0] += 0.1;
				MOVE_PLAYER_TRANSLATE[0][1] = 1 - abs(MOVE_PLAYER_TRANSLATE[0][0]);
			}
			break;
	}
}

void setupCamera(void)
{

	//Define the view direction based on the camera's position
	cameraView.x = cameraPos.x + cos(angle);
	cameraView.y = cameraPos.y;
	cameraView.z = cameraPos.z + sin(angle);


	//Creates the view matrix based on the position, the view direction and the up vector
	cameraMatrix = glm::lookAt(cameraPos,
								cameraView,
								cameraUp);
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

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); //Double buffer; Color display

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

