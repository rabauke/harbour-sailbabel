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
    text: "found "+dictionary.size+" phrases"
  }
  onStatusChanged: {
    if (status==PageStatus.Active) {
      dictionary.read()
    }
  }
}
