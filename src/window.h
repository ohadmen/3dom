#ifndef WINDOW_H
#define WINDOW_H

#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QMainWindow>
#include <QActionGroup>
#include <QFileSystemWatcher>
#include <QStatusBar>
#include "loader.h"
#include "canvas.h"
class Canvas;

class Window : public QMainWindow
{
	Q_OBJECT

private:

	const  QString Window::RECENT_FILE_KEY = "recentFiles";
	const  int MAX_RECENT_FILES = 8;

	void rebuild_recent_files()
	{
		QSettings settings;
		QStringList files = settings.value(RECENT_FILE_KEY).toStringList();

		const auto actions = recent_files_group->actions();
		for (auto a : actions)
		{
			recent_files_group->removeAction(a);
		}
		recent_files->clear();

		for (auto f : files)
		{
			const auto a = new QAction(f, recent_files);
			a->setData(f);
			recent_files_group->addAction(a);
			recent_files->addAction(a);
		}
		if (files.size() == 0)
		{
			auto a = new QAction("No recent files", recent_files);
			recent_files->addAction(a);
			a->setEnabled(false);
		}
		recent_files->addSeparator();
		recent_files->addAction(recent_files_clear_action);
	}
	QAction* const open_action;
	QAction* const about_action;
	QAction* const quit_action;
	QAction* const perspective_action;
	QAction* const orthogonal_action;
	QAction* const reload_action;
	QAction* const autoreload_action;


	QAction* const txtureMapping_none;
	QAction* const txtureMapping_normal;
	QAction* const txtureMapping_texture;


	QMenu* const recent_files;
	QActionGroup* const recent_files_group;
	QAction* const recent_files_clear_action;



	QFileSystemWatcher m_watcher;

	Canvas* m_canvas;
	

public:
	explicit Window(QWidget *parent = 0) :
		QMainWindow(parent),
		open_action(new QAction("Open", this)),
		about_action(new QAction("About", this)),
		quit_action(new QAction("Quit", this)),
		perspective_action(new QAction("Perspective", this)),
		orthogonal_action(new QAction("Orthographic", this)),
		reload_action(new QAction("Reload", this)),
		autoreload_action(new QAction("Autoreload", this)),
		recent_files(new QMenu("Open recent", this)),
		recent_files_group(new QActionGroup(this)),
		recent_files_clear_action(new QAction("Clear recent files", this)),
		m_watcher(this),
		txtureMapping_none(new QAction("none", this)),
		txtureMapping_normal(new QAction("normalMap", this)),
		txtureMapping_texture(new QAction("texture", this))

	{
		setWindowTitle("3Dom");
		setAcceptDrops(true);
		


		

		QGLFormat format;
		format.setVersion(2, 1);
		format.setSampleBuffers(true);

		m_canvas = new Canvas(format, this);
		setCentralWidget(m_canvas);

		QObject::connect(&m_watcher, &QFileSystemWatcher::fileChanged,
			this, &Window::on_watched_change);

		open_action->setShortcut(QKeySequence::Open);
		QObject::connect(open_action, &QAction::triggered,
			this, &Window::on_open);

		quit_action->setShortcut(QKeySequence::Quit);
		QObject::connect(quit_action, &QAction::triggered,
			this, &Window::close);

		autoreload_action->setCheckable(true);
		autoreload_action->setChecked(true);
		autoreload_action->setEnabled(false);
		QObject::connect(autoreload_action, &QAction::triggered,
			this, &Window::on_autoreload_triggered);

		reload_action->setShortcut(QKeySequence::Refresh);
		reload_action->setEnabled(false);
		QObject::connect(reload_action, &QAction::triggered,
			this, &Window::on_reload);

		QObject::connect(about_action, &QAction::triggered,
			this, &Window::on_about);

		QObject::connect(recent_files_clear_action, &QAction::triggered,
			this, &Window::on_clear_recent);
		QObject::connect(recent_files_group, &QActionGroup::triggered,
			this, &Window::on_load_recent);

		rebuild_recent_files();

		auto file_menu = menuBar()->addMenu("File");
		file_menu->addAction(open_action);
		file_menu->addMenu(recent_files);
		file_menu->addSeparator();
		file_menu->addAction(reload_action);
		file_menu->addAction(autoreload_action);
		file_menu->addAction(quit_action);

		auto view_menu = menuBar()->addMenu("View");
		auto projection_menu = view_menu->addMenu("Projection");
		projection_menu->addAction(perspective_action);
		projection_menu->addAction(orthogonal_action);
		auto projections = new QActionGroup(projection_menu);
		for (auto p : { perspective_action, orthogonal_action })
		{
			projections->addAction(p);
			p->setCheckable(true);
		}
		perspective_action->setChecked(true);
		projections->setExclusive(true);
		QObject::connect(projections, &QActionGroup::triggered,
			this, &Window::on_projection);



		auto texture_menu = view_menu->addMenu("Texture");
		texture_menu->addAction(txtureMapping_none);
		texture_menu->addAction(txtureMapping_normal);
		texture_menu->addAction(txtureMapping_texture);
		auto textures = new QActionGroup(texture_menu);
		for (auto p : { txtureMapping_none, txtureMapping_normal,txtureMapping_texture })
		{
			textures->addAction(p);
			p->setCheckable(true);
		}
		txtureMapping_none->setChecked(true);
		textures->setExclusive(true);
		QObject::connect(textures, &QActionGroup::triggered,
			this, &Window::on_texture);




		auto help_menu = menuBar()->addMenu("Help");
		help_menu->addAction(about_action);

		resize(600, 400);
	}

	~Window()
	{
		delete m_canvas;
	}
	bool load_stl(const QString& filename, bool is_reload = false)
	{
		if (!open_action->isEnabled())  return false;

		m_canvas->set_status("Loading " + filename);

		Loader* loader = new Loader(this, filename, is_reload);
		connect(loader, &Loader::started,
			this, &Window::disable_open);

		connect(loader, &Loader::got_mesh,
			m_canvas, &Canvas::load_mesh);
		connect(loader, &Loader::error_bad_stl,
			this, &Window::on_bad_stl);
		connect(loader, &Loader::error_empty_mesh,
			this, &Window::on_empty_mesh);
		connect(loader, &Loader::warning_confusing_stl,
			this, &Window::on_confusing_stl);
		connect(loader, &Loader::error_missing_file,
			this, &Window::on_missing_file);

		connect(loader, &Loader::finished,
			loader, &Loader::deleteLater);
		connect(loader, &Loader::finished,
			this, &Window::enable_open);
		connect(loader, &Loader::finished,
			m_canvas, &Canvas::clear_status);

		if (filename[0] != ':')
		{
			connect(loader, &Loader::loaded_file,
				this, &Window::setWindowTitle);
			connect(loader, &Loader::loaded_file,
				this, &Window::set_watched);
			autoreload_action->setEnabled(true);
			reload_action->setEnabled(true);
		}

		loader->start();
		return true;
	}

protected:
	void dragEnterEvent(QDragEnterEvent *event)
	{
		if (event->mimeData()->hasUrls())
		{
			auto urls = event->mimeData()->urls();
			if (urls.size() == 1 && urls.front().path().endsWith(".stl"))
				event->acceptProposedAction();
		}
	}
	void dropEvent(QDropEvent *event)
	{
		load_stl(event->mimeData()->urls().front().toLocalFile());
	}

	


	public slots:


	void on_open()
	{
		QString filename = QFileDialog::getOpenFileName(
			this, "Load .stl file", QString(), "*.stl");
		if (!filename.isNull())
		{
			load_stl(filename);
		}
	}

	void on_about()
	{
		QMessageBox::about(this, "",
			"<p align=\"center\"><b>3Dom</b></p>"
	}

	void on_bad_stl()
	{
		QMessageBox::critical(this, "Error",
			"<b>Error:</b><br>"
			"This <code>.stl</code> file is invalid or corrupted.<br>"
			"Please export it from the original source, verify, and retry.");
	}

	void on_empty_mesh()
	{
		QMessageBox::critical(this, "Error",
			"<b>Error:</b><br>"
			"This file is syntactically correct<br>but contains no triangles.");
	}

	void on_confusing_stl()
	{
		QMessageBox::warning(this, "Warning",
			"<b>Warning:</b><br>"
			"This <code>.stl</code> file begins with <code>solid </code>but appears to be a binary file.<br>"
			"<code>3dom</code> loaded it, but other programs may be confused by this file.");
	}

	void on_missing_file()
	{
		QMessageBox::critical(this, "Error",
			"<b>Error:</b><br>"
			"The target file is missing.<br>");
	}

	void enable_open()
	{
		open_action->setEnabled(true);
	}

	void disable_open()
	{
		open_action->setEnabled(false);
	}





	void set_watched(const QString& filename)
	{
		const auto files = m_watcher.files();
		if (files.size())
		{
			m_watcher.removePaths(m_watcher.files());
		}
		m_watcher.addPath(filename);

		QSettings settings;
		auto recent = settings.value(RECENT_FILE_KEY).toStringList();
		const auto f = QFileInfo(filename).absoluteFilePath();
		recent.removeAll(f);
		recent.prepend(f);
		while (recent.size() > MAX_RECENT_FILES)
		{
			recent.pop_back();
		}
		settings.setValue(RECENT_FILE_KEY, recent);
		rebuild_recent_files();
	}

	private slots:
	void on_projection(QAction* proj)
	{
		if (proj == perspective_action)
		{
			m_canvas->view_perspective();
		}
		else
		{
			m_canvas->view_orthographic();
		}
	}
	void on_texture(QAction* proj)
	{
		if (proj == perspective_action)
		{
			m_canvas->view_perspective();
		}
		else
		{
			m_canvas->view_orthographic();
		}
	}



	void on_watched_change(const QString& filename)
	{
		if (autoreload_action->isChecked())
		{
			load_stl(filename, true);
		}
	}
	void on_autoreload_triggered(bool b)
	{
		if (b)
		{
			on_reload();
		}
	}
	void on_clear_recent()
	{
		QSettings settings;
		settings.setValue(RECENT_FILE_KEY, QStringList());
		rebuild_recent_files();
	}
	void on_load_recent(QAction* a)
	{
		load_stl(a->data().toString());
	}

	void on_reload()
	{
		auto fs = m_watcher.files();
		if (fs.size() == 1)
		{
			load_stl(fs[0], true);
		}
	}




};

#endif // WINDOW_H
