#pragma once

#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QtGui/qevent.h>
#include <QtCore/qelapsedtimer.h>
#include <QtCore/qtimer.h>
#include <QOpenGLFunctions_4_5_Core>
#include <QDebug>
#include <QImage>
#include <QOpenGLTexture>

#include <geGL/geGL.h>
#include <geGL/Texture.h>
#include <geUtil/Text.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include <QtImGui.h>
#include <imgui.h>

#include <memory>
#include <iostream>

#include "SettingsWidget.hpp"
#include "Camera.hpp"
#include "GrassField.hpp"

class OpenGLWindow : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
	Q_OBJECT
public:
	explicit OpenGLWindow();
	~OpenGLWindow();

	SettingsWidget *settingsWidget;

public slots:
	void setTessLevel(int tessLevel);
	void setRasterizationMode(GLenum mode);
	void tick();

protected:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

	void printError() const;
	void drawGui();

	/* Event handlers */
	void wheelEvent(QWheelEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void keyPressEvent(QKeyEvent* event);

	unsigned int loadSkybox(std::vector<QString> faces);

private:
	bool initialized;
	int tessLevel	 = 5;
	float maxBendingFactor = 1.5;
	int windowWidth;
	int windowHeight;

	int windEnabled = 1;

	Camera *camera;
	GrassField *grassField;

	QTimer *tickTimer;
	QElapsedTimer timer;

	GLenum rasterizationMode = GL_FILL;

	std::shared_ptr<ge::gl::Buffer> grassPositionBuffer;
	std::shared_ptr<ge::gl::Buffer> grassCenterPositionBuffer;
	std::shared_ptr<ge::gl::Buffer> grassTexCoordBuffer;
	std::shared_ptr<ge::gl::Buffer> grassRandomsBuffer;
	std::shared_ptr<ge::gl::Buffer> terrainPositionBuffer;
	std::shared_ptr<ge::gl::Buffer> terrainIndexBuffer;
	std::shared_ptr<ge::gl::Buffer> terrainTexCoordBuffer;
	std::shared_ptr<ge::gl::Buffer> dummyPositionBuffer;
	std::shared_ptr<ge::gl::Buffer> dummyTexCoordBuffer;
	std::shared_ptr<ge::gl::Buffer> skyboxPositionBuffer;
	std::shared_ptr<ge::gl::Buffer> patchTransSSBO;

	std::shared_ptr<ge::gl::Context>	 gl;

	std::shared_ptr<ge::gl::Program>	 grassShaderProgram;
	std::shared_ptr<ge::gl::Program>	 terrainShaderProgram;
	std::shared_ptr<ge::gl::Program>	 dummyShaderProgram;
	std::shared_ptr<ge::gl::Program>	 skyboxShaderProgram;

	std::shared_ptr<ge::gl::VertexArray> grassVAO;
	std::shared_ptr<ge::gl::VertexArray> terrainVAO;
	std::shared_ptr<ge::gl::VertexArray> dummyVAO;
	std::shared_ptr<ge::gl::VertexArray> skyboxVAO;

	QOpenGLContext* context;
	QSurfaceFormat  surfaceFormat;

	QPointF clickStartPos;

	QOpenGLTexture *debugTexture;
	QOpenGLTexture *grassAlphaTexture;
	QOpenGLTexture *heightMap;

	unsigned int skyboxTexture;
	
	/* Debug parameters */
	GLenum grassRasterizationMode = GL_FILL;
	GLenum terrainRasterizationMode = GL_FILL;
};