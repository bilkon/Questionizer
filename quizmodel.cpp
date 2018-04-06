#include "quizmodel.h"

#include <QDir>
#include <QFile>
#include <QDebug>

static void listFiles(QString folder, QStringList &list)
{
	QDir dir(folder);
	QStringList files = dir.entryList(QStringList(), QDir::NoDotAndDotDot | QDir::Files);
	foreach (const QString &file, files)
		list << dir.filePath(file);
	files = dir.entryList(QStringList(), QDir::NoDotAndDotDot | QDir::Dirs);
	foreach (const QString &d, files)
		listFiles(dir.filePath(d), list);
}

QuizModel::QuizModel(QObject *parent)
	: QObject(parent)
{
	current = 0;
}

QString QuizModel::question()
{
	return questionTitle;
}

QStringList QuizModel::choicesList()
{
	return choices;
}

QString QuizModel::html()
{
	return htmlPart;
}

QString QuizModel::answerText()
{
	return answer;
}

int QuizModel::questionNo()
{
	return current + 1;
}

void QuizModel::reloadQuestions(const QString &targetDir)
{
	loadDir = targetDir;
	questions = QJsonArray();
	QStringList files;
	listFiles(targetDir, files);
	QStringList jsons;
	foreach (const QString &file, files)
		if (file.endsWith(".json"))
			jsons << file;

	QJsonObject lastHtmlQuestion;
	foreach (QString file, jsons) {
		qDebug() << file;
		QFile f(file);
		f.open(QIODevice::ReadOnly);
		QByteArray ba = f.readAll();
		f.close();
		QJsonObject obj = QJsonDocument::fromJson(ba).object();
		QJsonArray q = obj["questions"].toArray();
		for (int i = 0; i < q.size(); i++) {
			QJsonValue v = q[i];
			QJsonObject obj = v.toObject();
			if (obj["type"] == "html") {
				lastHtmlQuestion = obj;
			} else {
				if (!lastHtmlQuestion.isEmpty()) {
					QJsonObject obj = v.toObject();
					obj.insert("_html", lastHtmlQuestion);
					v = obj;
					lastHtmlQuestion = QJsonObject();
				}
				questions.append(v);
			}
		}
	}

	current = 0;
	load();
}

void QuizModel::next()
{
	if (current == questions.size() - 1)
		return;
	current++;
	load();
}

void QuizModel::prev()
{
	if (!current)
		return;
	current--;
	load();
}

void QuizModel::gotoQuestion(int q)
{
	if (q < 1 || q > questions.size())
		return;
	current = q - 1;
	load();
}

void QuizModel::load()
{
	int index = current;
	QJsonObject q = questions[index].toObject();

	htmlPart = "";
	if (q.contains("_html")) {
		QJsonObject hobj = q["_html"].toObject();
		QFile f(QString("%1/%2").arg(loadDir).arg(hobj["meta"].toString()));
		f.open(QIODevice::ReadOnly);
		htmlPart = QString::fromUtf8(f.readAll());
		f.close();
	}

	questionTitle = q["title"].toString();
	QJsonArray chen = q["choices"].toArray();
	choices.clear();
	for (int i = 0; i < chen.size(); i++)
		choices << chen[i].toString();

	answer = q["answer"].toString().toLower();

	emit questionLoaded();
}

