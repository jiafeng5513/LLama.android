import QtQuick
import QtQuick.Window
// import QtQuick.Controls 2.12
import QtQuick.Controls
import QtQuick.Layouts
//import QtQuick.Controls 1.4 as Ctrl1
import QtMultimedia
import ChatBot 1.0


Window {
    visible: true
    width: 1920
    height: 1080
    title: qsTr("ChatBot")
    property bool bMenuShown: false
    property real sliderRange: 0.5

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

    function onMenu()
    {
        menuTranslate.x = bMenuShown ? 0 : width * sliderRange
        bMenuShown = !bMenuShown;
    }

    Rectangle { // Slider ui
        width : parent.width * sliderRange
        height: parent.height
        color: "grey";
        opacity: bMenuShown ? 1 : 0
        Behavior on opacity {
            NumberAnimation {
                duration: 300
            }
        }

        Column{ //垂直布局子对象
            width : parent.width
            height: parent.height
            spacing: 20  //相邻项的间隔

            Rectangle{ // box for title
                height: 50
                width: parent.width
                Text {
                    id: menu_title_text
                    text: qsTr("Settings")
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            GroupBox { // prompt
                title: qsTr("Prompt")
                width: parent.width - 30
                anchors.horizontalCenter: parent.horizontalCenter
                Column {
                    spacing: 10
                    anchors.fill: parent
                    width: parent.width
                    height: parent.height

                    ScrollView {
                        id: scroll_view_for_setting_prompt
                        width: parent.width
                        height: 80
                        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
                        TextArea{
                            id: text_area_for_setting_prompt
                            text: "This is a conversation between user and llama, a friendly chatbot. respond in simple markdown."
                            width: parent.width
                            height: parent.height
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
                        id: grid_for_prompt
                        columns: 4
                        width: parent.width
                        Text {
                            id: text_user_name
                            text: qsTr("user name:")
                        }
                        TextField{
                            placeholderText: qsTr("user")
                        }
                        Text {
                            id: text_bot_name
                            text: qsTr("bot name:")
                        }
                        TextField{
                            placeholderText: qsTr("llama")
                        }
                    }
                    Text {
                        id: text_prompt_template
                        text: qsTr("Prompt template:")
                    }
                    ScrollView {
                        id: scroll_view_for_prompt_template
                        width: parent.width
                        height: 100
                        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
                        TextArea{
                            id: text_area_for_prompt_template
                            text: "{{prompt}}\n\n{{history}}\n{{char}}:"
                            width: parent.width
                            height: parent.height
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
                    Text {
                        id: text_chat_history_template
                        text: qsTr("Chat history template:")
                    }
                    ScrollView {
                        id: scroll_view_for_chat_history_template
                        width: parent.width
                        height: 40
                        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
                        TextArea{
                            id: text_area_for_chat_history_template
                            text: "{{name}}: {{message}}"
                            width: parent.width
                            height: parent.height
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
                }
            }

            GroupBox { // params 2
                title: qsTr("Params 2")
                width: parent.width-30
                anchors.horizontalCenter: parent.horizontalCenter
                RowLayout{
                    anchors.fill: parent
                    ColumnLayout {
                        width: parent.width * 0.5
                        GridLayout {
                            id: grid_for_params_2_1
                            columns: 3
                            width: parent.width
                            // 1
                            Text {text: qsTr("Predictions")}
                            Slider {
                                id: slider_predictions
                                from: -1
                                value: 512
                                to: 2048
                            }
                            Text {text: slider_predictions.value.toFixed(0)}
                            // 2
                            Text {text: qsTr("Temperature")}
                            Slider {
                                id: slider_temperature
                                from: 0
                                value: 0.7
                                to: 1
                            }
                            Text {text: slider_temperature.value.toPrecision(2)}
                            // 3
                            Text {text: qsTr("Penalize repeat sequence")}
                            Slider {
                                id: slider_penalize_repeat_sequence
                                from: 0
                                value: 1.18
                                to: 2
                            }
                            Text {text: slider_penalize_repeat_sequence.value.toPrecision(2)}
                        }
                    }
                    ColumnLayout {
                        width: parent.width * 0.5
                        GridLayout {
                            id: grid_for_params_2_2
                            columns: 3
                            width: parent.width

                            // 4
                            Text {text: qsTr("Consider N tokens for penalize")}
                            Slider {
                                id: slider_consider_n_tokens_for_penalize
                                from: 0
                                value: 128
                                to: 2048
                            }
                            Text {text: slider_consider_n_tokens_for_penalize.value.toFixed(0)}
                            // 5
                            Text {text: qsTr("Top-K sampling")}
                            Slider {
                                id: slider_top_k_sampling
                                from: -1
                                value: 40
                                to: 100
                            }
                            Text {text: slider_top_k_sampling.value.toFixed(0)}
                            // 6
                            Text {text: qsTr("Top-P sampling")}
                            Slider {
                                id: slider_top_p_sampling
                                from: 0
                                value: 0.5
                                to: 1
                            }
                            Text {text: slider_top_p_sampling.value.toPrecision(2)}
                        }
                    }
                }
            }

            GroupBox { // params 3
                title: qsTr("params 3")
                width: parent.width-30
                anchors.horizontalCenter: parent.horizontalCenter
                RowLayout{
                    anchors.fill: parent
                    ColumnLayout {
                        width: parent.width * 0.5
                        GridLayout {
                            id: grid_for_params_3_1
                            columns: 3
                            width: parent.width
                            // 1
                            Text {text: qsTr("TFS-Z")}
                            Slider {
                                id: slider_tfs_z
                                from: 0
                                value: 1
                                to: 1
                            }
                            Text {text: slider_tfs_z.value.toPrecision(2)}
                            // 2
                            Text {text: qsTr("Typical P")}
                            Slider {
                                id:slider_typical_p
                                from: 0
                                value: 1
                                to: 1
                            }
                            Text {text: slider_typical_p.value.toPrecision(2)}
                        }
                    }
                    ColumnLayout {
                        width: parent.width * 0.5
                        GridLayout {
                            id: grid_for_params_3_2
                            columns: 3
                            width: parent.width
                            // 3
                            Text {text: qsTr("Presence penalty")}
                            Slider {
                                id:slider_presence_penalty
                                from: 0
                                value: 0
                                to: 1
                            }
                            Text {text: slider_presence_penalty.value.toPrecision(2)}
                            // 4
                            Text {text: qsTr("Frequency penalty")}
                            Slider {
                                id: slider_frequency_penalty
                                from: 0
                                value: 0
                                to: 1
                            }
                            Text {text: slider_frequency_penalty.value.toPrecision(2)}
                        }
                    }
                }
            }

            GroupBox { // params 4
                title: qsTr("params 4")
                width: parent.width-30
                anchors.horizontalCenter: parent.horizontalCenter
                RowLayout {
                    anchors.fill: parent
                    GridLayout {
                        id: grid_for_params_4_1
                        columns: 3
                        width: parent.width * 0.5
                        // 1
                        Text {text: qsTr("Mirostat tau")}
                        Slider {
                            id: slider_mirostat_tau
                            from: 0
                            value: 5
                            to: 10
                        }
                        Text {text: slider_mirostat_tau.value.toFixed(0)}
                        // 2
                        Text {text: qsTr("Mirostat eta")}
                        Slider {
                            id: slider_mirostat_eta
                            from: 0
                            value: 0.1
                            to: 1
                        }
                        Text {text: slider_mirostat_eta.value.toPrecision(2)}
                    }
                    ColumnLayout{
                        width: parent.width * 0.5
                        RadioButton {
                            checked: true
                            text: qsTr(" no Mirostat")
                        }
                        RadioButton {
                            checked: false
                            text: qsTr("Mirostat v1")
                        }
                        RadioButton {
                            checked: false
                            text: qsTr("Mirostat v2")
                        }
                    }
                }
            }

            GroupBox { // params 5
                title: qsTr("Debug")
                width: parent.width-30
                anchors.horizontalCenter: parent.horizontalCenter
                ColumnLayout {
                    anchors.fill: parent
                    // reset
                    Button{
                        text: "Reset to default"
                    } // end of button reset
                    GridLayout {
                        columns: 3
                        anchors.margins: 10
                        // 数据类型
                        ComboBox{
                            id: send_type
                            model: ["text", "audio"]
                            currentIndex: 0
                        } // end of ComboBox
                        // A发送
                        Button{
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
                        } // end of button "Bot Send"
                        // B发送
                        Button{
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
                        } // end of button "human Send"
                    } // end of GridLayout contains 数据类型, A发送, B发送
                } // end of GridLayout contains reset button and GridLayout
            }
        }
    }

    Rectangle { // main ui
        id: main_ui
        anchors.fill: parent
        opacity: bMenuShown ? 0.5 : 1
        Behavior on opacity {
            NumberAnimation {
                duration: 300
            }
        }

        transform: Translate { //动画
            id: menuTranslate
            x: 0
            Behavior on x {
                NumberAnimation {
                    duration: 400;
                    easing.type: Easing.OutQuad
                }
            }
        }
        SplitView{
            anchors.fill: parent
            anchors.margins: 20
            orientation: Qt.Vertical

            Rectangle{ // box for title and menu button
                height: 50
                SplitView.minimumHeight: parent.height*0.05
                SplitView.maximumHeight: parent.height*0.05
                Button {
                    width: 140
                    height: 50
                    icon.source: "qrc:/image/title.png"
                    icon.height: Layout.preferredHeight
                    icon.width: Layout.preferredWidth
                    Layout.preferredHeight: parent.height * 0.2
                    Layout.preferredWidth: parent.width
                    Layout.row: 4
                    Layout.column: 0
                    Layout.columnSpan: 3
                    background: transientParent
                    onClicked: onMenu();
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
                id: input_and_button_box
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
                        // placeholderText: qsTr("Enter Message")
                        anchors.fill: transientParent
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
                    columns: 2
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    anchors.margins: 10

                    Button{ //清空
                        text: "Clear"
                        onClicked: {
                            text_area.clear();
                            talk_model.clearModel();
                        }
                    }

                    Button{ //chat
                        anchors.margins: 10
                        text: "Send Prompt"
                        onClicked: {
                            switch(send_type.currentText){
                            case "text":
                                if(true){ //M115
                                    if(text_area.text.length<1)
                                        return;
                                    talk_model.sendPrompt(text_area.text);
                                }break;
                            case "audio":
                                if(true){
                                    talk_model.appendAudio("B","B");
                                }break;
                            }
                        }
                    }

                    Button{ //chat
                        anchors.margins: 10
                        text: "test append"
                        onClicked: {
                            talk_model.appendTextStream(text_area.text);
                        }
                    }

                    
                    //end Button
                }
            }
        }
    }
}
