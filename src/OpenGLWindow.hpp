#pragma once

#include <QtGui/QWindow>
#include <QtGui/QOpenGLContext>

#include <geGL/geGL.h>
#include <geUtil/Text.h>

#include <memory>
#include <iostream>

#include "SettingsWidget.hpp"

class OpenGLWindow : public QWindow
{
Q_OBJECT
public:
	explicit OpenGLWindow(QWindow *parent = nullptr);

	~OpenGLWindow();

	void render();
	void printError() const;
	void initialize();

	SettingsWidget *settingsWidget;

public slots:
	void renderNow();
	void setTessLevel(int tessLevel);

protected:
	bool event(QEvent* event) override;
	void exposeEvent(QExposeEvent* event) override;

private:
	bool initialized;
	int tessLevel = 8;

	std::shared_ptr<ge::gl::Buffer> positionBuffer;
	std::shared_ptr<ge::gl::Buffer> centerPositionBuffer;
	std::shared_ptr<ge::gl::Buffer> elementBuffer;

	std::shared_ptr<ge::gl::Context> gl;
	std::shared_ptr<ge::gl::Program> shaderProgram;
	std::shared_ptr<ge::gl::VertexArray> VAO;
	QOpenGLContext* context;
	QSurfaceFormat surfaceFormat;
};