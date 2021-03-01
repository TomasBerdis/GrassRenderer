#include <iostream>

#include <QApplication>

#include "OpenGLWindow.hpp"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	OpenGLWindow window;
	window.resize(640, 480);
	window.show();

	std::cout << "Grass Renderer is on..." << std::endl << std::endl;
	return app.exec();
}