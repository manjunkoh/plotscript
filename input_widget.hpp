#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QWidget>
#include <QPlainTextEdit> 
#include <QLayout>
#include <QDebug>
#include <QKeyEvent>

class InputWidget : public QPlainTextEdit {

	Q_OBJECT

public:

	InputWidget(QWidget * parent = nullptr);

signals:

	void sendToPlotScript(QString arguments);

private slots:



private:

	QString inputText;
protected:
	void keyPressEvent(QKeyEvent *e);

};

#endif