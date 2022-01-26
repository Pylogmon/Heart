#include "mainwindow.hh"
#include "ui_mainwindow.h"

//构造函数
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init(); //初始化
}
//析构函数
MainWindow::~MainWindow()
{
    delete ui;
    stop(); //关闭程序时确保关闭串口
}
//刷新端口列表
void MainWindow::refreshPortList()
{
    ui->portBox->clear(); //清空下拉菜单

    portList = QSerialPortInfo::availablePorts(); //获取端口列表

    this->portNum = portList.size(); //获取端口数目
    //添加下拉菜单
    for (int i = 0; i < this->portNum; i++)
    {
        ui->portBox->addItem(this->portList.at(i).portName() + ":" + this->portList.at(i).description());
    }
    //打印日志信息
    addDebugInfo("刷新端口列表成功！");
    addDebugInfo("获取到" + QString::number(this->portNum) + "个有效端口");
}
//刷新蓝牙列表
void MainWindow::refreshBluetoothList()
{
    discover->setLowEnergyDiscoveryTimeout(5000); //设置
    //连接扫描完成信号
    connect(discover, &QBluetoothDeviceDiscoveryAgent::finished, this, [this]() {
        bluetoothList = discover->discoveredDevices(); //获取蓝牙列表
        bluetoothNum = bluetoothList.size();           //获取蓝牙数目
        ui->bluetoothBox->clear();                     //清空下拉菜单
        //重新添加下拉菜单
        for (int i = 0; i < bluetoothNum; i++)
        {
            ui->bluetoothBox->addItem(bluetoothList.at(i).name());
        }
        //打印日志信息
        addDebugInfo("刷新蓝牙列表成功！");
        addDebugInfo("获取到" + QString::number(bluetoothNum) + "个可用蓝牙设备");
    });
    discover->start(); //开始扫描
    addDebugInfo("开始扫描蓝牙列表...");
}
//保存日志信息
void MainWindow::saveDebugInfo()
{
    //弹出窗口获取保存路径
    QString filename = QFileDialog::getSaveFileName(this, tr("选择保存路径"), "", tr("*.txt"));
    //打开文件
    QFile file(filename);
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    //获取日志信息
    QString debugInfo = ui->debugText->toPlainText();
    //写入日志信息
    file.write(debugInfo.toUtf8());
    //弹出成功对话框
    QMessageBox::information(this, tr("保存日志"), tr("保存成功！"));
}
//开始测试
void MainWindow::start()
{
    //检查可用端口
    if (this->portNum == 0)
    {
        addDebugInfo("开启失败，找不到可用端口！");
        ui->label_7->hide();
        ui->label_6->show();
    }
    else
    {
        //如果串口已经打开，则先关闭串口
        if (m_serialPort->isOpen())
        {
            m_serialPort->clear();
            m_serialPort->close();
        }
        //设置要打开的串口号
        m_serialPort->setPortName(this->port);
        //尝试打开串口
        if (!m_serialPort->open(QIODevice::ReadWrite))
        {
            //打开失败，打印日志信息
            addDebugInfo("串口打开失败，请检查端口信息！");
            ui->label_7->hide();
            ui->label_6->show();

            return;
        }
        //打开成功，打印日志信息
        addDebugInfo("串口已打开！");
        //设置串口参数
        m_serialPort->setBaudRate(QSerialPort::Baud115200, QSerialPort::AllDirections); //设置波特率和读写方向
        m_serialPort->setDataBits(QSerialPort::Data8);                                  //数据位为8位
        m_serialPort->setFlowControl(QSerialPort::NoFlowControl);                       //无流控制
        m_serialPort->setParity(QSerialPort::NoParity);                                 //无校验位
        m_serialPort->setStopBits(QSerialPort::OneStop);                                //一位停止位

        //连接下位机发送信号
        connect(m_serialPort, &QSerialPort::readyRead, this, &MainWindow::receiveData);
        //打印日志信息
        addDebugInfo("开启成功，开始记录数据！");
    }
}
//停止测试
void MainWindow::stop()
{
    //关闭串口
    m_serialPort->clear();
    m_serialPort->close();
    addDebugInfo("串口已关闭！");
}
//获取当前时间
void MainWindow::getTime()
{
    this->now = QTime::currentTime();  //获取当前时间
    this->hour = this->now.hour();     //当前小时
    this->minute = this->now.minute(); //当前分钟
    this->second = this->now.second(); //当前秒
}
//设置当前所选串口端口号
void MainWindow::setPort()
{
    //检查下拉列表是否为空
    if (ui->portBox->currentIndex() != -1)
    {
        //将当前下拉列表所选项保存到port
        this->port = this->portList.at(ui->portBox->currentIndex()).portName();
    }
}
//设置当前所选蓝牙名称
void MainWindow::setBluetooth()
{
    //检查下拉列表是否为空
    if (ui->bluetoothBox->currentIndex() != -1)
    {
        //将当前下拉列表所选项保存到bluetooth
        this->bluetooth = ui->bluetoothBox->currentText();
    }
}
//系统初始化
void MainWindow::init()
{
    addDebugInfo("系统正在初始化...");            //打印日志信息
    this->setWindowTitle("可穿戴设备上位机程序"); //设置标题
    this->setWindowIcon(QIcon(":/img/icon.svg")); //设置图标
    //隐藏绿色状态灯
    ui->label_7->hide();
    ui->label_10->hide();
    //创建QSerialPort和QBluetoothDeviceDiscoveryAgent对象的实例
    this->m_serialPort = new QSerialPort();
    this->discover = new QBluetoothDeviceDiscoveryAgent(this);
    //刷新列表
    refreshPortList();
    refreshBluetoothList();
    //链接刷新按钮
    connect(ui->refreshBtn, &QPushButton::clicked, this, &MainWindow::refreshPortList);
    connect(ui->refreshBtn_b, &QPushButton::clicked, this, &MainWindow::refreshBluetoothList);
    //链接保存按钮
    connect(ui->saveBtn, &QPushButton::clicked, this, &MainWindow::saveDebugInfo);
    //链接开始按钮
    connect(ui->startBtn, &QPushButton::clicked, this, &MainWindow::start);
    //链接结束按钮
    connect(ui->overBtn, &QPushButton::clicked, this, &MainWindow::stop);

    //链接下拉按钮
    //注意：Qt5的新connect语法中使用这两个信号时需要通过函数指针来指明使用的是重载中的哪一个
    void (QComboBox::*fp)(int) = &QComboBox::currentIndexChanged;
    connect(ui->portBox, fp, this, &MainWindow::setPort);
    connect(ui->bluetoothBox, fp, this, &MainWindow::setBluetooth);
    //链接蓝牙扫描完成信号，完成时打印日志信息
    connect(discover, &QBluetoothDeviceDiscoveryAgent::finished, this, [this]() { addDebugInfo("系统初始化完成！"); });
}
//接收串口数据
void MainWindow::receiveData()
{
    QByteArray info = m_serialPort->readAll(); //接收串口信息
    QByteArray hexData = info.toHex();         //信息转换为16进制
    //打印串口信息
    addDebugInfo("接收到串口数据：" + hexData);
}
//打印日志信息
void MainWindow::addDebugInfo(const QString &text)
{
    getTime(); //获取当前时间
    //在QTextEdit对象中打印带时间前缀的日志信息
    ui->debugText->append(QString::number(hour) + ":" + QString::number(minute) + ":" + QString::number(second) + " " +
                          text);
}
