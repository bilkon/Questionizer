#include "quizwidget.h"
#include "ui_quizwidget.h"
#include "highlighter.h"
#include "quizmodel.h"

#include <QDir>
#include <QDebug>
#include <QShortcut>
#include <QInputDialog>

QuizWidget::QuizWidget(QuizModel *m, QString language, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QuizWidget)
{
	lang = language;
	QFile f("translations.json");
	f.open(QIODevice::ReadOnly);
	QJsonArray arr = QJsonDocument::fromJson(f.readAll()).array();
	f.close();
	foreach (QJsonValue v, arr) {
		QJsonObject obj = v.toObject();
		translationsNone.insert(obj["original"].toString(), obj["translated"].toString());
	}

	model = m;
	ui->setupUi(this);

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

	showAnswer = false;
	connect(model, SIGNAL(questionLoaded()), SLOT(load()));
}

void QuizWidget::setShowAnswer(bool v)
{
	showAnswer = v;
}

QuizWidget::~QuizWidget()
{
	delete ui;
}

#define hideRadio(_r) \
if (_r->text().isEmpty()) \
	_r->hide(); \
else \
	_r->show()

void QuizWidget::load()
{
	ui->labelQuestion->setText(QString("%1. %2").arg(model->questionNo()).arg(translate(model->question())));
	ui->textHtml->setText(translate(model->html()));
	QStringList ch = model->choicesList();
	ui->radioChoice_1->setText(ch.size() < 1 ? "" : translate(ch[0]));
	ui->radioChoice_2->setText(ch.size() < 2 ? "" : translate(ch[1]));
	ui->radioChoice_3->setText(ch.size() < 3 ? "" : translate(ch[2]));
	ui->radioChoice_4->setText(ch.size() < 4 ? "" : translate(ch[3]));
	ui->radioChoice_5->setText(ch.size() < 5 ? "" : translate(ch[4]));
	ui->radioChoice_6->setText(ch.size() < 6 ? "" : translate(ch[5]));
	hideRadio(ui->radioChoice_1);
	hideRadio(ui->radioChoice_2);
	hideRadio(ui->radioChoice_3);
	hideRadio(ui->radioChoice_4);
	hideRadio(ui->radioChoice_5);
	hideRadio(ui->radioChoice_6);
	if (model->html().size())
		ui->textHtml->show();
	else
		ui->textHtml->hide();

	if (showAnswer) {
		QString chs = model->answerText().toLower();
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
	if (lang == "en")
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

	if (lang == "en") {
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
	}

	return false;
}

void QuizWidget::on_pushNext_clicked()
{
	model->next();
}

void QuizWidget::on_pushPrev_clicked()
{
	model->prev();
}

void QuizWidget::on_pushGoTo_clicked()
{
	int val = QInputDialog::getInt(this, "", "");
	if (!val)
		return;
	model->gotoQuestion(val);
}
