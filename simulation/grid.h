#pragma once
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_3_3_Core>

struct sGridPrivateData;
class cGrid : protected QOpenGLFunctions_3_3_Core
{
public:
	cGrid();
	~cGrid();

	void init();
	void draw();
	void updateMvp(const QMatrix4x4 &mvp);

private:
	void initShaders();
	void initGridGeometry();

private:
	sGridPrivateData *d;
};

