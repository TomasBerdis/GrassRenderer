#include "SettingsWidget.hpp"

SettingsWidget::SettingsWidget(QWidget* parent)
{
	QString style = "background-color: white;"
		"padding: 5px; border: 1px solid #44A41C;"
		"border-radius: 12px;";
	setStyleSheet(style);
	

	tessLabel = new QLabel("Tessellation level: ", this);
	tessLabel->setFont(QFont("Arial", 14, QFont::Bold));

	tessSlider = new QSlider(this);
	tessSlider->setMaximum(100.0f);
	tessSlider->setMinimum(0.0f);
	tessSlider->setValue(8.0f);
	tessSlider->setOrientation(Qt::Horizontal);
	QObject::connect(tessSlider, &QSlider::valueChanged, this, &SettingsWidget::changeTessLevel);
	
	rasterModeLabel = new QLabel("Rasterization mode: ", this);
	rasterModeLabel->setFont(QFont("Arial", 14, QFont::Bold));

	pointModeBtn = new QRadioButton("Point", this);
	lineModeBtn = new QRadioButton("Line", this);
	fillModeBtn = new QRadioButton("Fill", this);
	fillModeBtn->setChecked(true);
	QObject::connect(pointModeBtn, &QRadioButton::clicked, this, &SettingsWidget::togglePointRasterMode);
	QObject::connect(lineModeBtn, &QRadioButton::clicked, this, &SettingsWidget::toggleLineRasterMode);
	QObject::connect(fillModeBtn, &QRadioButton::clicked, this, &SettingsWidget::toggleFillRasterMode);

	gridLayout = new QGridLayout(this);
	gridLayout->addWidget(tessLabel, 0, 0);
	gridLayout->addWidget(tessSlider, 0, 1);
	gridLayout->addWidget(rasterModeLabel, 1, 0);
	gridLayout->addWidget(pointModeBtn, 1, 1);
	gridLayout->addWidget(lineModeBtn, 1, 2);
	gridLayout->addWidget(fillModeBtn, 1, 3);
}

void SettingsWidget::changeTessLevel(int newTessLevel)
{
	emit tessLevelChanged(newTessLevel);
}

void SettingsWidget::togglePointRasterMode()
{
	emit rasterizationModeChanged(GL_POINT);
}

void SettingsWidget::toggleLineRasterMode()
{
	emit rasterizationModeChanged(GL_LINE);
}

void SettingsWidget::toggleFillRasterMode()
{
	emit rasterizationModeChanged(GL_FILL);
}
