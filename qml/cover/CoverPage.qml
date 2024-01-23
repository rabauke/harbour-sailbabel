import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {

  function basename(str) {
    return str.slice(str.lastIndexOf('/') + 1)
  }

  Image {
    source: '/usr/share/harbour-sailbabel/images/cover_background.png'
    x: 0
    y: parent.height - parent.width - Theme.paddingLarge
    z: -1
    opacity: 0.125
    width: parent.width
    height: parent.width
  }

  Item {
    anchors {
      top: parent.top
      bottom: parent.bottom
      left: parent.left
      right: parent.right
      topMargin: Theme.paddingLarge
      bottomMargin: 1.25 * Theme.paddingLarge
      leftMargin: Theme.paddingLarge
      rightMargin: Theme.paddingLarge
    }
    Text {
      id: title
      text: 'SailBabel'
      color: Theme.highlightColor
      font.pixelSize: Theme.fontSizeMedium
      wrapMode: Text.Wrap
      width: parent.width
      horizontalAlignment: Text.AlignHCenter
    }
    Text {
      anchors.bottom: parent.bottom
      text: basename(appModel.currentDictionary)
      color: Theme.highlightColor
      font.pixelSize: Theme.fontSizeMedium
      wrapMode: Text.Wrap
      width: parent.width
      horizontalAlignment: Text.AlignHCenter
    }
  }
}
