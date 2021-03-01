#include "SettingsWidget.hpp"

#include <iostream>
#include <QDebug>
#include "OpenGLWindow.hpp"

SettingsWidget::SettingsWidget(QWindow* parent)
{
	tessLabel = new QLabel("Tessellation level: ", this);
	tessLabel->setFont(QFont("Arial", 14, QFont::Bold));

	tessSlider = new QSlider(this);
	tessSlider->setMaximum(100.0f);
	tessSlider->setMinimum(0.0f);
	tessSlider->setValue(8.0f);
	tessSlider->setOrientation(Qt::Horizontal);
	QObject::connect(tessSlider, &QSlider::valueChanged, this, &SettingsWidget::changeTessLevel);

	gridLayout = new QGridLayout(this);
	gridLayout->addWidget(tessLabel, 0, 0);
	gridLayout->addWidget(tessSlider, 0, 1);
}

void SettingsWidget::changeTessLevel(int newTessLevel)
{
	emit tessLevelChanged(newTessLevel);
}