#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <mysql.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    MYSQL connector;    // Дескриптор подключения к MySQL
    bool _inited;       // Флаг соединения (установлено/не установлено)

public slots:
    void slotConnect();     // обработчик нажатия кнопки соединения
    void slotDisconnect();  // обработчик нажатия кнопки разъединения
    void slotQuery();       // обработчик нажатия кнопки запроса

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
