#include "OpenGLWindow.hpp"

OpenGLWindow::OpenGLWindow()
	: QOpenGLWidget()
{
	/* Create camera */
	camera = new Camera(glm::vec3(0.0f, 100.0f, 160.0f), 45, (float)width() / (float)height(), 0.1f, 1000.0f);
	camera->rotateCamera(900.0f, -250.0f);	// reset rotation

	/* Create grass field */
	grassField = new GrassField(200, 10, 100);
}

OpenGLWindow::~OpenGLWindow()
{
	makeCurrent();
	delete camera;
	delete grassField;
	doneCurrent();
}

void OpenGLWindow::initializeGL()
{
	initializeOpenGLFunctions();

	/* Initialize GPUEngine */
	ge::gl::init();
	gl = std::make_shared<ge::gl::Context>();

	/* OpenGL states */
	gl->glEnable(GL_DEPTH_TEST);
	gl->glEnable(GL_BLEND);
	gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Initialize settings widget */
	settingsWidget = new SettingsWidget(this);
	settingsWidget->setParent(this);
	settingsWidget->show();
	QObject::connect(settingsWidget, SIGNAL(tessLevelChanged(int)), this, SLOT(setTessLevel(int)));
	QObject::connect(settingsWidget, SIGNAL(rasterizationModeChanged(GLenum)), this, SLOT(setRasterizationMode(GLenum)));

	/* Shaders */
	std::cout << "Grass vertex shader path: " << GRASS_VS << std::endl;
	std::cout << "Grass tessellation control shader path: " << GRASS_TCS << std::endl;
	std::cout << "Grass tessellation evaluation shader path: " << GRASS_TES << std::endl;
	std::cout << "Grass fragment shader path: " << GRASS_FS << std::endl;
	std::cout << "Terrain vertex shader path: " << TERRAIN_VS << std::endl;
	std::cout << "Terrain fragment shader path: " << TERRAIN_FS << std::endl;
	std::cout << "Dummy vertex shader path: " << DUMMY_VS << std::endl;
	std::cout << "Dummy fragment shader path: " << DUMMY_FS << std::endl;

	std::shared_ptr<ge::gl::Shader> grassVS		= std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, ge::util::loadTextFile(GRASS_VS));
	std::shared_ptr<ge::gl::Shader> grassTCS	= std::make_shared<ge::gl::Shader>(GL_TESS_CONTROL_SHADER, ge::util::loadTextFile(GRASS_TCS));
	std::shared_ptr<ge::gl::Shader> grassTES	= std::make_shared<ge::gl::Shader>(GL_TESS_EVALUATION_SHADER, ge::util::loadTextFile(GRASS_TES));
	std::shared_ptr<ge::gl::Shader> grassFS		= std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, ge::util::loadTextFile(GRASS_FS));
	std::shared_ptr<ge::gl::Shader> terrainVS	= std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, ge::util::loadTextFile(TERRAIN_VS));
	std::shared_ptr<ge::gl::Shader> terrainFS	= std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, ge::util::loadTextFile(TERRAIN_FS));
	std::shared_ptr<ge::gl::Shader> dummyVS		= std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, ge::util::loadTextFile(DUMMY_VS));
	std::shared_ptr<ge::gl::Shader> dummyFS		= std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, ge::util::loadTextFile(DUMMY_FS));

	/* Shader programs */
	grassShaderProgram	 = std::make_shared<ge::gl::Program>(grassVS, grassTCS, grassTES, grassFS);
	terrainShaderProgram = std::make_shared<ge::gl::Program>(terrainVS, terrainFS);
	dummyShaderProgram	 = std::make_shared<ge::gl::Program>(dummyVS, dummyFS);

	/* Generating patches */
	std::vector<glm::vec3> *patchPositions = grassField->getPatchPositions();
	std::cout << "Number of patches: " << patchPositions->size() << std::endl;
	std::vector<glm::mat4> patchTranslations;

	for (size_t i = 0; i < patchPositions->size(); i++)
	{
		glm::mat4 mat = glm::translate(glm::mat4(1.0f), patchPositions->at(i));
		patchTranslations.push_back(mat);
	}

	patchTransSSBO = std::make_shared<ge::gl::Buffer>(patchTranslations.size() * sizeof(glm::mat4), patchTranslations.data());
	grassShaderProgram->bindBuffer("patchTranslationBuffer", patchTransSSBO);

	std::vector<float> dummyPos
	{
		-0.5f, -0.5f, -0.5f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,
		-0.5f,  0.5f, -0.5f,  1.0f,
		-0.5f, -0.5f, -0.5f,  1.0f,

		-0.5f, -0.5f,  0.5f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,
		-0.5f,  0.5f,  0.5f,  1.0f,
		-0.5f, -0.5f,  0.5f,  1.0f,

		-0.5f,  0.5f,  0.5f,  1.0f,
		-0.5f,  0.5f, -0.5f,  1.0f,
		-0.5f, -0.5f, -0.5f,  1.0f,
		-0.5f, -0.5f, -0.5f,  1.0f,
		-0.5f, -0.5f,  0.5f,  1.0f,
		-0.5f,  0.5f,  0.5f,  1.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,

		-0.5f, -0.5f, -0.5f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,
		-0.5f, -0.5f,  0.5f,  1.0f,
		-0.5f, -0.5f, -0.5f,  1.0f,

		-0.5f,  0.5f, -0.5f,  1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,
		-0.5f,  0.5f,  0.5f,  1.0f,
		-0.5f,  0.5f, -0.5f,  1.0f
	};
	std::vector<float> dummyTexCoord
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f
	};

	grassPositionBuffer		  = grassField->getGrassVertexBuffer();
	grassCenterPositionBuffer = grassField->getGrassCenterBuffer();
	grassTexCoordBuffer		  = grassField->getGrassTexCoordBuffer();
	grassRandomsBuffer		  = grassField->getGrassRandomsBuffer();

	grassVAO = std::make_shared<ge::gl::VertexArray>();
	grassVAO->addAttrib(grassPositionBuffer,	   0, 4, GL_FLOAT);
	grassVAO->addAttrib(grassCenterPositionBuffer, 1, 4, GL_FLOAT);
	grassVAO->addAttrib(grassTexCoordBuffer,	   2, 4, GL_FLOAT);
	grassVAO->addAttrib(grassRandomsBuffer,		   3, 4, GL_FLOAT);

	terrainPositionBuffer = grassField->getTerrain()->getTerrainVertexBuffer();
	terrainIndexBuffer    = grassField->getTerrain()->getTerrainIndexBuffer();
	terrainTexCoordBuffer = grassField->getTerrain()->getTerrainTexCoordBuffer();

	terrainVAO = std::make_shared<ge::gl::VertexArray>();
	terrainVAO->addElementBuffer(terrainIndexBuffer);
	terrainVAO->addAttrib(terrainPositionBuffer, 0, 2, GL_FLOAT);
	terrainVAO->addAttrib(terrainTexCoordBuffer, 1, 2, GL_FLOAT);

	dummyPositionBuffer = std::make_shared<ge::gl::Buffer>(dummyPos.size()      * sizeof(float), dummyPos.data());
	dummyTexCoordBuffer = std::make_shared<ge::gl::Buffer>(dummyTexCoord.size() * sizeof(float), dummyTexCoord.data());

	dummyVAO = std::make_shared<ge::gl::VertexArray>();
	dummyVAO->addAttrib(dummyPositionBuffer, 0, 4, GL_FLOAT);
	dummyVAO->addAttrib(dummyTexCoordBuffer, 1, 2, GL_FLOAT);

	// Elapsed time since initialization
	timer.start();

	tickTimer = new QTimer(this);
	QObject::connect(tickTimer, SIGNAL(timeout()), this, SLOT(tick()));
	tickTimer->start();

	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	debugTexture	  = new QOpenGLTexture(QImage(DEBUG_TEXTURE).mirrored());
	grassAlphaTexture = new QOpenGLTexture(QImage(GRASS_ALPHA));
	heightMap		  = new QOpenGLTexture(QImage(HEIGHT_MAP).mirrored());
	heightMap->setWrapMode(QOpenGLTexture::ClampToEdge);

	//debug
	
}

void OpenGLWindow::setTessLevel(int tessLevel)
{
	this->tessLevel = tessLevel;
	//DEBUG
	std::cout << "Tessellation Level: " << tessLevel << std::endl;
	update();
}

void OpenGLWindow::setRasterizationMode(GLenum mode)
{
	std::cout << "Rasterization mode changed: " << mode << std::endl;
	rasterizationMode = mode;
	update();
}

void OpenGLWindow::tick()
{
	update();
}

void OpenGLWindow::resizeGL(int w, int h)
{
	windowWidth = w;
	windowHeight = h;
}

void OpenGLWindow::paintGL()
{
	/* RENDER CALL BEGIN */
	const qreal retinaScale = devicePixelRatio();

	gl->glViewport(0, 0, windowWidth * retinaScale, windowHeight * retinaScale);
	gl->glClearColor(0.0, 0.0, 0.0, 1.0);
	gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glm::mat4 mvp = camera->getProjectionMatrix() * camera->getViewMatrix();
	float time    = timer.elapsed() / 10;

	//debug
	glm::vec3 camPos = camera->getPosition();
	std::cout << "Camera position: " << camPos.x << ", " << camPos.y << ", " << camPos.z << std::endl;

	/* DRAW TERRAIN */
	terrainShaderProgram->use();
	terrainVAO->bind();
	terrainShaderProgram->setMatrix4fv("uMVP", glm::value_ptr(mvp));

	gl->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	gl->glEnable(GL_PRIMITIVE_RESTART);
	gl->glPrimitiveRestartIndex(grassField->getTerrain()->getRestartIndex());
	gl->glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
	heightMap->bind();
	gl->glDrawElements(GL_TRIANGLE_STRIP, grassField->getTerrain()->getIndexCount(), GL_UNSIGNED_INT, 0);
	gl->glDisable(GL_PRIMITIVE_RESTART);

	/* DRAW DUMMY */
	dummyShaderProgram->use();
	dummyVAO->bind();
	dummyShaderProgram->setMatrix4fv("uMVP", glm::value_ptr(mvp));

	gl->glPolygonMode(GL_FRONT_AND_BACK, rasterizationMode);
	gl->glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
	debugTexture->bind();
	gl->glDrawArrays(GL_TRIANGLES, 0, 36);

	/* DRAW GRASS */
	grassShaderProgram->use();
	grassVAO->bind();
	grassShaderProgram->setMatrix4fv("uMVP", glm::value_ptr(mvp));
	grassShaderProgram->set1i("uTessLevel", tessLevel);
	grassShaderProgram->set1f("uMaxBendingFactor", maxBendingFactor);
	GLint uTime = glGetUniformLocation(grassShaderProgram->getId(), "uTime");
	gl->glUniform1f(uTime, time);
	GLint uFieldSize = glGetUniformLocation(grassShaderProgram->getId(), "uFieldSize");
	gl->glUniform1f(uFieldSize, grassField->getFieldSize());
	GLint uWindEnabled = glGetUniformLocation(grassShaderProgram->getId(), "uWindEnabled");
	gl->glUniform1i(uWindEnabled, windEnabled);

	gl->glPolygonMode(GL_FRONT_AND_BACK, rasterizationMode);
	gl->glPatchParameteri(GL_PATCH_VERTICES, 4);

	gl->glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
	grassAlphaTexture->bind();
	gl->glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
	heightMap->bind();
	GLint uAlphaTexture = glGetUniformLocation(grassShaderProgram->getId(), "uAlphaTexture");
	gl->glUniform1i(uAlphaTexture, 0);
	GLint uHeightMap = glGetUniformLocation(grassShaderProgram->getId(), "uHeightMap");
	gl->glUniform1i(uHeightMap, 1);

	gl->glDrawArraysInstanced(GL_PATCHES, 0, grassField->getGrassBladeCount() * 4, grassField->getPatchCount());

	/* RENDER CALL END */
	printError();
}

void OpenGLWindow::printError() const
{
	auto err = this->gl->glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cout << err << std::endl;
	}
}

void OpenGLWindow::wheelEvent(QWheelEvent *event)
{
	float angle = event->angleDelta().y();
	if (angle > 0)
		camera->decreaseFov(10);
	else
		camera->increaseFov(10);

	update();
}

void OpenGLWindow::mousePressEvent(QMouseEvent *event)
{
	// save position
	clickStartPos = event->pos();
}

void OpenGLWindow::mouseMoveEvent(QMouseEvent *event)
{
	QPointF movePos		  = event->pos();
	float horizontalDelta = movePos.x() - clickStartPos.x();
	float verticalDelta   = movePos.y() - clickStartPos.y();

	if (event->buttons() & Qt::LeftButton)
		camera->rotateCamera(horizontalDelta, verticalDelta);

	clickStartPos = event->pos();
	update();
}

void OpenGLWindow::keyPressEvent(QKeyEvent *event)
{
	float cameraSpeed = 1.0f;
	if (event->key() == Qt::Key_W)
		camera->moveCamera(Camera::Direction::FORWARDS, cameraSpeed);
	if (event->key() == Qt::Key_S)
		camera->moveCamera(Camera::Direction::BACKWARDS, cameraSpeed);
	if (event->key() == Qt::Key_A)
		camera->moveCamera(Camera::Direction::LEFT, cameraSpeed);
	if (event->key() == Qt::Key_D)
		camera->moveCamera(Camera::Direction::RIGHT, cameraSpeed);
	if (event->key() == Qt::Key_Space)
		camera->moveCamera(Camera::Direction::UP, cameraSpeed);
	if (event->key() == Qt::Key_Control)
		camera->moveCamera(Camera::Direction::DOWN, cameraSpeed);
	if (event->key() == Qt::Key_Escape)
	{
		if (settingsWidget->isVisible())
			settingsWidget->hide();
		else
			settingsWidget->show();
	}
	if (event->key() == Qt::Key_V)
	{
		if (windEnabled == 1)
			windEnabled = 0;
		else
			windEnabled = 1;
	}

	update();
}