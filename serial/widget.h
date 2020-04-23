#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "QDebug"
#include "QtSerialPort/QSerialPort"
#include "QtSerialPort/QSerialPortInfo"
#include "QTimer"
#include <QMessageBox>
#include <QDateTime>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void searchSerial();
    void show_Widgets();
private slots:
    void on_pushButton_openSerial_clicked();

    void on_pushButton_cleanRecv_clicked();

    void handleSerialError(QSerialPort::SerialPortError error);

    void timeUpSerial();

    void recv_data(void);

    void rxtimeSearchData(void);

private:
    Ui::Widget *ui;
    QSerialPort *mserial;
    QTimer *serchtimer;
    QTimer *rxtimer;
    QByteArray rxSerialbuf;
};

#endif // WIDGET_H
