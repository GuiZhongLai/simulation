#pragma once

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QPoint>
#include "model.h"
#include "axis.h"

class cOpenglWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
	Q_OBJECT

public:
	cOpenglWidget(QWidget *parent = Q_NULLPTR);
	~cOpenglWidget();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);

private:
	cModel m_model;
	cAxis m_axis;
	QPoint m_lastMousePos;

public:
	void valueChange_x(double v);
	void valueChange_y(double v);
	void valueChange_z(double v);

};
