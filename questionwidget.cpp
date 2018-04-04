#include "questionwidget.h"
#include "ui_questionwidget.h"

#include <QUuid>
#include <QDebug>
#include <QShortcut>

QuestionWidget::QuestionWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QuestionWidget)
{
	ui->setupUi(this);

	new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this, SLOT(add()));
}

QuestionWidget::~QuestionWidget()
{
	delete ui;
}

void QuestionWidget::add()
{
	if (!ui->plainHtml->toPlainText().isEmpty()) {
		QJsonObject h;
		h.insert("type", "html");
		QString uuid = QUuid::createUuid().toString().remove("{").remove("}").split("-").first();
		QString filename = QString("%1.c").arg(uuid);
		h.insert("meta", filename);
		QFile f("/home/amenmd/myfs/source-codes/bilkon/go/src/github.com/yca/survey/questions/staging/c/" + filename);
		f.open(QIODevice::WriteOnly);
		f.write(ui->plainHtml->toPlainText().toUtf8());
		f.close();
		questions.append(h);
	}
	questions.append(currentQ);
	currentQ = QJsonObject();
	ui->plainJson->clear();
	ui->plainChoices->clear();
	ui->lineTitle->setSelection(0, ui->lineTitle->text().size());
	ui->lineTitle->setFocus();
	ui->plainHtml->clear();

	QJsonObject p;
	p.insert("questions", questions);
	ui->plainAllJson->setPlainText(QString::fromUtf8(QJsonDocument(p).toJson()));
}

void QuestionWidget::on_lineTitle_returnPressed()
{
	ui->plainChoices->setFocus();
}

void QuestionWidget::on_plainChoices_textChanged()
{
	/* make-up choices and answer */
	QStringList lines = ui->plainChoices->toPlainText().split("\n");

	if (lines.first().trimmed().isEmpty())
		lines.removeFirst();
	if (lines.size() > 1 && lines[1].trimmed() == "") {
		ui->lineTitle->setText(lines[0].trimmed());
		lines.removeFirst();
		lines.removeFirst();
	}
	QStringList choices;
	QString answer;
	foreach (QString line, lines) {
		line = line.trimmed();
		if (line.startsWith("A.")
				|| line.startsWith("B.")
				|| line.startsWith("C.")
				|| line.startsWith("D.")
				|| line.startsWith("E.")
				|| line.startsWith("F.")
				)
			line = line.mid(2);
		line = line.trimmed();
		if (line.isEmpty())
			continue;

		if (line.startsWith("!"))
			answer = line.mid(1);
		else if (line.startsWith("Answer: Option"))
			answer = line.remove("Answer: Option").trimmed();
		else
			choices << line;
	}

	/* update question */
	QJsonObject q = currentQ;
	q.insert("answer", answer);
	q.insert("type", "radiogroup");
	q.insert("title", ui->lineTitle->text());
	q.insert("colCount", 1);
	QJsonArray arr;
	foreach (QString c, choices)
		arr.append(c);
	q.insert("choices", arr);
	currentQ = q;

	/* update json */
	ui->plainJson->setPlainText(QString::fromUtf8(QJsonDocument(q).toJson()));
}
