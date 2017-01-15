#include "Mesh_OGL3.hpp"

Mesh::Mesh(void)
{
}

Mesh::~Mesh(void)
{
}

void Mesh::initBuffers()
{
	glGenVertexArrays(1, &vertexArrayObject);
	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &indexBuffer);

	glBindVertexArray(vertexArrayObject);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), (void*)&vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)*2));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), (void*)&indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Mesh::draw()
{
	glBindVertexArray(vertexArrayObject);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void Mesh::drawInstanced(GLsizei cnt)
{
	glBindVertexArray(vertexArrayObject);

	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, cnt);

	glBindVertexArray(0);
}

void Mesh::addTransformAttribute(int attr, std::vector<glm::mat4>& _matrices)
{
	glBindVertexArray(vertexArrayObject);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, _matrices.size() * sizeof(glm::mat4), &_matrices[0], GL_STATIC_DRAW);
	
	GLsizei vec4Size = sizeof(glm::vec4);
	glEnableVertexAttribArray(attr);
	glVertexAttribPointer(attr, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)0);
	glEnableVertexAttribArray(attr+1);
	glVertexAttribPointer(attr+1, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(vec4Size));
	glEnableVertexAttribArray(attr+2);
	glVertexAttribPointer(attr+2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(2 * vec4Size));
	glEnableVertexAttribArray(attr+3);
	glVertexAttribPointer(attr+3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(3 * vec4Size));

	glVertexAttribDivisor(attr+0, 1);
	glVertexAttribDivisor(attr+1, 1);
	glVertexAttribDivisor(attr+2, 1);
	glVertexAttribDivisor(attr+3, 1);

	glBindVertexArray(0);
}
