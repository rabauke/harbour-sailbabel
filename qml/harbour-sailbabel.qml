import QtQuick 2.2
import Sailfish.Silica 1.0
import harbour.sailbabel.qmlcomponents 1.0
import 'pages'


ApplicationWindow {
  id: appWindow

  AppModel {
    id: appModel
  }

  Dictionary {
    id: dictionary
    onReadingFinished: {
      pageStack.replace('pages/MainPage.qml')
      pageStack.pushAttached('pages/History.qml')
    }
    onReadingError: {
      pageStack.replace('pages/Error.qml')
      appModel.currentDictionary = ''
    }
  }

  ListModel {
    id: searchHistoryListModel
  }

  ListModel {
    id: resultsListModel
  }

  property string queryFieldText: ''

  initialPage: appModel.autoLoadDictionary && appModel.currentDictionary !== '' ? Qt.resolvedUrl('pages/LoadDictionary.qml') : Qt.resolvedUrl('pages/ChooseDictionary.qml')
  cover: Qt.resolvedUrl('cover/CoverPage.qml')
  allowedOrientations: Orientation.All
  _defaultPageOrientations: Orientation.All

}
