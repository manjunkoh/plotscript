#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLayout>
#include <QDebug>
#include <QGraphicsTextItem>
#include <QFont>
#include <QGraphicsItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QPushButton>

class OutputWidget : public QWidget {

	Q_OBJECT

public: 

	OutputWidget(QWidget * parent = nullptr);

	public slots:

	void showResult(QString output);
	void plotPoint(std::vector<double> parameters);
	void plotLine(std::vector<double> parameters);
	void showText(QString text, double x, double y,double scale,double phi);
	void clearScreen();
private:

	QGraphicsScene * scene;
	QGraphicsView * outputWindow;
	QBrush blackBrush = Qt::black;
	QPainter * plots;

protected:
	void resizeEvent(QResizeEvent* event);

};





#endif // !OUTPUTWIDGET_H

