import QtQuick 2.2
import Sailfish.Silica 1.0
import harbour.sailbabel.qmlcomponents 1.0

Page {
  id: loadDictionaryPage

  function basename(str) {
    return str.slice(str.lastIndexOf('/') + 1)
  }

  BusyIndicator {
    id: busy
    size: BusyIndicatorSize.Large
    anchors.centerIn: parent
    running: true
  }
  Label {
    id: label1
    anchors.centerIn: parent
    anchors.verticalCenterOffset: 1.25 * busy.height
    text: qsTr('Reading %1.').arg(basename(appModel.currentDictionary))
    color: Theme.highlightColor
  }
  Label {
    id: label2
    anchors.horizontalCenter: label1.horizontalCenter
    anchors.top: label1.bottom
    anchors.topMargin: Theme.paddingMedium
    text: qsTr('%n dictionary entries found.', '', dictionary.size)
    color: Theme.highlightColor
  }
  onStatusChanged: {
    if (status === PageStatus.Active)
      dictionary.readAsync(appModel.currentDictionary)
  }
}
