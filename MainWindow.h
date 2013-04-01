/***************************************************************************
 *   This file is part of Robocut.                                         *
 *   Copyright (C) 2010 Tim Hutt <tdhutt@gmail.com>                        *
 *   Copyright (C) 2010 Markus Schulz <schulz@alpharesearch.de>            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsItem>

#include "CutDialog.h"

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow *ui;

	// The cutting paths that were loaded from the SVG.
	QList<QPolygonF> paths;

	// For displaying the cuts.
	QGraphicsScene* scene;

	// The dialog that asks what settings to use. We keep this around and reuse it as necessary.
	CutDialog* cutDialog;

	// The directory that the last file was opened from.
	QString lastOpenDir;

	// Timer for the cutting animation.
	QTimer* animationTimer;
	// The circle that marks where the cutter blade is.
	QGraphicsItem* cutMarker;
	// Cut marker progress.
	int cutMarkerPath; // Current path.
	int cutMarkerLine; // Current line in path
	double cutMarkerDistance; // Current distance along edge.

	QSizeF mediaSize;
	
	QString filename;

public:
	int sortFlag;
	int tspFlag;
	int cutFlag;
	char *fileValue;

private slots:
	void on_actionZoom_Out_triggered();
	void on_actionZoom_In_triggered();
	void on_actionReset_triggered();
	void on_actionAnimate_toggled(bool animate);
	void on_actionManual_triggered();
	void on_actionCut_triggered();
	void on_actionExit_triggered();
	void on_actionAbout_triggered();
	void on_actionOpen_triggered();
	void on_actionReload_triggered();
	void on_actionIdentify_triggered();

	// Advance the cutting animation frame.
	void animate();

private:
	// Use empty string to indicate no file is loaded.
	void setFileLoaded(QString filename);
	bool eventFilter(QObject *o, QEvent *e);
	void loadFile();
};

#endif // MAINWINDOW_H
