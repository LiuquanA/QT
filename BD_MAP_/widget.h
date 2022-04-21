#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <Qlist.h>
#include <qt_windows.h>
#include <QWebEngineView>


namespace Ui {
class widget;
}

class widget : public QWidget
{
    Q_OBJECT

public:
    explicit widget(QWidget *parent = 0);
    ~widget();

public:
    bool getSerialPortConfig(void);
    bool widget::nativeEvent(const QByteArray& eventType, void* message, long* result);
private slots:

    void on_ButtonSwitch_clicked();

    void on_Button_Send_clicked();

    void on_SerialPort_readyRead();


private:
    Ui::widget *ui;
    QSerialPort mSerialPort;

    QWebEngineView *webview;
    QString exepath;

    bool mIsOpen;   //串口打开状态

    QString mPortName;
};

#endif // WIDGET_H
