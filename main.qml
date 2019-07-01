import QtQuick 2.12
import QtQuick.Window 2.12
import "theme/theme.js" as THEME
import HYCO 1.0
import QtQuick.Controls 2.3

Window {
    visible: true
    width: 700
    height: 700
    title: qsTr("TK-15 ТРЕНАЖЕР")
    color: THEME.window_background_color
    TK15{
        id: tk15
        chart: plot.plot

    }
    Commands{
        id: cmd

    }
    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        id: f
        text: qsTr(tk15.filename)
        color:THEME.window_text_color
        height: THEME.window_text_size
    }
    PlotChart {
        id: plot
        anchors.left: listView.right
        anchors.bottom: parent.bottom
        width:  parent.width-100
        height: 600
    }
    SensorsListView {
        id: listView
        onDelegatChanged: {
            tk15.toggle(name,chk)
            print (name+" status: "+chk)
        }
    }

}
