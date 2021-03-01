#pragma once

#include <QtGui/QWindow>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QSlider>
#include <QtWidgets/QLabel>

class SettingsWidget : public QWidget
{
	Q_OBJECT
public:
	SettingsWidget(QWindow* parent);

	QGridLayout *gridLayout;
	QLabel *tessLabel;
	QSlider *tessSlider;
private slots:
	void changeTessLevel(int newTessLevel);
signals:
	void tessLevelChanged(int newTessLevel);
};