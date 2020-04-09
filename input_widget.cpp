#include "input_widget.hpp"


InputWidget::InputWidget(QWidget *parent) :QPlainTextEdit(parent) {

	qDebug() << "Inputwidget opened";
	zoomIn(2);
}


// this allows the text edit to not evaluate the inputs unless shift+return are pressed
void InputWidget::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Return && e->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier))
	{
		QString text = toPlainText();
		qDebug() << "From input widget " << text;
		emit sendToPlotScript(text);
		//clear();
	}
	else
		QPlainTextEdit::keyPressEvent(e);
}