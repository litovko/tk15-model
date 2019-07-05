import QtQuick 2.12
import QtQuick.Dialogs 1.2

Item {
    property var command: ["PLAY", "STOP", "OPEN", "LISTEN"]
    function execute(cmd) {
        print("Command = "+cmd)
        switch (cmd) {
            case "PLAY":
                tk15.play()
            break;
            case "LISTEN":
                tk15.listen()
            break;
            case "OPEN":
                fileDialog.visible=true;
            break
            case "PRINT":
                tk15.printdata()
            break;
            case "SWITCH":
                rect.visible=!rect.visible
            break;

        default:
            print("default")
        }
    }
    anchors.fill: parent
    visible: true
    focus: true
    Keys.onPressed: {
           if (event.key === Qt.Key_F2) execute("OPEN")
           if (event.key === Qt.Key_F5) execute("PLAY")
           if (event.key === Qt.Key_F3) execute("LISTEN")
           if (event.key === Qt.Key_F6) execute("PRINT")
           if (event.key === Qt.Key_F7) execute("SWITCH")
       }


    FileDialog {
        id: fileDialog
        title: "Выберите файл"
        visible: false

        nameFilters: [ "Файлы логов (*.log *.txt)"]
        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrls)
            tk15.filename = fileDialog.fileUrl;
        }
    }

}
