import QtQuick 2.0
import Sailfish.Silica 1.0

TextField {
  id: searchField

  property bool active: true
  property int transitionDuration: 200

  property bool _initialized

  signal hideClicked

  implicitHeight: Math.max(
                    Theme.itemSizeMedium,
                    _editor.height + Theme.paddingMedium + Theme.paddingSmall)
                  + (labelVisible ? _labelItem.height : 0)

  textMargin: Theme.horizontalPageMargin - Theme.paddingSmall
  focusOutBehavior: FocusBehavior.ClearPageFocus
  font {
    pixelSize: Theme.fontSizeLarge
    family: Theme.fontFamilyHeading
  }

  textTopMargin: height / 2 - _editor.implicitHeight / 2
  labelVisible: false

  onFocusChanged: if (focus)
                    cursorPosition = text.length

  inputMethodHints: Qt.ImhNoPredictiveText
  background: null

  Component.onCompleted: {
    _initialized = true
  }

  leftItem: Icon {
    source: 'image://theme/icon-m-search'
  }

  rightItem: IconButton {
    icon.source: 'image://theme/icon-m-clear'

    enabled: searchField.text.length > 0

    opacity: enabled ? 1.0 : 0.0
    Behavior on opacity {
      FadeAnimation {}
    }

    onClicked: {
      searchField.text = ''
      searchField.forceActiveFocus()
    }
  }

  states: State {
    name: 'inactive'
    when: !searchField.active

    PropertyChanges {
      target: searchField
      height: 0
      opacity: 0
      clip: true
    }
    PropertyChanges {
      target: searchField._editor
      focus: false
    }
  }

  transitions: [
    Transition {
      from: ''
      to: 'inactive'
      enabled: searchField._initialized

      SequentialAnimation {
        NumberAnimation {
          duration: searchField.transitionDuration
          easing.type: Easing.InOutQuad
          properties: 'opacity,height'
        }
        PropertyAction {
          target: searchField
          property: 'visible'
          value: false
        }
        PropertyAction {
          target: searchField
          property: 'text'
          value: ''
        }
      }
    },

    Transition {
      from: 'inactive'
      to: ''
      enabled: searchField._initialized

      SequentialAnimation {
        PropertyAction {
          target: searchField
          property: 'visible'
          value: true
        }
        NumberAnimation {
          duration: searchField.transitionDuration
          easing.type: Easing.InOutQuad
          properties: 'opacity,height'
        }
      }
    }
  ]
}
