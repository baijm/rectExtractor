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

	//----------------------------
public slots:
	void OnOpenOriginClicked();
	void OnOpenMaskClicked();
	void OnExtractContourClicked();
	void OnDrawContourClicked();
	void OnGetRectClicked();

public:
	IplImage* ipOriginImg;
	QImage QOriginImg;
	QImage QMaskImg;
	QImage QRgbImg;//for display

private:
	//widgets
	QWidget* mainWidget;
	QPushButton *openOriginButton;
	QPushButton *openMaskButton;
	QPushButton *extractContourButton;
	QPushButton *drawContourButton;
	QPushButton *getRectButton;
	QLabel *originDirLabel;
	QLabel *maskDirLabel;
	QString originPath;
	QString maskPath;
	QLabel *imgLabel;

	//image mat
	Mat originImage;//BGR
	Mat maskImage;//BINARY
	Mat rgbImage;//RGB

	//contours
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	//minimum upright bounding rect
	Rect minRect;
	//----------------------------
};

#endif // RECTEXTRACTOR_H
