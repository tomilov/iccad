#include "mainwindow.h"
#include "settings.h"

#include <QApplication>
#include <QSettings>
#include <QDebug>

int main(int argc, char * argv[])
{
    QApplication application(argc, argv);
    QCoreApplication::setOrganizationDomain(qApp->tr("Science"));
    QCoreApplication::setOrganizationName(qApp->tr("Homeworks"));
    QCoreApplication::setApplicationName(qApp->tr("insituccad"));
    QCoreApplication::setApplicationVersion(qApp->tr("0.1"));
    QSettings settings;
    Settings::initSettings(settings, QCoreApplication::applicationName());
    InsitucCADMainWindow window;
    window.show();
    return application.exec();
}
