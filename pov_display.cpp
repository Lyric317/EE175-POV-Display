#include "pov_display.h"
#include "ui_pov_display.h"
#include "QDebug"
#include "QMessageBox"
#include <QTimer>

POV_Display::POV_Display(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::POV_Display)
{
    ui->setupUi(this);
    mode = "*0";
    str = "Hello World";
    timer = new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(displaytimerUpDate()));//关联定时器计满信号和相应的槽函数
    initSerialPort();
}

POV_Display::~POV_Display()
{
    deviceStop();
    shutdown();
    delete ui;
}

//initialize bluretooth module serial ports
void POV_Display::initSerialPort()
{
    connect(&MotorSerial,SIGNAL(readyRead()),this,SLOT(recSerialData()));   //set serial port connection
    connect(&LEDCtrSerial,SIGNAL(readyRead()),this,SLOT(recSerialData()));
    //obtain all sriel ports and add then to displaycomboBox
    QList<QSerialPortInfo>  infos = QSerialPortInfo::availablePorts();
    if(infos.isEmpty())
    {
        //if no availiable port, use a massage box to inform user
        ui->displayComboBox->addItem("No port");
        return;
    }
    foreach (QSerialPortInfo info, infos) {
        ui->displayComboBox->addItem(info.portName());
        ui->dcmComboBox->addItem(info.portName());
        //qDebug() << info.portName(); //debug code, show availiable ports
    }
}

void POV_Display::on_displayComboBox_currentIndexChanged(const QString &arg1)
{
    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    int i = 0;
    //find the port of user choice
    foreach (disBTinfo, infos) {
        if(disBTinfo.portName() == arg1) break;
        i++;
        }
    if(i != infos.size ()){//can find
        LEDCtrSerial.setPort(disBTinfo);
    }else{
            LEDCtrSerial.close();
            QMessageBox::warning (this,tr("Warning"),tr("Failed to open this port"));
    }
}

void POV_Display::on_StartPushButton_clicked()
{
    int j;
    QString tl = ui->TimerSpinBox->text();
    tleft = tl.toInt();
    qDebug()<<disBTinfo.portName();
    //open serial ports
    LEDCtrSerial.setPort(disBTinfo);
    LEDCtrSerial.open(QIODevice::ReadWrite);
    LEDCtrSerial.setBaudRate(QSerialPort::Baud9600);
    //send infomation to LED control Arduino
    LEDCtrSerial.write("*");//Start of data sent to Led control
    LEDCtrSerial.write(mode.toUtf8());//send mode info
    LEDCtrSerial.write("#");//start of left time info
    LEDCtrSerial.write(tl.toUtf8());// send left time
    LEDCtrSerial.write("$");//end of left time
    LEDCtrSerial.write("<");
    LEDCtrSerial.write(str.toUtf8());//send text string
    LEDCtrSerial.write(">")
    LEDCtrSerial.write("&");//end of text string
    LEDCtrSerial.write("@");//end of led control
    MotorSerial.setPort(dcmBTinfo);
    MotorSerial.open(QIODevice::ReadWrite);
    MotorSerial.setBaudRate(QSerialPort::Baud9600);
    MotorSerial.setDataBits(QSerialPort::Data8);
    MotorSerial.write("1");//set motor running status : valid
    qDebug()<<(mode.toUtf8());
    qDebug()<<(str.toUtf8());
    j = tleft+1; //set j as priveta variable for countdown time
    qDebug()<<j;
    //count down
    if(j>0 && j<=60){
        timer->start();
        timer->setInterval(1000); //coutndown time interval is 1 second
        }
    else{
        //if the running time is less than 0 or more than 60, it will alert
        QMessageBox::warning (this,tr("Warning"),tr("Please enter valid time"));
        }
    qDebug()<<"buttonends";
}

void POV_Display::on_StopPushButton_clicked()
{
    ui->StartPushButton->setText("Start");
    ui->StartPushButton->setEnabled(true);
    //when stop button is clicked, stop all function and reset GUI
    timer->stop();
    deviceStop();
    qDebug()<<"stopped";
}

void POV_Display::on_lineEdit_returnPressed()
{
    str = ui->lineEdit->text();//get text edit information
    qDebug()<<str;
}

void POV_Display::on_modeComboBox_currentIndexChanged(const QString &arg1)
{
    mode = arg1;
    //get text mode select information
    if (mode == "Image"){
        ui->lineEdit->clear();
        mode = "*1";
    }
    else {
        mode = "*0";
    }
    qDebug()<<mode;
}

void POV_Display::shutdown()
{
    //when application is shut down, close serial port
    LEDCtrSerial.clear();
    MotorSerial.clear();
}

void POV_Display::recSerialData()
{
    //read current serial port
    QByteArray ba;
    ba = MotorSerial.readAll();
    qDebug()<<(ba);
}

void POV_Display::on_lineEdit_textChanged(const QString &arg1)
{
    str = ui->lineEdit->text();
    //str = arg1;
    qDebug()<<str;
}

void POV_Display::on_TimerSpinBox_valueChanged(int arg1)
{
    //get left time data
    tleft = arg1;
}

void POV_Display::displaytimerUpDate()
{
    //disable start button and set it as a countdown window
    QString tl;
    QString timeleft = QString::number(tleft);
    if(tleft>0){
        ui->StartPushButton->setText(timeleft);
        ui->StartPushButton->setEnabled(false);
        tleft--;
        }
    else{
        tl = ui->TimerSpinBox->text();
        ui->StartPushButton->setText("Start");
        ui->StartPushButton->setEnabled(true);
        qDebug()<<"time is up";
        deviceStop();
        timer->stop();
    }
    qDebug()<<tleft;
}

void POV_Display::deviceStop(){
    //when time is up, or stop button clicked
    //stop motor and close led control
    MotorSerial.setPort(dcmBTinfo);
    MotorSerial.open(QIODevice::ReadWrite);
    MotorSerial.setBaudRate(QSerialPort::Baud9600);
    MotorSerial.write("0");
    LEDCtrSerial.close();
}

void POV_Display::on_dcmComboBox_currentIndexChanged(const QString &arg1)
{
    //allow user to chose motor control port
    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    int i = 0;
    foreach (dcmBTinfo, infos) {
        if(dcmBTinfo.portName() == arg1) break;
        i++;
        }
    if(i != infos.size ()){//can find
        MotorSerial.setPort(dcmBTinfo);
    }else{
            MotorSerial.close();
            QMessageBox::warning (this,tr("Warning"),tr("Failed to open this port"));
    }
}
