import QtQuick 2.2
import Sailfish.Silica 1.0

Page {
  id: page

  SilicaListView {
    anchors.fill: parent
    id: listView

    VerticalScrollDecorator {
      flickable: listView
    }

    PullDownMenu {
      RemorsePopup {
        id: remorse_variables
      }
      MenuItem {
        text: qsTr('Clear search history')
        onClicked: remorse_variables.execute(qsTr('Clearing search history'),
                                             function () {
                                               searchHistoryListModel.clear()
                                             })
      }
    }

    header: Item {
      anchors.horizontalCenter: page.Center
      anchors.top: parent.Top
      height: pageHeader.height
      width: page.width
      PageHeader {
        id: pageHeader
        title: qsTr('Search history')
      }
    }

    model: searchHistoryListModel

    delegate: ListItem {
      width: parent.width
      contentWidth: parent.width
      contentHeight: query_text.height + Theme.paddingLarge
      menu: contextMenu
      Text {
        id: query_text
        x: Theme.horizontalPageMargin
        y: 0.5 * Theme.paddingLarge
        width: parent.width - 2 * Theme.horizontalPageMargin
        color: Theme.primaryColor
        wrapMode: TextEdit.Wrap
        font.pixelSize: Theme.fontSizeMedium
        horizontalAlignment: TextEdit.AlignLeft
        text: query
      }
      Component {
        id: contextMenu
        ContextMenu {
          MenuItem {
            text: qsTr('Search again')
            onClicked: {
              queryFieldText = searchHistoryListModel.get(model.index).query
              resultsListModel.clear()
              var trans = dictionary.translateAtoB(queryFieldText)
              for (var i in trans)
                resultsListModel.append({
                                          'lang1': trans[i][0],
                                          'lang2': trans[i][1]
                                        })
              if (trans.length > 0)
                resultsListModel.append({
                                          'lang1': '',
                                          'lang2': ''
                                        })
              var trans = dictionary.translateBtoA(queryFieldText)
              for (var i in trans)
                resultsListModel.append({
                                          'lang1': trans[i][0],
                                          'lang2': trans[i][1]
                                        })
              pageStack.navigateBack()
            }
          }
          MenuItem {
            text: qsTr('Remove query')
            onClicked: searchHistoryListModel.remove(model.index)
          }
        }
      }
    }
  }
}
