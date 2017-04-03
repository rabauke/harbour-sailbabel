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

import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {

  function basename(str) {
    return str.slice(str.lastIndexOf("/")+1)
  }

  Image {
    source: "/usr/share/harbour-sailbabel/images/cover_background.png"
    x: 0; y: parent.height-parent.width-Theme.paddingLarge; z: -1
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
      bottomMargin: 1.25*Theme.paddingLarge
      leftMargin: Theme.paddingLarge
      rightMargin: Theme.paddingLarge
    }
    Text {
      id: title
      text: "SailBabel"
      color: Theme.highlightColor
      font.pixelSize: Theme.fontSizeMedium
      wrapMode: Text.Wrap
      width: parent.width
      horizontalAlignment: Text.AlignHCenter
    }
    Text {
      anchors.top: title.bottom
      anchors.topMargin: 1.25*Theme.paddingLarge
      text: dictionary.coverTitle
      color: Theme.highlightColor
      font.pixelSize: Theme.fontSizeMedium
      wrapMode: Text.Wrap
      width: parent.width
      horizontalAlignment: Text.AlignHCenter
    }
  }

  CoverActionList {
          id: coverAction

          CoverAction {
              iconSource: "image://theme/icon-cover-search"
              onTriggered: {
                  pageStack.clear()
                  pageStack.replace("../pages/MainPage.qml")
                  appWindow.activate()
              }
          }
      }

}
