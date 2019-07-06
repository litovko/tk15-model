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
}
