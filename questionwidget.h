#ifndef QUESTIONWIDGET_H
#define QUESTIONWIDGET_H

#include <QHash>
#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

namespace Ui {
class QuestionWidget;
}

class QuestionWidget : public QWidget
{
	Q_OBJECT

public:
	explicit QuestionWidget(QWidget *parent = 0);
	~QuestionWidget();

private slots:
	void add();
	void on_lineTitle_returnPressed();

	void on_plainChoices_textChanged();

private:
	Ui::QuestionWidget *ui;
	QJsonArray questions;
	QJsonObject currentQ;
};

#endif // QUESTIONWIDGET_H
