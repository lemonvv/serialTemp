#include "widget.h"
#include "ui_widget.h"

#define SERIAL_TIME 1000

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle(tr("Test V1.0.0"));
    serchtimer = new QTimer(this);   //扫描串口定时器
    rxtimer = new QTimer(this); //接收数据定时器

    mserial = new QSerialPort(this);//创建串口对象

    connect(serchtimer, SIGNAL(timeout()), this, SLOT(timeUpSerial()));  //连接串口扫描函数
    connect(rxtimer,SIGNAL(timeout()),this,SLOT(rxtimeSearchData()));    //连接数据接收函数

    //连接串口异常错误槽函数
    connect(mserial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),  this, &Widget::handleSerialError);

    //串口数据接收定时器配置
    rxtimer->setSingleShot(true);   //单次触发，超时后只触发一次
    rxtimer->setInterval(0);        //超时时间间隔

    serchtimer->start(SERIAL_TIME);    //串口没打开时，设置每隔2秒扫描一次可用串口

    searchSerial();//串口扫描函数
    show_Widgets();
}

Widget::~Widget()
{
    if((ui->pushButton_openSerial->text()==tr("关闭")))
    {
        mserial->close();
    }
    delete ui;
}

//扫描串口函数
void Widget::searchSerial()
{
    static int count = 0;
    QStringList mserialName;
    foreach (const QSerialPortInfo &Info, QSerialPortInfo::availablePorts())
    {
        //if(Info.serialNumber())
        mserialName << Info.portName();

    }
    if(mserialName.length() > 0)
    {
        if(count != mserialName.length())
        {
            count = mserialName.length();
            ui->comboBox_serialPort->clear();
            ui->comboBox_serialPort->addItems(mserialName);  //显示串口号
            //qDebug()<<mserialName.length()<<endl;
        }
    }
}

//串口打开后意外中断时弹出错误警告
void Widget::handleSerialError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError)
    {
        mserial->close();
        QMessageBox::critical(this, tr("Error"), "串口连接中断，请检查是否正确连接！");

        serchtimer->start(SERIAL_TIME);
        ui->pushButton_openSerial->setText("打开");

        show_Widgets();
    }
}

//超时后扫描串口
void Widget::timeUpSerial()
{

    if((ui->pushButton_openSerial->text()==tr("打开")))
    {
        searchSerial();
    }
}

//串口数据接收rxSerialbuf
void Widget::recv_data(void)
{
    rxtimer->start(3); //50ms超时时间，根据实际来调整超时时间

    rxSerialbuf.append(mserial->readAll()); //触发函数后把缓冲区数据全部接收到rxSerialbuf
}

//超时后显示数据，使用定时器机制，可避免接收中文出现个别字符乱码
void Widget::rxtimeSearchData(void)
{
    rxtimer->stop();
    if(!rxSerialbuf.isEmpty())
    {
        QString myStrTemp = QString::fromLocal8Bit(rxSerialbuf); //支持中文显示

#if 0
        QString str = ui->textBrowser_serialRxBuf->toPlainText();
        str+=myStrTemp;

        ui->textBrowser_serialRxBuf->clear();
        ui->textBrowser_serialRxBuf->append(str);
#else
        ui->textBrowser_serialRxBuf->append(myStrTemp);
#endif
        ui->textBrowser_serialRxBuf->moveCursor(QTextCursor::End);

    }
    rxSerialbuf.clear();
}

//串口打开按钮
void Widget::on_pushButton_openSerial_clicked()
{
    if((ui->pushButton_openSerial->text()==tr("打开")))
    {
        ui->pushButton_openSerial->setText("关闭");
        mserial->setPortName(ui->comboBox_serialPort->currentText());
        if(mserial->open(QIODevice::ReadWrite))
        {

            mserial->open(QIODevice::ReadWrite);
            mserial->setBaudRate(ui->comboBox_serialBaud->currentText().toInt());
            mserial->setDataBits(QSerialPort::Data8);
            mserial->setParity(QSerialPort::NoParity);
            mserial->setFlowControl(QSerialPort::NoFlowControl);
            mserial->setStopBits(QSerialPort::OneStop);
            connect(mserial,SIGNAL(readyRead()),this,SLOT(recv_data()));
            serchtimer->stop();
                //QString str =
            ui->textBrowser_serialRxBuf->append("打开 "+ui->comboBox_serialPort->currentText()+" 成功");
            ui->textBrowser_serialRxBuf->moveCursor(QTextCursor::End);
        }
        else
        {
            mserial->close();
            serchtimer->start(SERIAL_TIME);
            ui->pushButton_openSerial->setText("打开");
            ui->textBrowser_serialRxBuf->append("打开 "+ui->comboBox_serialPort->currentText()+"错误");
            ui->textBrowser_serialRxBuf->moveCursor(QTextCursor::End);
            QMessageBox::warning(nullptr,"Warning","串口打开错误",QMessageBox::Close);
        }

    }
    else
    {
        ui->pushButton_openSerial->setText("打开");

        mserial->close();

        serchtimer->start(SERIAL_TIME);
        ui->textBrowser_serialRxBuf->append("关闭 "+ui->comboBox_serialPort->currentText());
        ui->textBrowser_serialRxBuf->moveCursor(QTextCursor::End);

    }
    show_Widgets();
}

//清除缓冲区内容
void Widget::on_pushButton_cleanRecv_clicked()
{
    ui->textBrowser_serialRxBuf->clear();
}

//控件使能
void Widget::show_Widgets()
{
    /*串口关闭*/
    if(ui->pushButton_openSerial->text()==tr("打开"))
    {
        ui->comboBox_serialBaud->setEnabled(true);
        ui->comboBox_serialPort->setEnabled(true);
    }
    else
    {
        ui->comboBox_serialBaud->setEnabled(false);
        ui->comboBox_serialPort->setEnabled(false);
    }


}


