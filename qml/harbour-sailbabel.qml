import QtQuick 2.2
import Sailfish.Silica 1.0
import harbour.sailbabel.qmlcomponents 1.0
import 'pages'


ApplicationWindow {
  id: appWindow

  Dictionary {
    id: dictionary
    onReadingFinished: {
      pageStack.replace('pages/MainPage.qml')
      pageStack.pushAttached('pages/History.qml')
    }
    onReadingError: pageStack.replace('pages/Error.qml')
  }

  ListModel {
    id: searchHistoryListModel
  }

  ListModel {
    id: resultsListModel
  }

  property string dictionaryFile

  property string queryFieldText:  ''

  initialPage: Component { ChooseDictionary { } }
  cover: Qt.resolvedUrl('cover/CoverPage.qml')
  allowedOrientations: Orientation.All
  _defaultPageOrientations: Orientation.All

}
