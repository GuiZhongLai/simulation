#pragma once
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_3_3_Core>

struct sAxisPrivateData;
class cAxis : protected QOpenGLFunctions_3_3_Core
{
public:
	cAxis();
	~cAxis();

	void init();
	void draw();

	void rotate(const float &dx, const float &dy);
	void resize(int w, int h);

private:
	sAxisPrivateData *d;

private:
	void addAxis(QVector<float>& vertices, const QVector3D& start, const QVector3D& end, const QVector3D& color);
	void addArrow(QVector<float>& vertices, const QVector3D& base, const QVector3D& direction,
		float length, float radius, const QVector3D& color);
};

