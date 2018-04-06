#include "translationwidget.h"
#include "ui_translationwidget.h"

#include <QDir>
#include <QDebug>
#include <QShortcut>

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

TranslationWidget::TranslationWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::TranslationWidget)
{
	ui->setupUi(this);
	loadTranslations();
	reloadQuestions();

	new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this, SLOT(on_pushNextEmpty_clicked()));
	new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Backspace), this, SLOT(on_pushPrev_clicked()));
	new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Space), this, SLOT(skip()));
}

TranslationWidget::~TranslationWidget()
{
	delete ui;
}

void TranslationWidget::skip()
{
	current++;
	ui->lineText->setText("__skipped__");
	loadNextEmpty();
}

void TranslationWidget::loadCurrent()
{
	if (!ui->lineText->text().isEmpty())
		translations.insert(ui->labelText->text(), ui->lineText->text());

	ui->lineText->setFocus();

#ifdef ITERATE_JSON
	QJsonObject obj = questions[current].toObject();
	if (obj.contains("title"))
		ui->labelText->setText(obj["title"].toString());
	else if (obj["type"].toString() == "html") {
#if 0
		QString fname = QString("%1/%2").arg(ui->lineFolder->text()).arg(obj["meta"].toString());
		QFile f(fname);
		f.open(QIODevice::ReadOnly);
		QStringList lines = QString::fromUtf8(f.readAll()).split("\n");
		f.close();
#endif
		ui->labelText->setText(obj["meta"].toString());
	}
#else
	ui->labelText->setText(queries[current]);
#endif

	if (translations.contains(ui->labelText->text()))
		ui->lineText->setText(translations[ui->labelText->text()]);
	else
		ui->lineText->setText("");
	ui->lineText->setSelection(0, ui->lineText->text().size());

	saveTranslations();

	ui->progressBar->setMinimum(0);
	ui->progressBar->setValue(current);
	ui->progressBar->setMaximum(queries.size());
}

void TranslationWidget::loadNextEmpty()
{
#ifdef ITERATE_JSON
	while (current < questions.size()) {
		QJsonObject obj = questions[current].toObject();
		if (obj.contains("title"))
			ui->labelText->setText(obj["title"].toString());
		else if (obj["type"].toString() == "html")
			ui->labelText->setText(obj["meta"].toString());
		if (!translations.contains(ui->labelText->text()))
			break;
		current++;
	}
#else
	while (current < queries.size()) {
		if (!translations.contains(queries[current]))
			break;
		current++;
	}
#endif

	loadCurrent();
}

void TranslationWidget::reloadQuestions()
{
	questions = QJsonArray();
	QStringList files;
	listFiles(ui->lineFolder->text(), files);
	QStringList jsons;
	foreach (const QString &file, files)
		if (file.endsWith(".json"))
			jsons << file;

	foreach (QString file, jsons) {
		QFile f(file);
		f.open(QIODevice::ReadOnly);
		QByteArray ba = f.readAll();
		f.close();
		QJsonObject obj = QJsonDocument::fromJson(ba).object();
		QJsonArray q = obj["questions"].toArray();
		foreach (QJsonValue v, q)
			questions.append(v);
	}

	foreach (QJsonValue v, questions) {
		QJsonObject q = v.toObject();
		if (q["type"] == "html") {
			continue;
		}
		queries << q["title"].toString();
		foreach (QJsonValue v2, q["choices"].toArray())
			queries << v2.toString();
	}

	current = 0;
	loadNextEmpty();
}

void TranslationWidget::saveTranslations()
{
	QJsonArray arr;
	QHashIterator<QString, QString> hi(translations);
	while (hi.hasNext()) {
		hi.next();
		QJsonObject obj;
		obj.insert("original", hi.key());
		obj.insert("translated", hi.value());
		arr.append(obj);
	}
	QFile f("translations.json");
	f.open(QIODevice::WriteOnly);
	f.write(QJsonDocument(arr).toJson());
	f.close();
}

void TranslationWidget::loadTranslations()
{
	QFile f("translations.json");
	f.open(QIODevice::ReadOnly);
	QJsonArray arr = QJsonDocument::fromJson(f.readAll()).array();
	f.close();
	foreach (QJsonValue v, arr) {
		QJsonObject obj = v.toObject();
		translations.insert(obj["original"].toString(), obj["translated"].toString());
	}
}

void TranslationWidget::on_lineFolder_returnPressed()
{
	reloadQuestions();
}

void TranslationWidget::on_pushNext_clicked()
{
	current++;
	loadCurrent();
}

void TranslationWidget::on_pushPrev_clicked()
{
	current--;
	loadCurrent();
}

void TranslationWidget::on_pushNextEmpty_clicked()
{
	current++;
	loadNextEmpty();
}
