#include "openglwidget.h"
#include <QCoreApplication>
#include <QMouseEvent>
#include <QWheelEvent>

const QString strLeftNodeNameBlock = "Block";
const QString strLeftNodeNameX = "X";
const QString strLeftNodeNameY = "Y";
const QString strLeftNodeNameZ = "Z";

cOpenglWidget::cOpenglWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
	QSurfaceFormat format;
	format.setSamples(4);				// 设置4倍多重采样
	format.setDefaultFormat(format);
}

cOpenglWidget::~cOpenglWidget()
{
}

void cOpenglWidget::initializeGL()
{
	initializeOpenGLFunctions();
	m_axis.init();
	m_model.init();
	m_model.loadModel(QCoreApplication::applicationDirPath() + "/model/simple.glb");
}

void cOpenglWidget::paintGL()
{
	glClearColor(0.21f, 0.21f, 0.21f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	m_model.draw();
	m_axis.draw();
}

void cOpenglWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
	m_model.resize(w, h);
	m_axis.resize(w, h);
}

void cOpenglWidget::mousePressEvent(QMouseEvent * event)
{
	m_lastMousePos = event->pos();
}

void cOpenglWidget::mouseMoveEvent(QMouseEvent * event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		// 处理视图矩阵平移
		// 计算屏幕移动量（反转Y轴）
		float dx = (event->pos().x() - m_lastMousePos.x()) * 0.01f;
		float dy = (m_lastMousePos.y() - event->pos().y()) * 0.01f;
		m_model.translate(dx, dy);
		update();
	}
	else if (event->buttons() & Qt::RightButton)
	{
		// 计算旋转量
		int dx = event->pos().x() - m_lastMousePos.x();
		int dy = event->pos().y() - m_lastMousePos.y();
		m_model.rotate(dx, dy);
		m_axis.rotate(dx, dy);
		update();
	}
	m_lastMousePos = event->pos();
}

void cOpenglWidget::wheelEvent(QWheelEvent * event)
{
	float delta = event->angleDelta().y() / 120.0f * 0.1f;
	m_model.scale(delta);
	update();
}

void cOpenglWidget::valueChange_x(double v)
{
	static double lastValue = 0;
	// x轴运动
	m_model.translateNode(strLeftNodeNameX, eTranslateDirection::GO_X, static_cast<float>(v - lastValue));
	m_model.translateNode(strLeftNodeNameY, eTranslateDirection::GO_X, static_cast<float>(v - lastValue));
	m_model.translateNode(strLeftNodeNameZ, eTranslateDirection::GO_X, static_cast<float>(v - lastValue));
	lastValue = v;
	update();
}

void cOpenglWidget::valueChange_y(double v)
{
	static double lastValue = 0;
	// y轴运动
	m_model.translateNode(strLeftNodeNameY, eTranslateDirection::GO_Y, static_cast<float>(v - lastValue));
	m_model.translateNode(strLeftNodeNameZ, eTranslateDirection::GO_Y, static_cast<float>(v - lastValue));
	lastValue = v;
	update();
}

void cOpenglWidget::valueChange_z(double v)
{
	static double lastValue = 0;
	// z轴运动，JIG运动
	m_model.translateNode(strLeftNodeNameZ, eTranslateDirection::GO_Z, static_cast<float>(v - lastValue));
	lastValue = v;
	update();
}