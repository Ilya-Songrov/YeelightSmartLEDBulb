#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , bulb(new YeelightBulb::Bulb())
{
    ui->setupUi(this);

    bulb->connectToBulb(QHostAddress("192.168.0.102"), 55443);
    connect(bulb.get(), &YeelightBulb::Bulb::disconnected, [](){
        qDebug() << "print_function:" << __FUNCTION__ << __LINE__ << " disconnected: " << Qt::endl;
    });
    connect(bulb.get(), &YeelightBulb::Bulb::stateChanged, [](QAbstractSocket::SocketState socketState){
        qDebug() << "print_function:" << __FUNCTION__ << __LINE__ << " socketState: " << QVariant::fromValue(socketState).toString() << Qt::endl;
    });
    connect(bulb.get(), &YeelightBulb::Bulb::errorOccurred, [](QAbstractSocket::SocketError socketError){
        qDebug() << "print_function:" << __FUNCTION__ << __LINE__ << " socketError: " << QVariant::fromValue(socketError).toString() << Qt::endl;
    });
    connect(bulb.get(), &YeelightBulb::Bulb::bulbErrorOccurred, [](YeelightBulb::Bulb::BulbError bulbError){
        qDebug() << "print_function:" << __FUNCTION__ << __LINE__ << " bulbError: " << bulbError << Qt::endl;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_set_power_clicked()
{
    static bool state = true;
    bulb->set_power(state, YeelightBulb::Bulb::Sudden, 1);
//    bulb->set_power(state, YeelightBulb::Bulb::Smooth, 1000);
    state = !state;
}

void MainWindow::on_pushButton_set_bright_clicked()
{
    static bool state = true;
    bulb->set_bright(state ? 99 : -99, YeelightBulb::Bulb::Smooth, 500);
    state = !state;
}



void MainWindow::on_pushButton_blink_clicked()
{
    const int time = 700;
    bulb->set_power(true, YeelightBulb::Bulb::Smooth, time);
    QTimer::singleShot(time, [this](){
        bulb->set_power(false, YeelightBulb::Bulb::Smooth, time);
    });
    QTimer::singleShot(time * 2, [this](){
        bulb->set_power(true, YeelightBulb::Bulb::Smooth, time);
    });
    QTimer::singleShot(time * 3, [this](){
        bulb->set_power(false, YeelightBulb::Bulb::Smooth, time);
    });
    QTimer::singleShot(time * 4, [this](){
        bulb->set_power(true, YeelightBulb::Bulb::Smooth, time);
    });
    QTimer::singleShot(time * 5, [this](){
        bulb->set_power(false, YeelightBulb::Bulb::Smooth, time);
    });
}

