#include <iostream>

#include <QApplication>

#include "OpenGLWindow.hpp"

int main(int argc, char **argv)
{
	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	format.setVersion(4, 5);
	format.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(format);

	QApplication app(argc, argv);

	OpenGLWindow window;
	window.showFullScreen();

	std::cout << "Grass Renderer is on..." << std::endl << std::endl;
	return app.exec();
}