#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "serialport.h"
#include "localdb.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
typedef enum {
    Boot_State_Idle = 0,
    Boot_State_Send_New_Firmware = 1,
    Boot_State_Jump_To_User_App = 2,
    Boot_State_Get_Basic_Info = 3,
    Boot_State_Waiting_IAP_Ready_Receive_New_Firmware = 4,
    Boot_State_default = 0xff
}Bootloader_State;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void start_request();
private slots:
    void on_btn_getVersion_clicked();

    void on_btn_browser_clicked();

    void on_btn_downloadFirmware_clicked();

    void on_btn_restart_clicked();

    void on_btn_openCom_clicked();

    void showDataReceived(QByteArray data);

    void processBootState();
private:
    Ui::MainWindow *ui;
    SerialPort _port;
    bool loadPort();
    QTimer heartbeatTicker;
    uint64_t systemTick = 0;
    uint64_t timeout_waiting_newfw = 0;
    uint64_t timeout_upgrade_fw = 0;
    Bootloader_State boot_state;
    LocalDB *db;
};
#endif // MAINWINDOW_H
