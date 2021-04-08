#include "OpenGLWindow.hpp"

OpenGLWindow::OpenGLWindow()
	: QOpenGLWidget()
{
	/* Create camera */
	camera = new Camera(glm::vec3(0.0f, 100.0f, 160.0f), 45, (float)width() / (float)height(), 0.1f, 1000.0f);
	camera->rotateCamera(900.0f, -250.0f);	// reset rotation

	/* Create grass field */
	grassField = std::make_shared<GrassField>(200.0f, 10.0f, 700);
	terrain = std::make_shared<Terrain>(200.0f, 100, 100);

}

OpenGLWindow::~OpenGLWindow()
{
	makeCurrent();
	delete camera;
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
	patchTransSSBO = grassField->getPatchTransSSBO();
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
	terrainPositionBuffer = terrain->getTerrainVertexBuffer();
	terrainIndexBuffer    = terrain->getTerrainIndexBuffer();

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

void OpenGLWindow::tick()
{
	/* Update wind speed */
	const float pi = glm::pi<float>();
	windParams.z = glm::cos(time * pi / 10000) / 2 + 0.5;	// 0 - 1	// period 10s

	std::cout << "Time: " << time << "Wind param: " << windParams.z << std::endl;
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

	mvp = camera->getProjectionMatrix() * camera->getViewMatrix();
	time = timer.elapsed();

	gl->glViewport(0, 0, windowWidth * retinaScale, windowHeight * retinaScale);
	gl->glClearColor(0.45, 0.5, 0.5, 1.0);
	gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	/* INITIALIZE GUI */
	if (guiEnabled)
		initGui();

	/* DRAW SKYBOX */
	if (skyboxEnabled)
		drawSkybox();

	/* DRAW TERRAIN */
	drawTerrain();

	/* DRAW DUMMY */
	drawDummy();

	/* DRAW GRASS */
	drawGrass();

	/* DRAW GUI */
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

void OpenGLWindow::initGui()
{
	/* ImGui */
	QtImGui::newFrame();

	using namespace ImGui;

	//ShowDemoWindow();

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

		SliderInt("Max. tessellation level", &maxTessLevel, 0, 10, "%d", NULL);
		SliderFloat("Max. bending factor", &maxBendingFactor, 0.0f, 5.0f, "%.1f");
		SliderFloat("Max. distance", &maxDistance, 0.0f, 1000.0f, "%.f");
		
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
		SliderFloat2("Wind parameters", glm::value_ptr(windParams), 0.0f, 5.0f, "%.1f");
		SliderFloat("Wind speed", &windParams.z, 0.0f, 1.0f, "%.1f");

		Checkbox("Skybox", &skyboxEnabled);

		Text("Camera");
		SliderFloat("Camera speed", &cameraSpeed, 0.5f, 5.0f, "%.1f");

		Text("Light");
		Checkbox("Lighting", &lightingEnabled);
		SliderFloat("Light X", &lightPosition.x, -500.0f, 500.0f, "%.f");
		SliderFloat("Light Y", &lightPosition.y, -500.0f, 500.0f, "%.f");
		SliderFloat("Light Z", &lightPosition.z, -500.0f, 500.0f, "%.f");
		SliderFloat("Light R", &lightColor.r, 0.0f, 1.0f, "%.01f");
		SliderFloat("Light G", &lightColor.g, 0.0f, 1.0f, "%.01f");
		SliderFloat("Light B", &lightColor.b, 0.0f, 1.0f, "%.01f");

		Separator();

		Text("Regenerate");

		{
			static int bladeCount = 700;
			static float fieldSize = 200.0f;
			static float patchSize = 10.0f;
			static float terrainSize = 200.0f;
			static int rows = 100;
			static int cols = 100;

			SliderFloat("Field size", &fieldSize, 100.0f, 1000.0f, "%.f");
			SliderFloat("Patch size", &patchSize, 1.0f, 100.0f, "%.f");
			SliderInt("Blades per patch", &bladeCount, 0, 1000, "%d", NULL);
			SliderFloat("Terrain size", &terrainSize, 100.0f, 1000.0f, "%.f");
			SliderInt("Terrain rows", &rows, 1, 1000, "%d", NULL);
			SliderInt("Terrain columns", &cols, 1, 1000, "%d", NULL);

			if (Button("Regenerate"))
				regenerateField(fieldSize, patchSize, bladeCount, terrainSize, rows, cols);
		}

	}

	ImGui::End();
}

void OpenGLWindow::drawTerrain()
{
	terrainShaderProgram->use();
	terrainVAO->bind();
	terrainShaderProgram->setMatrix4fv("uMVP", glm::value_ptr(mvp));
	terrainShaderProgram->set1f("uFieldSize", terrain->getTerrainSize());

	gl->glPolygonMode(GL_FRONT_AND_BACK, terrainRasterizationMode);
	gl->glEnable(GL_PRIMITIVE_RESTART);
	gl->glPrimitiveRestartIndex(terrain->getRestartIndex());

	// Textures
	gl->glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
	heightMap->bind();

	// Draw
	gl->glDrawElements(GL_TRIANGLE_STRIP, terrain->getIndexCount(), GL_UNSIGNED_INT, 0);

	gl->glDisable(GL_PRIMITIVE_RESTART);
}

void OpenGLWindow::drawGrass()
{
	GLint uTime			= gl->glGetUniformLocation(grassShaderProgram->getId(), "uTime");
	GLint uFieldSize	= gl->glGetUniformLocation(grassShaderProgram->getId(), "uFieldSize");
	GLint uWindEnabled	= gl->glGetUniformLocation(grassShaderProgram->getId(), "uWindEnabled");
	GLint uLightingEnabled = gl->glGetUniformLocation(grassShaderProgram->getId(), "uLightingEnabled");
	GLint uAlphaTexture = gl->glGetUniformLocation(grassShaderProgram->getId(), "uAlphaTexture");
	GLint uHeightMap	= gl->glGetUniformLocation(grassShaderProgram->getId(), "uHeightMap");
	GLint uCameraPos	= gl->glGetUniformLocation(grassShaderProgram->getId(), "uCameraPos");
	GLint uMaxDistance	= gl->glGetUniformLocation(grassShaderProgram->getId(), "uMaxDistance");

	grassShaderProgram->use();
	grassVAO->bind();

	glm::vec3 cameraPos = camera->getPosition();

	// Uniforms
	grassShaderProgram->setMatrix4fv("uMVP", glm::value_ptr(mvp));
	grassShaderProgram->set1i("uMaxTessLevel", maxTessLevel);
	grassShaderProgram->set1f("uMaxBendingFactor", maxBendingFactor);
	grassShaderProgram->set3fv("uCameraPos", glm::value_ptr(cameraPos));
	grassShaderProgram->set3fv("uLightPos", glm::value_ptr(lightPosition));
	grassShaderProgram->set3fv("uLightColor", glm::value_ptr(lightColor));
	grassShaderProgram->set3fv("uWindParams", glm::value_ptr(windParams));
	gl->glUniform1i(uTime, time);
	gl->glUniform1f(uFieldSize, grassField->getFieldSize());
	gl->glUniform1f(uMaxDistance, maxDistance);
	gl->glUniform1i(uWindEnabled, windEnabled);
	gl->glUniform1i(uLightingEnabled, lightingEnabled);
	gl->glUniform1i(uAlphaTexture, 0);
	gl->glUniform1i(uHeightMap, 1);

	gl->glPolygonMode(GL_FRONT_AND_BACK, grassRasterizationMode);
	gl->glPatchParameteri(GL_PATCH_VERTICES, 4);

	// Textures
	gl->glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
	grassAlphaTexture->bind();
	gl->glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
	heightMap->bind();

	// Draw
	gl->glDrawArraysInstanced(GL_PATCHES, 0, grassField->getGrassBladeCount() * 4, grassField->getPatchCount());
}

void OpenGLWindow::drawSkybox()
{
	glm::mat4 view = glm::mat4(glm::mat3(camera->getViewMatrix())); // remove translation from the view matrix
	glm::mat4 proj = camera->getProjectionMatrix();
	glm::mat4 skyboxMVP = proj * view;

	gl->glDepthMask(GL_FALSE);

	skyboxShaderProgram->use();
	skyboxShaderProgram->setMatrix4fv("uMVP", glm::value_ptr(skyboxMVP));
	skyboxVAO->bind();

	// Textures
	gl->glActiveTexture(GL_TEXTURE0);
	gl->glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	// Draw
	gl->glDrawArrays(GL_TRIANGLES, 0, 36);

	gl->glDepthMask(GL_TRUE);
}

void OpenGLWindow::drawDummy()
{
	dummyShaderProgram->use();
	dummyVAO->bind();
	dummyShaderProgram->setMatrix4fv("uMVP", glm::value_ptr(mvp));

	gl->glPolygonMode(GL_FRONT_AND_BACK, rasterizationMode);
	gl->glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
	debugTexture->bind();

	gl->glDrawArrays(GL_TRIANGLES, 0, 36);
}

void OpenGLWindow::regenerateField(float fieldSize, float patchSize, int grassBladeCount, float terrainSize, int rows, int cols)
{
	grassField.reset();
	terrain.reset();
	patchTransSSBO.reset();

	grassPositionBuffer.reset();
	grassCenterPositionBuffer.reset();
	grassTexCoordBuffer.reset();
	grassRandomsBuffer.reset();
	grassVAO.reset();

	terrainPositionBuffer.reset();
	terrainIndexBuffer.reset();
	terrainIndexBuffer.reset();
	terrainVAO.reset();

	grassField = std::make_shared<GrassField>(fieldSize, patchSize, grassBladeCount);
	terrain = std::make_shared<Terrain>(terrainSize, rows, cols);

	/* Grass VAO setup */
	grassPositionBuffer = grassField->getGrassVertexBuffer();
	grassCenterPositionBuffer = grassField->getGrassCenterBuffer();
	grassTexCoordBuffer = grassField->getGrassTexCoordBuffer();
	grassRandomsBuffer = grassField->getGrassRandomsBuffer();
	patchTransSSBO = grassField->getPatchTransSSBO();
	grassShaderProgram->bindBuffer("patchTranslationBuffer", patchTransSSBO);

	grassVAO = std::make_shared<ge::gl::VertexArray>();
	grassVAO->addAttrib(grassPositionBuffer, 0, 4, GL_FLOAT);
	grassVAO->addAttrib(grassCenterPositionBuffer, 1, 4, GL_FLOAT);
	grassVAO->addAttrib(grassTexCoordBuffer, 2, 4, GL_FLOAT);
	grassVAO->addAttrib(grassRandomsBuffer, 3, 4, GL_FLOAT);

	/* Terrain VAO setup */
	terrainPositionBuffer = terrain->getTerrainVertexBuffer();
	terrainIndexBuffer = terrain->getTerrainIndexBuffer();

	terrainVAO = std::make_shared<ge::gl::VertexArray>();
	terrainVAO->addElementBuffer(terrainIndexBuffer);
	terrainVAO->addAttrib(terrainPositionBuffer, 0, 2, GL_FLOAT);
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