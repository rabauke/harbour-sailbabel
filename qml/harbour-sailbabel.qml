import QtQuick 2.2
import Sailfish.Silica 1.0
import harbour.sailbabel.qmlcomponents 1.0
import 'pages'


ApplicationWindow {
  id: appWindow

  AppModel {
    id: appModel

    onReadingDictionaryFinished: {
      pageStack.replace(Qt.resolvedUrl('pages/MainPage.qml'))
      pageStack.pushAttached(Qt.resolvedUrl('pages/History.qml'))
    }
    onReadingDictionaryFailed: {
      pageStack.replace(Qt.resolvedUrl('pages/Error.qml'))
      appModel.currentDictionary = ''
    }
  }

  property string queryFieldText: ''

  ListModel {
    id: resultsListModel
  }

  property int numberOfResultsAtoB: 0
  property int numberOfResultsBtoA: 0

  ListModel {
    id: searchHistoryListModel
  }

  initialPage: appModel.autoLoadDictionary && appModel.currentDictionary !== '' ? Qt.resolvedUrl('pages/LoadDictionary.qml') : Qt.resolvedUrl('pages/ChooseDictionary.qml')
  cover: Qt.resolvedUrl('cover/CoverPage.qml')
  allowedOrientations: Orientation.All
  _defaultPageOrientations: Orientation.All

}
