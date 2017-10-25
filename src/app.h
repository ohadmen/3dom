#ifndef APP_H
#define APP_H

#include <QApplication>
#include <QFileOpenEvent>
#include <QDebug>

#include "window.h"



class App : public QApplication
{
    Q_OBJECT
public:
    explicit App(int& argc, char *argv[]) :
		QApplication(argc, argv), window(new Window())
	{
		QCoreApplication::setOrganizationName("Ohad");
		QCoreApplication::setOrganizationDomain("https://github.com/ohadmen/3dom");
		QCoreApplication::setApplicationName("3Dom");

		if (argc > 1)
			window->load_stl(argv[1]);
		else
			window->load_stl(":gl/horse.stl");
		window->show();
	}

protected:
	bool event(QEvent* e)
	{
		if (e->type() == QEvent::FileOpen)
		{
			window->load_stl(static_cast<QFileOpenEvent*>(e)->file());
			return true;
		}
		else
		{
			return QApplication::event(e);
		}
	}

private:
    Window* const window;

};

#endif // APP_H
