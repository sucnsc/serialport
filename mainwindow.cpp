#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial=new QSerialPort;
    this->setWindowTitle("serialport");
    portscan();
    connect(ui->Button_portscan,SIGNAL(clicked(bool)),this,SLOT(portscan()));  //扫描串口
    connect(ui->Button_portopen,SIGNAL(clicked(bool)),this,SLOT(connectport()));  //连接串口
    connect(ui->Button_send,SIGNAL(clicked(bool)),this,SLOT(send()));  //发送数据
    connect(serial,SIGNAL(readyRead()),this,SLOT(receive()));  //接收数据
    connect(ui->Button_clean,SIGNAL(clicked(bool)),this,SLOT(clean()));  //清空接收区
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::portscan()  //扫描串口
{
    ui->comboBox_portnum->clear();
    QList<QSerialPortInfo> coms=QSerialPortInfo::availablePorts();
    for (int i=0;i<coms.length();i++)
        ui->comboBox_portnum->addItem(coms.at(i).portName());
}

void MainWindow::connectport()  //连接串口
{
    int baudrate=ui->comboBox_baudrate->currentIndex();
    int databit=ui->comboBox_databit->currentIndex();
    int stopbit=ui->comboBox_stopbit->currentIndex();
    int cleckbit=ui->comboBox_checkbit->currentIndex();
    if(!serial->isOpen())
    {
        serial->setPortName(ui->comboBox_portnum->currentText());
        switch(baudrate)
        {
            case 0:serial->setBaudRate(QSerialPort::Baud1200); break;
            case 1:serial->setBaudRate(QSerialPort::Baud2400); break;
            case 2:serial->setBaudRate(QSerialPort::Baud4800); break;
            case 3:serial->setBaudRate(QSerialPort::Baud9600); break;
            case 4:serial->setBaudRate(QSerialPort::Baud19200); break;
            case 5:serial->setBaudRate(QSerialPort::Baud57600); break;
            case 6:serial->setBaudRate(QSerialPort::Baud115200); break;
            default:break;
        }
        switch(databit)
        {
            case 0:serial->setDataBits(QSerialPort::Data5); break;
            case 1:serial->setDataBits(QSerialPort::Data6); break;
            case 2:serial->setDataBits(QSerialPort::Data7); break;
            case 3:serial->setDataBits(QSerialPort::Data8); break;
            default:break;
        }
        switch(stopbit)
        {
            case 0:serial->setStopBits(QSerialPort::OneStop); break;
            case 1:serial->setStopBits(QSerialPort::OneAndHalfStop); break;
            case 2:serial->setStopBits(QSerialPort::TwoStop); break;
            default:break;
        }
        switch(cleckbit)
        {
            case 0:serial->setParity(QSerialPort::NoParity); break;
            case 1:serial->setParity(QSerialPort::OddParity); break;
            case 2:serial->setParity(QSerialPort::EvenParity); break;
            default:break;
        }
        if(!serial->open(QIODevice::ReadWrite))
            QMessageBox::information(this,"Error",serial->errorString());
        else
        {
            ui->comboBox_portnum->setEnabled(false);
            ui->comboBox_baudrate->setEnabled(false);
            ui->comboBox_databit->setEnabled(false);
            ui->comboBox_stopbit->setEnabled(false);
            ui->comboBox_checkbit->setEnabled(false);
            ui->Button_portopen->setText("断开连接");
        }
    }
    else
    {
        serial->close();
        ui->comboBox_portnum->setEnabled(true);
        ui->comboBox_baudrate->setEnabled(true);
        ui->comboBox_databit->setEnabled(true);
        ui->comboBox_stopbit->setEnabled(true);
        ui->comboBox_checkbit->setEnabled(true);
        ui->Button_portopen->setText("打开串口");
    }
}

void MainWindow::send()  //发送数据
{
    QString message=ui->textEdit->toPlainText();
    QByteArray mes;
    String2Hex(message,mes);
    if (ui->checkBox_send16->isChecked())
        serial->write(mes);
    else
        serial->write(message.toUtf8());
}

void MainWindow::receive()  //接受数据
{
    QByteArray message=serial->readAll();
    if (ui->checkBox_receive16->isChecked())
        ui->textBrowser->insertPlainText(message.toHex().toUpper()+" ");
    else
        ui->textBrowser->insertPlainText(QString(message)+" ");
}

void MainWindow::clean()  //清空接收区
{
    ui->textBrowser->clear();
}

/***************************************************************************************/
void MainWindow::String2Hex(QString str, QByteArray &senddata)
{
    int hexdata,lowhexdata;
        int hexdatalen = 0;
        int len = str.length();
        senddata.resize(len/2);
        char lstr,hstr;
        for(int i=0; i<len; )
        {
            //char lstr,
            hstr=str[i].toLatin1();
            if(hstr == ' ')
            {
                i++;
                continue;
            }
            i++;
            if(i >= len)
                break;
            lstr = str[i].toLatin1();
            hexdata = ConvertHexChar(hstr);
            lowhexdata = ConvertHexChar(lstr);
            if((hexdata == 16) || (lowhexdata == 16))
                break;
            else
                hexdata = hexdata*16+lowhexdata;
            i++;
            senddata[hexdatalen] = (char)hexdata;
            hexdatalen++;
        }
        senddata.resize(hexdatalen);
}

char MainWindow::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
            return ch-0x30;
        else if((ch >= 'A') && (ch <= 'F'))
            return ch-'A'+10;
        else if((ch >= 'a') && (ch <= 'f'))
            return ch-'a'+10;
        else return (-1);
}

