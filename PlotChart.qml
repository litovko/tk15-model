import QtQuick 2.0
import CustomPlot 1.0
import "theme/theme.js" as THEME
Item {
    property alias plot: customPlot
    CustomPlotItem {
        id: customPlot
        anchors.fill: parent

        Component.onCompleted:  {
            initCustomPlot()
            //tk15.chart = customPlot
        }


    }


}
