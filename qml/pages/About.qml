import QtQuick 2.2
import Sailfish.Silica 1.0

Page {
  id: about_page

  SilicaFlickable {
    anchors.fill: parent
    contentHeight: column.height

    Column {
      id: column

      width: parent.width
      spacing: Theme.paddingMedium
      PageHeader {
        title: "SailBabel"
      }
      TextArea {
        width: column.width
        color: Theme.primaryColor
        readOnly: true
        wrapMode: TextEdit.Wrap
        font.pixelSize: Theme.fontSizeMedium
        horizontalAlignment: TextEdit.AlignJustify
        text: qsTr("_discription_")
      }
      Label {
        text: "<br>© Heiko Bauke, 2016<br><br>Fork me on github!<br><a href=\"https://github.com/rabauke/harbour-sailbabel\">https://github.com/rabauke/harbour-sailbabel</a>"
        textFormat: Text.StyledText
        width: column.width
        color: Theme.primaryColor
        linkColor: Theme.highlightColor
        wrapMode: TextEdit.Wrap
        font.pixelSize: Theme.fontSizeSmall
        horizontalAlignment: TextEdit.AlignHCenter
        onLinkActivated: { Qt.openUrlExternally(link) }
      }

    }

  }

}
