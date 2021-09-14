#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>

// конструктор формы
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);  // нарисовать форму

    _inited = false;    // изначально - соединение с MySQL не установлен

    connect(ui->connect_but, SIGNAL(clicked()),SLOT(slotConnect()));
    connect(ui->disconnect_but, SIGNAL(clicked()),SLOT(slotDisconnect()));
    connect(ui->query_but, SIGNAL(clicked()),SLOT(slotQuery()));
    connect(ui->query_text, SIGNAL(mySignal()),SLOT(slotQuery()));
}

// деструктор формы
MainWindow::~MainWindow()
{
    if(_inited)
        mysql_close(&connector);    // при выходе разорвать соединение, если установлено
    delete ui;
}

// обработчик нажатия кнопки соединения
void MainWindow::slotConnect(){
    QMessageBox msgBox;
    if(_inited){
        msgBox.setText("Соединение уже установлено... Разорвите соединение!");
        msgBox.exec();
    }
    else {
        mysql_init(&connector); // получить дескриптор подключения
        // попытаться установить соединение
        _inited = mysql_real_connect(&connector,
                                     reinterpret_cast<char *>(ui->host_lbl->text().toLatin1().data()) /*хост*/,
                                     reinterpret_cast<char *>(ui->user_lbl->text().toLatin1().data()) /*имя пользователя*/,
                                     reinterpret_cast<char *>(ui->password_lbl->text().toLatin1().data()) /*пароль*/,
                                     reinterpret_cast<char *>(ui->database_lbl->text().toLatin1().data()) /*база данных по умолчанию*/,
                                     0 /*port = 3306*/,
                                     NULL /*unix socket*/,
                                     0 /*clientflag*/);
        if(!_inited){   // если соединение не удалось
            mysql_close(&connector);    // освобождаем дискриптор
            msgBox.setText(mysql_error(&connector));
            msgBox.exec();
        }
        else{           // если соединение удалось
            msgBox.setText("Соединение установлено!");
            msgBox.exec();
        }
    }
}

// обработчик нажатия кнопки разъединения
void MainWindow::slotDisconnect(){
    QMessageBox msgBox;
    if(_inited){                    // если соединение установлено и вывести сообщение
        mysql_close(&connector);    // разорвать соединение
        _inited = false;
        msgBox.setText("Соединение разорвано!");
        msgBox.exec();
    }
    else{                           // иначе вывести сообщение
        msgBox.setText("Соединение не установлено!");
        msgBox.exec();
    }
}

// обработчик нажатия кнопки запроса
void MainWindow::slotQuery(){
    QMessageBox msgBox;

    if(!_inited){               // если соединение не установлено - вывести сообщение
        msgBox.setText("Соединение не установлено!");
        msgBox.exec();
        return;
    }

    MYSQL_RES *result;  // структура результата
    MYSQL_ROW row;      // строка
    MYSQL_FIELD *field; // структура столбца


    if(
        // ВЫПОЛНЯЕМ ЗАПРОС (облегчённый стиль)
        /*mysql_query(&connector,
                      reinterpret_cast<char *>(ui->query_text->toPlainText().toLatin1().data()))){*/
        // ВЫПОЛНЯЕМ ЗАПРОС (расширенный стиль)
        mysql_real_query(&connector,
                         reinterpret_cast<char *>(ui->query_text->toPlainText().toLatin1().data()),
                         ui->query_text->toPlainText().length())
    ){
        msgBox.setText(mysql_error(&connector));    // если ошибка - вывести сообщение
        msgBox.exec();
        return;
    }

    if((result = mysql_store_result(&connector)))   // если команда "SELECT" - сохранить результат
    {
        //была команда SELECT
        int numRecords = mysql_num_rows(result);    // количество строк
        int numFields  = mysql_num_fields(result);  // количество столбцов
        if( (!numFields) || (!numRecords) ){
            msgBox.setText("Результат пуст!");      // если пусто - вывести сообщение
            msgBox.exec();
        }
        else{
            int i,j;

            ui->result_table->clear();
            ui->result_table->setRowCount(numRecords);
            ui->result_table->setColumnCount(numFields);
            ui->result_table->verticalHeader()->hide(); // заголовки строк не нужны

            QStringList _hdrs;
            for(i = 0;i<numFields;i++)
            {
                field = mysql_fetch_field(result);  // получить индентификатор очередного столбца
                _hdrs.append(field->name);          // сохранить его имя
            }
            ui->result_table->setHorizontalHeaderLabels(_hdrs); // инициализировать заголовки таблицы

            QTableWidgetItem* ptwi = 0; // указатель на новый элемент таблицы
            for(i = 0;i<numRecords;i++)
            {
                row = mysql_fetch_row(result);  // получить очередную строку
                for(j=0;j<numFields;j++){
                    ptwi = new QTableWidgetItem(row[j]);    // создать новый элемент с текстом
                    ui->result_table->setItem(i, j, ptwi);  // и послать его в соответствующую ячейку таблицы
                }
            }
            ui->result_table->resizeColumnsToContents();    // подогнать содержимое по ширине...
            ui->result_table->resizeRowsToContents();       // ...и по высоте

            mysql_free_result(result);                      // очистить память, выделенную под результат запроса
        }
    }
    else
    {
        // НЕ КОМАНДА "SELECT"
        if(mysql_field_count(&connector) == 0)
        {
            //команда НЕ "SELECT"
            int _row_count;
            _row_count = mysql_affected_rows(&connector);
            if(_row_count != 0){    // если были обработаны строки командой INSERT, UPDATE и т.д...
                msgBox.setText(mysql_info(&connector)); // ...вывести сообщение
                msgBox.exec();
            }
            else if(_row_count < 0){
                msgBox.setText(mysql_error(&connector));    // Ошибка! Вывести сообщение
                msgBox.exec();
            }
            /*else{
                // вывести другую информацию о запросе (USE DATABASE)
            }*/
        }
        else{   // ОШИБКА
            msgBox.setText(mysql_error(&connector));
            msgBox.exec();
        }
    }
}
