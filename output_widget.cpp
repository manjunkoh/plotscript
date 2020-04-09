#include "output_widget.hpp"
#include<cmath>

OutputWidget::OutputWidget(QWidget *parent) : QWidget(parent)
{
	scene = new QGraphicsScene();
	outputWindow = new QGraphicsView();
	outputWindow->setScene(scene);
	scene->setParent(outputWindow);
	outputWindow->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	outputWindow->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QVBoxLayout * layout = new QVBoxLayout();
	layout->addWidget(outputWindow);
	//outputWindow->centerOn(0, 0);
	setLayout(layout);
}

void OutputWidget::showResult(QString output)
{
	qDebug() << "outputWidget " << output;
	QGraphicsTextItem *resultText = new QGraphicsTextItem(output);
	QFont outputFont("Monospace");
	outputFont.setStyleHint(QFont::TypeWriter);
	outputFont.setPointSize(1);
	resultText->setFont(outputFont);//->addText(output);//, outputFont);
	scene->addItem(resultText);
	outputWindow->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void OutputWidget::plotPoint(std::vector<double> parameters)
{
	QPen blackPen(Qt::black);
	blackPen.setWidth(0);
	//std::vector<double> parameter(parameters.begin(), parameters.end());
	scene->addEllipse(parameters[0]-parameters[2]/2, parameters[1]-parameters[2]/2, parameters[2], parameters[2], blackPen ,blackBrush);
	outputWindow->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

//function

void OutputWidget::plotLine(std::vector<double> parameters)
{

	QPen blackPen(Qt::black);
	blackPen.setWidth(parameters[4]);
	scene->addLine(parameters[0], parameters[1], parameters[2], parameters[3], blackPen);
	outputWindow->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void OutputWidget::showText(QString text, double x, double y, double scale, double phi)
{
	qDebug() << "now we at showtext";
	qDebug() << "x: " << x << " y: " << y;
	QGraphicsTextItem *resultText = new QGraphicsTextItem(text);
	auto outputFont = QFont("Monospace");
	outputFont.setStyleHint(QFont::TypeWriter);
	outputFont.setPointSize(1);
	resultText->setFont(outputFont);
	resultText->setPlainText(text);
	resultText->setX(x - resultText->boundingRect().center().x());
	resultText->setY(y - resultText->boundingRect().center().y());
	resultText->setScale(scale);
	resultText->setTransformOriginPoint(resultText->boundingRect().center());
	resultText->setRotation(phi*(180 / (std::atan2(0, -1))));
	scene->addItem(resultText);
	outputWindow->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void OutputWidget::clearScreen()
{
	scene->clear();
}

void OutputWidget::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	outputWindow->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);	
}
