#ifndef TRANSLATIONWIDGET_H
#define TRANSLATIONWIDGET_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

namespace Ui {
class TranslationWidget;
}

class TranslationWidget : public QWidget
{
	Q_OBJECT

public:
	explicit TranslationWidget(QWidget *parent = 0);
	~TranslationWidget();

protected:
	void loadCurrent();
	void loadNextEmpty();
	void reloadQuestions();
	void saveTranslations();
	void loadTranslations();
private slots:
	void skip();
	void on_lineFolder_returnPressed();

	void on_pushNext_clicked();

	void on_pushPrev_clicked();

	void on_pushNextEmpty_clicked();

private:
	Ui::TranslationWidget *ui;
	QJsonArray questions;
	int current;
	QString currentJson;
	QStringList queries;
	QHash<QString, QString> translations;
};

#endif // TRANSLATIONWIDGET_H
