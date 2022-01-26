#include "mainwindow.hh"

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init(); //刷新端口列表
}
//析构函数
MainWindow::~MainWindow()
{
    delete ui;
    stop();
}
//刷新端口列表
void MainWindow::refreshPortList()
{
    ui->portBox->clear(); //清空下拉菜单

    portList = QSerialPortInfo::availablePorts();

    this->portNum = portList.size();
    //添加下拉菜单
    for (int i = 0; i < this->portNum; i++)
    {
        // portNameList.append(this->portList.at(i).portName());
        ui->portBox->addItem(this->portList.at(i).portName() + ":" + this->portList.at(i).description());
    }
    addDebugInfo("刷新端口列表成功！");
    addDebugInfo("获取到" + QString::number(this->portNum) + "个有效端口");
}
//刷新蓝牙列表
void MainWindow::refreshBluetoothList()
{

    discover->setLowEnergyDiscoveryTimeout(5000);
    connect(discover, &QBluetoothDeviceDiscoveryAgent::finished, this, [this]() {
        bluetoothList = discover->discoveredDevices();
        bluetoothNum = bluetoothList.size();
        ui->bluetoothBox->clear();
        for (int i = 0; i < bluetoothNum; i++)
        {
            ui->bluetoothBox->addItem(bluetoothList.at(i).name());
        }
        addDebugInfo("刷新蓝牙列表成功！");
        addDebugInfo("获取到" + QString::number(bluetoothNum) + "个可用蓝牙设备");
    });
    discover->start();
    addDebugInfo("开始扫描蓝牙列表...");
}
//保存日志信息
void MainWindow::saveDebugInfo()
{
    //弹出窗口获取保存路径
    QString filename = QFileDialog::getSaveFileName(this, tr("选择保存路径"), "", tr("*.txt"));
    if (filename.isEmpty())
    {
        return;
    }
    //打开文件
    QFile file(filename);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        return;
    }
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
    // refreshPortList();
    if (this->portNum == 0)
    {
        addDebugInfo("开启失败，找不到可用端口！");
        ui->label_7->hide();
        ui->label_6->show();
    }
    else
    {

        if (m_serialPort->isOpen()) //如果串口已经打开了 先给他关闭了
        {
            m_serialPort->clear();
            m_serialPort->close();
        }

        m_serialPort->setPortName(this->port);

        if (!m_serialPort->open(QIODevice::ReadWrite)) //用ReadWrite 的模式尝试打开串口
        {
            addDebugInfo("串口打开失败，请检查端口信息！");
            ui->label_7->hide();
            ui->label_6->show();

            return;
        }
        //打开成功

        addDebugInfo("串口已打开！");

        m_serialPort->setBaudRate(QSerialPort::Baud115200, QSerialPort::AllDirections); //设置波特率和读写方向
        m_serialPort->setDataBits(QSerialPort::Data8);                                  //数据位为8位
        m_serialPort->setFlowControl(QSerialPort::NoFlowControl);                       //无流控制
        m_serialPort->setParity(QSerialPort::NoParity);                                 //无校验位
        m_serialPort->setStopBits(QSerialPort::OneStop);                                //一位停止位

        //连接信号槽 当下位机发送数据QSerialPortInfo 会发送个 readyRead 信号,我们定义个槽void receiveInfo()解析数据
        connect(m_serialPort, &QSerialPort::readyRead, this, &MainWindow::receiveData);
        addDebugInfo("开启成功，开始记录数据！");
    }
}
//停止测试
void MainWindow::stop()
{
    m_serialPort->clear();
    m_serialPort->close();
    addDebugInfo("串口已关闭！");
}
//获取当前时间
void MainWindow::getTime()
{
    this->now = QTime::currentTime();
    this->hour = this->now.hour();
    this->minute = this->now.minute();
    this->second = this->now.second();
}
//设置当前选择串口
void MainWindow::setPort()
{

    if (ui->portBox->currentIndex() != -1)
    {
        this->port = this->portList.at(ui->portBox->currentIndex()).portName();
    }
    // this->port = this->portList.serial_list.at(0).portName();
}
//设置当前选择蓝牙
void MainWindow::setBluetooth()
{

    if (ui->bluetoothBox->currentIndex() != -1)
    {
        this->bluetooth = ui->bluetoothBox->currentText();
    }
    // this->port = this->portList.serial_list.at(0).portName();
}
//系统初始化
void MainWindow::init()
{
    addDebugInfo("系统正在初始化...");
    this->setWindowTitle("可穿戴设备上位机程序"); //设置标题
    this->setWindowIcon(QIcon(":/img/icon.svg")); //设置图标
    ui->label_7->hide();                          //隐藏绿色状态灯
    ui->label_10->hide();
    this->m_serialPort = new QSerialPort();
    this->discover = new QBluetoothDeviceDiscoveryAgent(this);
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
    setPort();
    connect(discover, &QBluetoothDeviceDiscoveryAgent::finished, this, [this]() { addDebugInfo("系统初始化完成！"); });
}
//接收串口数据
void MainWindow::receiveData()
{
    QByteArray info = m_serialPort->readAll();
    QByteArray hexData = info.toHex();
    //这里面的协议 你们自己定义就行  单片机发什么 代表什么 我们这里简单模拟一下
    addDebugInfo("接收到串口数据：" + hexData);
}
//添加日志信息
void MainWindow::addDebugInfo(const QString &text)
{
    getTime();
    ui->debugText->append(QString::number(hour) + ":" + QString::number(minute) + ":" + QString::number(second) + " " +
                          text);
}
