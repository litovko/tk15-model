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
    ProgressBar {
        id: progressBar
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.margins: 10
        from: 0
        to: 100
        value: tk15.progress
        visible: value>0
//        indeterminate: true
        background: Rectangle {
                 implicitWidth: parent.width
                 implicitHeight: 14
                 color: "#e6e6e6"
                 radius: 3
                 Text {
                     anchors.centerIn: parent
                     id: name
                     z: 1
                     color: "blue"
                     text: progressBar.value
                 }
             }
        contentItem: Item {
                  implicitWidth: 200
                  implicitHeight: parent.height
                  z:-1
                  Rectangle {
                      width: progressBar.visualPosition * parent.width
                      height: parent.height
                      radius: 2
                      z: 0
                      color: "#17a81a"
                  }
              }
    }

}
