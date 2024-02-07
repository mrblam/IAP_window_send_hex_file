#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QThread>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    boot_state = Boot_State_Idle;
    db = new LocalDB("database.db");
    ui->setupUi(this);
    this->setWindowTitle("Peco IAP");
    loadPort();
    connect(&_port,&SerialPort::showDataReceived,this,&MainWindow::showDataReceived);
    connect(&heartbeatTicker,&QTimer::timeout,this,&MainWindow::processBootState);
    heartbeatTicker.start(100);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::loadPort()
{
    foreach(auto &port,QSerialPortInfo::availablePorts()){
        ui->portList->addItem( port.portName());
    }
    return 1;
}

void MainWindow::on_btn_getVersion_clicked()
{
    _port.clearRxBuffer();
    auto numBytes = _port.writeSerialPort(QByteArray::fromRawData("\x02",1));
    if (numBytes == -1) {
        QMessageBox::critical(this,"Error","Send request false!!");
    }
}


void MainWindow::on_btn_browser_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this, tr("Open Directory"),
                                                    "C:/",
                                                    tr("Video files (*.hex)"));
    ui->path->setText(dir);
}


void MainWindow::on_btn_downloadFirmware_clicked()
{
    _port.clearRxBuffer();
    auto numBytes = _port.writeSerialPort(QByteArray::fromRawData("\x01",1));
    if (numBytes == -1) {
        QMessageBox::critical(this,"Error","Send request false!!");
    }
    boot_state = Boot_State_Waiting_IAP_Ready_Receive_New_Firmware;
    timeout_waiting_newfw = systemTick + 1000;
}


void MainWindow::on_btn_restart_clicked()
{
    _port.clearRxBuffer();
    auto numBytes = _port.writeSerialPort(QByteArray::fromRawData("\x03",1));
    if (numBytes == -1) {
        QMessageBox::critical(this,"Error","Send request false!!");
    }
}


void MainWindow::on_btn_openCom_clicked()
{
    auto isConnected = _port.connectPort(ui->portList->currentText());
    if(!isConnected){
        QMessageBox::critical(this,"Error","There is a problem connect to port");
    }else{
        QMessageBox::information(this,"Result","Port is opened");
        ui->btn_openCom->setEnabled(false);
        db->insertToDb("Open Com");
    }
}

void MainWindow::showDataReceived(QByteArray data)
{
    ui->plainTextEdit->insertPlainText(data);
    if(data.contains("error")){
        QMessageBox::critical(this,"Error","Firmware has problem,Exit upgrade!!");
        boot_state = Boot_State_Idle;
        db->insertToDb("Upgrade Error");
    }
    if(data.contains("Success")){
        db->insertToDb("Upgrade Success");
    }
}

void MainWindow::start_request()
{
    _port.setPath(ui->path->text());
    QThread *subThread = new QThread;
    connect(subThread,&QThread::started,&_port,&SerialPort::requestToServer);
    // connect(&_port,&SerialPort::workDone,subThread,&QThread::quit);
    connect(&_port,&SerialPort::workDone,this,&MainWindow::showDataReceived);
    connect(subThread,&QThread::finished,&_port,&SerialPort::deleteLater);
    connect(subThread,&QThread::finished,subThread,&QThread::deleteLater);
    _port.moveToThread(subThread);
    subThread->start();
}

void MainWindow::processBootState()
{
    systemTick += 100;
    switch(boot_state){
    case Boot_State_Idle:
        break;
    case Boot_State_Get_Basic_Info:
        break;
    case Boot_State_Send_New_Firmware:
        if(systemTick < timeout_upgrade_fw){
#if 0
            uint16_t line = 1;
            bool is_finish = true;
            QFile file(ui->path->text());
            if (!file.open(QIODevice::ReadOnly)){
                qDebug() << "Open file fault";
                QMessageBox::critical(this,"Error","Can't open this file!!");
                break;
            }
            QTextStream in(&file);
            QString line_data = in.readLine() + "\r\n";
            while (is_finish && line < 2000) {
                if(line_data.contains(":00000001FF")){
                    is_finish = false;
                }
                _port.writeSerialPort(line_data.toUtf8());
                line_data = in.readLine() + "\r\n";
                line ++;
            }
            boot_state = Boot_State_Idle;
#else
            start_request();
            boot_state = Boot_State_Idle;
#endif
        }else{
            QMessageBox::critical(this,"Timeout","Send new firmware timeout!!");
            boot_state = Boot_State_Idle;
        }
        break;
    case Boot_State_Jump_To_User_App:
        break;
    case Boot_State_Waiting_IAP_Ready_Receive_New_Firmware:
        if(systemTick < timeout_waiting_newfw){
            if(_port.getRxBuffer().contains("Waiting new firmware")){
                qDebug() << "IAP respone waiting new fw";
                _port.clearRxBuffer();
                boot_state = Boot_State_Send_New_Firmware;
                timeout_upgrade_fw = systemTick + 5000;
            }
        }else{
            QMessageBox::critical(this,"Timeout","IAP not response!!");
            boot_state = Boot_State_Idle;
        }
        break;
    default:
        break;
    }
}
