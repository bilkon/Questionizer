#ifndef QUIZWIDGET_H
#define QUIZWIDGET_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

namespace Ui {
class QuizWidget;
}

class QuizModel;

class QuizWidget : public QWidget
{
	Q_OBJECT

public:
	explicit QuizWidget(QuizModel *m, QString language = "en", QWidget *parent = 0);
	void setShowAnswer(bool v);
	~QuizWidget();

protected:
	QString translate(const QString &text);
private slots:
	void load();
	bool eventFilter(QObject *obj, QEvent *ev);
	void on_pushNext_clicked();

	void on_pushPrev_clicked();

	void on_pushGoTo_clicked();

private:
	Ui::QuizWidget *ui;
	QuizModel *model;
	bool showAnswer;
	QHash<QString, QString> translationsNone;
	QString lang;
};

#endif // QUIZWIDGET_H
