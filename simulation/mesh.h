#pragma once
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLTexture>

struct Vertex
{
	QVector3D Position;
	QVector3D Normal;
	QVector2D TexCoords;
};

struct Texture
{
	QOpenGLTexture texture;
	QString type;
	QString path;
	Texture() : texture(QOpenGLTexture::Target2D)
	{
		texture.create();
		texture.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
		texture.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
		texture.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
	}
};

struct Material
{
	QVector3D ambient = QVector3D(0.35f, 0.35f, 0.35f);
	QVector3D diffuse;
	QVector3D specular;
	float shininess = 45.0f;
};

struct Light {
	QVector3D position;
	QVector3D ambient;
	QVector3D diffuse;
	QVector3D specular;
};

class cMesh : protected QOpenGLFunctions_3_3_Core
{
public:
	cMesh(QVector<Vertex> vertices, QVector<GLuint> indices, Material material);
	~cMesh();

	cMesh(const cMesh&) = delete;
	cMesh& operator=(const cMesh&) = delete;

	void drawGl(QOpenGLShaderProgram &shader);

private:
	void initGl();

private:
	// 顶点属性向量
	QVector<Vertex> vertices;
	// 索引向量
	QVector<GLuint> indices;
	// 材质
	Material material;
	// 灯源
	Light light;
	// VAO VBO EBO
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vbo, ebo;
};

