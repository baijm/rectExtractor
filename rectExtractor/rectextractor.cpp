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
		this, SLOT(SaveROI()));

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
	fileDlg->setMode(QFileDialog::DirectoryOnly);
	if(fileDlg->exec() == QDialog::Accepted)
	{
		originPath = fileDlg->selectedFile();
		originDirLabel->setText(originPath);
		statusLabel->setText(tr("Finished Setting Origin Directory."));
	}
	else
	{
		QMessageBox::information(NULL, tr("error msg"), tr("no directory selected!"));
	}
}

void rectExtractor::OpenMaskDir()
{
	QFileDialog* fileDlg = new QFileDialog(this);
	fileDlg->setWindowTitle(tr("Open Mask Directory"));
	fileDlg->setDirectory(".");
	fileDlg->setMode(QFileDialog::DirectoryOnly);
	if(fileDlg->exec() == QDialog::Accepted)
	{
		maskPath = fileDlg->selectedFile();
		maskDirLabel->setText(maskPath);
		traverseDir(maskPath, maskPath, "*.bmp");
		numPairs = pairNames.size();
		statusLabel->setText("Finished Setting Mask Directory. ( "+ QString::number(numPairs,10) +" pictures )");
	}
	else
	{
		QMessageBox::information(NULL, 
			tr("error msg"), 
			tr("no directory selected!"));
	}
}

void rectExtractor::OpenSaveDir()
{
	QFileDialog* fileDlg = new QFileDialog(this);
	fileDlg->setWindowTitle(tr("Open Save Directory"));
	fileDlg->setDirectory(".");
	fileDlg->setMode(QFileDialog::DirectoryOnly);
	if(fileDlg->exec() == QDialog::Accepted)
	{
		savePath = fileDlg->selectedFile();
		saveDirLabel->setText(savePath);
		statusLabel->setText(tr("Finished Setting Save Directory. "));
	}
	else
	{
		QMessageBox::information(NULL, tr("error msg"), tr("no directory selected!"));
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
		this, SLOT(SaveROI()));
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
	statusLabel = new QLabel(this);
	statusLabel->setAlignment(Qt::AlignLeft);
	statusLabel->setIndent(3);
	statusBar()->addWidget(statusLabel);
}

void rectExtractor::traverseDir(const QString root, const QString path, const QString extFilter)
{
	QDir dir(path);
	//pairNames.clear();

	//add pair name to list
	foreach(QString imgFile, dir.entryList(extFilter, QDir::Files))
	{
		QString finPath = path;
		finPath.remove(0, root.length());
		QString finFile = imgFile;
		finFile.remove(finFile.length()-extLen, extLen);
		//QString finPath = path.remove(0, root.length());
		pairNames.push_back(finPath+'/'+finFile);
	}
	//traverse sub-directory
	foreach(QString subDir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
	{
		traverseDir(root, path+'/'+subDir, extFilter);
	}
}

void rectExtractor::OnDrawContourClicked()
{
/*
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
	*/
}

void rectExtractor::SaveROI()
{
	if(pairNames.size() == 0)
	{
		QMessageBox::information(NULL, 
			tr("error msg"), 
			tr("mask path not set !"));
		return;
	}
	
	//process each pair of origin & mask
	QVector<QString>::const_iterator iter;
	int count;
	QFile rectpos(savePath+'/'+"rectpos.txt");//ROI: name, lefttop point, width, height
	if (!rectpos.open(QIODevice::WriteOnly 
		| QIODevice::Append
		| QIODevice::Text))
	{
		QMessageBox::information(NULL, 
			tr("error msg"), 
			tr("cannot open log file!"));
		return;
	}
	QTextStream txtout(&rectpos);

	for(iter=pairNames.begin(), count=0; iter!=pairNames.end(); iter++, count++)
	{
		QString oriJpg = originPath+*iter+".jpg";
		QString maskBmp = maskPath+*iter+".bmp";
		//load origin(JPG) & mask(BMP)
		Mat originImage = imread((const char*)oriJpg.toLocal8Bit());
		Mat maskImage = imread((const char*)maskBmp.toLocal8Bit(), 0);
		//find Contours
		findContours(maskImage,
			contours,//vector of contours
			hierarchy,
			CV_RETR_CCOMP,
			CV_CHAIN_APPROX_NONE
			);
		if(contours.size()!=1)
		{
			QMessageBox::information(NULL, 
				tr("error msg"), 
				tr("0 or >1 contours found !")
				);
			continue;
		}
		//find minimum upright bounding rect
		RotatedRect minRRect = minAreaRect(contours[0]);
		minRect = minRRect.boundingRect();

		Mat roiImage;
		try
		{
			//save ROI as JPG in save dir
			originImage(minRect).copyTo(roiImage);
		}
		catch (...)
		{
			//write to log
			txtout << *iter << ' ';//name
			txtout << "exception !" << endl;
			continue;
		}
		ipSaveImg = roiImage;
		//get save path & file name
		QString sPath, sName;
		int idx = iter->findRev('/');
		sPath = savePath+iter->left(idx);
		sName = iter->right(iter->length()-idx-1)+".jpg";
		//(mkdir) save JPG
		QDir tDir;
		if(!tDir.exists(sPath))
		{
			tDir.mkpath(sPath);
		}
		cvSaveImage(sPath+'/'+sName, &ipSaveImg);
		//write to log
		txtout << *iter << ' ';//name
		txtout << minRect.x << ' ' << minRect.y << ' ';//topleft point (x,y)
		txtout << minRect.width << ' ' << minRect.height << endl;
		//update status bar
		statusLabel->setText("finished clipping image "+QString::number(count+1,10)+"/"+QString::number(numPairs,10));
	}
	//close log file
	rectpos.close();

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