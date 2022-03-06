#ifdef _WIN32
#define SYS 1
#elif __linux__
#define SYS 2
#elif __APPLE__
#define SYS 3
#endif
#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH
//确保MSVC编译器下中文显示正常
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothLocalDevice>
#include <QBluetoothServer>
#include <QBluetoothSocket>
#include <QChart>
#include <QChartView>
#include <QDebug>
#include <QFileDialog>
#include <QIODevice>
#include <QLineSeries>
#include <QList>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QTime>
#include <QUuid>
#include <cstdlib>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    int pointNum = 0;
    QChartView *chartView;
    QChart *chart;
    QLineSeries *series;
    QBluetoothSocket *socket;                  //蓝牙连接
    QBluetoothLocalDevice *localBluetooth;     //本机蓝牙设备
    QSerialPort *m_serialPort;                 //串口实例
    QList<QSerialPortInfo> portList;           //串口列表
    QBluetoothDeviceDiscoveryAgent *discover;  //蓝牙扫描实例
    QList<QBluetoothDeviceInfo> bluetoothList; //蓝牙列表
    QSerialPortInfo port;                      //当前所选串口端口号
    QBluetoothDeviceInfo bluetooth;            //当前所选蓝牙

    //当前时间
    QTime now;
    int hour;
    int minute;
    int second;
    int bluetoothNum;
    int portNum;

    MainWindow(QWidget *parent = nullptr);

    void refreshPortList();                 //刷新端口列表
    void refreshBluetoothList();            //刷新蓝牙列表
    void saveDebugInfo();                   //保存日志信息
    void startPort();                       //开始串口测试
    void getTime();                         //获取当前时间
    void setPort();                         //设置当前所选串口端口号
    void setBluetooth();                    //设置当前所选蓝牙名称
    void stopPort();                        //停止串口测试
    void init();                            //系统初始化
    void receiveData();                     //结束串口数据
    void addDebugInfo(const QString &text); //打印日志信息
    void connectBluetooth();                //蓝牙连接
    void disconnectBluetooth();             //蓝牙断开
    void refreshPairedList();               //刷新蓝牙连接列表
    void connectedInfo();
    void addChart();
    void addPoint(int value);

    ~MainWindow();

  private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_HH
