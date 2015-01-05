#include "rectextractor.h"

rectExtractor::rectExtractor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	mainWidget = new QWidget();
	this->setCentralWidget(mainWidget);
	//ui.setupUi(this);

	//open origin image
	openOriginButton = new QPushButton(tr("Open Origin File"));
	openOriginButton->setDefault(true);
	openOriginButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	connect(openOriginButton, SIGNAL(clicked()),
		this, SLOT(OnOpenOriginClicked()));
	originDirLabel = new QLabel(this);
	originDirLabel->setText(tr("please choose origin image"));
	originDirLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

	//open mask image
	openMaskButton = new QPushButton(tr("Open Mask File"));
	openMaskButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	connect(openMaskButton, SIGNAL(clicked()),
		this, SLOT(OnOpenMaskClicked()));
	maskDirLabel = new QLabel(this);
	maskDirLabel->setText(tr("please choose mask file"));
	maskDirLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

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
	originLayout->addWidget(openOriginButton);
	originLayout->addWidget(originDirLabel);
	//align : mask
	QHBoxLayout *maskLayout = new QHBoxLayout;
	maskLayout->addWidget(openMaskButton);
	maskLayout->addWidget(maskDirLabel);
	//align : extract, draw, rect
	QHBoxLayout *edrLayout = new QHBoxLayout;
	edrLayout->addWidget(extractContourButton);
	edrLayout->addWidget(drawContourButton);
	edrLayout->addWidget(getRectButton);
	//align : main
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(originLayout);
	mainLayout->addLayout(maskLayout);
	mainLayout->addLayout(edrLayout);
	mainLayout->addWidget(imgLabel);

	mainWidget->setLayout(mainLayout);
}

rectExtractor::~rectExtractor()
{

}

void rectExtractor::OnOpenOriginClicked()
{
	QFileDialog* fileDlg = new QFileDialog(this);
	fileDlg->setWindowTitle(tr("Open Origin File"));
	fileDlg->setDirectory(".");
	fileDlg->setFilter(tr("Image Files(*.jpg *.bmp)"));
	if(fileDlg->exec() == QDialog::Accepted)
	{
		originPath = fileDlg->selectedFiles()[0];
		//load jpeg
		originImage = imread((const char*)originPath.toLocal8Bit());
		ipOriginImg = &IplImage(originImage);
		/*Mat rgb;
		cvtColor(originImage, rgb, CV_BGR2RGB);

		QOriginImg = QImage((const unsigned char*)(rgb.data), 
			rgb.cols,
			rgb.rows,
			QImage::Format_RGB888);

		imgLabel->move(0,0);
		imgLabel->setPixmap(QPixmap::fromImage(QOriginImg));
		imgLabel->resize(imgLabel->pixmap()->size());
		imgLabel->show();
		*/
		originDirLabel->setText(originPath);
	}
	else
	{
		QMessageBox::information(NULL, tr("error msg"), tr("no files selected!"));
	}
}

void rectExtractor::OnOpenMaskClicked()
{
	QFileDialog* fileDlg = new QFileDialog(this);
	fileDlg->setWindowTitle(tr("Open Mask File"));
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
	rgbImage = originImage(minRect);

	cvtColor(rgbImage, rgbImage, CV_BGR2RGB);
	QRgbImg = QImage((const unsigned char*)(rgbImage.data),
		rgbImage.cols,
		rgbImage.rows,
		QImage::Format_RGB888);
	imgLabel->move(0,0);
	imgLabel->setPixmap(QPixmap::fromImage(QRgbImg));
	imgLabel->resize(imgLabel->pixmap()->size());
	imgLabel->show();
	/*
	rgbImage = originImage.clone();
	cvtColor(rgbImage, rgbImage, CV_BGR2RGB);
	QRgbImg = QImage((const unsigned char*)(rgbImage.data),
		rgbImage.cols,
		rgbImage.rows,
		QImage::Format_RGB888);
	imgLabel->move(0,0);
	imgLabel->setPixmap(QPixmap::fromImage(QRgbImg));
	imgLabel->resize(imgLabel->pixmap()->size());
	imgLabel->show();
	*/
}