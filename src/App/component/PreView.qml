import QtQuick 2.7
import QtQuick.Controls.Material 2.3

import an.qt.CModel 1.0

import "../scripts/AddTarget.js" as AddTarget

Item {
    visible: true;
    anchors.centerIn: parent;
    clip: true;

    Rectangle{                      // 画缩略图背景框
        anchors.fill: parent;
        border.width: 1;
        border.color: Material.foreground;
        color: Material.background;
    }

    Canvas{
        id: preViewCanvas;          // 画缩略图
        anchors.centerIn: parent;
        anchors.margins: 1;
        width: 1280;
        height: 720;
        clip: true;
        contextType: "2d";
        scale: 0.4;                 // canvas缩放系数

        onPaint: {
            AddTarget.drawShape( preViewCanvas.context,
                                 elementList,
                                 selected,
                                 0,
                                 0,
                                 1 );
        }
    }

    Rectangle{
        id: rectBox;
        x: 1;
        y: 1;
        width: 512;
        height: 288;
        border.width: 1;
        border.color: Material.accent;
        color: "transparent";
    }

    // 重新渲染
    function renderTargets(){
        preViewCanvas.context.clearRect(0,0,1280,720);
        AddTarget.drawShape( preViewCanvas.context,
                             elementList,
                             selected,
                             0,
                             0,
                             1 );
        preViewCanvas.requestPaint();
    }

    // 重新画缩略图预览框
    function renderRectBox(){
        if( elementScale < 1 ){
            rectBox.x = 1;
            rectBox.y = 1;
            rectBox.width = 512;
            rectBox.height = 288;
        }
        else{
            rectBox.x = 1-xOffset/elementScale*0.4;
            rectBox.y = 1-yOffset/elementScale*0.4;
            rectBox.width = 512/elementScale;
            rectBox.height = 288/elementScale;
        }
    }
}
