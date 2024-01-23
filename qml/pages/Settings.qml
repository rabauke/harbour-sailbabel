import QtQuick 2.2
import Sailfish.Silica 1.0

Page {
  id: page

  Column {
    anchors.fill: parent
    PageHeader {
      id: pageHeader
      title: qsTr('Settings')
    }
    TextSwitch {
      id: autoLoadDictionarySwitch
      checked: appModel.autoLoadDictionary
      text: qsTr('auto-load dictionary at program start-up')
      onCheckedChanged: appModel.autoLoadDictionary = checked
    }
  }
}
