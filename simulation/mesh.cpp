#include "mesh.h"

const QVector3D LIGHT_POS(0.0f, 0.0f, 30.0f);
const QVector3D LIGHT_COLOR(1.0f, 1.0f, 1.0f);

cMesh::cMesh(QVector<Vertex> vertices, QVector<GLuint> indices, Material material)
	: vbo(QOpenGLBuffer::VertexBuffer)
	, ebo(QOpenGLBuffer::IndexBuffer)
{
	initializeOpenGLFunctions();
	this->vertices = vertices;
	this->indices = indices;
	this->material = material;
	initGl();
}

cMesh::~cMesh()
{
}

void cMesh::drawGl(QOpenGLShaderProgram & shader)
{
	vao.bind();
	shader.setUniformValue("material.ambient", material.ambient);
	shader.setUniformValue("material.diffuse", material.diffuse);
	shader.setUniformValue("material.specular", material.specular);
	shader.setUniformValue("material.shininess", material.shininess);

	shader.setUniformValue("light.position", LIGHT_POS);
	shader.setUniformValue("light.ambient", LIGHT_COLOR * 0.8f);
	shader.setUniformValue("light.diffuse", LIGHT_COLOR * 0.8f);
	shader.setUniformValue("light.specular", LIGHT_COLOR);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	vao.release();
}

void cMesh::initGl()
{
	vao.create();
	vao.bind();
	vbo.create();
	ebo.create();

	vbo.bind();
	vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vbo.allocate(&vertices[0], vertices.size() * sizeof(Vertex));

	ebo.bind();
	ebo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	ebo.allocate(&indices[0], indices.size() * sizeof(GLuint));

	// 设置顶点属性指针
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	vao.release();
	vbo.release();
	ebo.release();
}
