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

	//auto mark tools
	extractContourButton = new QPushButton(tr("Extract Contour"));
	connect(extractContourButton, SIGNAL(clicked()),
		this, SLOT(OnExtractContourClicked()));
	saveAutoMarkButton = new QPushButton(tr("Save Auto Mark"));
	connect(saveAutoMarkButton, SIGNAL(clicked()),
		this, SLOT(OnSaveAutoMarkClicked()));

	//num of points
	countPoints = new QLabel(this);
	countPoints->setText(tr("total xxxx points"));
	//display image
	imgLabel = new QLabel(this);
	imgLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	//point list
	pointList = new QListWidget(this);
	pointList->setFixedWidth(200);

	//show last / next image
	lastImgButton = new QPushButton(tr("< Last"));
	connect(lastImgButton, SIGNAL(clicked()),
		this, SLOT(ShowLastImage()));
	nextImgButton = new QPushButton(tr("Next >"));
	connect(nextImgButton, SIGNAL(clicked()),
		this, SLOT(ShowNextImage()));

	//manual mark tools
	delPointButton = new QPushButton(tr("Del Last"));
	connect(delPointButton, SIGNAL(clicked()),
		this, SLOT(OnDelPointClicked()));
	skipPointButton = new QPushButton(tr("Skip"));
	connect(skipPointButton, SIGNAL(clicked()),
		this, SLOT(OnSkipPointClicked()));
	saveTxtButton = new QPushButton(tr("Save txt"));
	connect(saveTxtButton, SIGNAL(clicked()),
		this, SLOT(OnSaveTxtClicked()));

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
	//align : points count & point list
	QVBoxLayout *pointLayout = new QVBoxLayout;
	pointLayout->addWidget(countPoints);
	pointLayout->addWidget(pointList);
	pointLayout->addWidget(delPointButton);
	pointLayout->addWidget(skipPointButton);
	pointLayout->addWidget(saveTxtButton);
	//align : last, image, next, point
	QHBoxLayout *imgLayout = new QHBoxLayout;
	imgLayout->addWidget(lastImgButton);
	imgLayout->addWidget(imgLabel);
	imgLayout->addWidget(nextImgButton);
	imgLayout->addLayout(pointLayout);
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

		if(modeButtonGroup->checkedId() == 2)//manual marking
		{
			traverseDir(originPath, originPath, "*.jpg");
			numPairs = pairNames.size();

			//load rect TL pos
			loadRectTL(originPath);
		}

		statusLabel->setText("Finished Setting Origin Directory. ( "+ QString::number(numPairs,10) +" pictures )");
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
		maskDirHint->show();
		maskDirLabel->show();
		saveMaskDirHint->show();
		saveMaskDirLabel->show();

		extractContourButton->hide();
		saveAutoMarkButton->hide();

		lastImgButton->setEnabled(false);
		nextImgButton->setEnabled(false);
		imgLabel->setEnabled(false);
		countPoints->setEnabled(false);
		pointList->setEnabled(false);
		delPointButton->setEnabled(false);
		skipPointButton->setEnabled(false);
		saveTxtButton->setEnabled(false);

		selMaskAction->setEnabled(true);
		selSaveMaskAction->setEnabled(true);
		saveClippedAllAction->setEnabled(true);

		setMouseTracking(false);

		break;
	case 1:	//auto mark
		maskDirHint->show();
		maskDirLabel->show();
		saveMaskDirHint->hide();
		saveMaskDirLabel->hide();

		extractContourButton->show();
		saveAutoMarkButton->show();

		lastImgButton->setEnabled(true);
		nextImgButton->setEnabled(true);
		imgLabel->setEnabled(true);
		countPoints->setEnabled(true);
		pointList->setEnabled(true);
		delPointButton->setEnabled(false);
		skipPointButton->setEnabled(false);
		saveTxtButton->setEnabled(false);

		selMaskAction->setEnabled(true);
		selSaveMaskAction->setEnabled(false);
		saveClippedAllAction->setEnabled(false);//

		setMouseTracking(false);

		break;
	case 2:	//manual mark
		maskDirHint->hide();
		maskDirLabel->hide();
		saveMaskDirHint->hide();
		saveMaskDirLabel->hide();
		
		extractContourButton->hide();
		saveAutoMarkButton->hide();

		lastImgButton->setEnabled(true);
		nextImgButton->setEnabled(true);
		imgLabel->setEnabled(true);
		countPoints->setEnabled(true);
		pointList->setEnabled(true);
		delPointButton->setEnabled(true);
		skipPointButton->setEnabled(true);
		saveTxtButton->setEnabled(true);

		selMaskAction->setEnabled(false);
		selSaveMaskAction->setEnabled(false);
		saveClippedAllAction->setEnabled(false);

		setMouseTracking(true);

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

void rectExtractor::OnDelPointClicked()
{
	int lastIdx = pointList->count()-1;
	if(lastIdx < 0)
		return;
	pointList->takeItem(lastIdx);
	//pointList->AboveItem
}

void rectExtractor::OnSkipPointClicked()
{
	pointList->addItem(QString::number(pointList->count()+1)
		+"\t"
		+"x:NaN"
		+"\t"
		+"y:NaN"
		);
}

void rectExtractor::OnSaveTxtClicked()
{
	if(saveClippedOriginPath.isEmpty())
	{
		QMessageBox::information(NULL, 
			tr("error msg"), 
			tr("save path not set !"));
		return;
	}
	
	for(int i=0; i<pointList->count(); i++)
	{
		QListWidgetItem* ptrItm = pointList->item(i);
		QString itemText = ptrItm->text();
		/*TODO : save to TXT*/
	}
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
	imgLabel->move(0,0);
	imgLabel->setPixmap(QPixmap::fromImage(*QRgbImg));
	imgLabel->resize(imgLabel->pixmap()->size());
	update();//---------------------

	pointList->clear();
	countPoints->setText(tr("total xxxx points"));
	statusLabel->setText(roiName);

	if(modeButtonGroup->checkedId()==2
		&& nameTL.size()!=0)
	{
		map<string, string>::const_iterator mapIte;
		mapIte = nameTL.find(pairNames[showIdx].toStdString());
		if(mapIte == nameTL.end())
		{
			TLpos.x = TLpos.y = 0;
			return;
		}
		int pX,pY;
		sscanf(mapIte->second.c_str(),"%d %d", &pX,&pY);
		TLpos.x = pX;
		TLpos.y = pY;
	}
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
	imgLabel->move(0,0);
	imgLabel->setPixmap(QPixmap::fromImage(*QRgbImg));
	imgLabel->resize(imgLabel->pixmap()->size());
	update();//-------------------------

	pointList->clear();
	countPoints->setText(tr("total xxxx points"));
	statusLabel->setText(roiName);

	if(modeButtonGroup->checkedId()==2
		&& nameTL.size()!=0)
	{
		map<string, string>::const_iterator mapIte;
		mapIte = nameTL.find(pairNames[showIdx].toStdString());
		if(mapIte == nameTL.end())
		{
			TLpos.x = TLpos.y = 0;
			return;
		}
		int pX,pY;
		sscanf(mapIte->second.c_str(),"%d %d", &pX,&pY);
		TLpos.x = pX;
		TLpos.y = pY;
	}
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
	
	saveClippedAllAction = new QAction(tr("Save Clipped"), this);
	saveClippedAllAction->setStatusTip(tr("Save ROI (origin & mask) as JPG"));
	connect(saveClippedAllAction, SIGNAL(triggered()),
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
	editMenu->addAction(saveClippedAllAction);
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

void rectExtractor::loadRectTL(const QString path)
{
	nameTL.clear();

	QString posFile = path+'/'+"rectpos.txt";
	QFile file(posFile);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox::information(NULL, 
			tr("error msg"), 
			tr("cannot open log file!"));
		return;
	}

	QTextStream txtin(&file);
	while(!txtin.atEnd())
	{
		QString line = txtin.readLine();
		QStringList fields = line.split(' ');
		if(fields.size()<5)
			continue;
		string name = fields.takeFirst().toStdString();
		string tlXY = fields.takeFirst().toStdString();
		tlXY = tlXY+' '+fields.takeFirst().toStdString();
		nameTL.insert(pair<string,string>(name,tlXY));
	}
}

//handle mouse & key events
void rectExtractor::mouseMoveEvent(QMouseEvent *event)
{
	if(modeButtonGroup->checkedId() == 2)//manual marking
	{	
		QPoint mousePoint = event->pos();
		QPoint imgTL = imgLabel->geometry().topLeft();
		int menuHeight = menuBar()->height();

		statusLabel->setText("( "
			+QString::number(mousePoint.x()-imgTL.x())
			+" , "
			+QString::number(mousePoint.y()-menuHeight-imgTL.y())
			+" )");
	}
}

void rectExtractor::mousePressEvent(QMouseEvent *event)
{
	if (modeButtonGroup->checkedId() == 2)//manual marking
	{
		QPoint mousePoint = event->pos();
		QPoint imgTL = imgLabel->geometry().topLeft();
		int menuHeight = menuBar()->height();

		int imgX = mousePoint.x()-imgTL.x();
		int imgY = mousePoint.y()-menuHeight-imgTL.y();

		if ( imgX<0
			| imgX>imgLabel->pixmap()->width()
			| imgY<0
			| imgY>imgLabel->pixmap()->height())
		{
			return;
		}

		pointList->addItem(QString::number(pointList->count()+1)
			+"\t"
			+"x:"+QString::number(imgX+TLpos.x)
			+"\t"
			+"y:"+QString::number(imgY+TLpos.y)
			);
	}
}

void rectExtractor::keyPressEvent(QKeyEvent *event)
{
	if(modeButtonGroup->checkedId() == 2)//manual mark
	{

	}
}

void rectExtractor::paintEvent(QPaintEvent *event)
{
	if(modeButtonGroup->checkedId() > 0)
	{
		QPainter painter(this);
		QPen pen;
		pen.setColor(Qt::darkGreen);
		pen.setWidth(3);
		painter.setPen(pen);

		int menuHeight = menuBar()->height();

		painter.drawLine(imgLabel->geometry().topLeft().x(),
			imgLabel->geometry().topLeft().y()+menuHeight,
			imgLabel->geometry().bottomLeft().x(),
			imgLabel->geometry().bottomLeft().y()+menuHeight
			);
		painter.drawLine(imgLabel->geometry().topRight().x(),
			imgLabel->geometry().topRight().y()+menuHeight,
			imgLabel->geometry().bottomRight().x(),
			imgLabel->geometry().bottomRight().y()+menuHeight
			);
		painter.drawLine(imgLabel->geometry().topLeft().x(),
			imgLabel->geometry().topLeft().y()+menuHeight,
			imgLabel->geometry().topRight().x(),
			imgLabel->geometry().topRight().y()+menuHeight
			);
		painter.drawLine(imgLabel->geometry().bottomLeft().x(),
			imgLabel->geometry().bottomLeft().y()+menuHeight,
			imgLabel->geometry().bottomRight().x(),
			imgLabel->geometry().bottomRight().y()+menuHeight
			);

		painter.end();
	}
}