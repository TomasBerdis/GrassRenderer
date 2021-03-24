#include "OpenGLWindow.hpp"

OpenGLWindow::OpenGLWindow()
	: QOpenGLWidget()
{
}

OpenGLWindow::~OpenGLWindow()
{
}

void OpenGLWindow::initializeGL()
{
	initializeOpenGLFunctions();

	/* Initialize GPUEngine */
	ge::gl::init();
	gl = std::make_shared<ge::gl::Context>();
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

	/* Create grass field */
	GrassField *grassField = new GrassField(200, 25);

	/* Generating patches */
	std::vector<glm::vec3> *patchPositions = grassField->getPatchPositions();
	std::cout << "Number of patches: " << patchPositions->size() << std::endl;

	// Generating vertices
	srand(time(0));	// reset generator seed
	float r = glm::linearRand(0.0f, 1.0f);

	/* Rendering pipeline random values */
	float r0 = glm::linearRand( 0.00f, 360.0f);	// angle
	float r1 = glm::linearRand( 0.00f, 1.00f);	// x offset
	float r2 = glm::linearRand( 0.00f, 1.00f);	// z offset
	float r3 = glm::linearRand(-0.25f, 0.25f);	// TCS
	float r4 = glm::linearRand( 0.75f, 1.25f);	// TCS
	float r5 = glm::linearRand( 0.00f, 1.00f);	// R
	float r6 = glm::linearRand( 0.00f, 1.00f);	// G
	float r7 = glm::linearRand( 0.00f, 1.00f);	// B

	float wMin = 0.5f;
	float wMax = 2.0f;
	float hMin = 2.0f;
	float hMax = 5.0f;
	float density = 1.0f;
	float w = wMin + r * (wMax - wMin);
	float h = (hMin + r * (hMax - hMin)) * density;

	glm::vec4 pc { 0.0f, 0.0f, 0.0f, 1.0f };

	glm::vec4 p1 = pc + glm::vec4(-0.5f * w, 0.0f, 0.0f, 0.0f);
	glm::vec4 p2 = pc + glm::vec4( 0.5f * w, 0.0f, 0.0f, 0.0f);
	glm::vec4 p3 = pc + glm::vec4( 0.5f * w,	h, 0.0f, 0.0f);
	glm::vec4 p4 = pc + glm::vec4(-0.5f * w,	h, 0.0f, 0.0f);

	/* Model transformations */
	glm::mat4 model = glm::translate(glm::mat4(1.0f), patchPositions->front());
	p1 = model * p1;
	p2 = model * p2;
	p3 = model * p3;
	p4 = model * p4;
	pc = model * pc;

	std::vector<float> grassBladePos
	{
		p1.x, p1.y, p1.z, r0,
		p2.x, p2.y, p2.z, r0,
		p3.x, p3.y, p3.z, r0,
		p4.x, p4.y, p4.z, r0
		// x, y, z, r0
	};
	std::vector<float> grassCenterPos
	{
		pc.x, 0.0f, pc.z, r1,
		pc.x, 0.0f, pc.z, r1,
		pc.x, 1.0f, pc.z, r1,
		pc.x, 1.0f, pc.z, r1
		// x, lower/upper, z, r1
	};
	std::vector<float> grassTexCoord
	{
		0.0f, 0.0f, r2, r3,
		1.0f, 0.0f, r2, r3,
		1.0f, 1.0f, r2, r3,
		0.0f, 1.0f, r2, r3
		// s, t, r2, r3
	};
	std::vector<float> grassRandoms
	{
		r4, r5, r6, r7,
		r4, r5, r6, r7,
		r4, r5, r6, r7,
		r4, r5, r6, r7
	};
	std::vector<int> grassBladeInd
	{
		0, 1, 2, 3
	};
	std::vector<float> terrainPos
	{
		-100.0, 0.0, 100.0, 1.0,
		 100.0, 0.0, 100.0, 1.0,
		 100.0, 0.0,-100.0, 1.0,
		-100.0, 0.0,-100.0, 1.0
	};
	std::vector<int> terrainInd
	{
		0, 1, 2,
		2, 3, 0
	};
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


	grassPositionBuffer		  = std::make_shared<ge::gl::Buffer>(grassBladePos.size() * sizeof(float), grassBladePos.data());
	grassCenterPositionBuffer = std::make_shared<ge::gl::Buffer>(grassCenterPos.size() * sizeof(float), grassCenterPos.data());
	grassTexCoordBuffer		  = std::make_shared<ge::gl::Buffer>(grassTexCoord.size() * sizeof(float), grassTexCoord.data());
	grassRandomsBuffer		  = std::make_shared<ge::gl::Buffer>(grassRandoms.size() * sizeof(float), grassRandoms.data());
	grassElementBuffer		  = std::make_shared<ge::gl::Buffer>(grassBladeInd.size() * sizeof(int), grassBladeInd.data());

	grassVAO = std::make_shared<ge::gl::VertexArray>();
	grassVAO->addElementBuffer(grassElementBuffer);
	grassVAO->addAttrib(grassPositionBuffer, 0, 4, GL_FLOAT);
	grassVAO->addAttrib(grassCenterPositionBuffer, 1, 4, GL_FLOAT);
	grassVAO->addAttrib(grassTexCoordBuffer, 2, 4, GL_FLOAT);
	grassVAO->addAttrib(grassRandomsBuffer, 3, 4, GL_FLOAT);

	terrainPositionBuffer = std::make_shared<ge::gl::Buffer>(terrainPos.size() * sizeof(float), terrainPos.data());
	terrainElementBuffer  = std::make_shared<ge::gl::Buffer>(terrainInd.size() * sizeof(int), terrainInd.data());

	terrainVAO = std::make_shared<ge::gl::VertexArray>();
	terrainVAO->addElementBuffer(terrainElementBuffer);
	terrainVAO->addAttrib(terrainPositionBuffer, 0, 4, GL_FLOAT);

	dummyPositionBuffer = std::make_shared<ge::gl::Buffer>(dummyPos.size() * sizeof(float), dummyPos.data());
	dummyTexCoordBuffer = std::make_shared<ge::gl::Buffer>(dummyTexCoord.size() * sizeof(float), dummyTexCoord.data());

	dummyVAO = std::make_shared<ge::gl::VertexArray>();
	dummyVAO->addAttrib(dummyPositionBuffer, 0, 4, GL_FLOAT);
	dummyVAO->addAttrib(dummyTexCoordBuffer, 1, 2, GL_FLOAT);

	/* Camera */
	camera = new Camera(glm::vec3(0.0f, 2.0f, 4.0f), 45, (float)width() / (float)height(), 0.1f, 1000.0f);
	camera->rotateCamera(900.0f, -100.0f);	// reset rotation

	// Elapsed time since initialization
	timer.start();

	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	debugTexture = new QOpenGLTexture(QImage(DEBUG_TEXTURE).mirrored());
	grassAlphaTexture = new QOpenGLTexture(QImage(GRASS_ALPHA));
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

	/* DRAW TERRAIN */
	terrainShaderProgram->use();
	terrainVAO->bind();
	terrainShaderProgram->setMatrix4fv("uMVP", glm::value_ptr(mvp));

	gl->glPolygonMode(GL_FRONT_AND_BACK, rasterizationMode);
	gl->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	/* DRAW DUMMY */
	dummyShaderProgram->use();
	dummyVAO->bind();
	dummyShaderProgram->setMatrix4fv("uMVP", glm::value_ptr(mvp));

	gl->glPolygonMode(GL_FRONT_AND_BACK, rasterizationMode);
	debugTexture->bind();
	gl->glDrawArrays(GL_TRIANGLES, 0, 36);

	/* DRAW GRASS */
	grassShaderProgram->use();
	grassVAO->bind();
	grassShaderProgram->setMatrix4fv("uMVP", glm::value_ptr(mvp));
	grassShaderProgram->set1i("uTessLevel", tessLevel);
	grassShaderProgram->set1f("uMaxBendingFactor", maxBendingFactor);

	gl->glPolygonMode(GL_FRONT_AND_BACK, rasterizationMode);
	gl->glPatchParameteri(GL_PATCH_VERTICES, 4);
	grassAlphaTexture->bind();
	gl->glDrawElements(GL_PATCHES, 4, GL_UNSIGNED_INT, nullptr);

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
	if (event->key() == Qt::Key_Shift)
		camera->moveCamera(Camera::Direction::UP, cameraSpeed);
	if (event->key() == Qt::Key_Control)
		camera->moveCamera(Camera::Direction::DOWN, cameraSpeed);

	update();
}