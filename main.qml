import QtQuick
import QtQuick.Window
//import QtQuick.Controls 2.12
import QtQuick.Controls
import QtQuick.Layouts
//import QtQuick.Controls 1.4 as Ctrl1
import QtMultimedia
import TalkModel 1.0

Window {
    visible: true
    width: 720
    height: 820
    title: qsTr("ChatBot")

    TalkListModel{
        id: talk_model
        onModelReset: {
            talk_player.stop();
            update_timer.start();
        }
        onRowsInserted: {
            talk_player.stop();
            update_timer.start();
        }
    }

    MediaPlayer{
        id: talk_player
        //只是标记下当前播放对象
        property int currentId: -1
    }

    Timer{
        id: update_timer
        interval: 0
        repeat: false
        onTriggered: {
            //对应版本Qt5.13.1
            //positionViewAtEnd有问题，新增的大小受上次最后一项大小的影响
            //如果上次更短就没法滑倒底部
            //talk_view.positionViewAtEnd();
            talk_view.currentIndex=talk_view.count-1;
        }
    }

    SplitView{
        anchors.fill: parent
        anchors.margins: 20
        orientation: Qt.Vertical

//        handleDelegate: Rectangle{
//            height: 10
//        }
        Rectangle{
            height: 50
            SplitView.minimumHeight: parent.height*0.025
            SplitView.maximumHeight: parent.height*0.05
            Label{
                text: "ChatBot"
            }
        }
        Rectangle{ // box to post message
            Layout.fillHeight: false
            SplitView.minimumHeight: parent.height*0.7
            Layout.fillWidth: true
            radius: 4
            border.color: "gray"
            color: "#EEEEEE"
            //消息列表
            TalkListView{
                id: talk_view
                anchors.fill: parent
                anchors.margins: 15
                model: talk_model
                talkModel: talk_model
                audioPlayer: talk_player
            }
        }

        Rectangle{ // box to input and button
            height: 400
            implicitHeight: 400
            SplitView.maximumHeight: 400
            Layout.fillWidth: true
            radius: 4
            border.color: "gray"

            //文本编辑框
            ScrollView {
                id: view
                anchors{
                    fill: parent
                    leftMargin: 10
                    rightMargin: 10
                    topMargin: 10
                    bottomMargin: 80
                }

            TextArea{
                id: text_area
                text: "Test Message"
//                placeholderText: qsTr("Enter Message")
                anchors.fill: parent
                topPadding: 10
                font{
                    family: "Microsoft YaHei"
                    pixelSize: 14
                }
                color: "#666666"
                selectByMouse: true
                selectionColor: "black"
                selectedTextColor: "white"
                wrapMode: TextInput.WrapAnywhere
                background: Rectangle{
                    border.color: "gray"
                }
            }
            }

            GridLayout {
                id: grid
                columns: 4
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.margins: 10

                ComboBox{ //数据类型
                    id: send_type
                    model: ["text","audio"]
                    currentIndex: 0
                }
                Button{ //清空
                    text: "Clear"
                    onClicked: {
                        text_area.clear();
                        talk_model.clearModel();
                    }
                }

                Button{ //A发送
                    anchors.margins: 10
                    text: "Bot Send"
                    onClicked: {
                        switch(send_type.currentText){
                        case "text":
                            if(true){ //M115
                                if(text_area.text.length<1)
                                    return;
                                talk_model.appendText("B","A",text_area.text);
                            }break;
                        case "audio":
                            if(true){
                                talk_model.appendAudio("B","A");
                            }break;
                        }
                    }
                }

                Button{ //B发送
                    anchors.margins: 10
                    text: "human Send"
                    onClicked: {
                        switch(send_type.currentText){
                        case "text":
                            if(true){ //M115
                                if(text_area.text.length<1)
                                    return;
                                talk_model.appendText("B","B",text_area.text);
                            }break;
                        case "audio":
                            if(true){
                                talk_model.appendAudio("B","B");
                            }break;
                        }
                    }
                }//end Button

            }
        }
    }
}
