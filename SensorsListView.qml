import QtQuick 2.12
import QtQuick.Window 2.12
import "theme/theme.js" as THEME
import HYCO 1.0
import QtQuick.Controls 2.3
import QtQuick.Dialogs 1.3

ListView {
    id: listView

    width: 100
    height: parent.height

    signal delegatChanged(string name, bool chk)
    signal tagcolorChanged(string name, string col)
    delegate: Item {
        x: 5
        width: 80
        height: 40
        Rectangle{
            anchors.fill: parent
            anchors.margins: 5
            id: rect
            radius: 5
            border.color: THEME.window_background_color
            color: THEME.window_background_color
            Text {
                id: txt
                text: modelData
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                font.bold: true
                color: THEME.window_text_color
            }
            CheckBox {
                id: cb
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                focusPolicy: Qt.NoFocus
                onCheckedChanged: {
                    listView.delegatChanged(txt.text, cb.checked)

                }
                ColorDialog {
                       id: colorDialog
                       title: "Please choose a color"
                       visible: false
                       onAccepted: {
                           console.log("You chose: " + colorDialog.color)
                           listView.tagcolorChanged(txt.text, colorDialog.color)
                           colrect.color=colorDialog.color
                           cb.checked=true
                       }
                       onRejected: {
                           visible=false
                       }
                   }
                MouseArea {
                    id: ma
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    onClicked: if (mouse.button === Qt.RightButton) {
                                   print("color:"+txt.text+":"+tk15.color(txt.text));
                                   colorDialog.color=tk15.color(txt.text)
                                   colorDialog.visible=true;
                               }

                }
                indicator: Rectangle {
                    implicitWidth: 26
                    implicitHeight: 26
                    x: cb.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 3
                    border.color: cb.down ? "red": THEME.window_text_color
                    color: THEME.window_background_color
                    Rectangle {
                        id: colrect
                        width: 14
                        height: 14
                        x: 6
                        y: 6
                        radius: 2
                        color: cb.checked ? tk15.color(txt.text) : "#000000"
                        visible: cb.checked

                    }
                }
            }
        }
    }
}
