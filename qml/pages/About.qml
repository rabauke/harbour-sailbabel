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
        title: 'SailBabel v' + appModel.version
      }
      Text {
        x: Theme.horizontalPageMargin
        width: column.width - 2 * x
        color: Theme.primaryColor
        wrapMode: TextEdit.Wrap
        font.pixelSize: Theme.fontSizeMedium
        horizontalAlignment: TextEdit.AlignJustify
        text: qsTr('SailBabel is an offline dictionary for Sailfish OS.  Note that this program does not include translations or wordlists.  Just copy your own set of translations to a text file in the sub-folder Dictionaries within the Documents directory.  This file is expected to contain two tab-separated columns each containing a word or phrase in one language and its translation into another language, respectively.  Dictionary text files may be obtained from <a href="http://www1.dict.cc/translation_file_request.php?l=e">http://www.dict.cc</a> for personal use.  Enter a word or a phrase into the text field, matching results will be shown below.  Long dictionary entries may be shown truncated.  Press a dictionary entry to show its full content, tap a full dictionary entry to copy it to the clipboard.')
        textFormat: Text.StyledText
        linkColor: Theme.highlightColor
        onLinkActivated: {
          Qt.openUrlExternally(link)
        }
      }
      Label {
        text: '<br>© Heiko Bauke, 2016–2024<br><br>Fork me on github!<br><a href=\'https://github.com/rabauke/harbour-sailbabel\'>https://github.com/rabauke/harbour-sailbabel</a>'
        textFormat: Text.StyledText
        width: column.width
        color: Theme.primaryColor
        linkColor: Theme.highlightColor
        wrapMode: TextEdit.Wrap
        font.pixelSize: Theme.fontSizeSmall
        horizontalAlignment: TextEdit.AlignHCenter
        onLinkActivated: {
          Qt.openUrlExternally(link)
        }
      }
    }
  }
}
