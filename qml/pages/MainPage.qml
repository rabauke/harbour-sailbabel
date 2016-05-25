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
      anchors.horizontalCenter: main_page.Center
      height: pageHeader.height+query_field.height
      width: main_page.width
      PageHeader {
        id: pageHeader
        title: qsTr("Dictionary")
      }
      TextField {
        id: query_field
        anchors.top: pageHeader.bottom
        width: parent.width
        text: ""
        focus: true
        placeholderText: qsTr("Word or phrase")
        inputMethodHints: Qt.ImhNoAutoUppercase
        EnterKey.enabled: text.length>0
        EnterKey.onClicked: {
          listModel.clear()
          var trans=dictionary.translateAtoB(text)
          for (var i in trans)
            listModel.append({ lang1: trans[i][0], lang2: trans[i][1] })
          if (trans.length>0)
            listModel.append({ lang1: "", lang2: ""})
          var trans=dictionary.translateBtoA(text)
          for (var i in trans)
            listModel.append({ lang1: trans[i][0], lang2: trans[i][1] })
          if (listModel.count==0)
            no_results.visible=true
          else
            no_results.visible=false
        }
      }
      Text {
        id: no_results
        anchors.top: query_field.bottom
        x: Theme.horizontalPageMargin
        width: parent.width-2*x
        text: qsTr("No match in dictionary.")
        font.italic: true
        color: Theme.primaryColor
        visible: false
      }
    }

    model: ListModel {
      id: listModel
    }

    delegate: ListItem {
      width: ListView.view.width
      contentHeight : Theme.itemSizeSmall*1.25
      menu: contextMenu
      Label {
        id: textLang1
        text: lang1
        x: Theme.horizontalPageMargin
        width: parent.width-2*x
        truncationMode: TruncationMode.Fade
      }
      Label {
        id: textLang2
        anchors.top: textLang1.bottom
        text: lang2
        x: Theme.horizontalPageMargin
        width: parent.width-2*x
        truncationMode: TruncationMode.Fade
        color: Theme.secondaryHighlightColor
      }
      Component {
        id: contextMenu
        ContextMenu {
          TextArea {
            id: fullTextLang1
            text: lang1
            width: parent.width
            readOnly: true
            wrapMode: TextEdit.Wrap
            onClicked: { Clipboard.text=lang1; selectAll(); fullTextLang2.deselect() }
          }
          TextArea {
            id: fullTextLang2
            text: lang2
            width: parent.width
            readOnly: true
            wrapMode: TextEdit.Wrap
            color: Theme.highlightColor
            onClicked: { Clipboard.text=lang2; selectAll(); fullTextLang1.deselect() }
          }
        }
      }
    }
  }
}
