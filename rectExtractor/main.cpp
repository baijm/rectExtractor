#include "rectextractor.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	rectExtractor w;
	w.show();
	return a.exec();
}
