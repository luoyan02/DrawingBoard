#include "CG_Painter.h"
#include "algorithm.h"
#include <QMouseEvent>
#include <QPainter> 
#include <QDebug>
#include <QSlider>
#include <QLabel>
#include <QWidgetAction>
#include <QHBoxLayout>
#include <QPushButton>
#include <iostream>
#include <QIntValidator>
#include <QSpinBox>

CG_Painter::CG_Painter(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	refresh_ColorIcon();

	// 进行保存
	QAction* actionSave = new QAction(tr(u8"save"));
	actionSave->setStatusTip(tr(u8"save the file"));
	connect(actionSave, &QAction::triggered, this, &CG_Painter::action_to_save);
	ui.menuBar->addAction(actionSave);


	//清除所有图元的Action
	QAction* actionClear = new QAction(tr(u8"clear"));
	actionClear->setStatusTip(tr(u8"clear all pictures."));
	connect(actionClear, &QAction::triggered, this, &CG_Painter::state_to_clear);
	ui.menuBar->addAction(actionClear);

	//随意画的Action
	QAction* actionDraw = new QAction(tr(u8"random"));
	actionDraw->setStatusTip(tr(u8"use width and color bar to adjust the pen, draw creatively!"));
	connect(actionDraw, &QAction::triggered, this, &CG_Painter::state_to_draw);
	ui.menuBar->addAction(actionDraw);
	
	//绘制直线的Action
	QAction* actionLine = new QAction(tr(u8"StraightLine"));
	actionLine->setStatusTip(tr(u8"click and drag the mouse to draw a straight line."));
	connect(actionLine, &QAction::triggered, this, &CG_Painter::state_to_lineDDA);
	ui.menuBar->addAction(actionLine);

	//绘制多边形的Action
	QAction* actionPoly = new QAction(tr(u8"Polygon"));
	actionPoly->setStatusTip(tr(u8"click and drag the mouse several times to create a polygon."));
	connect(actionPoly, &QAction::triggered, this, &CG_Painter::state_to_polygonDDA);
	ui.menuBar->addAction(actionPoly);

	//增加绘制椭圆的Action
	QAction* actionEllipse = new QAction(tr(u8"Ellipse"));
	actionEllipse->setStatusTip(tr(u8"Left-click to create an ellipse, left-click and drag to resize it, double-click to finalize and exit, right-click to cancel drawing."));
	connect(actionEllipse, &QAction::triggered, this, &CG_Painter::state_to_ellipse);
	ui.menuBar->addAction(actionEllipse);

	//绘制曲线
	QAction* actionCurve_Bezier = new QAction(tr(u8"Bezier"));
	actionCurve_Bezier->setStatusTip(tr(u8"【Bezier】Left-click to sequentially determine control points, right-click to confirm and exit."));
	connect(actionCurve_Bezier, &QAction::triggered, this, &CG_Painter::action_to_curve_Bezier);
	QAction* actionCurve_Bspline = new QAction(tr(u8"B-spline"));
	actionCurve_Bspline->setStatusTip(tr(u8"【B-spline】Left-click to sequentially determine control points, right-click to confirm and exit."));
	connect(actionCurve_Bspline, &QAction::triggered, this, &CG_Painter::action_to_curve_Bspline);
	//曲线Menu
	QMenu *menu2 = new QMenu(ui.menuBar);
	menu2->setTitle(u8"Curve");
	menu2->addAction(actionCurve_Bezier);
	menu2->addAction(actionCurve_Bspline);
	ui.menuBar->addAction(menu2->menuAction());

	//旋转图元Action
	QAction* actionRotate = new QAction(tr(u8"Spin"));
	actionRotate->setStatusTip(tr(u8"Left-click to determine the rotation center, hold the left mouse button and drag to rotate the graphic element, right-click to exit."));
	connect(actionRotate, &QAction::triggered, this, &CG_Painter::action_to_rotate);
	ui.menuBar->addAction(actionRotate);

	//缩放图元Action
	QAction* actionScale = new QAction(tr(u8"Scale"));
	actionScale->setStatusTip(tr(u8"Left-click to determine the scaling center, hold the left mouse button and drag to scale the graphic element, right-click to exit."));
	connect(actionScale, &QAction::triggered, this, &CG_Painter::action_to_scale);
	ui.menuBar->addAction(actionScale);

	//裁剪Action
	QAction* actionClip_Cohen = new QAction(tr(u8"Cohen-Sutherland"));
	actionClip_Cohen->setStatusTip(tr(u8"【Cohen-Sutherland】Left-click to select the starting and ending points of the clipping window. Note: Only efficient for StraightLines"));
	connect(actionClip_Cohen, &QAction::triggered, this, &CG_Painter::action_to_clip_Cohen);
	QAction* actionClip_Liang = new QAction(tr(u8"Liang-Barsky"));
	actionClip_Liang->setStatusTip(tr(u8"【Liang-Barsky】Left-click to select the starting and ending points of the clipping window. Note: Only efficient for StraightLines"));
	connect(actionClip_Liang, &QAction::triggered, this, &CG_Painter::action_to_clip_Liang);

	//裁剪Menu
	QMenu *menu1 = new QMenu(ui.menuBar);
	menu1->setTitle(u8"Cut");
	menu1->addAction(actionClip_Cohen);
	menu1->addAction(actionClip_Liang);
	ui.menuBar->addAction(menu1->menuAction());

	//设置左侧工具栏
	QToolBar* toolBar = new QToolBar(this);  // 创建工具栏
	addToolBar(Qt::LeftToolBarArea, toolBar); // 设置默认停靠范围 [默认停靠左侧]
	toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);   // 允许上下拖动
	toolBar->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea);   // 允许左右拖动

	//设置红色颜色栏
	slider_red->setRange(0, 255);
	slider_red->resize(10, 80);
	QString style1 = "QSlider::handle {"
		"background-color: red;"
		"border-radius: 8px;"
		"}";
	slider_red->setStyleSheet(style1);
	toolBar->addWidget(slider_red);

	//设置绿色颜色栏
	slider_green->setRange(0, 255);
	slider_green->resize(10, 80);
	QString style3 = "QSlider::handle {"
		"background-color: green;"
		"border-radius: 8px;"
		"}";
	slider_green->setStyleSheet(style3);
	toolBar->addWidget(slider_green);

	//设置蓝色颜色栏
	slider_blue->setRange(0, 255);
	slider_blue->resize(10, 80);
	QString style2 = "QSlider::handle {"
		"background-color: blue;"
		"border-radius: 8px;"
		"}";
	slider_blue->setStyleSheet(style2);
	toolBar->addWidget(slider_blue);

	//添加一个按钮显示上面三者的配色情况
	connect(colorButton, &QPushButton::clicked, this, &CG_Painter::GetButtonColor);
	toolBar->addWidget(colorButton);

	//添加一个按钮来调整笔的粗细
	QLabel* labelText = new QLabel;
	labelText->setText(QStringLiteral("line width:"));
	labelText->setIndent(10); // 设置缩进
	ui.mainToolBar->addWidget(labelText);

	widthEdit->setRange(1, 10);
	widthEdit->setFixedWidth(50);
	ui.mainToolBar->addWidget(widthEdit);

	connect(widthEdit, QOverload<int>::of(&QSpinBox::valueChanged), this, &CG_Painter::setWidth);

	//QLineEdit* widthEdit = new QLineEdit(this);
	//widthEdit->setValidator(new QIntValidator(1, 10, widthEdit));  // 设置输入范围为 1 到 10
	//widthEdit->setFixedSize(80, 20);  // 设置方块控件的大小
	//toolBar->addWidget(widthEdit);

	//状态栏显示鼠标位置
	statusLabel = new QLabel();
	statusLabel->resize(100, 30);
	ui.statusBar->addPermanentWidget(statusLabel);
}

int CG_Painter::WWidth = 1;

void CG_Painter::SetColor()
{
	int red = slider_red->value();
	int green = slider_green->value();
	int blue = slider_blue->value();

	QColor color(red, green, blue);
	CG_Painter:: WColor = color;
}

QColor CG_Painter::WColor = "black";

void CG_Painter::GetButtonColor()
{
	SetColor();
	QPalette palette = colorButton->palette(); // 获取按钮的调色板
	palette.setColor(QPalette::Button, CG_Painter::WColor); // 设置调色板中按钮的颜色
	colorButton->setAutoFillBackground(true);
	colorButton->setFlat(true);
	colorButton->setPalette(palette); // 设置按钮的调色板
	colorButton->update(); // 更新按钮的外观
}

void CG_Painter::refreshStateLabel()
{
	//状态栏展示鼠标位置
	QString str = "(" + QString::number(mouse_x) + "," + QString::number(mouse_y) + ")";
	statusLabel->setText(state_info + algo_info + str);
}

void CG_Painter::refresh_ColorIcon()
{
	QImage icon(30, 30, QImage::Format_RGB32);
	icon.fill(nowColor);
	// ui.actionColor->setIcon(QPixmap::fromImage(icon));
}

void CG_Painter::setState(PAINTER_STATE newState)
{
	state = newState;
	buf_flag = false;
	update();
	switch (state)
	{
	case CG_Painter::NOT_DRAWING:
		state_info = "";
		algo_info = "";
		break;
	case CG_Painter::DRAW_LINE:
		state_info = u8"状态：画直线 | ";
		line_state = LINE_NON_POINT;
		break;
	case CG_Painter::DRAW_POLYGON:
		state_info = u8"状态：画多边形 | ";
		poly_state = POLY_NON_POINT;
		poly_points.clear();
		poly_id = getNewID();
		break;
	case CG_Painter::DRAW_ELLIPSE:
		state_info = u8"状态：画椭圆 | ";
		algo_info = "";
		elli_state = ELLI_NON_POINT;
		elli_id = getNewID();
		break;
	case CG_Painter::DRAW_ROTATE:
		state_info = u8"状态：旋转图元 | ";
		algo_info = "";
		rotate_state = ROTATE_NON;
		break;
	case CG_Painter::DRAW_SCALE:
		state_info = u8"状态：缩放图元 | ";
		algo_info = "";
		scale_state = SCALE_NON;
		break;
	case CG_Painter::DRAW_CLIP:
		state_info = u8"状态：线段裁剪 | ";
		clip_state = CLIP_NON;
		break;
	case CG_Painter::DRAW_CURVE:
		state_info = u8"状态：画曲线 | ";
		curve_points.clear();
		curve_state = CURVE_NON;
		break;
	default:
		break;
	}
	refreshStateLabel();
}

void CG_Painter::setAlgo(ALGORITHM newAlgo)
{
	algorithm = newAlgo;
	switch (newAlgo)
	{
	case DDA:
		algo_info = "DDA | ";
		break;
	case BRESENHAM:
		algo_info = "Bresenham | ";
		break;
	case COHEN:
		algo_info = "Cohen-Sutherland | ";
		break;
	case LIANG:
		algo_info = "Liang-Barsky | ";
		break;
	case BEZIER:
		algo_info = "Bezier | ";
		break;
	case B_SPLINE:
		algo_info = "B-spline | ";
		break;
	default:
		break;
	}
}

void CG_Painter::mousePressEvent(QMouseEvent * event)
{
	//鼠标当前位置
	int x = event->pos().x();
	int y = event->pos().y();
	mouse_x = x;
	mouse_y = y;

	if (state == NOT_DRAWING) {
		if (event->button() == Qt::LeftButton) {
			if (trans_state == TRANS_NON) {
				trans_ID = myCanvas.getID(x, y);
				if (trans_ID != -1) {
					trans_ix = x; trans_iy = y;
					bufCanvas = myCanvas;
					trans_state = TRANS_BEGIN;
				}
			}
		}
	}
	else if (state == DRAW_ELLIPSE) {
		if (event->button() == Qt::LeftButton) {
			if (elli_state == ELLI_PAINTING) {
				init_x = x; init_y = y;
			}
		}
	}
	else if (state == DRAW_ROTATE) {
		if (event->button() == Qt::LeftButton) {
			if (rotate_state == ROTATE_READY && myCanvas.getID(x, y) != -1) {
				selected_ID = myCanvas.getID(x, y);
				init_x = x; init_y = y;
				rotate_state = ROTATE_BEGIN;
			}
		}
	}
	else if (state == DRAW_SCALE) {
		if (event->button() == Qt::LeftButton) {
			if (scale_state == SCALE_READY && myCanvas.getID(x, y) != -1) {
				selected_ID = myCanvas.getID(x, y);
				init_x = x; init_y = y;
				scale_state = SCALE_BEGIN;
			}
		}
	}
	else if (state == DRAW_RANDOM)
	{
		/*bufCanvas = myCanvas;
		buf_flag = true;*/

		// 清空之前的路径
		drawingPath = QPainterPath();
		// 获取鼠标的位置
		QPoint pos = event->pos();
		// 将位置添加到绘制路径中
		drawingPath.moveTo(pos);
		// 强制刷新窗口，触发重绘
		update();
	}
	
	refreshStateLabel();
}

void CG_Painter::mouseMoveEvent(QMouseEvent * event)
{
	//鼠标当前位置
	int x = event->pos().x();
	int y = event->pos().y();
	mouse_x = x;
	mouse_y = y;

	//设置鼠标形状
	if (buf_flag) {
		if (bufCanvas.getID(x, y) != -1) {
			setCursor(Qt::PointingHandCursor);
		}
		else {
			setCursor(Qt::ArrowCursor);
		}
	}
	else {
		if (myCanvas.getID(x, y) != -1) {
			setCursor(Qt::PointingHandCursor);
		}
		else {
			setCursor(Qt::ArrowCursor);
		}
	}

	
	if (state==NOT_DRAWING) {
		if (trans_state==TRANS_BEGIN) {
			bufCanvas = myCanvas;
			buf_flag = true; // 设置为true代表是将buf_canvas中的物体进行绘制
			bufCanvas.translate(trans_ID, x - trans_ix, y - trans_iy); //trans_ID指定是将哪个物体进行平移
			update();
		}
	}
	else if (state == DRAW_LINE) {
		if (line_state == LINE_POINTA) {
			bufCanvas = myCanvas;
			buf_flag = true;
			bufCanvas.drawLine(-1, line_Ax, line_Ay, x, y, algorithm);
			update();
		}
	}
	else if (state == DRAW_POLYGON) {
		if (poly_state == POLY_PAINTING) {
			autoPoly(x, y);//自动贴合修正
			bufpoly_points = poly_points;
			bufpoly_points.push_back(Point(x, y));
			bufCanvas.drawPolygon(poly_id, bufpoly_points, algorithm);
			buf_flag = true;
			update();
		}
	}
	else if (state == DRAW_ELLIPSE) {
		if (elli_state == ELLI_PAINTING && (event->buttons() & Qt::LeftButton)) {
			changing_dx = qAbs(x - init_x);
			changing_dy = qAbs(y - init_y);
			changing_dx = ((init_x - cx)*(x - init_x) >= 0) ? changing_dx : (-changing_dx);
			changing_dy = ((init_y - cy)*(y - init_y) >= 0) ? changing_dy : (-changing_dy);
			bufCanvas = myCanvas;
			bufCanvas.drawEllipse(elli_id, cx, cy, rx + changing_dx, ry + changing_dy);
			buf_flag = true;
			update();
		}
	}
	else if (state == DRAW_ROTATE) {
		if (rotate_state == ROTATE_BEGIN && (event->buttons() & Qt::LeftButton)) {
			int r = getRotateR(init_x, init_y, rotate_rx, rotate_ry, x, y);
			bufCanvas = myCanvas;
			bufCanvas.drawDotPoint(-1, rotate_rx, rotate_ry);
			bufCanvas.rotate(selected_ID, rotate_rx, rotate_ry, r);
			buf_flag = true;
			update();
		}
	}
	else if (state == DRAW_SCALE) {
		if (scale_state == SCALE_BEGIN && (event->buttons() & Qt::LeftButton)) {
			double s = getScaleS(init_x, init_y, scale_rx, scale_ry, x, y);
			bufCanvas = myCanvas;
			bufCanvas.drawDotPoint(-1, scale_rx, scale_ry);
			bufCanvas.scale(selected_ID, scale_rx, scale_ry, s);
			buf_flag = true;
			update();
		}
	}
	else if (state == DRAW_CLIP) {
		if (clip_state == CLIP_BEGIN) { //裁剪时，无需按住左键
			bufCanvas = myCanvas;
			bufCanvas.drawRectangle(-1, init_x, init_y, x, y);
			buf_flag = true;
			update();
		}
	}
	else if (state == DRAW_RANDOM) {
		// 获取鼠标的位置
		QPoint pos = event->pos();
		// 将位置添加到绘制路径中
		drawingPath.lineTo(pos);
		// 强制刷新窗口，触发重绘
		update();
	}

	refreshStateLabel();
}


void CG_Painter::mouseReleaseEvent(QMouseEvent * event)
{
	//鼠标当前位置
	int x = event->pos().x();
	int y = event->pos().y();
	mouse_x = x;
	mouse_y = y;

	if (state != NOT_DRAWING && state != DRAW_CURVE) {
		if (event->button() == Qt::RightButton) {
			setState(NOT_DRAWING);
			return;
		}
	}
	
	if (state == NOT_DRAWING) {
		
		if (event->button() == Qt::LeftButton) {
			if (trans_state == TRANS_BEGIN) {
				trans_state = TRANS_NON;
				myCanvas = bufCanvas;
				buf_flag = false;
				update();
			}
		}
		else if(event->button() == Qt::RightButton){
			//正常状态下，右键单击，弹出菜单
			selected_ID = myCanvas.getID(x, y);
			if (selected_ID != -1 && myCanvas.getType(selected_ID) != CTRLPOINT) {
				//删除图元Action
				QAction* actionDelete = new QAction(tr(u8"删除"));
				actionDelete->setStatusTip(tr(u8"删除此图元"));
				connect(actionDelete, &QAction::triggered, this, &CG_Painter::action_to_delete);
				
				//添加菜单项
				QMenu menu;
				menu.addAction(actionDelete);
				//在鼠标位置显示
				menu.exec(QCursor::pos());
			}
		}
	}
	
	if (state == DRAW_LINE) {
		if (event->button() == Qt::LeftButton) {
			if (line_state == LINE_NON_POINT) {
				line_Ax = event->pos().x();
				line_Ay = event->pos().y();
				line_state = LINE_POINTA;
			}
			else if (line_state == LINE_POINTA) {
				line_Bx = event->pos().x();
				line_By = event->pos().y();
				myCanvas.drawLine(getNewID(), line_Ax, line_Ay, line_Bx, line_By, algorithm);
				setState(NOT_DRAWING);
			}
			
		}
	}
	else if (state == DRAW_POLYGON) {
		if (event->button() == Qt::LeftButton) {
			if (poly_state == POLY_NON_POINT) {
				startX = event->pos().x();
				startY = event->pos().y();
				bufCanvas = myCanvas;
				poly_points.push_back(Point(startX, startY));
				poly_state = POLY_PAINTING;
			}
			else if (poly_state == POLY_PAINTING) {
				if (autoPoly(x, y)) {//是否自动贴合判断
					poly_points.push_back(Point(x, y));
					myCanvas.drawPolygon(poly_id, poly_points, algorithm);
					setState(NOT_DRAWING);
				}
				else {
					poly_points.push_back(Point(x, y));
				}
				
			}
		}
		
	}
	else if (state == DRAW_ELLIPSE) {
		if (event->button() == Qt::LeftButton) {
			if (elli_state == ELLI_NON_POINT) {
				cx = x; cy = y;
				rx = DEFAULT_RX, ry = DEFAULT_RY;//长短轴
				bufCanvas = myCanvas;
				bufCanvas.drawEllipse(elli_id, cx, cy, rx, ry);
				buf_flag = true;
				update();
				elli_state = ELLI_PAINTING;
			}
			else if (elli_state == ELLI_PAINTING) {
				rx += changing_dx;
				ry += changing_dy;
			}
		}
	}
	else if (state == DRAW_ROTATE) {
		if (event->button() == Qt::LeftButton) {
			if (rotate_state == ROTATE_NON) {
				rotate_rx = x; rotate_ry = y;
				rotate_state = ROTATE_READY;
				bufCanvas = myCanvas;
				bufCanvas.drawDotPoint(-1, rotate_rx, rotate_ry);
				buf_flag = true;
				update();
			}else if (rotate_state == ROTATE_BEGIN) {
				rotate_state = ROTATE_READY;
				myCanvas = bufCanvas;
			}
		}
	}
	else if (state == DRAW_SCALE) {
		if (event->button() == Qt::LeftButton) {
			if (scale_state == SCALE_NON) {
				scale_rx = x; scale_ry = y;
				scale_state = SCALE_READY;
				bufCanvas = myCanvas;
				bufCanvas.drawDotPoint(-1, scale_rx, scale_ry);
				buf_flag = true;
				update();
			}
			else if (scale_state == SCALE_BEGIN) {
				scale_state = SCALE_READY;
				myCanvas = bufCanvas;
			}
		}
	}
	else if (state == DRAW_CLIP) {
		if (event->button() == Qt::LeftButton) {
			if (clip_state == CLIP_NON) {
				init_x = x; init_y = y;
				clip_state = CLIP_BEGIN;
			}
			else if (clip_state == CLIP_BEGIN) {
				myCanvas.clipAll(init_x, init_y, x, y, algorithm);
				setState(NOT_DRAWING);
			}
		}
	}
	else if (state == DRAW_CURVE) {
		if (event->button() == Qt::LeftButton) {
			curve_points.push_back(Point(x, y));
			bufCanvas = myCanvas;
			buf_flag = true;
			FoldLine* p = bufCanvas.drawFoldLine(-1, curve_points);
			for (size_t i = 0; i < curve_points.size(); i++) {
				bufCanvas.drawCtrlPoint(-1, i, p);
			}
			update();
		}
		else if (event->button() == Qt::RightButton) {
			if (curve_points.size() > 2) { //点的个数小于3时，曲线没有意义
				FoldLine *p = myCanvas.drawFoldLine(getNewID(), curve_points);
				myCanvas.drawCurve(getNewID(), algorithm, p);
				for (size_t i = 0; i < curve_points.size(); i++) {
					myCanvas.drawCtrlPoint(getNewID(), i, p);
				}
				setState(NOT_DRAWING);
				update();
			}
			else {
				setState(NOT_DRAWING);
				update();
			}
		}
	}


	refreshStateLabel();
}

void CG_Painter::mouseDoubleClickEvent(QMouseEvent * event)
{
	//鼠标当前位置
	int x = event->pos().x();
	int y = event->pos().y();

	if (state == DRAW_ELLIPSE) {
		if (event->button() == Qt::LeftButton) {
			if (elli_state == ELLI_PAINTING) {
				myCanvas = bufCanvas;
				setState(NOT_DRAWING);
				update();
			}
		}
	}
}

void CG_Painter::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	QPen* pen = new QPen;
	QImage *image;

	if (state == DRAW_RANDOM)
	{
		pen->setColor(CG_Painter::WColor);
		pen->setWidth(CG_Painter::WWidth);
		painter.setPen(*pen);
		painter.drawPath(drawingPath);
	}
	else
	{
		if (!buf_flag) {
			if (state == CLEAR)
			{
				myCanvas.clearAll();
				return;
			}
			image = new QImage(geometry().width(), geometry().height(), QImage::Format_RGB888);
			myCanvas.getIamge(image);
		}
		else {
			if (state == CLEAR)
			{
				bufCanvas.clearAll();
				return;
			}
			image = new QImage(geometry().width(), geometry().height(), QImage::Format_RGB888);
			bufCanvas.getIamge(image);
		}
		painter.drawImage(0, 0, *image);
		delete image;
	}
}

bool CG_Painter::autoPoly(int & nowx, int & nowy)
{
	if ((nowx - startX)*(nowx - startX) + (nowy - startY)*(nowy - startY) < AUTO_BIAS*AUTO_BIAS) {
		nowx = startX; nowy = startY;
		return true;
	}
	return false;
}

int CG_Painter::getNewID()
{
	return ID_Counter++;
}
