#pragma once

#include <QtGui/QWindow>
#include <QtGui/QOpenGLContext>
#include <QtGui/qevent.h>

#include <geGL/geGL.h>
#include <geUtil/Text.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
	void setRasterizationMode(GLenum mode);

protected:
	bool event(QEvent* event) override;
	void exposeEvent(QExposeEvent* event) override;
	void wheelEvent(QWheelEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	float sign(float f);

private:
	bool initialized;
	int tessLevel = 8;
	int windowWidth = 0;
	int windowHeight = 0;

	GLenum rasterizationMode = GL_FILL;

	std::shared_ptr<ge::gl::Buffer> grassPositionBuffer;
	std::shared_ptr<ge::gl::Buffer> grassCenterPositionBuffer;
	std::shared_ptr<ge::gl::Buffer> grassElementBuffer;
	std::shared_ptr<ge::gl::Buffer> terrainPositionBuffer;
	std::shared_ptr<ge::gl::Buffer> terrainElementBuffer;

	std::shared_ptr<ge::gl::Context> gl;
	std::shared_ptr<ge::gl::Program> grassShaderProgram;
	std::shared_ptr<ge::gl::Program> terrainShaderProgram;
	std::shared_ptr<ge::gl::VertexArray> grassVAO;
	std::shared_ptr<ge::gl::VertexArray> terrainVAO;
	QOpenGLContext* context;
	QSurfaceFormat surfaceFormat;

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 mvp;

	QPointF clickStartPos;
};