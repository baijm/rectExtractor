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

	//select mode : clip & auto & manual
	clipModeButton = new QRadioButton(tr("clip origin & mask"));
	autoMarkModeButton = new QRadioButton(tr("auto mark"));
	manualMarkModeButton = new QRadioButton(tr("manual mark"));
	modeButtonGroup = new QButtonGroup();
	modeButtonGroup->addButton(clipModeButton, 0);
	modeButtonGroup->addButton(autoMarkModeButton, 1);
	modeButtonGroup->addButton(manualMarkModeButton, 2);
	connect(modeButtonGroup, SIGNAL(buttonClicked(int)),
		this, SLOT(SwitchMode(int)));

	//extract, draw, get rect
	extractContourButton = new QPushButton(tr("Extract Contour"));
	connect(extractContourButton, SIGNAL(clicked()),
		this, SLOT(OnExtractContourClicked()));
	saveAutoMarkButton = new QPushButton(tr("Save Auto Mark"));
	connect(saveAutoMarkButton, SIGNAL(clicked()),
		this, SLOT(OnSaveAutoMarkClicked()));
	//connect
	countPoints = new QLabel(this);
	countPoints->setText(tr("total xxxx points"));

	//display image
	imgLabel = new QLabel(this);
	//point list (result of cv::findContour)
	pointList = new QListWidget(this);
	pointList->setFixedWidth(100);

	//show last / next image
	lastImgButton = new QPushButton(tr("< Last"));
	connect(lastImgButton, SIGNAL(clicked()),
		this, SLOT(ShowLastImage()));
	nextImgButton = new QPushButton(tr("Next >"));
	connect(nextImgButton, SIGNAL(clicked()),
		this, SLOT(ShowNextImage()));

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
	//align : select mode
	QHBoxLayout *modeLayout = new QHBoxLayout;
	modeLayout->addWidget(clipModeButton);
	modeLayout->addWidget(autoMarkModeButton);
	modeLayout->addWidget(manualMarkModeButton);
	//align : extract, draw
	QHBoxLayout *edrLayout = new QHBoxLayout;
	edrLayout->addWidget(extractContourButton);
	edrLayout->addWidget(saveAutoMarkButton);
	edrLayout->addWidget(countPoints);
	//align : last, image, next, point list
	QHBoxLayout *imgLayout = new QHBoxLayout;
	imgLayout->addWidget(lastImgButton);
	imgLayout->addWidget(imgLabel);
	imgLayout->addWidget(nextImgButton);
	imgLayout->addWidget(pointList);
	//align : main
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(originLayout);
	mainLayout->addLayout(maskLayout);
	mainLayout->addLayout(saveLayout);
	mainLayout->addLayout(modeLayout);//select mode
	mainLayout->addLayout(edrLayout);
	mainLayout->addLayout(imgLayout);
	mainWidget->setLayout(mainLayout);

	//----------------------------------------
	createActions();
	createMenus();
	createStatusBar();
	//----------------------------------------

	QRgbImg = new QImage;
	showIdx = -1;//invalid index
}

rectExtractor::~rectExtractor()
{

}
//slots
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
		QMessageBox::information(NULL, 
			tr("error msg"), 
			tr("no directory selected!"));
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
		QMessageBox::information(NULL, 
			tr("error msg"), 
			tr("no directory selected!"));
	}
}

void rectExtractor::SwitchMode(int btnId)
{
	switch(btnId)
	{
	case 0: 
		QMessageBox::information(NULL, 
			tr("info"), 
			tr("changed mode to clipping !"));
		break;
	case 1:	
		QMessageBox::information(NULL, 
			tr("info"), 
			tr("changed mode to auto mark !"));
		break;
	case 2:	
		QMessageBox::information(NULL, 
			tr("info"), 
			tr("changed mode to manual mark !"));
		break;
	}
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
		minRect.x -= margin;//expand bouding rect to contain antenna
		minRect.y -= margin;
		minRect.height += 2*margin;
		minRect.width += 2*margin;

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

void rectExtractor::OnExtractContourClicked()
{
	if(showIdx<0 || showIdx==pairNames.size())
	{
		return;
	}
	QString roiName = savePath+pairNames[showIdx]+".jpg";
	currGrayImage = imread((const char*)roiName.toLocal8Bit(),0);
	currImage = imread((const char*)roiName.toLocal8Bit());
	//find contours
	findContours(currGrayImage,
		contours,
		hierarchy,
		CV_RETR_CCOMP,
		CV_CHAIN_APPROX_SIMPLE
		);
	
	pointList->clear();
	int total=0;
	for(vector<vector<Point>>::const_iterator allConIte = contours.begin(); allConIte!= contours.end(); allConIte++)
	{
		for(vector<Point>::const_iterator pointIte = allConIte->begin(); pointIte != allConIte->end(); pointIte++)
		{
			pointList->addItem("x:"+QString::number(pointIte->x)+"  y:"+QString::number(pointIte->y));
			total++;
		}
	}
	countPoints->setText(tr("total "+QString::number(total)+" points"));
	statusLabel->setText(tr("extract contour points finished"));
}

void rectExtractor::OnSaveAutoMarkClicked()
{
	ipSaveImg = currImage;
	for(vector<vector<Point>>::const_iterator allConIte = contours.begin(); allConIte!= contours.end(); allConIte++)
	{
		for(vector<Point>::const_iterator pointIte = allConIte->begin(); pointIte != allConIte->end(); pointIte++)
		{
			cvCircle(&ipSaveImg,
				*pointIte,
				2,
				CV_RGB(255,0,0),
				2
				);
		}
	}
	QString autoMarkName = savePath+pairNames[showIdx]+"_auto.jpg";
	cvSaveImage(autoMarkName, &ipSaveImg);
	statusLabel->setText("save auto mark to "+autoMarkName);

	if (!QRgbImg->load(autoMarkName))
	{
		return;
	}
	imgLabel->setPixmap(QPixmap::fromImage(*QRgbImg));
}

void rectExtractor::ShowLastImage()
{
	showIdx--;
	if(pairNames.size()==0)
		return;
	if(showIdx<0)
		showIdx = pairNames.size()-1;

	QString roiName = savePath+pairNames[showIdx]+".jpg";

	if (!QRgbImg->load(roiName))
	{
		return;
	}
	imgLabel->setPixmap(QPixmap::fromImage(*QRgbImg));

	pointList->clear();
	countPoints->setText(tr("total xxxx points"));
	statusLabel->setText(roiName);
}

void rectExtractor::ShowNextImage()
{
	showIdx++;
	if (pairNames.size()==0)
		return;
	if(showIdx == pairNames.size())
		showIdx = 0;

	QString roiName = savePath+pairNames[showIdx]+".jpg";

	if(!QRgbImg->load(roiName))
	{
		return;
	}
	imgLabel->setPixmap(QPixmap::fromImage(*QRgbImg));

	pointList->clear();
	countPoints->setText(tr("total xxxx points"));
	statusLabel->setText(roiName);
}

//create functions
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
		this, SLOT(OnExtractContourClicked()));
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

