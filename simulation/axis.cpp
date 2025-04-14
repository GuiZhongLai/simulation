#include "axis.h"
#include <QtMath>

struct sAxisPrivateData
{
	QOpenGLShaderProgram shader;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	QMatrix4x4 model;
	int width = 0;
	int height = 0;
};

cAxis::cAxis()
{
	d = new sAxisPrivateData;
}

cAxis::~cAxis()
{
	delete d;
}

void cAxis::init()
{
	initializeOpenGLFunctions();
	if (!d->shader.isLinked())
	{
		d->shader.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Resources/axis.vs");
		d->shader.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Resources/axis.fs");
		if (!d->shader.link())
			qDebug() << "Axis Shader Link Failed!";
		else
			qDebug() << "Axis Shader Link Success!";
	}

	// ��ʼ��model
	d->model.rotate(-45.0f, 0.0f, 1.0f, 0.0f);

	// �����ᶥ�����ݣ�����ͷ��
	struct Vertex {
		QVector3D pos;
		QVector3D color;
	};

	QVector<Vertex> axisVertices = {
		// X��
		{{0, 0, 0}, {1, 0, 0}}, {{1, 0, 0}, {1, 0, 0}},
		// Y��
		{{0, 0, 0}, {0, 1, 0}}, {{0, 1, 0}, {0, 1, 0}},
		// Z��
		{{0, 0, 0}, {0, 0, 1}}, {{0, 0, 1}, {0, 0, 1}},
		// ��ͷ����
		{{0.9f, 0.1f, 0}, {1, 0, 0}}, {{1, 0, 0}, {1, 0, 0}},
		{{0.9f, -0.1f, 0}, {1, 0, 0}}, {{1, 0, 0}, {1, 0, 0}},
		{{0.1f, 0.9f, 0}, {0, 1, 0}}, {{0, 1, 0}, {0, 1, 0}},
		{{-0.1f, 0.9f, 0}, {0, 1, 0}}, {{0, 1, 0}, {0, 1, 0}},
		{{0, 0.1f, 0.9f}, {0, 0, 1}}, {{0, 0, 1}, {0, 0, 1}},
		{{0, -0.1f, 0.9f}, {0, 0, 1}}, {{0, 0, 1}, {0, 0, 1}},
	};

	d->vao.create();
	d->vao.bind();

	d->vbo.create();
	d->vbo.bind();
	d->vbo.allocate(axisVertices.constData(), axisVertices.size() * sizeof(Vertex));

	// λ������
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

	// ��ɫ����
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));


	d->vbo.release();
	d->vao.release();
}

void cAxis::draw()
{
	glViewport(10, d->height - 210, 200, 200);  // ���Ͻ�����
	// ����ͶӰ�����������꣩
	QMatrix4x4 projection;
	projection.ortho(-1.2f, 1.2f, -1.2f, 1.2f, -10, 10);

	QMatrix4x4 view;
	view.scale(0.9f);

	QMatrix4x4 mvp = projection * view * d->model;
	d->shader.bind();
	d->shader.setUniformValue("mvp", mvp);

	d->vao.bind();
	// ��������
	glDrawArrays(GL_LINES, 0, 6);
	// ���Ƽ�ͷ
	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 6, 12);
	glLineWidth(1.0f);

	d->vao.release();

	d->shader.release();
}

void cAxis::rotate(const float & dx, const float & dy)
{
	// ������Y����ת
	QMatrix4x4 rotY;
	rotY.rotate(dx * 0.5f, QVector3D(0, 1, 0));
	d->model = rotY * d->model;

	// ������X����ת
	QMatrix4x4 rotX;
	rotX.rotate(dy * 0.5f, QVector3D(1, 0, 0));
	d->model = rotX * d->model;
}

void cAxis::resize(int w, int h)
{
	d->width = w;
	d->height = h;
}

void cAxis::addAxis(QVector<float>& vertices, const QVector3D & start, const QVector3D & end, const QVector3D & color)
{
	vertices << start.x() << start.y() << start.z()
		<< color.x() << color.y() << color.z();

	// �յ�
	vertices << end.x() << end.y() << end.z()
		<< color.x() << color.y() << color.z();
}

void cAxis::addArrow(QVector<float>& vertices, const QVector3D & base, const QVector3D & direction, float length, float radius, const QVector3D & color)
{
	const int segments = 8;
	QVector3D tip = base + direction * length;

	// ������ͷ׶���Բ�λ���
	QVector3D perp1, perp2;
	if (direction.x() != 0 || direction.y() != 0) {
		perp1 = QVector3D::crossProduct(direction, QVector3D(0, 0, 1)).normalized();
	}
	else {
		perp1 = QVector3D::crossProduct(direction, QVector3D(1, 0, 0)).normalized();
	}
	perp2 = QVector3D::crossProduct(direction, perp1).normalized();

	// ���׶�嶥��
	for (int i = 0; i <= segments; ++i) {
		float angle = 2.0f * M_PI * i / segments;
		QVector3D circlePoint = base + (perp1 * cos(angle)) * radius + (perp2 * sin(angle)) * radius;

		// ׶�����������
		vertices << tip.x() << tip.y() << tip.z() << color.x() << color.y() << color.z();
		vertices << circlePoint.x() << circlePoint.y() << circlePoint.z() << color.x() << color.y() << color.z();

		if (i > 0) {
			QVector3D prevCirclePoint = base + (perp1 * cos(angle - 2.0f * M_PI / segments)) * radius
				+ (perp2 * sin(angle - 2.0f * M_PI / segments)) * radius;
			vertices << prevCirclePoint.x() << prevCirclePoint.y() << prevCirclePoint.z()
				<< color.x() << color.y() << color.z();
		}
	}

	// ��ӻ���Բ��
	for (int i = 0; i <= segments; ++i) {
		float angle = 2.0f * M_PI * i / segments;
		QVector3D circlePoint = base + (perp1 * cos(angle)) * radius + (perp2 * sin(angle)) * radius;

		vertices << base.x() << base.y() << base.z() << color.x() << color.y() << color.z();
		vertices << circlePoint.x() << circlePoint.y() << circlePoint.z() << color.x() << color.y() << color.z();

		if (i > 0) {
			QVector3D prevCirclePoint = base + (perp1 * cos(angle - 2.0f * M_PI / segments)) * radius
				+ (perp2 * sin(angle - 2.0f * M_PI / segments)) * radius;
			vertices << prevCirclePoint.x() << prevCirclePoint.y() << prevCirclePoint.z()
				<< color.x() << color.y() << color.z();
		}
	}
}
