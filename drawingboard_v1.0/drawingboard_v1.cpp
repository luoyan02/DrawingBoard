#include "drawingboard_v1.h"
#include <QMouseEvent>
#include <QPainter>

drawingboard_v1::drawingboard_v1(QWidget* parent)
	: QWidget(parent)
{
	setFixedSize(400, 400);
}

void drawingboard_v1::mouseMoveEvent(QMouseEvent* event)
{
	// 超出了绘图框，所以直接return
	if (event->x() < 0 || event->x() > 400 || event->y() < 0 || event->y() > 400)
		return;
	// 移动过程，记录鼠标的位置
	m_vPoint.push_back(QPoint(event->x(), event->y()));

	update();
}

void drawingboard_v1::mousePressEvent(QMouseEvent* event)
{
	if (event->x() < 0 || event->x() > 400 || event->y() < 0 || event->y() > 400)
		return;
	
	m_vPoint.push_back(QPoint(event->x(), event->y()));
}

void drawingboard_v1::paintEvent(QPaintEvent* event)
{
	QPainter painter(this); // this就是这个框

	QPen pen;
	pen.setWidth(10); // 设置笔的宽度为10
	pen.setColor(Qt::red); // 设置笔的颜色为红色

	painter.setPen(pen);

	painter.drawPoints(QPolygon(m_vPoint));

}

