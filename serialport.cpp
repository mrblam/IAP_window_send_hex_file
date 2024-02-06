#include "serialport.h"
#include <QDebug>
#include <QFileDialog>

SerialPort::SerialPort(QObject *parent)
    : QObject{parent}
{}

bool SerialPort::connectPort(QString portName)
{
    if(serialPort != nullptr){
        serialPort->close();
        delete serialPort;
    }
    serialPort = new QSerialPort;
    serialPort->setPortName(portName);
    serialPort->setBaudRate(QSerialPort::BaudRate::Baud115200);
    serialPort->setParity(QSerialPort::Parity::NoParity);
    serialPort->setDataBits(QSerialPort::DataBits::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    if(serialPort->open(QIODevice::ReadWrite)){
        connect(serialPort,&QSerialPort::readyRead,this,&SerialPort::dataReady);
        qDebug()<<"Openning";
    }else{
        qDebug() << QString("Can't open %1, error code %2").arg(portName).arg(serialPort->error()) << serialPort->errorString();
    }
    return serialPort->isOpen();
}

qint64 SerialPort::writeSerialPort(QByteArray data)
{
    if(serialPort == nullptr || !serialPort->isOpen()){
        return -1;
    }
    return serialPort->write(data);
}

SerialPort::SerialPort(QString l_cmd)
{
    path = l_cmd;
}

QString SerialPort::requestToServer()
{
    QString rx_data;
    uint16_t line = 1;
    bool is_finish = true;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)){
        qDebug() << "Open file fault";
    }
    QTextStream in(&file);
    QString line_data = in.readLine() + "\r\n";
    while (is_finish && line < 2000) {
        if(line_data.contains(":00000001FF")){
            is_finish = false;
        }
        serialPort->write(line_data.toLatin1());
        // serialPort->waitForBytesWritten(50);
        if(serialPort->waitForReadyRead(50)){
            qDebug() << "Reading: " << serialPort->bytesAvailable();
            rx_data = serialPort->readAll();
            qDebug() << "rx_data --> " + rx_data;
            emit showDataReceived(rx_data.toUtf8());
        }
        line_data = in.readLine() + "\r\n";
        line ++;
    }
    return rx_data;
}

void SerialPort::setPath(QString l_path)
{
    path = l_path;
}

void SerialPort::receiveData(const QByteArray &new_data)
{
    emit showDataReceived(new_data);
    rxBuffer.append(new_data);
}

void SerialPort::clearRxBuffer()
{
    rxBuffer.clear();
}

QByteArray SerialPort::getRxBuffer()
{
    return rxBuffer;
}

QSerialPort *SerialPort::getSerialPort()
{
    return serialPort;
}
void SerialPort::dataReady()
{
    if(serialPort->bytesAvailable() > 0){
        receiveData(serialPort->readAll());
    }
}
