#ifndef POV_DISPLAY_H
#define POV_DISPLAY_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>

namespace Ui {
class POV_Display;
}

class POV_Display : public QMainWindow
{
    Q_OBJECT

public:
    explicit POV_Display(QWidget *parent = nullptr);
    ~POV_Display();
    QSerialPort MotorSerial,LEDCtrSerial;
    void initSerialPort();//declare initialazation of serial port function
    void shutdown();
    QString str,mode;
    int tleft=10;

public slots:

    void displaytimerUpDate();

    void deviceStop();

private slots:

    void on_StartPushButton_clicked();

    void on_StopPushButton_clicked();

    void on_displayComboBox_currentIndexChanged(const QString &arg1);

    void on_modeComboBox_currentIndexChanged(const QString &arg1);

    void on_lineEdit_returnPressed();

    void recSerialData();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_TimerSpinBox_valueChanged(int arg1);

    void on_dcmComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::POV_Display *ui;
    QTimer * timer;
    QSerialPortInfo disBTinfo,dcmBTinfo;
};

#endif // POV_DISPLAY_H
