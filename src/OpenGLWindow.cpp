#include "OpenGLWindow.hpp"

OpenGLWindow::OpenGLWindow(QWindow* parent)
	: QWindow(parent)
	, initialized(false)
	, context(nullptr)
	, settingsWidget(nullptr)
{
	setSurfaceType(QWindow::OpenGLSurface);
	surfaceFormat.setVersion(4, 5);
	surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
}

OpenGLWindow::~OpenGLWindow()
{
}

void OpenGLWindow::initialize()
{
	if (initialized)
		return;

	if (!context)
	{
		context = new QOpenGLContext(this);
		context->setFormat(surfaceFormat);
		if (!context->create())
		{
			// fail
		}
	}

	context->makeCurrent(this);

	/* Initialize GPUEngine */
	ge::gl::init();
	gl = std::make_shared<ge::gl::Context>();

	/* Initialize settings widget */
	settingsWidget = new SettingsWidget(this);
	settingsWidget->show();
	QObject::connect(settingsWidget, SIGNAL(tessLevelChanged(int)), this, SLOT(setTessLevel(int)));

	/* Shaders */
	std::cout << "Grass vertex shader path: "					<< GRASS_VS << std::endl;
	std::cout << "Grass tessellation control shader path: "		<< GRASS_TCS << std::endl;
	std::cout << "Grass tessellation evaluation shader path: "	<< GRASS_TES << std::endl;
	std::cout << "Grass fragment shader path: "					<< GRASS_FS << std::endl;
	std::cout << "Terrain vertex shader path: "					<< TERRAIN_VS << std::endl;
	std::cout << "Terrain fragment shader path: "				<< TERRAIN_FS << std::endl;

	std::shared_ptr<ge::gl::Shader> grassVS		= std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, ge::util::loadTextFile(GRASS_VS));
	std::shared_ptr<ge::gl::Shader> grassTCS	= std::make_shared<ge::gl::Shader>(GL_TESS_CONTROL_SHADER, ge::util::loadTextFile(GRASS_TCS));
	std::shared_ptr<ge::gl::Shader> grassTES	= std::make_shared<ge::gl::Shader>(GL_TESS_EVALUATION_SHADER, ge::util::loadTextFile(GRASS_TES));
	std::shared_ptr<ge::gl::Shader> grassFS		= std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, ge::util::loadTextFile(GRASS_FS));
	std::shared_ptr<ge::gl::Shader> terrainVS	= std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, ge::util::loadTextFile(TERRAIN_VS));
	std::shared_ptr<ge::gl::Shader> terrainFS	= std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, ge::util::loadTextFile(TERRAIN_FS));

	grassShaderProgram		= std::make_shared<ge::gl::Program>(grassVS, grassTCS, grassTES, grassFS);
	terrainShaderProgram	= std::make_shared<ge::gl::Program>(terrainVS, terrainFS);

	// Vertices
	std::vector<float> grassBladePos
	{
		-0.20f, -0.75f, 0.0f, 1.0f,
		 0.20f, -0.75f, 0.0f, 1.0f,
		 0.05f,  0.75f, 0.0f, 1.0f,
		-0.05f,  0.75f, 0.0f, 1.0f
		// x, y, z, w
	};

	std::vector<int> grassBladeInd
	{
		0, 1, 2, 3
	};

	std::vector<float> grassCenterPos
	{
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f
		// x, y, z, w
	};

	std::vector<float> terrainPos
	{
		-0.5f, -0.5f,  0.5f, 1.0f,
		 0.5f, -0.5f,  0.5f, 1.0f,
		 0.5f, -0.4f, -0.5f, 1.0f,
		-0.5f, -0.4f, -0.5f, 1.0f
	};

	std::vector<int> terrainInd
	{
		0, 1, 2,
		2, 3, 0
	};

	model = glm::mat4(1.0f); // glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::mat4(1.0f);
	proj = glm::mat4(1.0f);
	glm::mat4 mvp = proj * view * model;

	grassShaderProgram->setMatrix4fv("uMVP", glm::value_ptr(mvp));
	terrainShaderProgram->setMatrix4fv("uMVP", glm::value_ptr(mvp));
	
	grassPositionBuffer       = std::make_shared<ge::gl::Buffer>(grassBladePos.size() * sizeof(float), grassBladePos.data());
	grassCenterPositionBuffer = std::make_shared<ge::gl::Buffer>(grassCenterPos.size() * sizeof(float), grassCenterPos.data());
	grassElementBuffer        = std::make_shared<ge::gl::Buffer>(grassBladeInd.size() * sizeof(int), grassBladeInd.data());

	grassVAO = std::make_shared<ge::gl::VertexArray>();
	grassVAO->addElementBuffer(grassElementBuffer);
	grassVAO->addAttrib(grassPositionBuffer, 0, 4, GL_FLOAT);
	grassVAO->addAttrib(grassCenterPositionBuffer, 1, 4, GL_FLOAT);

	terrainPositionBuffer	= std::make_shared<ge::gl::Buffer>(terrainPos.size() * sizeof(float), terrainPos.data());
	terrainElementBuffer	= std::make_shared<ge::gl::Buffer>(terrainInd.size() * sizeof(int), terrainInd.data());
	
	terrainVAO = std::make_shared<ge::gl::VertexArray>();
	terrainVAO->addElementBuffer(terrainElementBuffer);
	terrainVAO->addAttrib(terrainPositionBuffer, 0, 4, GL_FLOAT);

	initialized = true;
}

void OpenGLWindow::setTessLevel(int tessLevel)
{
	this->tessLevel = tessLevel;
	//DEBUG
	std::cout << "Tessellation Level: " << tessLevel << std::endl;
	render();
}

void OpenGLWindow::render()
{
	/* RENDER CALL BEGIN */
	context->makeCurrent(this);
	const qreal retinaScale = devicePixelRatio();
	gl->glViewport(0, 0, width() * retinaScale, height() * retinaScale);
	gl->glClearColor(0.0, 0.0, 0.0, 1.0);
	gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	/* DRAW TERRAIN */
	terrainShaderProgram->use();
	terrainVAO->bind();
	gl->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	gl->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	/* DRAW GRASS */
	grassShaderProgram->use();

	// Uniforms
	grassShaderProgram->set1i("uTessLevel", tessLevel);

	grassVAO->bind();
	gl->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	gl->glPatchParameteri(GL_PATCH_VERTICES, 4);
	gl->glDrawElements(GL_PATCHES, 4, GL_UNSIGNED_INT, nullptr);

	/* RENDER CALL END */
	printError();

	context->swapBuffers(this);
}

void OpenGLWindow::printError() const
{
	auto err = this->gl->glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cout << err << std::endl;
	}
}

void OpenGLWindow::renderNow()
{
	if (!isExposed())
		return;
	if (!initialized)
		initialize();

	render();
}

bool OpenGLWindow::event(QEvent* event)
{
	switch (event->type())
	{
		case QEvent::UpdateRequest:
			renderNow();
			return true;
		case QEvent::WindowStateChange:
			renderNow();
			return true;
		default:
			return QWindow::event(event);
	}
}

void OpenGLWindow::exposeEvent(QExposeEvent* event)
{
	if (isExposed())
		renderNow();
}