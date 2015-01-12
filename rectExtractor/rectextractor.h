#ifndef RECTEXTRACTOR_H
#define RECTEXTRACTOR_H

#include <QtGui/QMainWindow>
//#include "ui_rectextractor.h"

#include <QtGui/QFileDialog>
#include <QtGui/QLabel>
#include <QtGui/QImage>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QButtonGroup>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPainter>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QListWidget>

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QIODevice>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

const int extLen = 4;
const int margin = 10;

class rectExtractor : public QMainWindow
{
	Q_OBJECT

public:
	rectExtractor(QWidget *parent = 0, Qt::WFlags flags = 0);
	~rectExtractor();

private:
	//Ui::rectExtractorClass ui;

private:
	//widgets
	QWidget* mainWidget;
	//-- directories
	QLabel *originDirHint, *originDirLabel;
	QLabel *maskDirHint, *maskDirLabel;
	QLabel *saveOriginDirHint, *saveOriginDirLabel;
	QLabel *saveMaskDirHint, *saveMaskDirLabel;
	//-- modes
	QRadioButton *clipModeButton;//clip origin & mask
	QRadioButton *autoMarkModeButton;//auto mark clipped origin
	QRadioButton *manualMarkModeButton;//manually mark clipped origin
	QButtonGroup *modeButtonGroup;
	//-- process buttons
	QPushButton *extractContourButton;
	QPushButton *saveAutoMarkButton;
	//-- navigate pictures & picture info
	QPushButton *lastImgButton;//show last image
	QPushButton *nextImgButton;//show next image
	QLabel *imgLabel;//display image
	QLabel *countPoints;
	QListWidget *pointList;
	
	//menus & status bar
	QLabel *statusLabel;//status bar
	QMenu *fileMenu;
	QMenu *editMenu;
	
	//actions
	QAction *selOriginAction;
	QAction *selMaskAction;
	QAction *selSaveOriginAction;
	QAction *selSaveMaskAction;
	QAction *drawContourAction;
	QAction *saveClippedAction;
	
	//data members
	QString originPath;//origin dir
	QString maskPath;//mask dir
	QString saveClippedOriginPath;//save extracted ROI of origin
	QString saveClippedMaskPath;//save extracted ROI of mask
	int numPairs;//total number of pairs
	QVector<QString> pairNames;
	//Mat originImage;//BGR
	//Mat maskImage;//BINARY
	Mat currGrayImage;//current grayscale ROI (for drawing points)
	Mat currImage;//current color ROI
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	Rect minRect;//minimum upright bounding rect
	IplImage ipSaveImg;//save ROI
	int showIdx;//index of current shown image
	QImage *QRgbImg;//for display ROI

public slots:
	//open dirs
	void OpenOriginDir();
	void OpenMaskDir();
	void OpenSaveOriginDir();
	void OpenSaveMaskDir();

	void SwitchMode(int btnId);
	void OnExtractContourClicked();

	void ShowLastImage();//show last image
	void ShowNextImage();//show next image
	void SaveClippedROI();
	void OnSaveAutoMarkClicked();

private:
	void createActions();
	void createMenus();
	void createStatusBar();
	void traverseDir(const QString root, const QString path, const QString extFilter);
};

#endif // RECTEXTRACTOR_H
