import QtQuick 2.2
import Sailfish.Silica 1.0
import harbour.sailbabel.qmlcomponents 1.0

Page {
  id: loadDictionaryPage
  BusyIndicator {
    id: busy
    size: BusyIndicatorSize.Large
    anchors.centerIn: parent
    running: true
  }
  Label {
    anchors.centerIn: parent
    anchors.verticalCenterOffset: busy.height
    text: qsTr("%n dictionary entries found.", "", dictionary.size)
  }
  onStatusChanged: {
    if (status==PageStatus.Active) {
      dictionary.read()
    }
  }
}
