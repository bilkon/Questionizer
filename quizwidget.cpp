#include "quizwidget.h"
#include "ui_quizwidget.h"
#include "highlighter.h"

#include <QDir>
#include <QDebug>
#include <QShortcut>
#include <QInputDialog>

#define TARGET_DIR "/home/amenmd/myfs/source-codes/bilkon/go/src/github.com/yca/survey/questions/final/c"

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

QuizWidget::QuizWidget(QuizWidget *child, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QuizWidget)
{
	chw = child;
	ui->setupUi(this);
	loadTranslations();
	reloadQuestions();

	new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this, SLOT(on_pushNext_clicked()));
	new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Backspace), this, SLOT(on_pushPrev_clicked()));
	//new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Space), this, SLOT(skip()));

	new Highlighter(ui->textHtml->document());

	ui->radioChoice_1->installEventFilter(this);
	ui->radioChoice_2->installEventFilter(this);
	ui->radioChoice_3->installEventFilter(this);
	ui->radioChoice_4->installEventFilter(this);
	ui->radioChoice_5->installEventFilter(this);
	ui->radioChoice_6->installEventFilter(this);
	ui->labelQuestion->installEventFilter(this);
}

void QuizWidget::setChilde(QuizWidget *w)
{
	chw = w;
}

QuizWidget::~QuizWidget()
{
	delete ui;
}

bool QuizWidget::isTr()
{
	if (chw)
		return true;
	return false;
}

void QuizWidget::loadTranslations()
{
	QFile f("translations.json");
	f.open(QIODevice::ReadOnly);
	QJsonArray arr = QJsonDocument::fromJson(f.readAll()).array();
	f.close();
	foreach (QJsonValue v, arr) {
		QJsonObject obj = v.toObject();
		translationsNone.insert(obj["original"].toString(), obj["translated"].toString());
	}
}

void QuizWidget::reloadQuestions()
{
	questions = QJsonArray();
	QStringList files;
	listFiles(TARGET_DIR, files);
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

	current = 0;
	load();
}

#define hideRadio(_r) \
if (_r->text().isEmpty()) \
	_r->hide(); \
else \
	_r->show()

void QuizWidget::load()
{
	int index = current;
	QJsonObject q = questions[index].toObject();

	if (q["type"] == "html") {
		QFile f(QString("%1/%2").arg(TARGET_DIR).arg(q["meta"].toString()));
		f.open(QIODevice::ReadOnly);
		lastHtml = QString::fromUtf8(f.readAll());
		f.close();
		current++;
		return load();
	}

	QString title = translate(q["title"].toString());
	ui->labelQuestion->setText(QString("%1: %2").arg(current + 1).arg(title));
	ui->textHtml->setText("");
	QJsonArray chen = q["choices"].toArray();
	QJsonArray ch;
	for (int i = 0; i < chen.size(); i++) {
		QString choice = chen[i].toString();
		ch.append(translate(choice));
	}
	ui->radioChoice_1->setText(ch.size() < 1 ? "" : ch[0].toString());
	ui->radioChoice_2->setText(ch.size() < 2 ? "" : ch[1].toString());
	ui->radioChoice_3->setText(ch.size() < 3 ? "" : ch[2].toString());
	ui->radioChoice_4->setText(ch.size() < 4 ? "" : ch[3].toString());
	ui->radioChoice_5->setText(ch.size() < 5 ? "" : ch[4].toString());
	ui->radioChoice_6->setText(ch.size() < 6 ? "" : ch[5].toString());
	hideRadio(ui->radioChoice_1);
	hideRadio(ui->radioChoice_2);
	hideRadio(ui->radioChoice_3);
	hideRadio(ui->radioChoice_4);
	hideRadio(ui->radioChoice_5);
	hideRadio(ui->radioChoice_6);
	if (lastHtml.size()) {
		if (lastHtml.contains("#include")) {
			ui->textHtml->setPlainText(lastHtml);
		} else {
			QStringList lines2;
			QStringList lines = lastHtml.split("\n");
			foreach (QString line, lines) {
				lines2 << translate(line);
			}
			ui->textHtml->setPlainText(lines2.join("\n"));

		}
		ui->textHtml->show();
	} else {
		ui->textHtml->hide();
	}

	if (!chw) {
		QString chs = q["answer"].toString().toLower();
		if (chs == "a")
			ui->radioChoice_1->setChecked(true);
		if (chs == "b")
			ui->radioChoice_2->setChecked(true);
		if (chs == "c")
			ui->radioChoice_3->setChecked(true);
		if (chs == "d")
			ui->radioChoice_4->setChecked(true);
		if (chs == "e")
			ui->radioChoice_5->setChecked(true);
	}
}

QString QuizWidget::translate(const QString &text)
{
	if (!isTr())
		return text;
	if (!translationsNone.contains(text))
		return text;
	QString text2 = translationsNone[text];
	if (text2.startsWith("__"))
		return text;
	return text2;
}

bool QuizWidget::eventFilter(QObject *obj, QEvent *ev)
{
	if (ev->type() != QEvent::MouseButtonDblClick)
		return false;
	QString query;
	QRadioButton *radio = qobject_cast<QRadioButton *>(obj);
	if (radio) {
		query = radio->text();
	}

	QLabel *label = qobject_cast<QLabel *>(obj);
	if (label) {
		query = label->text();
		query = query.mid(query.indexOf(". "));
	}

	QString trans = QInputDialog::getText(this, "", "", QLineEdit::Normal, query);
	if (!trans.isEmpty()) {
		translationsNone.insert(query, trans);
		QJsonArray arr;
		QHashIterator<QString, QString> hi(translationsNone);
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

	return false;
}

void QuizWidget::on_pushNext_clicked()
{
	lastHtml.clear();
	current++;
	load();

	if (chw) {
		chw->current++;
		chw->load();
	}
}

void QuizWidget::on_pushPrev_clicked()
{
	lastHtml.clear();
	current--;
	load();
}

void QuizWidget::on_pushGoTo_clicked()
{
	int val = QInputDialog::getInt(this, "", "");
	if (!val)
		return;
	current = val;
	load();
	if (chw) {
		chw->current = val;
		chw->load();
	}
}
