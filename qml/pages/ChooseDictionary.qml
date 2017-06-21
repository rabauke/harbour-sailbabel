import QtQuick 2.2
import Sailfish.Silica 1.0
import harbour.sailbabel.qmlcomponents 1.0


Page {
  id: chooseDictionary_page

  FolderListModel {
    id: folderModel
    folder: StandardPaths.documents+"/Dictionaries"
  }

  SilicaListView {
    anchors.fill: parent
    id: listView
    VerticalScrollDecorator { flickable: listView }

    PullDownMenu {
      MenuItem {
        text: qsTr("About SailBabel")
        onClicked: pageStack.push(Qt.resolvedUrl("About.qml"))
      }
    }

    header: Item {
      anchors.horizontalCenter: chooseDictionary_page.Center
      height: pageHeader.height
      width: chooseDictionary_page.width
      PageHeader {
        id: pageHeader
        title: qsTr("Choose your dictionary")
      }
    }

    model: folderModel

    delegate: ListItem {
      width: parent.width
      contentWidth: parent.width
      contentHeight: fileName!="." ? result_text.height+Theme.paddingLarge : 0
      visible: fileName!="."
      Label {
        id: result_text
        y: 0.5*Theme.paddingLarge
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
        text: fileName!=".." ? fileName : qsTr("Parent directory")
        font.italic: fileName==".."
        color: fileIsDir ? Theme.highlightColor : Theme.primaryColor
      }

      onClicked: {
        if (fileIsDir) {
          folderModel.folder=filePath
        } else {
          dictionaryFile=filePath
          pageStack.replace("LoadDictionary.qml")
        }
      }

    }

  }

}
