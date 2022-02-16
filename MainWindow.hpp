#pragma once

#include <QMainWindow>
#include <QTimer>

#include "src/Bulb.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_set_power_clicked();

    void on_pushButton_set_bright_clicked();

    void on_pushButton_blink_clicked();

private:
    Ui::MainWindow *ui;
    QScopedPointer<YeelightBulb::Bulb> bulb;
};
