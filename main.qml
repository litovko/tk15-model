import QtQuick 2.12
import QtQuick.Window 2.12
import "theme/theme.js" as THEME
import HYCO 1.0
import QtQuick.Controls 2.5


Window {
    id: mainwindow
    visible: true
    width: 700
    height: 700
    title: qsTr("TK-15 ТРЕНАЖЕР")
    color: THEME.window_background_color

    TK15 {
        id: tk15
        chart: plotarea.plot
    }
    Commands {
        id: cmd
    }
    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        id: f
        text: qsTr(tk15.filename)
        color: THEME.window_text_color
        height: THEME.window_text_size
    }
    Plotarea {
        id: plotarea
        visible: !tk15.onload
        anchors {
                    left: listView_s.right
                    right: listView_c.left
                    bottom: parent.bottom
                    top: parent.top
                    margins: 50
        }
    }

    SensorsListView {
        id: listView_s
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        model: tk15.data_sensors
        onDelegatChanged: {
            tk15.toggle(name, chk)
            //print(name + " status: " + chk)
        }
        onTagcolorChanged: {
            print (name+"==="+col)
            tk15.tagcolor(name, col)
        }
    }
    SensorsListView {
        id: listView_c
        anchors.right:  parent.right
        anchors.bottom: parent.bottom
        model: tk15.data_control
        onDelegatChanged: {
            tk15.toggle(name, chk)
            //print(name + " status: " + chk)
        }
        onTagcolorChanged: {
            print (name+"==="+col)
            tk15.tagcolor(name, col)
        }
    }
}
