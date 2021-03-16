#pragma once

#include <QtGui/QWindow>
#include <QtGui/QOpenGLContext>
#include <QtGui/qevent.h>
#include <QtCore/qelapsedtimer.h>
#include <QtCore/qtimer.h>

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
	void keyPressEvent(QKeyEvent* event);
	float sign(float f);

private:
	bool initialized;
	int tessLevel	 = 8;
	int windowWidth  = 0;
	int windowHeight = 0;

	QElapsedTimer timer;
	QTimer *tickTimer;

	GLenum rasterizationMode = GL_FILL;

	std::shared_ptr<ge::gl::Buffer> grassPositionBuffer;
	std::shared_ptr<ge::gl::Buffer> grassCenterPositionBuffer;
	std::shared_ptr<ge::gl::Buffer> grassElementBuffer;
	std::shared_ptr<ge::gl::Buffer> terrainPositionBuffer;
	std::shared_ptr<ge::gl::Buffer> terrainElementBuffer;
	std::shared_ptr<ge::gl::Buffer> dummyPositionBuffer;

	std::shared_ptr<ge::gl::Context>	 gl;
	std::shared_ptr<ge::gl::Program>	 grassShaderProgram;
	std::shared_ptr<ge::gl::Program>	 terrainShaderProgram;
	std::shared_ptr<ge::gl::Program>	 dummyShaderProgram;
	std::shared_ptr<ge::gl::VertexArray> grassVAO;
	std::shared_ptr<ge::gl::VertexArray> terrainVAO;
	std::shared_ptr<ge::gl::VertexArray> dummyVAO;
	QOpenGLContext* context;
	QSurfaceFormat  surfaceFormat;

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 mvp;

	glm::vec3 cameraPos	  = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp	  = glm::vec3(0.0f, 1.0f, 0.0f);
	float yaw   = 0.0;
	float pitch = 0.0;
	float fov	= 45.0;

	QPointF clickStartPos;
};