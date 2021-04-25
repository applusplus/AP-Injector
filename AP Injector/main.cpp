#include "apinjector.h"
#include <QApplication>
#include <QtPlugin>

Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
Q_IMPORT_PLUGIN(QICOPlugin)

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	APInjector w;
	if (argc < 2)
	{
		w.show();
	}
	return a.exec();
}
