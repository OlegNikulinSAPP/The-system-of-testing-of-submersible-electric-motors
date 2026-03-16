#include "window_experience_15.h"
#include "ui_window_experience_15.h"

#include <QAxObject>
#include <QMessageBox>


window_experience_15::window_experience_15(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::window_experience_15)
{
    ui->setupUi(this);

    this->setWindowTitle("15. Показания ТМС");

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint); // скрываем кнопку "?"
    //this->setWindowFlags(this->windowFlags() & ~Qt::WindowCloseButtonHint);// скрываем кнопку "Х"
}

window_experience_15::~window_experience_15()
{
    delete ui;
}

void window_experience_15::on_pushButton_testTMS_clicked()
{
    ui->pushButton_STOPtestTMS->setEnabled(true);
    ui->pushButton_testTMS->setEnabled(false);
    ui->pushButton_saveTMS->setEnabled(false);
    emit signal_testTMS();
}

void window_experience_15::closeEvent(QCloseEvent *)
{
    ui->lineEdit_temperature_oil->clear();
    ui->lineEdit_temperature_winding->clear();
    ui->lineEdit_connection_pressure->clear();
    ui->lineEdit_connection_vibration->clear();
    ui->lineEdit_connection_isolation->clear();
    ui->lineEdit_TMS_result->clear();

    ui->pushButton_STOPtestTMS->setEnabled(false);
    ui->pushButton_testTMS->setEnabled(true);
    ui->pushButton_saveTMS->setEnabled(false);
}

void window_experience_15::slotInExperienceOneNumberProtocol(QString numberProtocol)
{
    fileProtocol = numberProtocol;
}

void window_experience_15::slotFromMainTo_window_experience_15_temperature_oil(float temperature_oil)
{
    ui->lineEdit_temperature_oil->setText(QString::number(temperature_oil));
}

void window_experience_15::slotFromMainTo_window_experience_15_temperature_winding(float temperature_winding)
{
    ui->lineEdit_temperature_winding->setText(QString::number(temperature_winding));
}

void window_experience_15::slotFromMainTo_window_experience_15_connection_pressure(int connection_pressure)
{
    if (connection_pressure == 2) { // если в ответе получено два байта
        ui->lineEdit_connection_pressure->setText("Соединение установлено");
    } else {
        ui->lineEdit_connection_pressure->setText("Нет связи");
    }
}

void window_experience_15::slotFromMainTo_window_experience_15_connection_vibration(int connection_vibration)
{
    if (connection_vibration == 2) { // если в ответе получено два байта
        ui->lineEdit_connection_vibration->setText("Соединение установлено");
    } else {
        ui->lineEdit_connection_vibration->setText("Нет связи");
    }
}

void window_experience_15::slotFromMainTo_window_experience_15_connection_isolation(float connection_isolation)
{
    ui->lineEdit_connection_isolation->setText(QString::number(connection_isolation));

    // заключение о годности ТМС
    if (ui->lineEdit_temperature_oil->text().toFloat() > 0 && ui->lineEdit_temperature_winding->text().toFloat() > 0
            && ui->lineEdit_connection_pressure->text() == "Соединение установлено"
            && ui->lineEdit_connection_isolation->text() > 0
            && ui->lineEdit_connection_vibration->text() == "Соединение установлено") {
        ui->lineEdit_TMS_result->setText("Годен");
    } else {
        ui->lineEdit_TMS_result->setText("Не годен");
    }
}

void window_experience_15::on_pushButton_saveTMS_clicked()
{
    if (ui->lineEdit_temperature_oil->text() == NULL || ui->lineEdit_temperature_winding->text() == NULL
            || ui->lineEdit_connection_pressure->text() == NULL|| ui->lineEdit_connection_vibration->text() == NULL
            || ui->lineEdit_connection_isolation->text() == NULL || ui->lineEdit_TMS_result->text() == NULL){
        QMessageBox::about(this, "Предупреждение!", "Не заполнено одно из полей!");
        return;
    }



    QAxObject * excel = new QAxObject("Excel.Application",this);

    excel->dynamicCall ("SetVisible(bool)", false); //  отображать форму
    excel->setProperty ("DisplayAlerts", false); //  отображать какую-либо предупреждающую информацию, например, сохранять ли подсказку при закрытии

    QAxObject * workbooks = excel-> querySubObject ("Workbooks"); // Получить коллекцию книг (файлы excel)
    QAxObject* workbook = workbooks->querySubObject("Open(const QString&)", fileProtocol);
    QAxObject *sheets = workbook->querySubObject("Worksheets");

    QAxObject *sheet = sheets->querySubObject("Item(int)", 1); // 2 - лист, к которому обращаемся


    //=================================================================================
    // извлекаем и сохраняем в файл
    QAxObject* data = sheet->querySubObject("Cells(int, int)", 58, 13);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_temperature_oil->text());

    data = sheet->querySubObject("Cells(int, int)", 59, 13);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_temperature_winding->text());

    data = sheet->querySubObject("Cells(int, int)", 60, 13);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_connection_pressure->text());

    data = sheet->querySubObject("Cells(int, int)", 61, 13);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_connection_vibration->text());

    data = sheet->querySubObject("Cells(int, int)", 62, 13);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_connection_isolation->text());

    data = sheet->querySubObject("Cells(int, int)", 58, 19);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_TMS_result->text());



    workbook->dynamicCall("Close(SaveChanges)",  true);
    excel->dynamicCall("Quit(void)");
    delete excel;

    close();
    emit signalExperienceFifteenIsEnded();
}

void window_experience_15::on_pushButton_STOPtestTMS_clicked()
{
    ui->pushButton_STOPtestTMS->setEnabled(false);
    ui->pushButton_testTMS->setEnabled(true);
    ui->pushButton_saveTMS->setEnabled(true);

    emit signal_stop_testTMS();

    QMessageBox::about(this, "Уведомление", "Опыт завершен");
}
