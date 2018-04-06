#include "questionwidget.h"
#include "translationwidget.h"
#include "quizwidget.h"
#include "quizmodel.h"

#include <QApplication>

#define TARGET_DIR "/home/amenmd/myfs/source-codes/bilkon/go/src/github.com/yca/survey/questions/final/c"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//QuestionWidget w;
	//TranslationWidget w;
	QuizModel model;
	QuizWidget w2(&model, "tr");
	QuizWidget w(&model);
	w.show();
	w.setShowAnswer(true);
	w2.show();

	model.reloadQuestions(TARGET_DIR);
	return a.exec();
}
