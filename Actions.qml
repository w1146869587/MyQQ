﻿import QtQuick 2.11
import QtQuick.Controls 2.5

Rectangle {
    property alias setAct: setAction
    property alias okCancelAct: okCancelAction
    property alias accountRAct: accountRAct
    property alias passwdRAct: passwdRAct
    property alias registerAct: registerAction //打开注册界面
    property alias myQQRegAct: myQQRegAction //注册界面注册
    property alias immediateLogAct: immediateLoginAction //注册的立即登录
    property alias loginAct: loginAction //登录
    property alias nodebookAct: nodebookAction //记录号码

    property alias mesWinForOkAct: mesWinForOkAction //切换账号接收键
    property alias alterCoverAct: alterCoverAction //更改封面界面打开

    property alias mainMenuAct: mainMenuAction //底部菜单
    property alias addFriendsAct: addFriendsAction //添加好友按钮
    property alias showWeatherAct: showWeatherAction //露出天气界面
    property alias hideWeatherAct: hideWeatherAction //hide天气界面
    property alias updgradeAct: updgradeAction //升级按钮
    property alias aboutAct: aboutAction //打开关于窗口
    property alias switchAct: switchAction //切换账号按钮
    property alias headImgAct: headImgAction //头像悬浮时弹出界面
    property alias openAlterHImgAct: openAlterHImgAction //更改头像
    property alias openAlterUserInfoAct: openAlterUserInfoAction //用户资料修改

    //登录界面Actions
    Action {
        //设置点击动作
        id: setAction
        onTriggered: {
            inCenterLoader.item.loginInf.setAn.start()
            inCenterLoader.item.loginSetInf.ok.forceActiveFocus()
        }
    }

    Action {
        //设置界面确定与取消动作
        id: okCancelAction
        onTriggered: {
            inCenterLoader.item.loginSetInf.okCancelAn.start()
            inCenterLoader.item.loginInf.account.edit.forceActiveFocus()
        }
    }

    Action {
        id: accountRAct
        onTriggered: {
            if (inCenterLoader.item.loginInf.account.isTopArrow) {
                inCenterLoader.item.loginInf.account.isTopArrow = false
            } else {
                inCenterLoader.item.loginInf.account.btnR.hoverEnabled = false
                inCenterLoader.item.loginInf.account.underL.hoverEnabled = false
                inCenterLoader.item.loginInf.account.isTopArrow = true
                inCenterLoader.item.loginInf.account.iconL = "qrc:/images/qq小图标.png"
                inCenterLoader.item.loginInf.account.underL.color = "#ff12b7f5"
            }
        }
    }

    Action {
        id: passwdRAct
        onTriggered: {

            //调用虚拟键盘


            /*
 keyboard.visible = !keyboard.visible
            loginInf.passwd.focus = true
            */
        }
    }

    Action {
        //弹出注册界面
        id: registerAction
        onTriggered: {
            mainWin.inf = Math.pow(2, 1) //标记为注册界面

            func.mainWinReSize(mainWin.x - ((850 - 495) / 2),
                               mainWin.y - ((500 - 470) / 2), 850, 500)
            console.log("status", inCenterLoader.status)
            inCenterLoader.sourceComponent = undefined
            inCenterLoader.source = "./register/RegisterInterface.qml"
        }
    }

    //注册界面Actions
    Action {
        //注册界面的注册动作
        id: myQQRegAction
        onTriggered: {
            if (!inCenterLoader.item.hasClicked) {
                funcc.registerMyQQ(inCenterLoader.item.name,
                                   inCenterLoader.item.passwd)
                inCenterLoader.item.hasClicked = true
            }
        }
    }

    Action {
        //注册界面的笔记本按钮，用于记录号码信息
        id: nodebookAction
        onTriggered: {
            console.log("nodebook trigger")
            var str = "恭喜你,你申请的MyQQ号码是:" + inCenterLoader.item.myqq
            var title = "保存MyQQ号码"
            var dir = "../MyQQ号码_" + inCenterLoader.item.myqq + ".txt"
            if (funcc.saveStringToTxt(str, title, dir))
                console.log("saving myqq")
        }
    }

    Action {
        //注册界面的立即登录动作
        id: immediateLoginAction
        onTriggered: {
            mainWin.visible = false
            mainWin.inf = Math.pow(2, 0) //标记为登录界面
            inCenterLoader.source = ''
            func.mainWinReSize(mainWin.x + ((850 - 495) / 2),
                               mainWin.y + ((500 - 470) / 2), 495, 470)
            inCenterLoader.sourceComponent = comptLogin
            mainWin.visible = true
        }
    }

    Action {
        //登录界面的登录动作
        property bool isClicked: false
        id: loginAction
        onTriggered: {
            if (!isClicked) {
                isClicked = true
                var account = inCenterLoader.item.loginInf.account.edit.text
                var passwd = inCenterLoader.item.loginInf.passwd.edit.text
                time60sForLogin.restart()
                funcc.login(account, passwd)
                console.log("account and passwd:", account, passwd)
                inCenterLoader.item.loginInf.loginText = "登录中..."
            }
        }
    }
    Connections {
        //登录结果处理
        target: funcc
        onLoginResult: {
            inCenterLoader.item.loginInf.loginText = "登录"
            if (result === 1) {
                inCenterLoader.item.loginInf.loginTip.text = qsTr("没有这个账号！")
                inCenterLoader.item.loginInf.loginTip.visible = true
            } else if (result === 2) {
                inCenterLoader.item.loginInf.loginTip.text = qsTr("密码错误！")
                inCenterLoader.item.loginInf.loginTip.visible = true
            } else if (result === 3) {
                inCenterLoader.item.loginInf.loginTip.text = qsTr("登录失败！")
                inCenterLoader.item.loginInf.loginTip.visible = true
            } else if (result === 0) {
                mainWin.inf = Math.pow(2, 2) //标记为main界面
                inCenterLoader.sourceComponent = undefined
                inCenterLoader.source = "./main/mainInterface.qml"
                mainWin.hide()
                //从组件到原文件的加载会导致不能对其设置属性，inCenterLoader.status一直等于Loader.loading
                //故只能在qqMain内部想方法,设置其坐标
                // mainWin.adjustCoordination()
                //建议加载器的加载方式最好一致，否则很容易出现其属性null的情况
            }

            time60sForLogin.stop()
            loginAction.isClicked = false
        }
    }

    //主界面的Actions
    Action {
        id: mainMenuAction
        onTriggered: {
            inCenterLoader.item.menuPop.visible = true
        }
    }

    //天气界面显示
    Action {
        id: showWeatherAction
        onTriggered: {
            var main = inCenterLoader.item
            if (main.weatherCanShow !== 2)
                return
            if (main.loaderForWeather.status === Loader.Null) {

                main.loaderForWeather.source = "qrc:/main/WeatherWin.qml"
            }
            while (true) {
                if (main.loaderForWeather.status === Loader.Ready) {
                    if (main.loaderForWeather.item.win === 2)
                        return
                    //0 1 2 3;分别代表任务栏位置位于下、上、右、左
                    //注意qml与qt的控件坐标系是不同的，qml是桌面原点为起点，qt是AvailableDesk原点为起点
                    var posx = main.x, w = main.width, weatherWidth = main.loaderForWeather.item.width
                    switch (funcc.taskDirection()) {
                    case 3:
                        if ((posx + w + weatherWidth - funcc.taskBarWidth(
                                 )) < mainWin.desktopAvailableWidth) {
                            main.loaderForWeather.item.x = posx + w
                        } else {
                            main.loaderForWeather.item.x = posx - weatherWidth
                        }
                        break
                    default:
                        if ((posx + w + weatherWidth) < mainWin.desktopAvailableWidth) {
                            main.loaderForWeather.item.x = posx + w
                        } else {
                            main.loaderForWeather.item.x = posx - weatherWidth
                        }
                        break
                    }
                    main.loaderForWeather.item.y = main.y - main.enableSize
                    main.loaderForWeather.item.show()
                    break
                } else if (main.loaderForWeather.status === Loader.Error) {
                    console.log("Action.qml curred error in line 165")
                    break
                }
            }
        }
    }

    //天气界面隐藏
    Action {
        id: hideWeatherAction
        onTriggered: {
            if (inCenterLoader.item.weatherCanShow !== 2)
                return
            time2sForHideWeather.restart()
        }
    }

    //qqmainwin的添加好友动作
    Action {
        id: addFriendsAction
        onTriggered: {
            var obj = {}
            obj["myqq"] = inCenterLoader.item.myqq
            obj["sex"] = inCenterLoader.item.sex
            obj["所在地"] = inCenterLoader.item.where
            obj["故乡"] = inCenterLoader.item.townmans
            funcc.startAddFriendsProcess(inCenterLoader.item, obj)
        }
    }

    //qqmainwin的升级动作
    Action {
        id: updgradeAction
        onTriggered: {
            funcc.openTempMesWin() //打开一个提示窗，位置中心
        }
    }

    //qqmainwin的关于动作
    Action {
        id: aboutAction
        onTriggered: {
            console.log("about c->")
            if (funcc.win)
                //如果已经有占用win资源，返回
                return
            if (!(inCenterLoader.item.loaderForAbout.status === Loader.Ready)) {
                console.log("about is excuted->")
                inCenterLoader.item.loaderForAbout.source = "qrc:/main/AboutMyQQ.qml"
            }
            while (true) {
                if (inCenterLoader.item.loaderForAbout.status === Loader.Ready) {
                    inCenterLoader.item.loaderForAbout.item.x = inCenterLoader.item.x
                            + (inCenterLoader.item.width
                               - inCenterLoader.item.loaderForAbout.item.width) / 2
                    inCenterLoader.item.loaderForAbout.item.y = inCenterLoader.item.y
                            + (inCenterLoader.item.height
                               - inCenterLoader.item.loaderForAbout.item.height) / 2
                    funcc.win = inCenterLoader.item.loaderForAbout.item
                    funcc.sourceIco = ":/images/QQTray.png"
                    console.log("about show")
                    inCenterLoader.item.loaderForAbout.item.show()
                    funcc.win = null
                    break
                } else if (inCenterLoader.item.loaderForAbout.status === Loader.Error) {
                    console.log("Action.qml curred error in line 230")
                    break
                }
            }
        }
    }

    //qqmainwin的切换账号动作
    Action {
        id: switchAction
        onTriggered: {
            if (!(Loader.Ready === inCenterLoader.item.loaderForMesWin.status)) {
                inCenterLoader.item.loaderForMesWin.source = "qrc:/main/MessageDialogForSwitch.qml"
            }
            while (true) {
                if (inCenterLoader.item.loaderForMesWin.status === Loader.Ready) {
                    inCenterLoader.item.loaderForMesWin.item.x = inCenterLoader.item.x
                            + (inCenterLoader.item.width
                               - inCenterLoader.item.loaderForMesWin.item.width) / 2
                    inCenterLoader.item.loaderForMesWin.item.y = inCenterLoader.item.y
                            + (inCenterLoader.item.height
                               - inCenterLoader.item.loaderForMesWin.item.height) / 2
                    inCenterLoader.item.loaderForMesWin.item.show()
                    break
                } else if (inCenterLoader.item.loaderForMesWin.status === Loader.Error) {
                    console.log("MessageDialogForSwitch.qml curred error in line 260")
                    break
                }
            }
        }
    }
    //用户头像界面显示
    Action {
        id: headImgAction
        onTriggered: {
            if (inCenterLoader.item.loaderForHeadImg === undefined)
                return
            var loader = inCenterLoader.item.loaderForHeadImg
            if (!(Loader.Ready === loader.status)) {
                loader.source = "qrc:/main/HeadInf.qml"
            }
            while (true) {
                if (loader.status === Loader.Ready) {
                    console.log("headinterface show")
                    var main = inCenterLoader.item
                    var posx = main.x, w = main.width, headWidth = loader.item.width
                    switch (funcc.taskDirection()) {
                    case 3:
                        if ((posx + w + headWidth - funcc.taskBarWidth(
                                 )) < mainWin.desktopAvailableWidth) {
                            loader.item.x = posx + w
                        } else {
                            loader.item.x = posx - headWidth
                        }
                        break
                    default:
                        if ((posx + w + headWidth) < mainWin.desktopAvailableWidth) {
                            loader.item.x = posx + w
                        } else {
                            loader.item.x = posx - headWidth
                        }
                        break
                    }
                    loader.item.y = main.headImgY //和头像同高
                    loader.item.name = main.name
                    loader.item.signature = main.signature
                    loader.item.grade = 122 //main.grade
                    loader.item.headImgHovered = true //初始化悬浮在头像
                    loader.item.model.clear() //重置模型数据
                    var crown = parseInt(
                                loader.item.grade / 64), cur //JS除法默认为浮点数运算，需显示认定为整型
                    cur = parseInt(loader.item.grade - crown * 64)
                    var sun = parseInt(cur / 16) //显示认定为整型
                    cur = cur - sun * 16
                    var moon = parseInt(cur / 4)
                    var star = parseInt(cur - moon * 4)
                    console.log(crown, sun, moon, star)
                    //添加皇冠图像路径
                    for (var i = 0; i < crown; i++) {
                        loader.item.model.append({
                                                     "img": "qrc:/images/mainInterface/crown.png"
                                                 })
                    }
                    for (i = 0; i < sun; i++) {
                        loader.item.model.append({
                                                     "img": "qrc:/images/mainInterface/sun.png"
                                                 })
                    }
                    for (i = 0; i < moon; i++) {
                        loader.item.model.append({
                                                     "img": "qrc:/images/mainInterface/moon.png"
                                                 })
                    }
                    for (i = 0; i < star; i++) {
                        loader.item.model.append({
                                                     "img": "qrc:/images/mainInterface/star.png"
                                                 })
                    }
                    loader.item.show()
                    break
                } else if (loader.status === Loader.Error) {
                    console.log("MessageDialogForSwitch.qml curred error in line 331")
                    break
                }
            }
        }
    }

    Action {
        id: openAlterHImgAction
        onTriggered: {
            console.log("open?")
            if (inCenterLoader.item.loaderForAlterHImg === undefined)
                return
            var loader = inCenterLoader.item.loaderForAlterHImg
            if (!(Loader.Ready === loader.status)) {
                console.log("loaded")
                loader.source = "qrc:/main/AlterHImg.qml"
            }
            while (true) {
                if (loader.status === Loader.Ready) {
                    console.log("star show")
                    console.log("opened the AlterHImg")
                    loader.item.x = (mainWin.desktopAvailableWidth - loader.item.width) / 2
                    loader.item.y = (mainWin.desktopAvailableHeight - loader.item.height) / 2
                    funcc.addHeadWidget(
                                loader.item, 19, 84, images.findPixmap(
                                    inCenterLoader.item.myqq + "101"),
                                inCenterLoader.item.myqq,
                                inCenterLoader.item.histroyImgModel.isgot)
                    loader.item.show()
                    loader.item.raise()
                    loader.item.requestActivate()
                    break
                } else if (loader.status === Loader.Error) {
                    console.log("AlterHImg.qml curred error in line 412")
                    break
                }
            }
        }
    }
    Action {
        id: openAlterUserInfoAction
        onTriggered: {
            console.log("openAlterUserInfoAction")
            if (inCenterLoader.item.loaderForAlterInfo === undefined)
                return
            var loader = inCenterLoader.item.loaderForAlterInfo
            if (!(Loader.Ready === loader.status)) {
                console.log("loaded")
                loader.source = "qrc:/main/IndividualData.qml"
                while (true) {
                    if (loader.status === Loader.Ready) {
                        funcc.getIndividualData() //远程获取数据，初始化界面
                        console.log("start show")
                        console.log("opened the IndividualData.qml")
                        loader.item.x = (mainWin.desktopAvailableWidth - loader.item.width) / 2
                        loader.item.y = (mainWin.desktopAvailableHeight - loader.item.height) / 2

                        loader.item.showNormal()
                        loader.item.raise()
                        loader.item.requestActivate()
                        break
                    } else if (loader.status === Loader.Error) {
                        console.log("loaderForAlterInfo  occured a error")
                        break
                    }
                }
            } else {
                //显示窗口
                //  loader.item.x = (mainWin.desktopAvailableWidth - loader.item.width) / 2
                // loader.item.y = (mainWin.desktopAvailableHeight - loader.item.height) / 2
                loader.item.opacity = 1.0
                loader.item.showNormal()
                loader.item.raise()
                loader.item.requestActivate()
            }
        }
    }
    //附属于主界面窗口的Actions
    Action {
        //切换账号窗口的确认动作
        id: mesWinForOkAction
        onTriggered: {
            mainWin.inf = Math.pow(2, 0)
            //inCenterLoader.item.hide()
            inCenterLoader.source = ""
            func.mainWinReSize(mainWin.x + ((850 - 495) / 2),
                               mainWin.y + ((500 - 470) / 2), 495, 470)
            inCenterLoader.sourceComponent = comptLogin
            mainWin.show()
            mainWin.raise()
            mainWin.requestActivate()
            funcc.deleteNetTimer() //删除网络监测器
        }
    }
    Action {
        //打开更改封面界面
        id: alterCoverAction
        onTriggered: {
            console.log("alterCoverAction")
            var loader = inCenterLoader.item.loaderForAlterInfo.item.loaderForAlterCover
            if (loader === undefined)
                return
            if (!(Loader.Ready === loader.status)) {
                console.log("loaded")
                loader.source = "qrc:/main/AlterCover.qml"
            }
            while (true) {
                if (loader.status === Loader.Ready) {
                    console.log("start show")
                    console.log("first,opened the AlterCover.qml")
                    loader.item.x = (mainWin.desktopAvailableWidth - loader.item.width) / 2
                    loader.item.y = (mainWin.desktopAvailableHeight - loader.item.height) / 2
                    funcc.addCoverWidget(loader.item, 35, 60, source.file)
                    loader.item.show()
                    loader.item.raise()
                    loader.item.requestActivate()
                    break
                } else if (loader.status === Loader.Error) {
                    console.log("loaderForAlterInfo  occured a error")
                    break
                }
            }
        }
    }

    //辅助功能计时器
    Timer {
        id: time2sForHideWeather
        interval: 2000
        onTriggered: {
            if (!inCenterLoader.item.loaderForWeather.item.isMouseInside
                    && inCenterLoader.item.loaderForWeather.item.win === 1
                    && !inCenterLoader.item.loaderForWeather.item.popIsOpen)
                inCenterLoader.item.loaderForWeather.item.hide()
        }
    }
    Timer {
        id: time60sForLogin
        interval: 60000
        repeat: false
        onTriggered: {
            if (loginAction.isClicked)
                funcc.loginResult(3)
        }
    }
}
