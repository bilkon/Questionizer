#include "questionwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QuestionWidget w;
	w.show();

	return a.exec();
}
