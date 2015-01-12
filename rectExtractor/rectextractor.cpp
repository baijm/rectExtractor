#include "rectextractor.h"

rectExtractor::rectExtractor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	mainWidget = new QWidget();
	this->setCentralWidget(mainWidget);
	//ui.setupUi(this);

	//disp origin dir
	originDirHint = new QLabel(this);
	originDirHint->setText(tr("Oirgin Dir"));
	originDirLabel = new QLabel(this);
	originDirLabel->setText(tr("------------"));
	
	//disp mask dir
	maskDirHint = new QLabel(this);
	maskDirHint->setText(tr("Mask Dir"));
	maskDirLabel = new QLabel(this);
	maskDirLabel->setText(tr("------------"));

	//disp clipped origin dir
	saveOriginDirHint = new QLabel(this);
	saveOriginDirHint->setText(tr("Save Clipped Origin Dir"));
	saveOriginDirLabel = new QLabel(this);
	saveOriginDirLabel->setText(tr("------------"));

	//disp clipped mask dir
	saveMaskDirHint = new QLabel(this);
	saveMaskDirHint->setText(tr("Save Clipped Mask Dir"));
	saveMaskDirLabel = new QLabel(this);
	saveMaskDirLabel->setText(tr("------------"));

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
	//align : save origin
	QHBoxLayout *saveOriLayout = new QHBoxLayout;
	saveOriLayout->addWidget(saveOriginDirHint);
	saveOriLayout->addWidget(saveOriginDirLabel);
	//align : save mask
	QHBoxLayout *saveMasLayout = new QHBoxLayout;
	saveMasLayout->addWidget(saveMaskDirHint);
	saveMasLayout->addWidget(saveMaskDirLabel);
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
	mainLayout->addLayout(saveOriLayout);
	mainLayout->addLayout(saveMasLayout);
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

//open dirs
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

void rectExtractor::OpenSaveOriginDir()
{
	QFileDialog* fileDlg = new QFileDialog(this);
	fileDlg->setWindowTitle(tr("Open Save Origin Directory"));
	fileDlg->setDirectory(".");
	fileDlg->setMode(QFileDialog::DirectoryOnly);
	if(fileDlg->exec() == QDialog::Accepted)
	{
		saveClippedOriginPath = fileDlg->selectedFile();
		saveOriginDirLabel->setText(saveClippedOriginPath);
		statusLabel->setText(tr("Finished Setting Save Origin Directory. "));
	}
	else
	{
		QMessageBox::information(NULL, 
			tr("error msg"), 
			tr("no directory selected!"));
	}
}

void rectExtractor::OpenSaveMaskDir()
{
	QFileDialog* fileDlg = new QFileDialog(this);
	fileDlg->setWindowTitle(tr("Open Save Mask Directory"));
	fileDlg->setDirectory(".");
	fileDlg->setMode(QFileDialog::DirectoryOnly);
	if(fileDlg->exec() == QDialog::Accepted)
	{
		saveClippedMaskPath = fileDlg->selectedFile();
		saveMaskDirLabel->setText(saveClippedMaskPath);
		statusLabel->setText(tr("Finished Setting Save Mask Directory. "));
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
	originPath.clear();
	maskPath.clear();
	saveClippedOriginPath.clear();
	saveClippedMaskPath.clear();
	numPairs = 0;
	pairNames.clear();
	contours.clear();
	hierarchy.clear();
	showIdx = -1;

	switch(btnId)
	{
	case 0: //clipping origin & mask
		originDirHint->show();
		originDirLabel->show();
		maskDirHint->show();
		maskDirLabel->show();
		saveOriginDirHint->show();
		saveOriginDirLabel->show();
		saveMaskDirHint->show();
		saveMaskDirLabel->show();

		extractContourButton->hide();
		saveAutoMarkButton->hide();

		imgLabel->hide();
		countPoints->hide();
		pointList->hide();

		selOriginAction->setEnabled(true);
		selMaskAction->setEnabled(true);
		selSaveOriginAction->setEnabled(true);
		selSaveMaskAction->setEnabled(true);
		drawContourAction->setEnabled(false);
		saveClippedAction->setEnabled(true);

		break;
	case 1:	//auto mark
		originDirHint->show();
		originDirLabel->show();
		maskDirHint->show();
		maskDirLabel->show();
		saveOriginDirHint->show();//TODO : setText("Save result to...")
		saveOriginDirLabel->show();
		saveMaskDirHint->hide();
		saveMaskDirLabel->hide();

		extractContourButton->show();
		saveAutoMarkButton->show();

		lastImgButton->show();
		nextImgButton->show();
		imgLabel->show();
		countPoints->show();
		pointList->show();

		selOriginAction->setEnabled(true);
		selMaskAction->setEnabled(true);
		selSaveOriginAction->setEnabled(true);//TODO : setText("Save result to...")
		selSaveMaskAction->setEnabled(false);
		drawContourAction->setEnabled(false);//
		saveClippedAction->setEnabled(false);//

		break;
	case 2:	//manual mark
		originDirHint->show();
		originDirLabel->show();
		maskDirHint->hide();
		maskDirLabel->hide();
		saveOriginDirHint->show();
		saveOriginDirLabel->show();
		saveMaskDirHint->hide();
		saveMaskDirLabel->hide();
		
		extractContourButton->hide();
		saveAutoMarkButton->hide();

		lastImgButton->show();
		nextImgButton->show();
		imgLabel->show();
		countPoints->show();
		pointList->show();

		selOriginAction->setEnabled(true);
		selMaskAction->setEnabled(false);
		selSaveOriginAction->setEnabled(true);
		selSaveMaskAction->setEnabled(false);
		drawContourAction->setEnabled(false);
		saveClippedAction->setEnabled(false);

		break;
	}
}

void rectExtractor::SaveClippedROI()
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
	QFile rectpos(saveClippedOriginPath+'/'+"rectpos.txt");//ROI: name, lefttop point, width, height
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
		Mat maskImage2 = imread((const char*)maskBmp.toLocal8Bit(), 0);
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

		//save origin ROI as JPG in save dir
		Mat roiImage;
		try
		{
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
		sPath =saveClippedOriginPath+iter->left(idx);
		sName = iter->right(iter->length()-idx-1)+".jpg";
		//(mkdir) save JPG
		QDir tDir;
		if(!tDir.exists(sPath))
		{
			tDir.mkpath(sPath);
		}
		cvSaveImage(sPath+'/'+sName, &ipSaveImg);

		//save mask ROI as JPG in save dir
		try{
			maskImage2(minRect).copyTo(roiImage);
		}catch(...)
		{
			continue;
		}
		ipSaveImg = roiImage;
		//get save path & file name
		sPath = saveClippedMaskPath+iter->left(idx);
		sName = iter->right(iter->length()-idx-1)+".bmp";
		//(mkdir) save JPG
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
	QString originName =originPath + pairNames[showIdx] + ".jpg";
	QString maskName = maskPath + pairNames[showIdx] + ".bmp";
	//Mat maskImage = imread((const char*)maskBmp.toLocal8Bit(), 0);
	currImage = imread((const char*)originName.toLocal8Bit());
	currGrayImage = imread((const char*)maskName.toLocal8Bit(),0);
	//find contours
	findContours(currGrayImage,
		contours,
		hierarchy,
		CV_RETR_CCOMP,
		CV_CHAIN_APPROX_NONE
		);
	approxPolyDP(contours[0],
		contours[0],
		10,//TODO : add widget
		true);
	
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
				1,
				CV_RGB(255,0,0),
				2
				);
		}
	}
	QString autoMarkName =saveClippedOriginPath+pairNames[showIdx]+".jpg";
	int idx = pairNames[showIdx].findRev('/');
	QString sPath = saveClippedOriginPath + pairNames[showIdx].left(idx);
	QString sName = pairNames[showIdx].right(pairNames[showIdx].length()-idx-1)+".jpg";
	QDir tDir;
	if(!tDir.exists(sPath))
	{
		tDir.mkpath(sPath);
	}
	cvSaveImage(sPath+'/'+sName, &ipSaveImg);

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

	QString roiName =originPath+pairNames[showIdx]+".jpg";

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

	QString roiName =originPath+pairNames[showIdx]+".jpg";

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
	
	selSaveOriginAction = new QAction(tr("&Save Origin to"), this);
	selSaveOriginAction->setStatusTip(tr("Open Save (origin) Directory"));
	connect(selSaveOriginAction, SIGNAL(triggered()),
		this, SLOT(OpenSaveOriginDir()));

	selSaveMaskAction = new QAction(tr("Save Mask to"), this);
	selSaveMaskAction->setStatusTip(tr("Open Save (mask) Directory"));
	connect(selSaveMaskAction, SIGNAL(triggered()),
		this, SLOT(OpenSaveMaskDir()));
	
	drawContourAction = new QAction(tr("&Draw Contour"), this);
	drawContourAction->setStatusTip(tr("Draw Contour on Origin Image"));
	connect(drawContourAction, SIGNAL(triggered()),
		this, SLOT(OnExtractContourClicked()));
	
	saveClippedAction = new QAction(tr("Save Clipped"), this);
	saveClippedAction->setStatusTip(tr("Save ROI (origin & mask) as JPG"));
	connect(saveClippedAction, SIGNAL(triggered()),
		this, SLOT(SaveClippedROI()));
}

void rectExtractor::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(selOriginAction);
	fileMenu->addAction(selMaskAction);
	fileMenu->addAction(selSaveOriginAction);
	fileMenu->addAction(selSaveMaskAction);

	editMenu = menuBar()->addMenu(tr("Edit"));
	editMenu->addAction(drawContourAction);
	editMenu->addAction(saveClippedAction);
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

