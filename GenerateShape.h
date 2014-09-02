#pragma once

#include "Shape.h"

class GenerateShape
{
public:
	static shape makeTorus(GLfloat radius, GLfloat girth);
	static shape makeSierpinski(GLint recursions, bool threeD);
	static shape makeSierpinskiTrue3D(GLint recursions);
	static shape makeSphere(shape sphere, GLuint recursions, float radius);
	static shape makeTetrahedron(float radius);
	static shape makeCube();
	static shape makeCube(glm::vec3 center);
	GenerateShape(void);
	~GenerateShape(void);
};

