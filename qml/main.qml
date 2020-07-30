import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

import Custom 1.0

Window {
    id: windowRoot
    visible: true
    width: 720
    height: width + 100
    title: qsTr("Minesweeper")

    MinesweeperQ {
        id: data; rows: 1; columns: 1;
        onNumLeftChanged: checkWinningConditions()
        onNumFlagsChanged: checkWinningConditions()
    }

    function checkWinningConditions() {
        if (data.numLeft - data.numFlags == 0 && data.numFlags == data.numBombs) {
            header.color = "green"
            prepareOverlay.enabled = true
        }
    }

    onHeightChanged: table.forceLayout()
    onWidthChanged: table.forceLayout()

    Rectangle {
        id: background
        anchors.fill: parent
        color: "black"
    }

    Column {
        anchors.fill: parent

        Rectangle {
            id: header
            height: 100
            width: parent.width
            border { width: 1; color: "black" }
            color: "white"

            Row {
                anchors.fill: parent
                Rectangle {
                    height: parent.height
                    width: parent.width/2
                    color: "#00000000"

                    Text {
                        anchors.centerIn: parent
                        text: "Tiles Unopened: " + (data.numLeft - data.numFlags)
                    }
                }
                Rectangle {
                    height: parent.height
                    width: parent.width/2
                    color: "#00000000"

                    Text {
                        anchors.centerIn: parent
                        text: "Bombs Flagged: " + data.numFlags
                    }
                }
            }
        }

        TableView {
            id: table
            height: parent.height - 100
            reuseItems: false
            width: parent.width

            rowHeightProvider: function() { return table.height/data.rows - 0.05*data.rows}
            columnWidthProvider: function() { return table.width/data.columns - 0.05*data.columns}

            rowSpacing: 1
            columnSpacing: 1

            model: data

            delegate:
                Rectangle {
                    id: root
                    height: table.cellHeight
                    width: table.cellWidth
                    color: isRevealed ? (number == -1 ? "red" : "grey") : (isFlagged ? "orange" : "green")

                    Text {
                        id: label
                        visible: isRevealed
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: number == -1 || number == 0 ? "" : number
                    }

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton | Qt.RightButton

                        onPressed: {
                            if (!isFlagged && pressedButtons == Qt.LeftButton) {
                                isRevealed = true
                                if (number == -1) {
                                    header.color = "red"
                                    prepareOverlay.enabled = true
                                }
                            } else if (!isRevealed && data.numLeft != 0 && pressedButtons == Qt.RightButton) {
                                isFlagged = !isFlagged
                            }
                        }
                    }
                }
        }
    }

    MouseArea {
        id: prepareOverlay
        anchors.fill: parent
        enabled: false
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            overlay.state = ""
            enabled = false
        }
    }

    Item {
        id: overlay
        width: parent.width
        height: parent.height

        function setData(rows, columns, num_bombs) {
            data.rows = rows
            data.columns = columns
            data.numBombs = num_bombs
            overlay.state = "retracted"
        }

        Rectangle {
            anchors.fill: parent
            color: "green"
        }

        MouseArea {
            //antialiasing: true
            anchors.fill: parent
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 2

            Button {
                id: easyButton
                width: 100
                height: 50
                text: "Easy"
                onClicked: overlay.setData(10, 10, 10)
            }
            Button {
                id: mediumButton
                width: 100
                height: 50
                text: "Medium"
                onClicked: overlay.setData(18, 18, 40)
            }
            Button {
                id: hardButton
                width: 100
                height: 50
                text: "Hard"
                onClicked: overlay.setData(24, 24, 99)
            }
        }

        states: State {
            name: "retracted";
            PropertyChanges { target: overlay; y: -parent.height; visible: false }
        }

        transitions:
            [Transition {
                from: ""; to: "retracted"
                SequentialAnimation {
                    NumberAnimation { properties: "y"; easing.type: Easing.InOutQuad; duration: 500 }
                    PropertyAnimation { properties: "visible" }
                }
            }, Transition {
                from: "retracted"; to: ""
                SequentialAnimation {
                    PropertyAnimation { properties: "visible" }
                    NumberAnimation { properties: "y"; easing.type: Easing.InOutQuad; duration: 500 }
                    ScriptAction { script: {data.reset(); header.color = "white"} }
                }
            }]
    }
}
