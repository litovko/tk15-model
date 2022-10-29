import QtQuick 2.12
import QtQuick.Window 2.12
import "theme/theme.js" as THEME
import HYCO 1.0

Rectangle {
    id: plotarea
    property alias plot: plot.plot
    color: "transparent"
    border.color: THEME.plot_border_color
    border.width: 1
    radius: 5
    PlotChart {
        id: plot
        anchors.fill: parent
        anchors.margins: 5
    }
    Row {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: 5
        spacing: 5
        Rectangle {
            color: "transparent"
            border.color: THEME.plot_border_color
            height: THEME.window_text_size + 4
            width: 80
            Text {
                text: plot.plot.graph_name
                anchors.centerIn: parent
                color: THEME.window_text_color
                font.pixelSize: THEME.window_text_size
                z: 5
            }
        }
        Rectangle {
            color: "transparent"
            border.color: THEME.window_text_hilight_color
            height: THEME.window_text_size + 4
            width: 80
            Text {
                text: plot.plot.coord_x
                anchors.centerIn: parent
                color: THEME.window_text_color
                font.pixelSize: THEME.window_text_size
                z: 5
            }
        }
        Rectangle {
            color: "transparent"
            border.color: THEME.plot_border_color
            height: THEME.window_text_size + 4
            width: 80
            Text {
                text: plot.plot.coord_y
                anchors.centerIn: parent
                color: THEME.window_text_color
                font.pixelSize: THEME.window_text_size
                z: 5
            }
        }
    }
}
