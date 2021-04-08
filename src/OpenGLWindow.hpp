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

#include "Camera.hpp"
#include "GrassField.hpp"

class OpenGLWindow : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
	Q_OBJECT
public:
	explicit OpenGLWindow();
	~OpenGLWindow();

public slots:
	void tick();

protected:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

	void printError() const;
	void initGui();
	void drawTerrain();
	void drawGrass();
	void drawSkybox();
	void drawDummy();

	void regenerateField(float fieldSize, float patchSize, int grassBladeCount, float terrainSize, int rows, int cols, GrassField::BladeDimensions bladeDimensions);

	/* Event handlers */
	void wheelEvent(QWheelEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent *event);

	unsigned int loadSkybox(std::vector<QString> faces);

private:
	bool initialized;
	int maxTessLevel = 5;
	float maxBendingFactor = 0.3f;
	float maxDistance = 500.0f;
	float maxTerrainHeight = 30.0f;
	int time;
	float cameraSpeed = 2.0f;
	int windowWidth;
	int windowHeight;

	bool windEnabled = true;
	bool lightingEnabled = false;
	bool skyboxEnabled = true;
	bool guiEnabled = true;
	bool controlPressed = false;

	glm::mat4 mvp;
	glm::vec3 lightPosition { 100.0, 500.0, 100.0 };
	glm::vec3 lightColor{ 0.086, 0.837, 0.388 };
	glm::vec3 windParams{ 1.0, 1.0, 0.0 };

	Camera *camera;
	std::shared_ptr<GrassField> grassField;
	std::shared_ptr<Terrain> terrain;

	QTimer *tickTimer;
	QElapsedTimer timer;

	GLenum rasterizationMode = GL_FILL;
	GLenum grassRasterizationMode = GL_FILL;
	GLenum terrainRasterizationMode = GL_FILL;

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
	
};