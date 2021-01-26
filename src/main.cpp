#include <iostream>

#include <QtGui/QGuiApplication>

#include "OpenGLWindow.hpp"

int main(int argc, char **argv)
{
	QGuiApplication app(argc, argv);

	OpenGLWindow window;
	window.resize(640, 480);
	window.show();

	std::cout << "Grass Renderer is on...";
	return app.exec();
}