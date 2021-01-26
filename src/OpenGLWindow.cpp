#include "OpenGLWindow.hpp"

OpenGLWindow::OpenGLWindow(QWindow* parent)
	: QWindow(parent)
	, initialized(false)
	, context(nullptr)
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

	/* Shaders */
	std::cout << "\nVertex shader path: " << VERTEX_SHADER;
	std::cout << "\nFragment shader path: " << FRAGMENT_SHADER << std::endl << std::endl;

	std::shared_ptr<ge::gl::Shader> vertexShader	= std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, ge::util::loadTextFile(VERTEX_SHADER));
	std::shared_ptr<ge::gl::Shader> tessControlShader = std::make_shared<ge::gl::Shader>(GL_TESS_CONTROL_SHADER, ge::util::loadTextFile(TESS_CONTROL_SHADER));
	std::shared_ptr<ge::gl::Shader> tessEvaluationShader = std::make_shared<ge::gl::Shader>(GL_TESS_EVALUATION_SHADER, ge::util::loadTextFile(TESS_EVALUATION_SHADER));
	std::shared_ptr<ge::gl::Shader> fragmentShader = std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, ge::util::loadTextFile(FRAGMENT_SHADER));

	shaderProgram = std::make_shared<ge::gl::Program>(vertexShader, tessControlShader, tessEvaluationShader, fragmentShader);

	std::vector<float> grassBladePos
	{
		-0.25f, -0.5f, 0.0f,
		 0.25f, -0.5f, 0.0f,
		 0.25f,  0.5f, 0.0f,
		-0.25f,  0.5f, 0.0f
		// x, y, z
	};

	std::vector<int> grassBladeInd
	{
		0, 1, 2, 3
	};

	std::vector<float> centerPos
	{
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
		// x, y, z
	};
	
	positionBuffer       = std::make_shared<ge::gl::Buffer>(grassBladePos.size() * sizeof(float), grassBladePos.data());
	centerPositionBuffer = std::make_shared<ge::gl::Buffer>(centerPos.size() * sizeof(float), centerPos.data());
	elementBuffer        = std::make_shared<ge::gl::Buffer>(grassBladeInd.size() * sizeof(int), grassBladeInd.data());

	VAO = std::make_shared<ge::gl::VertexArray>();
	VAO->addElementBuffer(elementBuffer);
	VAO->addAttrib(positionBuffer, 0, 3, GL_FLOAT);
	VAO->addAttrib(centerPositionBuffer, 1, 3, GL_FLOAT);

	initialized = true;
}

void OpenGLWindow::render()
{
	const qreal retinaScale = devicePixelRatio();
	gl->glViewport(0, 0, width() * retinaScale, height() * retinaScale);
	gl->glClearColor(0.0, 0.0, 0.0, 1.0);
	gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	shaderProgram->use();
	VAO->bind();
	gl->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	gl->glPatchParameteri(GL_PATCH_VERTICES, 4);
	gl->glDrawElements(GL_PATCHES, 4, GL_UNSIGNED_INT, nullptr);

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
		default:
			return QWindow::event(event);
	}
}

void OpenGLWindow::exposeEvent(QExposeEvent* event)
{
	if (isExposed())
		renderNow();
}