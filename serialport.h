#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>

class SerialPort : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    SerialPort(QString l_cmd);
    bool connectPort(QString portName);
    qint64 writeSerialPort(QByteArray data);
    void receiveData(const QByteArray &new_data);
    void clearRxBuffer(void);
    QByteArray getRxBuffer(void);
    QSerialPort* getSerialPort();
    QString requestToServer();
    void setPath(QString);
signals:
    void showDataReceived(QByteArray data);
    void workDone(QByteArray rx_data);
private:
    QSerialPort *serialPort = nullptr;
    QTimer heartbeatTicker;
    QByteArray rxBuffer;
    QString path;

private slots:
    void dataReady();
};

#endif // SERIALPORT_H
