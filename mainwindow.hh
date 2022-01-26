#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothServer>
#include <QDebug>
#include <QFileDialog>
#include <QList>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    QSerialPort *m_serialPort;
    QList<QSerialPortInfo> portList;
    QBluetoothDeviceDiscoveryAgent *discover;
    QList<QBluetoothDeviceInfo> bluetoothList;
    QString port;
    QString bluetooth;

    QTime now;
    int hour;
    int minute;
    int second;
    int bluetoothNum;
    int portNum;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void refreshPortList();
    void refreshBluetoothList();
    void saveDebugInfo();
    void start();
    void getTime();
    void setPort();
    void setBluetooth();
    void stop();
    void init();
    void receiveData();
    void addDebugInfo(const QString &text);

  private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_HH
