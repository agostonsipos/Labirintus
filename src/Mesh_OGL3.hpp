#pragma once

#include <GL/glew.h>

#include <vector>
#include <glm/glm.hpp>

class Mesh
{
public:
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoord;
	};

	Mesh(void);
	~Mesh(void);

	void initBuffers();
	void draw();
	void drawInstanced(GLsizei);
	
	bool inited = false;
	
	void addTransformAttribute(int attr, std::vector<glm::mat4>& _matrices);

	void addVertex(const Vertex& vertex) {
		vertices.push_back(vertex);
	}
	void addIndex(unsigned int index) {
		indices.push_back(index);
	}
	glm::vec3 getSampleVertPos(){ return vertices[0].position; }
private:
	GLuint vertexArrayObject;
	GLuint vertexBuffer;
	GLuint indexBuffer;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};
