#include "questionwidget.h"
#include "translationwidget.h"
#include "quizwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//QuestionWidget w;
	//TranslationWidget w;
	QuizWidget w2(NULL);
	QuizWidget w(&w2);
	w.show();
	w2.show();

	return a.exec();
}
