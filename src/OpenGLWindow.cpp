#include "OpenGLWindow.hpp"

OpenGLWindow::OpenGLWindow()
	: QOpenGLWidget()
{
	/* Create camera */
	camera = new Camera(glm::vec3(0.0f, 100.0f, 160.0f), 45, (float)width() / (float)height(), 0.1f, 1000.0f);
	camera->rotateCamera(900.0f, -250.0f);	// reset rotation

	/* Create grass field */
	grassField = new GrassField(200, 10, 500);
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

	/* Initialize QtImGui */
	QtImGui::initialize(this);

	/* Initialize GPUEngine */
	ge::gl::init();
	gl = std::make_shared<ge::gl::Context>();

	/* OpenGL states */
	gl->glEnable(GL_DEPTH_TEST);
	gl->glEnable(GL_BLEND);
	gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Shaders */
	std::shared_ptr<ge::gl::Shader> grassVS		= std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER		    , ge::util::loadTextFile(GRASS_VS));
	std::shared_ptr<ge::gl::Shader> grassTCS	= std::make_shared<ge::gl::Shader>(GL_TESS_CONTROL_SHADER   , ge::util::loadTextFile(GRASS_TCS));
	std::shared_ptr<ge::gl::Shader> grassTES	= std::make_shared<ge::gl::Shader>(GL_TESS_EVALUATION_SHADER, ge::util::loadTextFile(GRASS_TES));
	std::shared_ptr<ge::gl::Shader> grassFS		= std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER		, ge::util::loadTextFile(GRASS_FS));
	std::shared_ptr<ge::gl::Shader> terrainVS	= std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER			, ge::util::loadTextFile(TERRAIN_VS));
	std::shared_ptr<ge::gl::Shader> terrainFS	= std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER		, ge::util::loadTextFile(TERRAIN_FS));
	std::shared_ptr<ge::gl::Shader> dummyVS		= std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER			, ge::util::loadTextFile(DUMMY_VS));
	std::shared_ptr<ge::gl::Shader> dummyFS		= std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER		, ge::util::loadTextFile(DUMMY_FS));
	std::shared_ptr<ge::gl::Shader> skyboxVS	= std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER			, ge::util::loadTextFile(SKYBOX_VS));
	std::shared_ptr<ge::gl::Shader> skyboxFS	= std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER		, ge::util::loadTextFile(SKYBOX_FS));

	/* Shader programs */
	grassShaderProgram	 = std::make_shared<ge::gl::Program>(grassVS, grassTCS, grassTES, grassFS);
	terrainShaderProgram = std::make_shared<ge::gl::Program>(terrainVS, terrainFS);
	dummyShaderProgram	 = std::make_shared<ge::gl::Program>(dummyVS, dummyFS);
	skyboxShaderProgram	 = std::make_shared<ge::gl::Program>(skyboxVS, skyboxFS);

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
	std::vector<float> skyboxPos
	{         
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	/* Grass VAO setup */
	grassPositionBuffer		  = grassField->getGrassVertexBuffer();
	grassCenterPositionBuffer = grassField->getGrassCenterBuffer();
	grassTexCoordBuffer		  = grassField->getGrassTexCoordBuffer();
	grassRandomsBuffer		  = grassField->getGrassRandomsBuffer();

	grassVAO = std::make_shared<ge::gl::VertexArray>();
	grassVAO->addAttrib(grassPositionBuffer,	   0, 4, GL_FLOAT);
	grassVAO->addAttrib(grassCenterPositionBuffer, 1, 4, GL_FLOAT);
	grassVAO->addAttrib(grassTexCoordBuffer,	   2, 4, GL_FLOAT);
	grassVAO->addAttrib(grassRandomsBuffer,		   3, 4, GL_FLOAT);

	/* Terrain VAO setup */
	terrainPositionBuffer = grassField->getTerrain()->getTerrainVertexBuffer();
	terrainIndexBuffer    = grassField->getTerrain()->getTerrainIndexBuffer();

	terrainVAO = std::make_shared<ge::gl::VertexArray>();
	terrainVAO->addElementBuffer(terrainIndexBuffer);
	terrainVAO->addAttrib(terrainPositionBuffer, 0, 2, GL_FLOAT);

	/* Dummy VAO setup */
	dummyPositionBuffer = std::make_shared<ge::gl::Buffer>(dummyPos.size()      * sizeof(float), dummyPos.data());
	dummyTexCoordBuffer = std::make_shared<ge::gl::Buffer>(dummyTexCoord.size() * sizeof(float), dummyTexCoord.data());

	dummyVAO = std::make_shared<ge::gl::VertexArray>();
	dummyVAO->addAttrib(dummyPositionBuffer, 0, 4, GL_FLOAT);
	dummyVAO->addAttrib(dummyTexCoordBuffer, 1, 2, GL_FLOAT);

	/* Skybox VAO setup */
	skyboxPositionBuffer = std::make_shared<ge::gl::Buffer>(skyboxPos.size() * sizeof(float), skyboxPos.data());

	skyboxVAO = std::make_shared<ge::gl::VertexArray>();
	skyboxVAO->addAttrib(skyboxPositionBuffer, 0, 3, GL_FLOAT);

	// Elapsed time since initialization
	timer.start();

	// Timer for application ticks
	tickTimer = new QTimer(this);
	QObject::connect(tickTimer, SIGNAL(timeout()), this, SLOT(tick()));
	tickTimer->start();

	// Texture parameters
	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load textures (mirrored vertically because of y axis differences between OpenGL and QImage)
	debugTexture	  = new QOpenGLTexture(QImage(DEBUG_TEXTURE).mirrored());
	grassAlphaTexture = new QOpenGLTexture(QImage(GRASS_ALPHA));
	heightMap		  = new QOpenGLTexture(QImage(HEIGHT_MAP).mirrored());

	// Load skybox
	std::vector<QString> faces
	{
		SKYBOX_RIGHT,
		SKYBOX_LEFT,
		SKYBOX_TOP,
		SKYBOX_BOTTOM,
		SKYBOX_FRONT,
		SKYBOX_BACK
	};
	skyboxTexture = loadSkybox(faces);
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
	if (guiEnabled)
	{
		drawGui();
	}

	const qreal retinaScale = devicePixelRatio();

	gl->glViewport(0, 0, windowWidth * retinaScale, windowHeight * retinaScale);
	gl->glClearColor(0.25, 0.3, 0.3, 1.0);
	gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glm::mat4 mvp = camera->getProjectionMatrix() * camera->getViewMatrix();
	float time    = timer.elapsed() / 10;

	/* DRAW SKYBOX */
	if (skyboxEnabled)
	{
		gl->glDepthMask(GL_FALSE);
		skyboxShaderProgram->use();
		glm::mat4 view = glm::mat4(glm::mat3(camera->getViewMatrix())); // remove translation from the view matrix
		glm::mat4 proj = camera->getProjectionMatrix();
		glm::mat4 skyboxMVP = proj * view;
		skyboxShaderProgram->setMatrix4fv("uMVP", glm::value_ptr(skyboxMVP));
		// skybox cube
		skyboxVAO->bind();
		gl->glActiveTexture(GL_TEXTURE0);
		gl->glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		gl->glDrawArrays(GL_TRIANGLES, 0, 36);
		gl->glDepthMask(GL_TRUE);
	}

	/* DRAW TERRAIN */
	terrainShaderProgram->use();
	terrainVAO->bind();
	terrainShaderProgram->setMatrix4fv("uMVP", glm::value_ptr(mvp));
	terrainShaderProgram->set1f("uFieldSize", grassField->getFieldSize());

	gl->glPolygonMode(GL_FRONT_AND_BACK, terrainRasterizationMode);
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

	// Uniforms
	grassShaderProgram->setMatrix4fv("uMVP", glm::value_ptr(mvp));
	grassShaderProgram->set1i("uTessLevel", tessLevel);
	grassShaderProgram->set1f("uMaxBendingFactor", maxBendingFactor);
	GLint uTime			= glGetUniformLocation(grassShaderProgram->getId(), "uTime");
	GLint uFieldSize	= glGetUniformLocation(grassShaderProgram->getId(), "uFieldSize");
	GLint uWindEnabled	= glGetUniformLocation(grassShaderProgram->getId(), "uWindEnabled");
	GLint uAlphaTexture = glGetUniformLocation(grassShaderProgram->getId(), "uAlphaTexture");
	GLint uHeightMap	= glGetUniformLocation(grassShaderProgram->getId(), "uHeightMap");
	gl->glUniform1f(uTime, time);
	gl->glUniform1f(uFieldSize, grassField->getFieldSize());
	gl->glUniform1i(uWindEnabled, windEnabled);
	gl->glUniform1i(uAlphaTexture, 0);
	gl->glUniform1i(uHeightMap, 1);

	gl->glPolygonMode(GL_FRONT_AND_BACK, grassRasterizationMode);
	gl->glPatchParameteri(GL_PATCH_VERTICES, 4);

	// Textures
	gl->glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
	grassAlphaTexture->bind();
	gl->glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
	heightMap->bind();

	gl->glDrawArraysInstanced(GL_PATCHES, 0, grassField->getGrassBladeCount() * 4, grassField->getPatchCount());

	/* ImGui */
	if (guiEnabled)
	{
		gl->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		ImGui::Render();
		QtImGui::render();
	}

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

void OpenGLWindow::drawGui()
{
	/* ImGui */
	QtImGui::newFrame();

	using namespace ImGui;

	ShowDemoWindow();

	ImGuiStyle &style	= ImGui::GetStyle();
	style.FrameRounding = 10.0f;
	style.GrabRounding	= 10.0f;
	style.Colors[ImGuiCol_FrameBg]			= ImVec4(0.160, 0.480, 0.199, 0.540);
	style.Colors[ImGuiCol_TitleBgActive]	= ImVec4(0.160, 0.480, 0.199, 0.540);
	style.Colors[ImGuiCol_CheckMark]		= ImVec4(0.353, 0.880, 0.240, 1.000);
	style.Colors[ImGuiCol_SliderGrab]		= ImVec4(0.353, 0.880, 0.240, 1.000);

	if (Begin("Application parameters", nullptr, NULL))
	{
		Text("Grass");

		SliderInt("Tessellation level", &tessLevel, 0, 10, "%d", NULL);
		SliderFloat("Max. bending factor", &maxBendingFactor, 0.0f, 5.0f, "%.1f");
		
		{
			static int radioValue = 2;
			Text("Rasterization mode");					SameLine();
			RadioButton("GL_POINT##g", &radioValue, 0); SameLine();
			RadioButton("GL_LINE##g" , &radioValue, 1); SameLine();
			RadioButton("GL_FILL##g" , &radioValue, 2);

			if (radioValue == 0)
				grassRasterizationMode = GL_POINT;
			else if (radioValue == 1)
				grassRasterizationMode = GL_LINE;
			else if (radioValue == 2)
				grassRasterizationMode = GL_FILL;
		}

		Separator();

		Text("Terrain");

		{
			static int radioValue = 2;
			Text("Rasterization mode");					SameLine();
			RadioButton("GL_POINT##t", &radioValue, 0); SameLine();
			RadioButton("GL_LINE##t" , &radioValue, 1); SameLine();
			RadioButton("GL_FILL##t" , &radioValue, 2);

			if (radioValue == 0)
				terrainRasterizationMode = GL_POINT;
			else if (radioValue == 1)
				terrainRasterizationMode = GL_LINE;
			else if (radioValue == 2)
				terrainRasterizationMode = GL_FILL;
		}

		Separator();

		Text("Scene");

		Checkbox("Wind", &windEnabled);
		Checkbox("Skybox", &skyboxEnabled);

	}

	ImGui::End();
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

	if (event->buttons() & Qt::RightButton)
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
		if (guiEnabled == true)
			guiEnabled = false;
		else
			guiEnabled = true;
	}
	if (event->key() == Qt::Key_V)
	{
		if (windEnabled == true)
			windEnabled = false;
		else
			windEnabled = true;
	}

	update();
}

unsigned int OpenGLWindow::loadSkybox(std::vector<QString> faces)
{
	unsigned int textureID;
	gl->glGenTextures(1, &textureID);
	gl->glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		QImage image(faces[i]);
		image = image.convertToFormat(QImage::Format_ARGB32);
		gl->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, image.width(), image.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());
	}
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}