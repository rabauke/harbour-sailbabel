import QtQuick 2.2
import Sailfish.Silica 1.0
import harbour.sailbabel.qmlcomponents 1.0
import '../components'

Page {
  id: main_page

  SilicaListView {
    id: listView

    property int numberOfResultsAtoB: 0
    property int numberOfResultsBtoA: 0

    anchors.fill: parent

    VerticalScrollDecorator {
      flickable: listView
    }

    PullDownMenu {
      MenuItem {
        text: qsTr('About SailBabel')
        onClicked: pageStack.push(Qt.resolvedUrl('About.qml'))
      }
      MenuItem {
        text: qsTr('Settings')
        onClicked: pageStack.push(Qt.resolvedUrl('Settings.qml'))
      }
      MenuItem {
        text: qsTr('Change dictionary')
        onClicked: {
          pageStack.push(Qt.resolvedUrl('ChooseDictionary.qml'))
          resultsListModel.clear()
          queryFieldText = ''
        }
      }
    }

    Component {
      id: hearderComponent
      FocusScope {
        x: hearderComponentItem.x
        y: hearderComponentItem.y
        width: hearderComponentItem.width
        height: hearderComponentItem.height
        Item {
          id: hearderComponentItem
          anchors.horizontalCenter: main_page.Center
          height: pageHeader.height + queryField.height
          width: main_page.width
          PageHeader {
            id: pageHeader
            title: qsTr('Dictionary')
          }
          QueryField {
            id: queryField
            anchors.top: pageHeader.bottom
            width: parent.width
            text: queryFieldText
            focus: true
            placeholderText: qsTr('Word or phrase')
            inputMethodHints: Qt.ImhNoAutoUppercase
            EnterKey.iconSource: 'image://theme/icon-m-enter-next'
            EnterKey.enabled: text.length > 0
            EnterKey.onClicked: {
              queryFieldText = text.replace(/\s\s*/g,
                                            ' ').replace(/^\s*/g,
                                                         '').replace(/\s*$/g,
                                                                     '')
              if (searchHistoryListModel.count === 0
                  || searchHistoryListModel.get(0).query !== text)
                searchHistoryListModel.insert(0, {
                                                'query': text
                                              })
              resultsListModel.clear()
              var transAtoB = appModel.dictionary.translateAtoB(text)
              var transBtoA = appModel.dictionary.translateBtoA(text)
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
              listView.numberOfResultsAtoB = transAtoB.length
              listView.numberOfResultsBtoA = transBtoA.length
            }
          }
          Text {
            id: no_results
            anchors.top: queryField.bottom
            x: Theme.horizontalPageMargin
            width: parent.width - 2 * x
            text: qsTr('No match in dictionary.')
            font.italic: true
            font.pointSize: Theme.fontSizeSmall
            color: Theme.highlightColor
            visible: resultsListModel.count === 0 && queryFieldText !== ''
          }
        }
      }
    }

    header: hearderComponent

    model: resultsListModel

    section.property: 'section'
    section.delegate: Item {
      x: Theme.horizontalPageMargin
      width: parent.width - 2 * x
      height: 5 * bar.width
      Rectangle {
        rotation: 90
        id: bar
        height: parent.width
        width: section === 'BtoA' && listView.numberOfResultsAtoB > 0
               && listView.numberOfResultsBtoA > 0 ? Theme.paddingSmall : 0
        radius: width / 3
        anchors.centerIn: parent
        opacity: 0.75
        gradient: Gradient {
          GradientStop {
            position: 0.667
            color: Theme.secondaryColor
          }
          GradientStop {
            position: 0.0
            color: 'transparent'
          }
        }
      }
    }

    delegate: ListItem {
      width: main_page.width
      contentHeight: textLang1.height + textLang2.height + Theme.paddingLarge + Theme.paddingSmall
      menu: contextMenu
      showMenuOnPressAndHold: !(lang1 === '' && lang2 === '')
      Item {
        id: item
        height: textLang1.height + textLang2.height + Theme.paddingLarge
        width: parent.width
        Label {
          id: textLang1
          text: lang1
          x: Theme.horizontalPageMargin
          width: parent.width - 2 * x
          anchors.top: item.top
          anchors.topMargin: 0.35 * Theme.paddingLarge
          truncationMode: TruncationMode.Fade
        }
        Label {
          id: textLang2
          text: lang2
          x: Theme.horizontalPageMargin
          width: parent.width - 2 * x
          anchors.top: textLang1.bottom
          anchors.topMargin: Theme.paddingSmall
          truncationMode: TruncationMode.Fade
          color: Theme.highlightColor
        }
      }
      Component {
        id: contextMenu
        ContextMenu {
          Item {
            width: parent.width
            height: fullTextLang1.height
            TextArea {
              id: fullTextLang1
              text: lang1
              width: Screen.sizeCategory
                     >= Screen.Large ? parent.width - Theme.iconSizeMedium
                                       - Theme.paddingMedium : parent.width
                                       - Theme.iconSizeSmall - Theme.paddingMedium
              readOnly: true
              wrapMode: TextEdit.Wrap
              labelVisible: false
              onClicked: {
                Clipboard.text = lang1
                selectAll()
                fullTextLang2.deselect()
                clipboard1.visible = true
                clipboard2.visible = false
              }
            }
            Image {
              id: clipboard1
              anchors {
                verticalCenter: fullTextLang1.top
                verticalCenterOffset: fullTextLang1.textVerticalCenterOffset
                left: fullTextLang1.right
                leftMargin: Theme.paddingMedium - Theme.horizontalPageMargin
              }
              source: Screen.sizeCategory
                      >= Screen.Large ? 'image://theme/icon-m-clipboard?'
                                        + Theme.highlightColor : 'image://theme/icon-s-clipboard?'
                                        + Theme.highlightColor
              visible: false
            }
          }
          Item {
            width: parent.width
            height: fullTextLang2.height
            TextArea {
              id: fullTextLang2
              text: lang2
              width: Screen.sizeCategory
                     >= Screen.Large ? parent.width - Theme.iconSizeMedium
                                       - Theme.paddingMedium : parent.width
                                       - Theme.iconSizeSmall - Theme.paddingMedium
              readOnly: true
              wrapMode: TextEdit.Wrap
              color: Theme.highlightColor
              labelVisible: false
              onClicked: {
                Clipboard.text = lang2
                selectAll()
                fullTextLang1.deselect()
                clipboard1.visible = false
                clipboard2.visible = true
              }
            }
            Image {
              id: clipboard2
              anchors {
                verticalCenter: fullTextLang2.top
                verticalCenterOffset: fullTextLang2.textVerticalCenterOffset
                left: fullTextLang2.right
                leftMargin: Theme.paddingMedium - Theme.horizontalPageMargin
              }
              source: Screen.sizeCategory
                      >= Screen.Large ? 'image://theme/icon-m-clipboard?'
                                        + Theme.highlightColor : 'image://theme/icon-s-clipboard?'
                                        + Theme.highlightColor
              visible: false
            }
          }
        }
      }
    }
  }

  onStatusChanged: {
    if (status === PageStatus.Active) {
      listView.headerItem.forceActiveFocus()
    }
  }
}
