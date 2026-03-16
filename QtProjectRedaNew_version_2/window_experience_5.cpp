#include "window_experience_5.h"
#include "ui_window_experience_5.h"

#include <QAxObject>
#include <QMessageBox>

window_experience_5::window_experience_5(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::window_experience_5)
{
    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint); // скрываем кнопку "?"

    this->setWindowTitle("5. Опыт короткого замыкания");

    // снимаем галочку в чек-боксе, если стоит в другом
    connect(ui->checkBox_second_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_first_run_in->setCheckState(Qt::Unchecked); });
    connect(ui->checkBox_first_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_second_run_in->setCheckState(Qt::Unchecked); });
}

window_experience_5::~window_experience_5()
{
    delete ui;
}

void window_experience_5::slotFromMainTo_window_experience_5_Parameters(float VoltageShortCircuit, float CurrentShortCircuit, float LossesShortCircuit)
{
    ui->lineEdit_VoltageShortCircuit_Nominal->setText(QString::number(qRound(VoltageShortCircuit*100)/100));
    ui->lineEdit_CurrentShortCircuit_Nominal->setText(QString::number(qRound(CurrentShortCircuit*100)/100));
    ui->lineEdit_LossesShortCircuit_Nominal->setText(QString::number(qRound(LossesShortCircuit*100)/100));
}

void window_experience_5::slotInExperienceOneNumberProtocol(QString numberProtocol)
{
    fileProtocol = numberProtocol;
}

void window_experience_5::slotInExperienceFive_high_voltage(float high_voltage)
{
    //QMessageBox::about(this, "Уведомление", "Установите напряжение на трансформаторе 924В!");
    value_high_voltage = 704 /*high_voltage*/;
    ui->lineEdit_high_voltage->setText(QString::number(value_high_voltage));
}

void window_experience_5::slotFromMainTo_window_experience_5_voltage(float VoltageShortCircuit)
{
    VoltageShortCircuit = VoltageShortCircuit*(value_high_voltage/380);
    VoltageShortCircuit = qRound(VoltageShortCircuit*100);
    VoltageShortCircuit = VoltageShortCircuit/100;
    ui->lineEdit_VoltageShortCircuit->setText(QString::number(VoltageShortCircuit));

    if (ui->lineEdit_VoltageShortCircuit->text().toFloat() >= ui->lineEdit_VoltageShortCircuit_Nominal->text().toFloat()) {
        ui->lineEdit_VoltageShortCircuit->setText(ui->lineEdit_VoltageShortCircuit_Nominal->text());
        ui->pushButton_LossesShortCircuit_save->setEnabled(true);
        ui->pushButton_stopExperienceFive->setEnabled(false);
        ui->pushButton_startExperienceFive->setEnabled(true);

       if (ui->lineEdit_CurrentShortCircuit->text().toFloat() >= ui->lineEdit_CurrentShortCircuit_Nominal->text().toFloat()){
           ui->lineEdit_CurrentShortCircuit_Result->setText("Годен");
       } else {
          ui->lineEdit_CurrentShortCircuit_Result->setText("Не годен");
       }

       if (ui->lineEdit_LossesShortCircuit->text().toFloat() <= ui->lineEdit_LossesShortCircuit_Nominal->text().toFloat()){
           ui->lineEdit_LossesShortCircuit_Result->setText("Годен");
       } else {
         ui->lineEdit_LossesShortCircuit_Result->setText("Не годен");
       }
       emit signal_stopExperienceFive();
    }
}

void window_experience_5::slotFromMainTo_window_experience_5_current(float CurrentShortCircuit)
{
    CurrentShortCircuit = CurrentShortCircuit/(value_high_voltage/380);
    CurrentShortCircuit = qRound(CurrentShortCircuit*100);
    CurrentShortCircuit = CurrentShortCircuit/100;
    ui->lineEdit_CurrentShortCircuit->setText(QString::number(CurrentShortCircuit));

    float LossesShortCircuit = (ui->lineEdit_CurrentShortCircuit->text().toFloat()
                                *ui->lineEdit_VoltageShortCircuit->text().toFloat()*0.001)/1.73;
    LossesShortCircuit = qRound(LossesShortCircuit*100);
    LossesShortCircuit = LossesShortCircuit/100;
    ui->lineEdit_LossesShortCircuit->setText(QString::number(LossesShortCircuit));
}

void window_experience_5::slotFromMainTo_window_experience_5_losses(float LossesShortCircuit)
{
    //ui->lineEdit_LossesShortCircuit->setText(QString::number(LossesShortCircuit));
}

void window_experience_5::on_pushButton_LossesShortCircuit_save_clicked()
{
    if (ui->lineEdit_CurrentShortCircuit->text() == NULL || ui->lineEdit_LossesShortCircuit->text() == NULL){
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
    if (ui->checkBox_first_run_in->isChecked()){
        QAxObject* data = sheet->querySubObject("Cells(int, int)", 34, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_CurrentShortCircuit_Nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 34, 13);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_CurrentShortCircuit->text());

        data = sheet->querySubObject("Cells(int, int)", 34, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_CurrentShortCircuit_Result->text());

        data = sheet->querySubObject("Cells(int, int)", 35, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_LossesShortCircuit_Nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 35, 13);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_LossesShortCircuit->text());

        data = sheet->querySubObject("Cells(int, int)", 35, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_LossesShortCircuit_Result->text());

    } else if (ui->checkBox_second_run_in->isChecked()){
        QAxObject* data = sheet->querySubObject("Cells(int, int)", 34, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_CurrentShortCircuit_Nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 34, 16);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_CurrentShortCircuit->text());

        data = sheet->querySubObject("Cells(int, int)", 34, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_CurrentShortCircuit_Result->text());

        data = sheet->querySubObject("Cells(int, int)", 35, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_LossesShortCircuit_Nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 35, 16);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_LossesShortCircuit->text());

        data = sheet->querySubObject("Cells(int, int)", 35, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_LossesShortCircuit_Result->text());

    } else {
        QMessageBox::about(this, "Предупреждение!", "Выберите номер обкатки!");
        workbook->dynamicCall("Close(SaveChanges)",  false);
        excel->dynamicCall("Quit(void)");
        delete excel;
        return;
    }


    workbook->dynamicCall("Close(SaveChanges)",  true);
    excel->dynamicCall("Quit(void)");
    delete excel;

    close();
    emit signalExperienceFiveIsEnded();
}

void window_experience_5::closeEvent(QCloseEvent *)
{
    ui->lineEdit_VoltageShortCircuit->clear();
    ui->lineEdit_CurrentShortCircuit->clear();
    ui->lineEdit_LossesShortCircuit->clear();
    ui->lineEdit_CurrentShortCircuit_Result->clear();
    ui->lineEdit_LossesShortCircuit_Result->clear();
    emit signal_stopExperienceFive();
}

void window_experience_5::on_pushButton_startExperienceFive_clicked()
{
    if (ui->lineEdit_high_voltage->text() == NULL){
        ui->lineEdit_high_voltage->setStyleSheet("*{background-color: orange;}");
        QMessageBox::about(this, "Предупреждение!", "Не заполнено поле: Напряжение трансформатора на высокой стороне");
        return;
    } else {
        ui->pushButton_startExperienceFive->setEnabled(false);
        ui->pushButton_stopExperienceFive->setEnabled(true);
        ui->lineEdit_high_voltage->setStyleSheet("*{background-color: white;}");
        ui->lineEdit_high_voltage->setEnabled(false);
        emit signal_startExperienceFive();
    }

//    ui->lineEdit_CurrentShortCircuit->clear();
//    ui->lineEdit_LossesShortCircuit->clear();
//    ui->lineEdit_CurrentShortCircuit_Result->clear();
//    ui->lineEdit_LossesShortCircuit_Result->clear();
}

void window_experience_5::on_pushButton_stopExperienceFive_clicked()
{
    ui->pushButton_startExperienceFive->setEnabled(true);
    ui->pushButton_stopExperienceFive->setEnabled(false);
    emit signal_stopExperienceFive();
}
