#ifndef RECTEXTRACTOR_H
#define RECTEXTRACTOR_H

#include <QtGui/QMainWindow>
//#include "ui_rectextractor.h"

//------------------
#include <QtGui/QFileDialog>
#include <QtGui/QLabel>
#include <QtGui/QImage>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPainter>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;
//------------------

class rectExtractor : public QMainWindow
{
	Q_OBJECT

public:
	rectExtractor(QWidget *parent = 0, Qt::WFlags flags = 0);
	~rectExtractor();

private:
	//Ui::rectExtractorClass ui;
public slots:
	//void OnOpenOriginClicked();
	//void OnOpenMaskClicked();
	void OnExtractContourClicked();
	void OnDrawContourClicked();
	void OnGetRectClicked();

	void OpenOriginDir();
	void OpenMaskDir();
	void OpenSaveDir();

public:
	IplImage ipSaveImg;//save ROI
	QImage QRgbImg;//for display (full)
	QImage QRoiImg;//ROI

private:
	//widgets
	QWidget* mainWidget;
	QPushButton *extractContourButton;
	QPushButton *drawContourButton;
	QPushButton *getRectButton;
	QLabel *originDirHint, *originDirLabel;
	QLabel *maskDirHint, *maskDirLabel;
	QLabel *saveDirHint, *saveDirLabel;
	QLabel *imgLabel;
	QMenu *fileMenu;
	QMenu *editMenu;
	QAction *selOriginAction;
	QAction *selMaskAction;
	QAction *selSaveAction;
	QAction *drawContourAction;
	QAction *saveROIAction;
	//----------------------------------
	QString originPath;//origin dir
	QString maskPath;//mask dir
	QString savePath;//save extracted ROI
	int numPairs;//total number of pairs
	QVector<QString> pairNames;
	//----------------------------------

	//image mat
	Mat originImage;//BGR
	Mat maskImage;//BINARY
	Mat rgbImage;//RGB
	Mat roiImage;//ROI

	//contours
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	//minimum upright bounding rect
	Rect minRect;

private:
	void createActions();
	void createMenus();
	void createStatusBar();
	void traverseDir(QString root);
};

#endif // RECTEXTRACTOR_H
