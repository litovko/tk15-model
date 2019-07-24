import QtQuick 2.12
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.12
import "theme/theme.js" as THEME

Item {
    id: commands
    property var command: ["PLAY", "STOP", "OPEN", "LISTEN", "SWITCH", "CANSEL", "QUIT"]
    anchors.fill: parent
    visible: true
    focus: true
    function execute(cmd) {

        print("Command = " + cmd)
        //        if (!(cmd in command) ) {
        //            print("uncnown command:"+cmd);
        //            return;
        //        }
        switch (cmd) {
        case "PLAY":
            tk15.play()
            break
        case "LISTEN":
            tk15.listen()
            break
        case "OPEN":
            if (tk15.onload)
                return
            fileDialog.visible = true
            break
        case "STOP":
            if (!tk15.onload)
                return
            tk15.stop_readfile()
            break
        case "PRINT":
            tk15.printdata()
            break
        case "SWITCH":
            rect.visible = !rect.visible
            break
        case "QUIT":
            Qt.quit()
            break
        default:
            print("default")
        }
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_F2)
            execute("OPEN")
        if (event.key === Qt.Key_F5)
            execute("PLAY")
        if (event.key === Qt.Key_F3)
            execute("LISTEN")
        if (event.key === Qt.Key_F6)
            execute("PRINT")
        if (event.key === Qt.Key_F7)
            execute("SWITCH")
    }

    FileDialog {
        id: fileDialog
        title: "Выберите файл"
        visible: false

        nameFilters: ["Файлы логов (*.log *.txt)"]
        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrls)
            tk15.filename = fileDialog.fileUrl
        }
    }
    Rectangle {
        id: loaddata
        //visible: tk15.onload
        width: 300
        height: 110
        visible: tk15.onload
        radius: 5
        border.color: THEME.window_text_color
        color: THEME.window_dialog_background_color
        anchors.centerIn: parent
        Text {
            color: THEME.window_text_color
            text: "Загрузка файла данных"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.margins: 5
            font.pointSize: THEME.window_text_size
            font.bold: true
        }
        ProgressBar {
            id: progressBar
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.margins: 35
            from: 0
            to: 100
            value: tk15.progress
            width: 200
            //visible: value>0
            background: Rectangle {
                implicitWidth: progressBar.width - 4
                implicitHeight: 14
                color: "#e6e6e6"
                radius: 3
            }
            contentItem: Item {
                implicitWidth: progressBar.width - 4
                implicitHeight: parent.height
                z: -1
                Rectangle {
                    width: progressBar.visualPosition * parent.width
                    height: parent.height
                    radius: 2
                    color: "#17a81a"
                }
            }
            Text {
                anchors.centerIn: parent
                height: parent.height
                id: proctext
                color: "blue"
                text: progressBar.value
            }
        }
        Button{
            text: "Отмена"
            onClicked: commands.execute("STOP")
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 10
        }
    }
}
