#include "simulation.h"
#include <QTimer>

simulation::simulation(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	connect(this, &simulation::signalValueChangedX, ui.widget, &cOpenglWidget::valueChange_x);
	connect(this, &simulation::signalValueChangedY, ui.widget, &cOpenglWidget::valueChange_y);
	connect(this, &simulation::signalValueChangedZ, ui.widget, &cOpenglWidget::valueChange_z);

	QTimer *timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, [=]()
	{
		static double xpos = 0.0f;
		static double ypos = 0.0f;
		static double zpos = 0.0f;
		static bool bDirect = true;

		// 正向运动
		if (bDirect)
		{
			// x轴
			if (xpos < 60.f)
			{
				xpos += 0.5f;
			}
			else
			{
				xpos = 60.0f;
				bDirect = false;
			}

			// y轴
			if (ypos < 17.0f)
				ypos += 0.5f;
			else
				ypos = 17.0f;

			// z轴
			if (zpos < 20.0f)
				zpos += 0.5f;
			else
				zpos = 20.0f;
		}
		else
		{
			if (xpos < 0.0f)
			{
				xpos = 0.0f;
				bDirect = true;
			}
			else
			{
				xpos -= 0.5f;
			}

			// y轴
			if (ypos <= 0.0f)
				ypos = 0.0f;
			else
				ypos -= 0.5f;

			// z轴
			if (zpos <= 0.0f)
				zpos = 0.0f;
			else
				zpos -= 0.5f;
		}

		emit signalValueChangedX(xpos);
		emit signalValueChangedY(ypos);
		emit signalValueChangedZ(zpos);
	});

	timer->start(50);
}
