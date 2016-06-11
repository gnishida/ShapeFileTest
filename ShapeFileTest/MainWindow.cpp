#include "MainWindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	connect(ui.actionFileOpen, SIGNAL(triggered()), this, SLOT(onFileOpen()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));

	this->setCentralWidget(&canvas);
}

MainWindow::~MainWindow() {
}

void MainWindow::onFileOpen() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open shape file..."), "", tr("Shape Files (*.shp)"));
	if (filename.isEmpty()) return;

	canvas.loadShapfile(filename.toUtf8().constData());
	canvas.update();
}