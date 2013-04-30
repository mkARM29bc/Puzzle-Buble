#include <Gl/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <FreeImage.h>

#include "display.h"
#include "main.h"

void display_at(int va_pos,float tx,float ty,float tz, float ra,float rx,float ry,float rz,float sx,float sy,float sz){
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, baseTextureId);

	GLuint loc = glGetUniformLocation(programId, "tex");
	glUniform1i(loc, 0); 


	loc = glGetUniformLocation(programId, "pMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, (GLfloat *)&perspectiveMatrix[0]);

	loc = glGetUniformLocation(programId, "vMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, (GLfloat *)&cameraMatrix[0]);
	
	loc = glGetUniformLocation(programId, "mMatrix");

	glBindVertexArray(vao[va_pos]);
	glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, tz));
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), ra, glm::vec3(rx, ry, rz)); 
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(sx,sy,sz));

	glm::mat4 model = translateMatrix * rotate * scale;
	glUniformMatrix4fv(loc, 1, GL_FALSE, (GLfloat *)&model[0]);
	glm::mat4 cameraModelMatrix = cameraMatrix * model;
	
	loc = glGetUniformLocation(programId, "nMatrix");
	glm::mat3 normalMatrix = glm::mat3(cameraMatrix);
	glUniformMatrix3fv(loc, 1, GL_FALSE, (GLfloat *)&normalMatrix[0]); 
	/*
		if using non-uniform scaling:

		glm::mat4 cameraModelMatrix = cameraMatrix * model;
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(cameraModelMatrix)));
	*/

	loc = glGetUniformLocation(programId, "lightDir");
	glm::vec4 transformedLightDir = cameraMatrix * glm::vec4(lightDir[0], lightDir[1], lightDir[2], 0.0f);
	glUniform3fv(loc, 1, (GLfloat *)&transformedLightDir[0]);

	loc = glGetUniformLocation(programId, "lightIntensity");
	glUniform4fv(loc, 1, lightIntensity);

	loc = glGetUniformLocation(programId, "ambientIntensity");
	glUniform4fv(loc, 1, ambientComponent);

	loc = glGetUniformLocation(programId, "diffuseColor");
	glUniform4fv(loc, 1, diffuseColor);

	const unsigned int *indices = object[va_pos].getIndicesArray();
	glDrawElements(GL_TRIANGLES, object[va_pos].getNIndices(), GL_UNSIGNED_INT, indices); //type of geometry; number of indices; type of indices array, indices pointer

}