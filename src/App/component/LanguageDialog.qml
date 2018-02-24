import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3

import an.qt.LanguageSetting 1.0

Dialog {
    id: languageDialog;
    title: qsTr("<font size='4'>Language</font>");
    Material.background: appWnd.Material.primary;
    width: 265;
    height: 230;
    x: -width/2;
    y: -height/2;
    modal: true;
    focus: true;

    LanguageSetting{
        id: languageSetting;
    }

    standardButtons: Dialog.Ok | Dialog.Cancel;
    onAccepted: {
        languageSetting.laguageIndex = language.currentIndex;
        updateLanguage();
    }
    onRejected: {
        //when click Cancel button set shown language is current language
        language.currentIndex = languageSetting.laguageIndex;
        languageDialog.close();
    }

    Component.onCompleted: {
        //save language type to C++
        languageSetting.setLanguageType(0,LanguageSetting.CHINESE);
        languageSetting.setLanguageType(1,LanguageSetting.ENGLISH);
        language.model = languageSetting.languageList; //set ComboBox model
        language.currentIndex = 1;
        updateLanguage();
    }

    contentItem: ColumnLayout {
        spacing: 20;

        RowLayout {
            spacing: 10;

            Label {
                text: qsTr("Style:");
                font.pixelSize: 18;
            }

            ComboBox {
                id: language;
                Layout.fillWidth: true;
            }
        }
    }

    //This function will according to language currentIndex to update Window language
    function updateLanguage()
    {
        if(0 == language.currentIndex){
            languages.setLanguage(LanguageSetting.CHINESE);
        }
        else if(1 == language.currentIndex)
        {
            languages.setLanguage(LanguageSetting.ENGLISH);
        }
    }
}
