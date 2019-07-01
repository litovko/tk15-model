import QtQuick 2.12
import QtQuick.Window 2.12
import "theme/theme.js" as THEME
import HYCO 1.0
import QtQuick.Controls 2.3

ListView {
    id: listView
    anchors.left: parent.left
    anchors.bottom: parent.bottom
    width: 100
    height: parent.height
    model: tk15.data_sensors
    signal delegatChanged(string name, bool chk)
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
                indicator: Rectangle {
                    implicitWidth: 26
                    implicitHeight: 26
                    x: cb.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 3
                    border.color: cb.down ? Qt.red : THEME.window_text_color
                    color: THEME.window_background_color
                    Rectangle {
                        width: 14
                        height: 14
                        x: 6
                        y: 6
                        radius: 2
                        color: cb.checked ? tk15.color(txt.text) : "#00FF00"
                        visible: cb.checked
                    }
                }
            }
        }
    }
}
