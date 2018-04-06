#ifndef QUIZMODEL_H
#define QUIZMODEL_H

#include <QHash>
#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>
#include <QJsonDocument>

class QuizModel : public QObject
{
	Q_OBJECT
public:
	explicit QuizModel(QObject *parent = 0);

	QString question();
	QStringList choicesList();
	QString html();
	QString answerText();
	int questionNo();

	void reloadQuestions(const QString &targetDir);
signals:
	void switchedToNext();
	void switchedToPrev();
	void questionLoaded();
public slots:
	void next();
	void prev();
	void gotoQuestion(int q);

protected:
	void load();

	int current;
	QString answer;
	QString questionTitle;
	QStringList choices;
	QJsonArray questions;
	QString htmlPart;
	QString loadDir;
};

#endif // QUIZMODEL_H
