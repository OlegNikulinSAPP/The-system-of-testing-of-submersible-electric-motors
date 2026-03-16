#include "password.h"
#include "ui_password.h"

#include <QMessageBox>


Password::Password(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Password)
{
    ui->setupUi(this);

    //отключаем кнопки закрыть и т.д. в правом верхнем углу окна
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}

Password::~Password()
{
    delete ui;
}


void Password::on_pushButton_password_clicked()
{
    QString password = ui->lineEdit_password->text();
    if (password != "123"){
        this->close();
        emit signal_password();
        QMessageBox::about(this, "Предупреждение!", "Доступ к базе закрыт!");
    } else {
        this->close();
    }
}
