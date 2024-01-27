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
              var transAtoB = appModel.dictionary.translateAtoB(queryFieldText)
              var transBtoA = appModel.dictionary.translateBtoA(queryFieldText)
              var i
              for (i in transAtoB)
                resultsListModel.append({
                                          'section': 'AtoB',
                                          'lang1': transAtoB[i][0],
                                          'lang2': transAtoB[i][1]
                                        })
              for (i in transBtoA)
                resultsListModel.append({
                                          'section': 'BtoA',
                                          'lang1': transBtoA[i][0],
                                          'lang2': transBtoA[i][1]
                                        })
              numberOfResultsAtoB = transAtoB.length
              numberOfResultsBtoA = transBtoA.length
              queryFieldText = searchHistoryListModel.get(model.index).query
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
