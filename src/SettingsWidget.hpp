#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QSlider>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>

#include <geGL/geGL.h>

class SettingsWidget : public QWidget
{
	Q_OBJECT
public:
	SettingsWidget(QWidget* parent);

	QGridLayout *gridLayout;
	QLabel *tessLabel;
	QLabel *rasterModeLabel;
	QSlider *tessSlider;
	QRadioButton *pointModeBtn;
	QRadioButton *lineModeBtn;
	QRadioButton *fillModeBtn;
private slots:
	void changeTessLevel(int newTessLevel);
	void togglePointRasterMode();
	void toggleLineRasterMode();
	void toggleFillRasterMode();
signals:
	void tessLevelChanged(int newTessLevel);
	void rasterizationModeChanged(GLenum newMode);
};