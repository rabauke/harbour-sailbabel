/*
  Copyright (C) 2016 Heiko Bauke
  Contact: Heiko Bauke <heiko.bauke@mail.de>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

import QtQuick 2.2
import Sailfish.Silica 1.0
import harbour.sailbabel.qmlcomponents 1.0


Page {
  id: main_page

  SilicaListView {
    id: listView

    anchors.fill: parent

    VerticalScrollDecorator { flickable: listView }

    PullDownMenu {
      MenuItem {
        text: qsTr("About SailBabel")
        onClicked: pageStack.push(Qt.resolvedUrl("About.qml"))
      }
      MenuItem {
        text: qsTr("Change Dictionary")
        onClicked: {
            pageStack.push(Qt.resolvedUrl("ChooseDictionary.qml"))
            resultsListModel.clear()
            queryFieldText=""
        }
      }
    }

    Component {
      id: hearderComponent
      Item {
        anchors.horizontalCenter: main_page.Center
        height: pageHeader.height+queryField.height
        width: main_page.width
        PageHeader {
          id: pageHeader
          title: qsTr("Dictionary "+dictionary.langFrom+" -> "+dictionary.langTo)
        }
        TextField {
          id: queryField
          anchors.top: pageHeader.bottom
          width: parent.width
          text: queryFieldText
          focus: true
          placeholderText: qsTr("Word or phrase")
          inputMethodHints: Qt.ImhNoAutoUppercase
          EnterKey.enabled: text.length>0
          EnterKey.onClicked: {
            queryFieldText=text.replace(/\s\s*/g," ").replace(/^\s*/g,"").replace(/\s*$/g,"")
            if (searchHistoryListModel.count===0 ||
                searchHistoryListModel.get(0).query!=text)
              searchHistoryListModel.insert(0, { query: text })
            resultsListModel.clear()
            var trans=dictionary.translateAtoB(text)
            for (var i in trans)
              resultsListModel.append({ lang1: trans[i][0], lang2: trans[i][1] })
            if (trans.length>0)
              resultsListModel.append({ lang1: "", lang2: ""})
            var trans=dictionary.translateBtoA(text)
            for (var i in trans)
              resultsListModel.append({ lang1: trans[i][0], lang2: trans[i][1] })
          }
        }
        Text {
          id: no_results
          anchors.top: queryField.bottom
          x: Theme.horizontalPageMargin
          width: parent.width-2*x
          text: qsTr("No match in dictionary.")
          font.italic: true
          font.pointSize: Theme.fontSizeSmall
          color: Theme.highlightColor
          visible: resultsListModel.count==0 && queryFieldText!=""
        }
      }
    }

    header: hearderComponent

    model: resultsListModel

    delegate: ListItem {
      width: main_page.width
      contentHeight: textLang1.height+textLang2.height+Theme.paddingLarge+Theme.paddingSmall
      menu: contextMenu
      showMenuOnPressAndHold: !(lang1=="" && lang2=="")
      Item {
        id: item
        height: textLang1.height+textLang2.height+Theme.paddingLarge
        width: parent.width
        Label {
          id: textLang1
          text: lang1
          x: Theme.horizontalPageMargin
          width: parent.width-2*x
          anchors.top: item.top
          anchors.topMargin: 0.35*Theme.paddingLarge
          truncationMode: TruncationMode.Fade
        }
        Label {
          id: textLang2
          text: lang2
          x: Theme.horizontalPageMargin
          width: parent.width-2*x
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
              width: Screen.sizeCategory>=Screen.Large ? parent.width-Theme.iconSizeMedium-Theme.paddingMedium :  parent.width-Theme.iconSizeSmall-Theme.paddingMedium
              readOnly: true
              wrapMode: TextEdit.Wrap
              labelVisible: false
              onClicked: {
                Clipboard.text=lang1
                selectAll()
                fullTextLang2.deselect()
                clipboard1.visible=true
                clipboard2.visible=false
              }
            }
            Image {
              id: clipboard1
              anchors {
                verticalCenter: fullTextLang1.top
                verticalCenterOffset: fullTextLang1.textVerticalCenterOffset
                left: fullTextLang1.right
                leftMargin: Theme.paddingMedium-Theme.horizontalPageMargin
              }
              source: Screen.sizeCategory>=Screen.Large ? "image://theme/icon-m-clipboard?"+Theme.highlightColor : "image://theme/icon-s-clipboard?"+Theme.highlightColor
              visible: false
            }
          }
          Item {
            width: parent.width
            height: fullTextLang2.height
            TextArea {
              id: fullTextLang2
              text: lang2
              width: Screen.sizeCategory>=Screen.Large ? parent.width-Theme.iconSizeMedium-Theme.paddingMedium :  parent.width-Theme.iconSizeSmall-Theme.paddingMedium
              readOnly: true
              wrapMode: TextEdit.Wrap
              color: Theme.highlightColor
              labelVisible: false
              onClicked: {
                Clipboard.text=lang2
                selectAll()
                fullTextLang1.deselect()
                clipboard1.visible=false
                clipboard2.visible=true
              }
            }
            Image {
              id: clipboard2
              anchors {
                verticalCenter: fullTextLang2.top
                verticalCenterOffset: fullTextLang2.textVerticalCenterOffset
                left: fullTextLang2.right
                leftMargin: Theme.paddingMedium-Theme.horizontalPageMargin
              }
              source: Screen.sizeCategory>=Screen.Large ? "image://theme/icon-m-clipboard?"+Theme.highlightColor : "image://theme/icon-s-clipboard?"+Theme.highlightColor
              visible: false
            }
          }
        }
      }
    }

  }

}
