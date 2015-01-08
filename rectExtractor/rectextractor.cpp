#include "rectextractor.h"

rectExtractor::rectExtractor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	mainWidget = new QWidget();
	this->setCentralWidget(mainWidget);
	//ui.setupUi(this);

	//disp origin dir
	originDirHint = new QLabel(this);
	originDirHint->setText(tr("Current Oirgin Dir"));
	originDirLabel = new QLabel(this);
	originDirLabel->setText(tr("------------"));
	
	//disp mask dir
	maskDirHint = new QLabel(this);
	maskDirHint->setText(tr("Current Mask Dir"));
	maskDirLabel = new QLabel(this);
	maskDirLabel->setText(tr("------------"));

	//disp save dir
	saveDirHint = new QLabel(this);
	saveDirHint->setText(tr("Current Save Dir"));
	saveDirLabel = new QLabel(this);
	saveDirLabel->setText(tr("------------"));

	//extract, draw, get rect
	extractContourButton = new QPushButton(tr("Extract Contour"));
	connect(extractContourButton, SIGNAL(clicked()),
		this, SLOT(OnExtractContourClicked()));
	drawContourButton = new QPushButton(tr("Draw Contour"));
	connect(drawContourButton, SIGNAL(clicked()),
		this, SLOT(OnDrawContourClicked()));
	getRectButton = new QPushButton(tr("Get Rect"));
	connect(getRectButton, SIGNAL(clicked()),
		this, SLOT(OnGetRectClicked()));

	//display image
	imgLabel = new QLabel(this);

	//align : origin
	QHBoxLayout *originLayout = new QHBoxLayout;
	originLayout->addWidget(originDirHint);
	originLayout->addWidget(originDirLabel);
	//align : mask
	QHBoxLayout *maskLayout = new QHBoxLayout;
	maskLayout->addWidget(maskDirHint);
	maskLayout->addWidget(maskDirLabel);
	//align : save
	QHBoxLayout *saveLayout = new QHBoxLayout;
	saveLayout->addWidget(saveDirHint);
	saveLayout->addWidget(saveDirLabel);
	//align : extract, draw, rect
	QHBoxLayout *edrLayout = new QHBoxLayout;
	edrLayout->addWidget(extractContourButton);
	edrLayout->addWidget(drawContourButton);
	edrLayout->addWidget(getRectButton);
	//align : main
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(originLayout);
	mainLayout->addLayout(maskLayout);
	mainLayout->addLayout(saveLayout);
	mainLayout->addLayout(edrLayout);
	mainLayout->addWidget(imgLabel);
	mainWidget->setLayout(mainLayout);

	//----------------------------------------
	createActions();
	createMenus();
	createStatusBar();
	//----------------------------------------
}

rectExtractor::~rectExtractor()
{

}

void rectExtractor::OpenOriginDir()
{
	QFileDialog* fileDlg = new QFileDialog(this);
	fileDlg->setWindowTitle(tr("Open Origin Directory"));
	fileDlg->setDirectory(".");
	fileDlg->setFilter(tr("Image Files(*.jpg *.bmp)"));
	if(fileDlg->exec() == QDialog::Accepted)
	{
		originPath = fileDlg->selectedFiles()[0];
		//load jpeg
		originImage = imread((const char*)originPath.toLocal8Bit());
		originDirLabel->setText(originPath);
	}
	else
	{
		QMessageBox::information(NULL, tr("error msg"), tr("no files selected!"));
	}
}

void rectExtractor::OpenMaskDir()
{
	QFileDialog* fileDlg = new QFileDialog(this);
	fileDlg->setWindowTitle(tr("Open Mask Directory"));
	fileDlg->setDirectory(".");
	fileDlg->setFilter(tr("Image Files(*.jpg *.bmp)"));
	if(fileDlg->exec() == QDialog::Accepted)
	{
		maskPath = fileDlg->selectedFiles()[0];
		maskImage = imread((const char*)maskPath.toLocal8Bit(),0);
		maskDirLabel->setText(maskPath);
	}
	else
	{
		QMessageBox::information(NULL, tr("error msg"), tr("no files selected!"));
	}
}

void rectExtractor::OpenSaveDir()
{
	QFileDialog* fileDlg = new QFileDialog(this);
	fileDlg->setWindowTitle(tr("Open Save Directory"));
	fileDlg->setDirectory(".");
	fileDlg->setFilter(tr("Image Files(*.jpg *.bmp)"));
	if(fileDlg->exec() == QDialog::Accepted)
	{
		savePath = fileDlg->selectedFiles()[0];
		saveDirLabel->setText(maskPath);
	}
	else
	{
		QMessageBox::information(NULL, tr("error msg"), tr("no files selected!"));
	}
}

void rectExtractor::createActions()
{
	selOriginAction = new QAction(tr("&Origin Dir"), this);
	selOriginAction->setStatusTip(tr("Open Origin Directory"));
	connect(selOriginAction, SIGNAL(triggered()),
		this, SLOT(OpenOriginDir()));
	selMaskAction = new QAction(tr("&Mask Dir"), this);
	selMaskAction->setStatusTip(tr("Open Mask Directory"));
	connect(selMaskAction, SIGNAL(triggered()),
		this, SLOT(OpenMaskDir()));
	selSaveAction = new QAction(tr("&Save Dir"), this);
	selSaveAction->setStatusTip(tr("Open Save Directory"));
	connect(selSaveAction, SIGNAL(triggered()),
		this, SLOT(OpenSaveDir()));
	drawContourAction = new QAction(tr("&Draw Contour"), this);
	drawContourAction->setStatusTip(tr("Draw Contour on Origin Image"));
	connect(drawContourAction, SIGNAL(triggered()),
		this, SLOT(OnDrawContourClicked()));
	saveROIAction = new QAction(tr("Save &ROI"), this);
	saveROIAction->setStatusTip(tr("Save ROI as JPG in Save Dir"));
	connect(saveROIAction, SIGNAL(triggered()),
		this, SLOT(OnGetRectClicked()));
}

void rectExtractor::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(selOriginAction);
	fileMenu->addAction(selMaskAction);
	fileMenu->addAction(selSaveAction);

	editMenu = menuBar()->addMenu(tr("Edit"));
	editMenu->addAction(drawContourAction);
	editMenu->addAction(saveROIAction);
}

void rectExtractor::createStatusBar()
{
}

void rectExtractor::traverseDir(QString root)
{
}

void rectExtractor::OnExtractContourClicked()
{
	if(maskPath.isEmpty())
	{
		QMessageBox::information(NULL, 
			tr("error msg"), 
			tr("mask path is empty !"));
		return;
	}
	findContours(maskImage, 
		contours, //vector of contours
		hierarchy, 
		CV_RETR_CCOMP, 
		CV_CHAIN_APPROX_NONE
		);
}

void rectExtractor::OnDrawContourClicked()
{
	if(originPath.isEmpty())
	{
		QMessageBox::information(NULL, 
			tr("error msg"), 
			tr("origin path is empty !"));
		return;
	}
	if(contours.size() == 0)
	{
		QMessageBox::information(NULL, 
			tr("error msg"), 
			tr("no contours found !"));
		return;
	}
	if(contours.size() > 1)
	{
		QMessageBox::information(NULL, 
			tr("error msg"), 
			tr("more than one contour found !"));
		return;
	}
	rgbImage = originImage.clone();
	drawContours(rgbImage,//draw contour on rgb image
		contours,//all contours
		0,//just draw the 1st one
		CV_RGB(255,0,0),//in red
		2//thickness of 2
		);
	cvtColor(rgbImage, rgbImage, CV_BGR2RGB);
	QRgbImg = QImage((const unsigned char*)(rgbImage.data),
		rgbImage.cols,
		rgbImage.rows,
		QImage::Format_RGB888);
	imgLabel->move(0,0);
	imgLabel->setPixmap(QPixmap::fromImage(QRgbImg));
	imgLabel->resize(imgLabel->pixmap()->size());
	imgLabel->show();
}

void rectExtractor::OnGetRectClicked()
{
	if(contours.size() != 1)
	{
		QMessageBox::information(NULL, 
			tr("error msg"), 
			tr("0 or >1 contours found !"));
		return;
	}
	RotatedRect minRRect = minAreaRect(contours[0]);
	minRect = minRRect.boundingRect();
	originImage(minRect).copyTo(roiImage);

	//------------------------
	ipSaveImg = roiImage;
	cvSaveImage("test.jpg", &ipSaveImg);
	//------------------------
	/*
	cvtColor(roiImage, roiImage, CV_BGR2RGB);
	QRoiImg = QImage((const unsigned char*)(roiImage.data),
		roiImage.cols,
		roiImage.rows,
		QImage::Format_RGB888);
	imgLabel->move(0,0);
	imgLabel->setPixmap(QPixmap::fromImage(QRoiImg));
	imgLabel->resize(imgLabel->pixmap()->size());
	imgLabel->show();
	*/
}