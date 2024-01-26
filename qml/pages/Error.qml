import QtQuick 2.2
import Sailfish.Silica 1.0


Page {
  id: error_page

  SilicaFlickable {
    anchors.fill: parent
    contentHeight: column.height

    Column {
      id: column

      width: parent.width
      spacing: Theme.paddingMedium
      PageHeader {
        title: qsTr('Error')
      }
      TextField {
        text: qsTr('Unable to open dictionary.')
        readOnly: true
      }
      Button {
        text: qsTr('Ok')
        anchors.horizontalCenter: column.horizontalCenter
        onClicked: {
          pageStack.replace(Qt.resolvedUrl('MainPage.qml'))
          pageStack.pushAttached(Qt.resolvedUrl('History.qml'))
        }
      }

    }

  }

}
