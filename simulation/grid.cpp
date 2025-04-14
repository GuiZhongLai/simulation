#include "grid.h"
struct sGridPrivateData
{
	QOpenGLShaderProgram shader;
	QOpenGLVertexArrayObject vaoGrid;
	QOpenGLBuffer vboGrid = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	QMatrix4x4 mvp;
	int gridCount = 0;
};

cGrid::cGrid()
{
	d = new sGridPrivateData;
}

cGrid::~cGrid()
{
	delete d;
}

void cGrid::init()
{
	initializeOpenGLFunctions();
	initShaders();
	initGridGeometry();
}

void cGrid::draw()
{
	d->shader.bind();

	d->shader.setUniformValue("mvp", d->mvp);
	d->vaoGrid.bind();
	glDrawArrays(GL_LINES, 0, d->gridCount);
	d->vaoGrid.release();

	d->shader.release();
}

void cGrid::updateMvp(const QMatrix4x4 & mvp)
{
	d->mvp = mvp;
}

void cGrid::initShaders()
{
	if (!d->shader.isLinked())
	{
		d->shader.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Resources/grid.vs");
		d->shader.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Resources/grid.fs");
		if (!d->shader.link())
			qDebug() << "cGrid Shader Link Failed!";
		else
			qDebug() << "cGrid Shader Link Success!";
	}
}

void cGrid::initGridGeometry()
{
	QVector<GLfloat> vertices;
	const float spacing = 10.0f;
	const float extent = 200.0f;
	const QVector3D grayColor(0.5f, 0.5f, 0.5f);	// 灰色
	const QVector3D xColor(0.0f, 0.0f, 1.0f);		// (X轴)
	const QVector3D yColor(1.0f, 0.0f, 0.0f);		// (Z轴)
	// 生成网格线
	for (float x = -extent; x <= extent; x += spacing) {
		QVector3D color = (x == 0.0f) ? xColor : grayColor;
		vertices << x << 0 << -extent << color.x() << color.y() << color.z();
		vertices << x << 0 << extent << color.x() << color.y() << color.z();

	}
	for (float z = -extent; z <= extent; z += spacing) {
		QVector3D color = (z == 0.0f) ? yColor : grayColor;
		vertices << -extent << 0 << z << color.x() << color.y() << color.z();
		vertices << extent << 0 << z << color.x() << color.y() << color.z();
	}
	d->gridCount = vertices.size() / 6;
	d->vaoGrid.create();
	d->vaoGrid.bind();
	d->vboGrid.create();
	d->vboGrid.bind();
	d->vboGrid.allocate(vertices.constData(), vertices.size() * sizeof(float));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	d->vboGrid.release();

	d->vaoGrid.release();
}