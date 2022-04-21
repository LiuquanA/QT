#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QtWebEngineWidgets>

widget::widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget)
{
    ui->setupUi(this);
    mIsOpen = false;

    //识别当前系统的有效串口号
    QList<QSerialPortInfo> serialPortInfo = QSerialPortInfo::availablePorts();
    int count = serialPortInfo.count();
    for (int i=0;i<count;i++){
        ui->CboxSerialport->addItem(serialPortInfo.at(i).portName());   //循环将可用串口号添加到  选择下拉栏
    }
    connect(&mSerialPort, SIGNAL(readyRead()), this, SLOT(on_SerialPort_readyRead()));

    webview = new QWebEngineView(this);
    webview->setGeometry(400,20,750,550);

    QWebEnginePage *page = webview->page();
    QString exepath = QCoreApplication::applicationDirPath();
    QString mappathA = "file:///" + exepath + "/baiduMap.html";//百度地图htlm文件
    qDebug()<<exepath;
    page->load(QUrl(("file:///C:/AUserLiu/SourceCode/QT/BD_MAP_/baiduMap.html")));
    //page->load(QUrl(("file:///baiduMap.html")));
    page->load(QUrl((mappathA)));


}




widget::~widget()
{
    delete ui;
}


void widget::on_ButtonSwitch_clicked()   //开关
{
    if(true == mIsOpen)
    {
        //当前串口助手已经打开了一个串口，这时应该要执行关闭的动作
        mSerialPort.close();
        ui->ButtonSwitch->setText(QString::fromLocal8Bit("打开"));
        mIsOpen = false;

        ui->Button_Send->setEnabled(mIsOpen);
        ui->CboxSerialport->setEnabled(true);
    }
    else
    {
        //当前串口助手没有打开串口，这时要执行打开串口的动作
        if(true == getSerialPortConfig())
        {
            mIsOpen = true;
            ui->ButtonSwitch->setText(QString::fromLocal8Bit("关闭"));
            ui->Button_Send->setEnabled(mIsOpen);
            ui->CboxSerialport->setEnabled(false);

        }
//        else
//        {
//            mIsOpen = false;
//        }
    }

}

bool widget::nativeEvent(const QByteArray& eventType, void* message, long* result)
{

    MSG* msg = reinterpret_cast<MSG*>(message);
    if (msg->message == WM_DEVICECHANGE)//发生硬件热插拔事件，更新可用串口号
    {
        //识别当前系统的有效串口号
        QList<QSerialPortInfo> serialPortInfo = QSerialPortInfo::availablePorts();
        int count = serialPortInfo.count();
        for (int i=0;i<count;i++){
            ui->CboxSerialport->addItem(serialPortInfo.at(i).portName());   //循环将可用串口号添加到  选择下拉栏
        }
    }
    return false;
}


bool widget::getSerialPortConfig(void)
{
     //获取串口配置
    mPortName = ui->CboxSerialport->currentText();

    mSerialPort.setPortName(mPortName);//端口号

    //以下均为串口配置
    //以下配置不可更改
    //默认波特率115200，无校验位，字长8，一个停止位
    mSerialPort.setBaudRate(QSerialPort::Baud115200);
    mSerialPort.setParity(QSerialPort::NoParity);
    mSerialPort.setDataBits(QSerialPort::Data8);
    mSerialPort.setStopBits(QSerialPort::OneStop);

     return mSerialPort.open(QSerialPort::ReadWrite);

}

//成都航空职业技术学院博学楼
//经度：104.24264063829423
//纬度：30.528206585495298
void widget::on_Button_Send_clicked()       //发送串口数据
{

    std::string strflag1 = "JD:" ;
    std::string strflag2 = "WD:" ;
    std::string strflag3 = ".JWDend\r\n";
    if(true == mIsOpen)
    {
        std::string JdSendData = ui->textEditSend_JD->toPlainText().toStdString();
        std::string WdSendData = ui->textEditSend_WD->toPlainText().toStdString();
        std::string JWdSendData = "JWD_start:" ;

        //合并需要通过串口发生的字符串
        JWdSendData+=strflag1;
        JWdSendData+=JdSendData;
        JWdSendData+=strflag2;
        JWdSendData+=WdSendData;
        JWdSendData+=strflag3;

        mSerialPort.write(JWdSendData.c_str());

    }
}


void widget::on_SerialPort_readyRead()
{
    if(true == mIsOpen)
    {

        std::string JWdRecvDataJD ;
        std::string JWdRecvDataWD ;
        QString recvDataALL = mSerialPort.readAll();
        std::string JWdRecvDataALL = recvDataALL.toStdString();

        if(JWdRecvDataALL[0] == 'J'&&JWdRecvDataALL[1] =='W'&&JWdRecvDataALL[2]=='D'&&JWdRecvDataALL[3]=='_'&&JWdRecvDataALL[4]=='s')
        {
            int JDNum_start = 0;
            int WDNum_start = 0;
            int JDNum_end = 0;
            int WDNum_end = 0;
            bool JDNum_flag = false;
            bool WDNum_flag = false;
            bool ENDNum_flag = false;
            for(int i=9;i<100;i++){
                if(JWdRecvDataALL[i]=='J'&&JWdRecvDataALL[i+1]=='D'&&JWdRecvDataALL[i+2]==':')
                {
                    JDNum_start = i+3;
                    JDNum_flag = true;
                }
                if(JWdRecvDataALL[i]=='W'&&JWdRecvDataALL[i+1]=='D'&&JWdRecvDataALL[i+2]==':')
                {
                    WDNum_start = i+3;
                    JDNum_end = i-1;
                    WDNum_flag = true;

                }
                if(JWdRecvDataALL[i]=='.'&&JWdRecvDataALL[i+1]=='J'&&JWdRecvDataALL[i+2]=='W'&&JWdRecvDataALL[i+3]=='D'&&JWdRecvDataALL[i+4]=='e')
                {
                    WDNum_end = i-1;
                    ENDNum_flag = true;
                }
                if(JDNum_flag && WDNum_flag && ENDNum_flag)
                {
                     JWdRecvDataJD = JWdRecvDataALL.substr(JDNum_start,JDNum_end-JDNum_start+1);
                     JWdRecvDataWD = JWdRecvDataALL.substr(WDNum_start,WDNum_end-WDNum_start+1);
                     ui->textEdiReceive_JD->clear();
                     ui->textEdiReceive_WD->clear();
                    break;
                }

            }
        }

        QString recvDataJD = QString::fromStdString(JWdRecvDataJD);
        QString recvDataWD = QString::fromStdString(JWdRecvDataWD);

        ui->textEdiReceive_JD->append(QString(recvDataJD));
        ui->textEdiReceive_WD->append(QString(recvDataWD));

        QString Lng = ui->textEdiReceive_JD->toPlainText();//获取经度
        QString Lat = ui->textEdiReceive_WD->toPlainText();//获取纬度
        QWebEnginePage *page = webview->page();
        QString command = QString("movepoint(%1,%2)").arg(Lng).arg(Lat);
        page->runJavaScript(command);
    }
}
