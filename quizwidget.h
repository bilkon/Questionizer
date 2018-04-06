#ifndef QUIZWIDGET_H
#define QUIZWIDGET_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

namespace Ui {
class QuizWidget;
}

class QuizWidget : public QWidget
{
	Q_OBJECT

public:
	explicit QuizWidget(QuizWidget *child, QWidget *parent = 0);
	void setChilde(QuizWidget *w);
	~QuizWidget();

protected:
	bool isTr();
	void loadTranslations();
	void reloadQuestions();
	void load();
	QString translate(const QString &text);
private slots:
	bool eventFilter(QObject *obj, QEvent *ev);
	void on_pushNext_clicked();

	void on_pushPrev_clicked();

	void on_pushGoTo_clicked();

private:
	Ui::QuizWidget *ui;
	QJsonArray questions;
	int current;
	QString lastHtml;
	QHash<QString, QString> translationsNone;
	QuizWidget *chw;
};

#endif // QUIZWIDGET_H
