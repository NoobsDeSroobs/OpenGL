#include "GenerateShape.h"
#include <glm/glm.hpp>
#include <glm\gtx\transform.hpp>
#include "Vertex.h"

#define NUM_ELEMENTS_ARRAY(a) sizeof(a)/ sizeof(*a)

shape GenerateShape::makeTorus(GLfloat radius, GLfloat girth)
{
	GLfloat degrees = 10.0f;
	GLuint vertsPerCircle = 360/degrees;
	glm::vec3 origoAxis(0.0f, 1.0f, 0.0f); // To spin around, 
	glm::vec3 centerAxis(0.0f, 0.0f, 1.0f);// To spin around the center of the torus
	glm::vec3 originalCenterAxis(0.0f, 0.0f, 1.0f);// To spin around the center of the torus
	glm::mat3x3 origoRotationMatrix = glm::mat3x3(glm::rotate(glm::mat4(), degrees, origoAxis));
	glm::mat3x3 centerRotationMatrix = glm::mat3x3(glm::rotate(glm::mat4(), -degrees, centerAxis));
	glm::vec3 origoToCenter(radius, 0.0f, 0.0f); //The center of the torus tube to the surface of the tube
	glm::vec3 centerToSurface(girth, 0.0f, 0.0f); //The center of the shape to the center of the tube
	shape torus;

	GLuint i = 0;
	GLuint j = 0;
	while(1){
		vertex newVert;
		newVert.colour = glm::vec3(1.0f, 0.8431372549f, 0.0f);
		newVert.position = origoToCenter + centerToSurface;
		newVert.normal = glm::normalize(centerToSurface);

		torus.vertices.push_back(newVert);
		i++;
		centerToSurface = centerRotationMatrix*centerToSurface;
		if(i == vertsPerCircle)
		{
			i=0;
			j++;
			origoToCenter = origoRotationMatrix*origoToCenter;
			centerRotationMatrix = glm::mat3x3(glm::rotate(glm::mat4(), degrees*j, originalCenterAxis));
			centerAxis = centerRotationMatrix*centerAxis;
			
		}
		if(j==vertsPerCircle) break;
	}

	for (i = 0; i  < vertsPerCircle*vertsPerCircle; i++)
	{
		torus.indices.push_back(i);

		if(i+vertsPerCircle>=vertsPerCircle*vertsPerCircle)
			torus.indices.push_back(i+vertsPerCircle-(vertsPerCircle*vertsPerCircle));//To loop around the center of the model
		else
			torus.indices.push_back(i+vertsPerCircle);

		if((i+1)%vertsPerCircle == 0) //If we are not at the start and we have done one revolution
			torus.indices.push_back(i-vertsPerCircle+1);//We need to loop back to cover the last triangles. Around the center of the torus tube.
		else
			torus.indices.push_back(i+1);

		if(i == 15)
			printf("\n");

		torus.indices.push_back(i);
		
		if((i+1)%vertsPerCircle == 0) //If we are not at the start and we have done one revolution
			torus.indices.push_back(i-vertsPerCircle+1);//We need to loop back to cover the last triangles. Around the center of the torus tube.
		else
			torus.indices.push_back(i+1);

		if((i+1) < vertsPerCircle+vertsPerCircle)
		{
			if((i+1)%vertsPerCircle == 0)
				torus.indices.push_back(i-vertsPerCircle-vertsPerCircle+1+(vertsPerCircle*vertsPerCircle));
			else
				torus.indices.push_back(i-vertsPerCircle+1+(vertsPerCircle*vertsPerCircle));//We need to loop back to cover the last triangles. Around the center of the torus tube.
		}
		else
		{
			if((i+1)%vertsPerCircle == 0)
				torus.indices.push_back(i-vertsPerCircle-vertsPerCircle+1);
			else
			torus.indices.push_back(i-vertsPerCircle+1);
		}
	}
	//i, i+1, i+vertsPerCircle
	//i, i-vertsPerCircle+1, i+1

	torus.numIndices = torus.indices.size();
	torus.numVerts = torus.vertices.size();

	for(j = 0; j<torus.vertices.size(); j++)
	{
		printf("Vert %d, X:%f, Y:%f, Z:%f\n", j, torus.vertices[j].position.x, torus.vertices[j].position.y, torus.vertices[j].position.z);
	}

	for(j = 0; j<torus.indices.size(); j+=3)
	{
		printf("%d, %d, %d\n", torus.indices[j], torus.indices[j+1], torus.indices[j+2]);
	}

	return torus;
}

shape GenerateShape::makeSierpinski(GLint recursions, bool threeD)
{
	shape retShape;
	retShape.indices.clear();
	retShape.vertices.clear();
	//The original triangle
	vertex temp[] = {
		glm::vec3(0.0f, 100.0f, 0.0f),		glm::vec3(1.0f, 0.8431372549f, 0.0f),	glm::vec3(0.0f, 0.0f, 1.0f),	//2 - 2
		glm::vec3(-100.0f, -100.0f, 0.0f),	glm::vec3(1.0f, 0.8431372549f, 0.0f),	glm::vec3(0.0f, 0.0f, -1.0f),	//0 - 0
		glm::vec3(100.0f,  -100.0f, 0.0f),	glm::vec3(1.0f, 0.8431372549f, 0.0f),	glm::vec3(0.0f,  0.0f, -1.0f),	//1 - 1
	};
	retShape.vertices.push_back(temp[0]);
	retShape.vertices.push_back(temp[1]);
	retShape.vertices.push_back(temp[2]);

	retShape.indices.push_back(0);
	retShape.indices.push_back(1);
	retShape.indices.push_back(2);

	std::vector<GLuint> newIndices;

	for (int j = 0; j < recursions; j++)
	{
		for (GLuint i = 0; i < retShape.indices.size(); i+=3)
		{
			vertex oldVert1 = retShape.vertices[retShape.indices[i]];
			vertex oldVert2 = retShape.vertices[retShape.indices[i+1]];
			vertex oldVert3 = retShape.vertices[retShape.indices[i+2]];
			vertex newVert1;
			vertex newVert2;
			vertex newVert3;

			//Set the colour so it can be seen in the renderer
			newVert1.colour = glm::vec3(1.0f, 0.8431372549f, 0.0f);
			newVert2.colour = glm::vec3(1.0f, 0.8431372549f, 0.0f);
			newVert3.colour = glm::vec3(1.0f, 0.8431372549f, 0.0f);

			//Calculate the vector representing the edge between two vertices
			glm::vec3 v1Tov2 = oldVert2.position - oldVert1.position;
			glm::vec3 v2Tov3 = oldVert3.position - oldVert2.position;
			glm::vec3 v3Tov1 = oldVert1.position - oldVert3.position;

			//Halving it to point to the center point.
			v1Tov2 = v1Tov2 * 0.5f;
			v2Tov3 = v2Tov3 * 0.5f;
			v3Tov1 = v3Tov1 * 0.5f;

			//Set the position of each new vertex
			newVert1.position = oldVert1.position + v1Tov2;
			newVert2.position = oldVert2.position + v2Tov3;
			newVert3.position = oldVert3.position + v3Tov1;

			//Change it to be of length 1 so we can use it as normals
			newVert1.normal = glm::normalize(newVert1.position);
			newVert2.normal = glm::normalize(newVert2.position);
			newVert3.normal = glm::normalize(newVert3.position);

			//Add the new vertices and store the new indices
			retShape.vertices.push_back(newVert1);
			GLuint newVertIndex1 = (GLuint)retShape.vertices.size() - 1;
			retShape.vertices.push_back(newVert2);
			GLuint newVertIndex2 = newVertIndex1 + 1;
			retShape.vertices.push_back(newVert3);
			GLuint newVertIndex3 = newVertIndex2 + 1;

			//Calculate and add the new triangles; TRIFORCE!!!!
			//			/\
			//		   /__\
			//		  /\  /\
			//		 /__\/__\

			//Top
			newIndices.push_back(retShape.indices[i]);
			newIndices.push_back(newVertIndex1);
			newIndices.push_back(newVertIndex3);
			//Bottom left
			newIndices.push_back(retShape.indices[i+1]);
			newIndices.push_back(newVertIndex2);
			newIndices.push_back(newVertIndex1);
			//Bottom right
			newIndices.push_back(retShape.indices[i+2]);
			newIndices.push_back(newVertIndex3);
			newIndices.push_back(newVertIndex2);
		}
		retShape.numVerts = retShape.vertices.size();
		retShape.indices.swap(newIndices);
		newIndices.clear();
		retShape.numIndices = retShape.indices.size();
	}



	if(threeD)
	{
		GLfloat thickness = 3.0f;
		//Copy the current vertices one unit backwards
		int vertEnd = retShape.vertices.size();
		for (int i = 0; i < vertEnd; i++)
		{
			vertex behindVert = retShape.vertices[i];
			behindVert.position = behindVert.position + glm::vec3(0.0f, 0.0f, -thickness);
			retShape.vertices.push_back(behindVert);
		}

		int indEnd = retShape.indices.size();
		for (int i = 0; i < indEnd; i+=3)
		{
			retShape.indices.push_back(retShape.indices[i] + vertEnd);
			retShape.indices.push_back(retShape.indices[i+2] + vertEnd);
			retShape.indices.push_back(retShape.indices[i+1] + vertEnd);			
		}

		//The walls
		for (int i = 0; i < indEnd; i+=3)
		{
			//Current indices
			

			//right
			int rightTopLeft = i;
			int rightTopRight = rightTopLeft + indEnd;
			int rightBottomLeft =  rightTopLeft + 2;
			int rightBottomRight = rightTopRight + 1;

			retShape.indices.push_back(retShape.indices[rightTopLeft]);
			retShape.indices.push_back(retShape.indices[rightBottomLeft]);
			retShape.indices.push_back(retShape.indices[rightTopRight]);

			retShape.indices.push_back(retShape.indices[rightTopRight]);
			retShape.indices.push_back(retShape.indices[rightBottomLeft]);
			retShape.indices.push_back(retShape.indices[rightBottomRight]);

			//left
			int leftTopLeft = i + indEnd;
			int leftTopRight = i;
			int leftBottomLeft =  leftTopLeft + 2;
			int leftBottomRight = leftTopRight + 1;

			retShape.indices.push_back(retShape.indices[leftTopLeft]);
			retShape.indices.push_back(retShape.indices[leftBottomLeft]);
			retShape.indices.push_back(retShape.indices[leftTopRight]);

			retShape.indices.push_back(retShape.indices[leftTopRight]);
			retShape.indices.push_back(retShape.indices[leftBottomLeft]);
			retShape.indices.push_back(retShape.indices[leftBottomRight]);

			//bottom
			
			retShape.indices.push_back(retShape.indices[leftBottomLeft]);
			retShape.indices.push_back(retShape.indices[rightBottomRight]);
			retShape.indices.push_back(retShape.indices[leftBottomRight]);

			retShape.indices.push_back(retShape.indices[leftBottomRight]);
			retShape.indices.push_back(retShape.indices[rightBottomRight]);
			retShape.indices.push_back(retShape.indices[rightBottomLeft]);

		}

		retShape.numVerts = retShape.vertices.size();
		retShape.numIndices = retShape.indices.size();
	}



return retShape;
}

shape GenerateShape::makeSierpinskiTrue3D(GLint recursions)
{
	shape retShape = makeTetrahedron(10);

	std::vector<vertex> newVerts;
	int i;
	for (i = 0; i < retShape.numVerts; i+=3)
	{
		//The next tetrahedron
		vertex tri1 = retShape.vertices[i];
		vertex tri2 = retShape.vertices[i+1];
		vertex tri3 = retShape.vertices[i+2];
		vertex top = retShape.vertices[i+3];

		//The triangles needed to be carved
		glm::vec3 topTriPos1; 
		glm::vec3 topTriPos2; 
		glm::vec3 topTriPos3; 

		glm::vec3 bottomTriPos1; 
		glm::vec3 bottomTriPos2; 
		glm::vec3 bottomTriPos3; 

		//The new vertices to be made
		vertex newVert1;
		vertex newVert2;
		vertex newVert3;
		vertex newVert4;
		vertex newVert5;
		vertex newVert6;
		vertex newVert7;
		vertex newVert8;

		//Set the colour so it can be seen in the renderer
		newVert1.colour = glm::vec3(1.0f, 0.8431372549f, 0.0f);
		newVert2.colour = glm::vec3(1.0f, 0.8431372549f, 0.0f);
		newVert3.colour = glm::vec3(1.0f, 0.8431372549f, 0.0f);
		newVert4.colour = glm::vec3(1.0f, 0.8431372549f, 0.0f);
		newVert5.colour = glm::vec3(1.0f, 0.8431372549f, 0.0f);
		newVert6.colour = glm::vec3(1.0f, 0.8431372549f, 0.0f);
		newVert7.colour = glm::vec3(1.0f, 0.8431372549f, 0.0f);
		newVert8.colour = glm::vec3(1.0f, 0.8431372549f, 0.0f);

		////Calculat.colour = glm::vec3(1.0f, 0.8431372549f, 0.0f);e the vector representing the edge between two vertices
		//glm::vec3 v1Tov2 = oldVert2.position - oldVert1.position;
		//glm::vec3 v2Tov3 = oldVert3.position - oldVert2.position;
		//glm::vec3 v3Tov1 = oldVert1.position - oldVert3.position;

		////Halving it to point to the center point.
		//v1Tov2 = v1Tov2 * 0.5f;
		//v2Tov3 = v2Tov3 * 0.5f;
		//v3Tov1 = v3Tov1 * 0.5f;

		////Set the position of each new vertex
		//newVert1.position = oldVert1.position + v1Tov2;
		//newVert2.position = oldVert2.position + v2Tov3;
		//newVert3.position = oldVert3.position + v3Tov1;

		////Change it to be of length 1 so we can use it as normals
		//newVert1.normal = glm::normalize(newVert1.position);
		//newVert2.normal = glm::normalize(newVert2.position);
		//newVert3.normal = glm::normalize(newVert3.position);

		////Add the new vertices and store the new indices
		//retShape.vertices.push_back(newVert1);
		//GLuint newVertIndex1 = (GLuint)retShape.vertices.size() - 1;
		//retShape.vertices.push_back(newVert2);
		//GLuint newVertIndex2 = newVertIndex1 + 1;
		//retShape.vertices.push_back(newVert3);
		//GLuint newVertIndex3 = newVertIndex2 + 1;
		//newPosition.x = (oldVert1.position.x + oldVert2.position.x + oldVert3.position.x);
		//newPosition.y = (oldVert1.position.y + oldVert2.position.y + oldVert3.position.y);
		//newPosition.z = (oldVert1.position.z + oldVert2.position.z + oldVert3.position.z);

		//newPosition.x = newPosition.x / 3.0f;
		//newPosition.y = newPosition.y / 3.0f;
		//newPosition.z = newPosition.z / 3.0f;



	}
	
return retShape;
}

shape GenerateShape::makeSphere(shape sphere, GLuint recursions, float radius)
{

	if(sphere.numVerts == 0)//AKA the shape is empty
		sphere = makeTetrahedron(radius);// Make the base; a tetrahedron.

	if(!recursions) //If there are no recursions to make left.
		return sphere;

	std::vector<GLuint> newIndices;

	for (GLuint i = 0; i < sphere.numIndices; i+=3)
	{
		vertex oldVert1 = sphere.vertices[sphere.indices[i]];
		vertex oldVert2 = sphere.vertices[sphere.indices[i+1]];
		vertex oldVert3 = sphere.vertices[sphere.indices[i+2]];
		vertex newVert1;
		vertex newVert2;
		vertex newVert3;
		
		//Set the colour so it can be seen in the renderer
		newVert1.colour = glm::vec3(0.5f, 0.2f, 0.9f);
		newVert2.colour = glm::vec3(0.8f, 0.7f, 0.6f);
		newVert3.colour = glm::vec3(0.45f, 1.0f, 0.0f);

		//Calculate the vector representing the edge between two vertices
		glm::vec3 v1Tov2 = oldVert2.position - oldVert1.position;
		glm::vec3 v2Tov3 = oldVert3.position - oldVert2.position;
		glm::vec3 v3Tov1 = oldVert1.position - oldVert3.position;

		//Halving it to point to the center point.
		v1Tov2 = v1Tov2 * 0.5f;
		v2Tov3 = v2Tov3 * 0.5f;
		v3Tov1 = v3Tov1 * 0.5f;

		//Set the position of each new vertex
		newVert1.position = oldVert1.position + v1Tov2;
		newVert2.position = oldVert2.position + v2Tov3;
		newVert3.position = oldVert3.position + v3Tov1;

		//Change it to be of length 1 so we can use it as normals
		newVert1.normal = glm::normalize(newVert1.position);
		newVert2.normal = glm::normalize(newVert2.position);
		newVert3.normal = glm::normalize(newVert3.position);

		//Set the final position on the membrane at length radius
		newVert1.position = newVert1.normal * radius;
		newVert2.position = newVert2.normal * radius;
		newVert3.position = newVert3.normal * radius;

		//Add the new vertices and store the new indices
		sphere.vertices.push_back(newVert1);
		GLuint newVertIndex1 = (GLuint)sphere.vertices.size() - 1;
		sphere.vertices.push_back(newVert2);
		GLuint newVertIndex2 = newVertIndex1 + 1;
		sphere.vertices.push_back(newVert3);
		GLuint newVertIndex3 = newVertIndex2 + 1;

		//Calculate and add the new triangles; TRIFORCE!!!!
//			/\
//		   /__\
//		  /\  /\
//		 /__\/__\

		//Top
		newIndices.push_back(sphere.indices[i]);
		newIndices.push_back(newVertIndex1);
		newIndices.push_back(newVertIndex3);
		//Bottom left
		newIndices.push_back(sphere.indices[i+1]);
		newIndices.push_back(newVertIndex2);
		newIndices.push_back(newVertIndex1);
		//Bottom right
		newIndices.push_back(sphere.indices[i+2]);
		newIndices.push_back(newVertIndex3);
		newIndices.push_back(newVertIndex2);
		//Center
		newIndices.push_back(newVertIndex1);
		newIndices.push_back(newVertIndex2);
		newIndices.push_back(newVertIndex3);


	}

	//Set important data
	sphere.indices.resize(newIndices.size());
	sphere.indices.swap(newIndices);

	sphere.numVerts = (GLuint)sphere.vertices.size();
	sphere.numIndices = (GLuint)sphere.indices.size();

	return makeSphere(sphere, recursions-1, radius);
}

shape GenerateShape::makeTetrahedron(float radius)
{
	shape retShape;
	vertex verts[] = {
		//Position															//Colour						//Normal vector					//Corner ID-Vertex
		glm::normalize(glm::vec3(-1.0f, 0.0f, -1.0f/sqrtf(2.0f)))*radius,	glm::vec3(1.0f, 0.0f, 0.0f),	glm::normalize(glm::vec3(-1.0f, 0.0f, -1.0f/sqrtf(2.0f))),	//0 - 0
		glm::normalize(glm::vec3(1.0f,  0.0f, -1.0f/sqrtf(2.0f)))*radius,	glm::vec3(1.0f, 1.0f, 0.0f),	glm::normalize(glm::vec3(1.0f,  0.0f, -1.0f/sqrtf(2.0f))),	//1 - 1
		glm::normalize(glm::vec3(0.0f, -1.0f, 1.0f/sqrtf(2.0f)))*radius,	glm::vec3(1.0f, 0.0f, 1.0f),	glm::normalize(glm::vec3(0.0f, -1.0f, 1.0f/sqrtf(2.0f))),	//2 - 2
		glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f/sqrtf(2.0f)))*radius,		glm::vec3(0.0f, 1.0f, 1.0f),	glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f/sqrtf(2.0f))),		//3 - 3
	};

	//Triangle sets
	GLuint elements[] = {
		//Front
		0, 1, 2, 
		2, 1, 3, 

		//Back																											    
		0, 2, 3, 
		0, 3, 1, 
	};

	retShape.numVerts = NUM_ELEMENTS_ARRAY(verts);
	retShape.vertices.resize(retShape.numVerts);
	retShape.numIndices = NUM_ELEMENTS_ARRAY(elements);
	retShape.indices.resize(retShape.numIndices);

	for (uint16_t i = 0; i < retShape.numVerts; i++)
	{
		retShape.vertices[i] = verts[i];
	}

	for (uint16_t i = 0; i < retShape.numIndices; i++)
	{
		retShape.indices[i] = elements[i];
	}

	return retShape;
}

shape GenerateShape::makeCube()
{
	shape retShape;
	vertex verts[] = {
		//Position						//Colour						//Normal vector									//Corner ID-Vertex
		//Front
		glm::vec3(-0.5f, -0.5f, 0.5f),	glm::vec3(1.0f, 0.0f, 0.0f),	glm::vec3(0.0f, 0.0f, 1.0f),	//0 - 0
		glm::vec3(0.5f, -0.5f, 0.5f),	glm::vec3(1.0f, 0.0f, 0.0f),	glm::vec3(0.0f, 0.0f, 1.0f),	//1 - 1
		glm::vec3(0.5f, 0.5f, 0.5f),	glm::vec3(1.0f, 0.0f, 0.0f),	glm::vec3(0.0f, 0.0f, 1.0f),	//2 - 2
		glm::vec3(-0.5f, 0.5f, 0.5f),	glm::vec3(1.0f, 0.0f, 0.0f),	glm::vec3(0.0f, 0.0f, 1.0f),	//3 - 3
		//Back																							    
		glm::vec3(-0.5f, 0.5f, -0.5f),	glm::vec3(1.0f, 1.0f, 0.0f),	glm::vec3(0.0f, 0.0f, -1.0f),	//4 - 4
		glm::vec3(-0.5f, -0.5f, -0.5f),	glm::vec3(1.0f, 1.0f, 0.0f),	glm::vec3(0.0f, 0.0f, -1.0f),	//5 - 5
		glm::vec3(0.5f, -0.5f, -0.5f),	glm::vec3(1.0f, 1.0f, 0.0f),	glm::vec3(0.0f, 0.0f, -1.0f),	//6 - 6
		glm::vec3(0.5f, 0.5f, -0.5f),	glm::vec3(1.0f, 1.0f, 0.0f),	glm::vec3(0.0f, 0.0f, -1.0f),	//7 - 7
		//Top																							    
		glm::vec3(-0.5f, -0.5f, 0.5f),	glm::vec3(0.0f, 1.0f, 1.0f),	glm::vec3(0.0f, -1.0f, 0.0f),	//0 - 8
		glm::vec3(0.5f, -0.5f, 0.5f),	glm::vec3(0.0f, 1.0f, 1.0f),	glm::vec3(0.0f, -1.0f, 0.0f),	//1 - 9
		glm::vec3(-0.5f, -0.5f, -0.5f),	glm::vec3(0.0f, 1.0f, 1.0f),	glm::vec3(0.0f, -1.0f, 0.0f),	//5 - 10
		glm::vec3(0.5f, -0.5f, -0.5f),	glm::vec3(0.0f, 1.0f, 1.0f),	glm::vec3(0.0f, -1.0f, 0.0f),	//6 - 11
		//Bottom																						    
		glm::vec3(0.5f, 0.5f, 0.5f),	glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec3(0.0f, 1.0f, 0.0f),	//2 - 12
		glm::vec3(-0.5f, 0.5f, 0.5f),	glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec3(0.0f, 1.0f, 0.0f),	//3 - 13
		glm::vec3(-0.5f, 0.5f, -0.5f),	glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec3(0.0f, 1.0f, 0.0f),	//4 - 14
		glm::vec3(0.5f, 0.5f, -0.5f),	glm::vec3(0.0f, 0.0f, 1.0f),	glm::vec3(0.0f, 1.0f, 0.0f),	//7 - 15
		//Left																   
		glm::vec3(-0.5f, -0.5f, 0.5f),	glm::vec3(1.0f, 0.0f, 1.0f),	glm::vec3(-1.0f, 0.0f, 0.0f),	//0 - 16
		glm::vec3(-0.5f, 0.5f, 0.5f),	glm::vec3(1.0f, 0.0f, 1.0f),	glm::vec3(-1.0f, 0.0f, 0.0f),	//3 - 17
		glm::vec3(-0.5f, 0.5f, -0.5f),	glm::vec3(1.0f, 0.0f, 1.0f),	glm::vec3(-1.0f, 0.0f, 0.0f),	//4 - 18
		glm::vec3(-0.5f, -0.5f, -0.5f),	glm::vec3(1.0f, 0.0f, 1.0f),	glm::vec3(-1.0f, 0.0f, 0.0f),	//5 - 19
		//Right																   				  
		glm::vec3(0.5f, -0.5f, 0.5f),	glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec3(1.0f, 0.0f, 0.0f),	//1 - 20
		glm::vec3(0.5f, 0.5f, 0.5f),	glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec3(1.0f, 0.0f, 0.0f),	//2 - 21
		glm::vec3(0.5f, -0.5f, -0.5f),	glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec3(1.0f, 0.0f, 0.0f),	//6 - 22
		glm::vec3(0.5f, 0.5f, -0.5f),	glm::vec3(0.0f, 1.0f, 0.0f),	glm::vec3(1.0f, 0.0f, 0.0f),	//7 - 23

	};

	//Triangle sets
	GLuint elements[] = {
		//Front
		0, 1, 3, 
		1, 2, 3, 

		//Back																											    
		5, 4, 6, 
		4, 7, 6, 

		//Top	
		11, 8, 10, 
		11, 9, 8,

		//Bottom			
		13, 15, 14, 
		13, 12, 15, 

		//Left	
		19, 17, 18, 
		19, 16, 17, 

		//Right	
		20, 23, 21, 
		20, 22, 23
	};

	retShape.numVerts = NUM_ELEMENTS_ARRAY(verts);
	retShape.vertices.resize(retShape.numVerts);
	retShape.numIndices = NUM_ELEMENTS_ARRAY(elements);
	retShape.indices.resize(retShape.numIndices);

	for (uint16_t i = 0; i < retShape.numVerts; i++)
	{
		retShape.vertices[i] = verts[i];
	}

	for (uint16_t i = 0; i < retShape.numIndices; i++)
	{
		retShape.indices[i] = elements[i];
	}

	return retShape;
}

shape GenerateShape::makeCube(glm::vec3 center)
{
	//The center is in world coordinates

	shape retShape;
	vertex verts[] = {
		//Position					  //Colour						//Vertex ID
		glm::vec3(0.0f + center.x, 0.0f + center.y, 0.5f + center.z), glm::vec3(1.0f, 1.0f, 1.0f),	//0
		glm::vec3(0.5f + center.x, 0.0f + center.y, 0.0f + center.z),  glm::vec3(1.0f, 0.0f, 0.3f),	//1
		glm::vec3(0.5f + center.x, 0.5f + center.y, 0.0f + center.z),  glm::vec3(1.0f, 0.5f, 0.0f),	//2
		glm::vec3(0.0f + center.x, 0.5f + center.y, 0.5f + center.z),  glm::vec3(1.0f, 0.5f, 0.3f),	//3

		glm::vec3(-0.5f + center.x, 0.5f + center.y, 0.0f + center.z), glm::vec3(0.0f, 0.5f, 0.3f),	//4
		glm::vec3(-0.5f + center.x, 0.0f + center.y, 0.0f + center.z), glm::vec3(1.0f, 0.9f, 0.3f),	//5
		glm::vec3(0.0f + center.x, 0.0f + center.y, -0.5f + center.z), glm::vec3(1.0f, 0.0f, 0.0f),	//6
		glm::vec3(0.0f + center.x, 0.5f + center.y, -0.5f + center.z), glm::vec3(1.0f, 1.0f, 0.3f)	//7

	};

	//Triangle sets
	GLuint elements[] = {
		0, 1, 2,
		0, 2, 3,
		0, 4, 5,
		0, 3, 4,
		0, 5, 6,
		0, 6, 1,

		1, 2, 7,
		2, 3, 7,
		3, 4, 7,
		4, 5, 7,
		5, 6, 7,
		6, 1, 7
	};

	retShape.numVerts = NUM_ELEMENTS_ARRAY(verts);
	retShape.vertices.resize(retShape.numVerts);
	retShape.numIndices = NUM_ELEMENTS_ARRAY(elements);
	retShape.indices.resize(retShape.numIndices);

	for (uint16_t i = 0; i < retShape.numVerts; i++)
	{
		retShape.vertices[i] = verts[i];
	}

	for (uint16_t i = 0; i < retShape.numIndices; i++)
	{
		retShape.indices[i] = elements[i];
	}

	return retShape;
}

GenerateShape::GenerateShape(void)
{
}


GenerateShape::~GenerateShape(void)
{
}

/*

OLD MAKESPHARE
shape GenerateShape::makeSphere(shape sphere, GLuint recursions, float radius)
{

	if(sphere.numVerts == 0)//AKA the shape is empty
		sphere = makeTetrahedron(radius);// Make the base; a tetrahedron.

	if(!recursions) //If there are no recursions to make left.
		return sphere;

	std::vector<GLuint> newIndices;
	int i;
	for (i = 0; i < sphere.numIndices; i+=3)
	{
		vertex oldVert1 = sphere.vertices[sphere.indices[i]];
		vertex oldVert2 = sphere.vertices[sphere.indices[i+1]];
		vertex oldVert3 = sphere.vertices[sphere.indices[i+2]];
		vertex newVert;

		glm::vec3 newPosition; //The centroid
		newPosition.x = (oldVert1.position.x + oldVert2.position.x + oldVert3.position.x);
		newPosition.y = (oldVert1.position.y + oldVert2.position.y + oldVert3.position.y);
		newPosition.z = (oldVert1.position.z + oldVert2.position.z + oldVert3.position.z);

		newPosition.x = newPosition.x / 3.0f;
		newPosition.y = newPosition.y / 3.0f;
		newPosition.z = newPosition.z / 3.0f;

		newPosition = glm::normalize(newPosition) * radius;
		newVert.position = newPosition;
		newVert.colour = glm::vec3(1, 1, 0); //Test colour
		printf("new x:%f, y:%f, z:%f \n", oldVert1.position.x, oldVert1.position.y, oldVert1.position.z);
		printf("old 1 x:%f, y:%f, z:%f \n", oldVert2.position.x, oldVert2.position.y, oldVert2.position.z);
		printf("old 2 x:%f, y:%f, z:%f \n", oldVert3.position.x, oldVert3.position.y, oldVert3.position.z);
		printf("old 3 x:%f, y:%f, z:%f \n\n", newPosition.x, newPosition.y, newPosition.z);
		sphere.vertices.push_back(newVert); //Add the vertex to the shapes list

		//Calculate the new vertex's index and add the indices for the three new triangles.
		GLuint newVertIndex = sphere.vertices.size()-1;
		//printf("New index:%d\n", newVertIndex);
		newIndices.push_back(sphere.indices[i]);
		newIndices.push_back(sphere.indices[i+1]);
		newIndices.push_back(newVertIndex);

		newIndices.push_back(sphere.indices[i+1]);
		newIndices.push_back(sphere.indices[i+2]);
		newIndices.push_back(newVertIndex);

		newIndices.push_back(sphere.indices[i+2]);
		newIndices.push_back(sphere.indices[i]);
		newIndices.push_back(newVertIndex);
	}

	//Set important data
	sphere.indices.resize(newIndices.size());
	sphere.indices.swap(newIndices);

	sphere.numVerts = sphere.vertices.size();
	sphere.numIndices = sphere.indices.size();

	return makeSphere(sphere, recursions-1, radius);
}

*/