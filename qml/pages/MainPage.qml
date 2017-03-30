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
import QtQuick.LocalStorage 2.0
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
      RemorsePopup { id: remorse_erase }
      MenuItem {
        text: qsTr("Erase Dictionary")
        onClicked: remorse_erase.execute(qsTr("Erasing database"),
                                         function() {
                                             mdl.clear()
                                             dictionaries.clear()
                                             queryFieldText=""
                                             eraseDB()
                                         })
      }
      MenuItem {
        text: qsTr("Change Dictionary")
        onClicked: {
            pageStack.replace(Qt.resolvedUrl("ChooseDictionary.qml"))
            mdl.clear()
            dictionaries.clear()
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
          title: pageTitle
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
            mdl.clear()
            searchQuery(queryFieldText)
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
          visible: mdl.count==0 && queryFieldText!=""
        }
      }
    }

    header: hearderComponent

    model: ListModel{
    id:mdl
    Component.onCompleted: openDB()
    }

    delegate: ListItem {
      width: main_page.width
      contentHeight: textLang1.height+textLang2.height+Theme.paddingLarge+Theme.paddingSmall
      menu: contextMenu
      showMenuOnPressAndHold: !(model.definition1=="" && model.definition2=="")
      Item {
        id: item
        height: textLang1.height+textLang2.height+Theme.paddingLarge
        width: parent.width
        Label {
          id: textLang1
          text: model.definition1
          x: Theme.horizontalPageMargin
          width: parent.width-2*x
          anchors.top: item.top
          anchors.topMargin: 0.35*Theme.paddingLarge
          truncationMode: TruncationMode.Fade
        }
        Label {
          id: textLang2
          text: model.definition2
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
              text: model.definition1
              width: Screen.sizeCategory>=Screen.Large ? parent.width-Theme.iconSizeMedium-Theme.paddingMedium :  parent.width-Theme.iconSizeSmall-Theme.paddingMedium
              readOnly: true
              wrapMode: TextEdit.Wrap
              labelVisible: false
              onClicked: {
                Clipboard.text=model.definition1
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
              text: model.definition2
              width: Screen.sizeCategory>=Screen.Large ? parent.width-Theme.iconSizeMedium-Theme.paddingMedium :  parent.width-Theme.iconSizeSmall-Theme.paddingMedium
              readOnly: true
              wrapMode: TextEdit.Wrap
              color: Theme.highlightColor
              labelVisible: false
              onClicked: {
                Clipboard.text=model.definition2
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

  property var db : null
  property string pageTitle: ""

  function initDB(tx) {

      // Create the database if it doesn't already exist
      tx.executeSql("create table if not exists definitions (DID integer PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,"+
                    "definition1 varchar(250),"+
                    "lang1 varchar(5), "+
                    "definition2 varchar(250), "+
                    "lang2 varchar(5))");
      tx.executeSql("create table if not exists occurrences (OID integer PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE, "+
                    "langFrom varchar(5),"+
                    "langTo varchar(5), "+
                    "wordId INTEGER REFERENCES words(id), "+
                    "defId INTEGER REFERENCES definitions(id)) ");
      tx.executeSql("create table if not exists words (WID integer PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,"+
                    "word varchar(250),"+
                    "lang varchar(5)) ");
  }

  function openDB() {
      main_page.db = LocalStorage.openDatabaseSync(dbName, "1.0", "SailBabel's database", 1000000);
        console.log("Opening DB")
      main_page.db.transaction(initDB)
      main_page.db.transaction(listDictsInDB)
  }

  function listDictsInDB(tx){
      main_page.db.transaction(
                  function(tx) {
                      dictionaries.clear()
                      var langs = tx.executeSql("SELECT DISTINCT lang1,lang2 FROM definitions");
                      for(var i = 0; i < langs.rows.length; i++) {
                          dictionaries.append({"l1":langs.rows.item(i).lang1,"l2":langs.rows.item(i).lang2})
                          console.log("Dict: "+dictionaries.get(i).l1+" "+dictionaries.get(i).l2)
                      }
                      if(langs.rows.length>0){
                          pageTitle="Dictionary "+dictionaries.get(0).l1+"->"+dictionaries.get(0).l2
                          dictionary.coverTitle=""
                      } else {
                          pageTitle="No dictionary loaded"
                          dictionary.coverTitle="Not loaded"
                      }
                  })
  }

  function eraseDB(){
      main_page.db.transaction(
                  function(tx) {
      tx.executeSql("DROP TABLE if exists definitions");
      tx.executeSql("DROP TABLE if exists words");
      tx.executeSql("DROP TABLE if exists occurrences");
                  })
      dictionary.coverTitle="Not Loaded"
      pageTitle="No dictionary loaded"
      main_page.db.transaction(initDB)
  }

  function searchQuery(queryFieldText) {
      var arr=queryFieldText.split(' ').map(Function.prototype.call, String.prototype.trim) //.join("' and word='")
      var words=[]
      var q=""
      if(arr.length>1){
          q='WITH tbl AS (';
          var clauses=[]
          for(var clause in arr){
              clauses.push('SELECT * FROM words w INNER JOIN occurrences o ON o.wordId=w.wid INNER JOIN definitions d ON o.defId = d.did WHERE word=?')
              words.push(arr[clause])
          }
          q+=clauses.join(" UNION ALL ")
          q+=') SELECT * FROM tbl GROUP BY definition1,definition2 HAVING COUNT(*)='+words.length;
      } else {
          q='SELECT * FROM words w INNER JOIN occurrences o ON o.wordId=w.wid INNER JOIN definitions d ON o.defId = d.did WHERE word=?'
          words.push(arr[0])
      }
      main_page.db.transaction(
                  function(tx) {
                      // Show all added greetings
                      console.log(q)
                      console.log(words)
                      var rs = tx.executeSql(q,words)
                      for(var i = 0; i < rs.rows.length; i++) {
                          mdl.append({definition1: rs.rows.item(i).definition1,definition2:rs.rows.item(i).definition2})
                      }
                  }
                  )
  }

  Connections {
      target: dictionary
      onInitDB:{
          openDB()
      }
      onInitLangs:{
          listDictsInDB()
      }
  }

}
