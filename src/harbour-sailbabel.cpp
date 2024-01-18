#include <QtQuick>
#include <QString>
#include <QObject>
#include <QtQml>
#include <sailfishapp.h>
#include "Dictionary.hpp"
#include "FolderListModel.hpp"


int main(int argc, char *argv[]) {
  QScopedPointer<QGuiApplication> app{SailfishApp::application(argc, argv)};
  app->setApplicationName("harbour-sailbabel");
  app->setOrganizationDomain("rabauke");

  qmlRegisterType<Dictionary>("harbour.sailbabel.qmlcomponents", 1, 0, "Dictionary");
  qmlRegisterType<FolderListModel>("harbour.sailbabel.qmlcomponents", 1, 0, "FolderListModel");

  QScopedPointer<QQuickView> view{SailfishApp::createView()};
  view->setSource(SailfishApp::pathTo("qml/harbour-sailbabel.qml"));
  view->show();
  return app->exec();
}
