import QtQuick 2.12
import QtQuick.Window 2.12
import "theme/theme.js" as THEME
import HYCO 1.0
import QtQuick.Controls 2.5


MenuBar {
    id: mainmenu
    Menu { title: qsTr("Меню")
        Action {
            text: "Загрузить файл"
            shortcut: "Ctrl+R"
            onTriggered: cmd.execute("OPEN")
        }
        Action {
            text: "Выход"
            shortcut: "Ctrl+Q"
            onTriggered: cmd.execute("QUIT")
        }
        delegate: MenuItem {
            id: menuItem
            implicitWidth: 200
            implicitHeight: 40
            
            arrow: Canvas {
                x: parent.width - width
                implicitWidth: 40
                implicitHeight: 40
                visible: menuItem.subMenu
                onPaint: {
                    var ctx = getContext("2d")
                    ctx.fillStyle = menuItem.highlighted ? "#ffffff" : "yellow"
                    ctx.moveTo(15, 15)
                    ctx.lineTo(width - 15, height / 2)
                    ctx.lineTo(15, height - 15)
                    ctx.closePath()
                    ctx.fill()
                }
            }
            
            indicator: Item {
                implicitWidth: 40
                implicitHeight: 40
                Rectangle {
                    width: 26
                    height: 26
                    anchors.centerIn: parent
                    visible: menuItem.checkable
                    border.color: THEME.window_border_color
                    radius: 3
                    Rectangle {
                        width: 14
                        height: 14
                        anchors.centerIn: parent
                        visible: menuItem.checked
                        color: THEME.window_background_color
                        radius: 5
                    }
                }
            }
            
            contentItem: Text {
                leftPadding: menuItem.indicator.width
                rightPadding: menuItem.arrow.width
                text: menuItem.text
                font: menuItem.font
                opacity: enabled ? 1.0 : 0.3
                color: menuItem.highlighted ? "#ffffff" : THEME.window_text_color
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
            
            background: Rectangle {
                implicitWidth: 200
                implicitHeight: 40
                opacity: enabled ? 1 : 0.3
                color: menuItem.highlighted ? "#252525"  : "black"
                Rectangle {
                    color: THEME.window_border_color
                    width: parent.width
                    height: 1
                    anchors.bottom: parent.bottom
                }
            }
        }
    }
    delegate: MenuBarItem {
        id: menuBarItem
        
        contentItem: Text {
            text: menuBarItem.text
            font: menuBarItem.font
            opacity: enabled ? 1.0 : 0.3
            color: menuBarItem.highlighted ? THEME.window_text_hilight_color : THEME.window_text_color
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
        
        background: Rectangle {
            implicitWidth: 40
            implicitHeight: 40
            opacity: enabled ? 1 : 0.3
            color: menuBarItem.highlighted ? "#252525" : THEME.window_background_color
            Rectangle {
                color: THEME.window_border_color
                width: parent.width
                height: 1
                anchors.bottom: parent.bottom
            }
        }
    }
    background: Rectangle {
        implicitWidth: 40
        implicitHeight: 40
        color: THEME.window_dialog_background_color
        
        Rectangle {
            color: THEME.window_text_color
            width: parent.width
            height: 1
            anchors.bottom: parent.bottom
        }
    }
    
}
