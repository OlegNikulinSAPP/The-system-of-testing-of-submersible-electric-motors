#include "window_experience_7.h"
#include "ui_window_experience_7.h"

#include <QAxObject>
#include <QMessageBox>

window_experience_7::window_experience_7(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::window_experience_7)
{
    ui->setupUi(this);

    this->setWindowTitle("7. Обкатка ПЭД");

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint); // скрываем кнопку "?"

    // снимаем галочку в чек-боксе, если стоит в другом
    connect(ui->checkBox_second_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_first_run_in->setCheckState(Qt::Unchecked); });
    connect(ui->checkBox_first_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_second_run_in->setCheckState(Qt::Unchecked); });
}

window_experience_7::~window_experience_7()
{
    delete ui;
}

void window_experience_7::slotFromMainTo_window_experience_7_Parameters(float Voltage, float Current, float Losses, float Vibration)
{
    ui->lineEdit_voltage_nominal->setText(QString::number(Voltage));
    ui->lineEdit_current_nominal->setText(QString::number(Current));
    ui->lineEdit_losses_nominal->setText(QString::number(Losses));
    ui->lineEdit_vibration_nominal->setText(QString::number(Vibration));
}

void window_experience_7::slotInExperienceOneNumberProtocol(QString numberProtocol)
{
    fileProtocol = numberProtocol;
}

void window_experience_7::slotInExperienceSeven_high_voltage(float high_voltage)
{
    value_high_voltage = high_voltage;
    ui->lineEdit_high_voltage->setText(QString::number(value_high_voltage));
}

void window_experience_7::slotFromMainTo_window_experience_7_voltage_run(float voltage)
{
    voltage = voltage*(value_high_voltage/380);
    voltage = qRound(voltage*100);
    voltage = voltage/100;
    ui->lineEdit_voltage_fact->setText(QString::number(voltage));
}

void window_experience_7::slotFromMainTo_window_experience_7_current_run(float current)
{
    current = current/(value_high_voltage/380);
    current = qRound(current*100);
    current = current/100;
    ui->lineEdit_current_fact->setText(QString::number(current));

    if (ui->lineEdit_current_fact->text().toFloat() >= ui->lineEdit_current_nominal->text().toFloat()) {
        //emit signal_stop();
    }
}

void window_experience_7::slotFromMainTo_window_experience_7_losses_run(float losses)
{
    losses = qRound(losses*100);
    losses = losses/100;
    ui->lineEdit_losses_fact->setText(QString::number(losses));
}

void window_experience_7::slotFromMainTo_window_experience_7_BQ1_run(float BQ1)
{
//    temp_vibrationBQ1 = 30*(((float)BQ1-4000)/(20000-4000));
//    ui->lineEdit_vibrationBQ1_fact->setText(QString::number(temp_vibrationBQ1)); // датчик 1 не исправен!
}

void window_experience_7::slotFromMainTo_window_experience_7_BQ2_run(float BQ2)
{
    temp_vibrationBQ2 = 30*(((float)BQ2-4000)/(20000-4000));
    temp_vibrationBQ2 = qRound(temp_vibrationBQ2*100);
    ui->lineEdit_vibrationBQ2_fact->setText(QString::number(temp_vibrationBQ2/100));
}

void window_experience_7::slotFromMainTo_window_experience_7_BQ3_run(float BQ3)
{
    temp_vibrationBQ3 = 30*(((float)BQ3-4000)/(20000-4000));
    temp_vibrationBQ3 = qRound(temp_vibrationBQ3*100);
    ui->lineEdit_vibrationBQ3_fact->setText(QString::number(temp_vibrationBQ3/100));

    ui->lineEdit_vibrationBQ1_fact->setText(QString::number(temp_vibrationBQ3/100)); // датчик 1 не исправен!
}

void window_experience_7::slotFromMainTo_window_experience_7_BQ4_run(float BQ4)
{
    temp_vibrationBQ4 = 30*(((float)BQ4-4000)/(20000-4000));
    temp_vibrationBQ4 = qRound(temp_vibrationBQ4*100);
    ui->lineEdit_vibrationBQ4_fact->setText(QString::number(temp_vibrationBQ4/100));
}

void window_experience_7::slotFromMainTo_window_experience_7_BQ5_run(float BQ5)
{
    temp_vibrationBQ5 = 30*(((float)BQ5-4000)/(20000-4000));
    temp_vibrationBQ5 = qRound(temp_vibrationBQ5*100);
    ui->lineEdit_vibrationBQ5_fact->setText(QString::number(temp_vibrationBQ5/100));
}

void window_experience_7::slotFromMainTo_window_experience_7_BQ6_run(float BQ6)
{
    temp_vibrationBQ6 = 30*(((float)BQ6-4000)/(20000-4000));
    temp_vibrationBQ6 = qRound(temp_vibrationBQ6*100);
    ui->lineEdit_vibrationBQ6_fact->setText(QString::number(temp_vibrationBQ6/100));
}

void window_experience_7::slotFromMainTo_window_experience_7_temperature_run(float temperature)
{
    //float temp = temperature/ 100;
        //ui->lineEdit_temperature->setText(QString::number(temp));

    float temp = -50 + (350 - (-50)) * ((temperature - 4000)/(20000 - 4000));
    temp = qRound(temp*100);
    temp = temp/100;
    ui->lineEdit_temperature->setText(QString::number(temp));

    if (ui->lineEdit_temperature->text().toFloat() >= ui->lineEdit_temperature_before_stop_run->text().toFloat()) {
        emit signal_stop();
    }
}

void window_experience_7::slot_stop_run()
{
    if (ui->lineEdit_current_fact->text().toFloat() <= ui->lineEdit_current_nominal->text().toFloat()){ // разберись с условием
        ui->lineEdit_current_result->setText("Годен");
    } else {
        ui->lineEdit_current_result->setText("Не годен");
    }

    if (ui->lineEdit_losses_fact->text().toFloat() <= ui->lineEdit_losses_nominal->text().toFloat()){ // разберись с условием
        ui->lineEdit_losses_result->setText("Годен");
    } else {
        ui->lineEdit_losses_result->setText("Не годен");
    }

    if (((ui->lineEdit_vibrationBQ1_fact->text().toFloat() +
            ui->lineEdit_vibrationBQ2_fact->text().toFloat() +
         ui->lineEdit_vibrationBQ3_fact->text().toFloat() +
         ui->lineEdit_vibrationBQ4_fact->text().toFloat() +
         ui->lineEdit_vibrationBQ5_fact->text().toFloat() +
         ui->lineEdit_vibrationBQ6_fact->text().toFloat())/6) <= ui->lineEdit_vibration_nominal->text().toFloat()) {
        ui->lineEdit_vibration_result->setText("Годен");
    } else {
        ui->lineEdit_vibration_result->setText("Не годен");
    }
}

void window_experience_7::on_pushButton_save_clicked()
{
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
        QAxObject* data = sheet->querySubObject("Cells(int, int)", 40, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_current_nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 40, 13);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_current_fact->text());

        data = sheet->querySubObject("Cells(int, int)", 40, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_current_result->text());

        data = sheet->querySubObject("Cells(int, int)", 41, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_losses_nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 41, 13);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_losses_fact->text());

        data = sheet->querySubObject("Cells(int, int)", 41, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_losses_result->text());

        data = sheet->querySubObject("Cells(int, int)", 42, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_vibration_nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 42, 13);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_vibrationBQ1_fact->text());

        data = sheet->querySubObject("Cells(int, int)", 42, 14);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_vibrationBQ2_fact->text());

        data = sheet->querySubObject("Cells(int, int)", 42, 15);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_vibrationBQ3_fact->text());

        data = sheet->querySubObject("Cells(int, int)", 43, 13);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_vibrationBQ4_fact->text());

        data = sheet->querySubObject("Cells(int, int)", 43, 14);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_vibrationBQ5_fact->text());

        data = sheet->querySubObject("Cells(int, int)", 43, 15);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_vibrationBQ6_fact->text());

        data = sheet->querySubObject("Cells(int, int)", 42, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_vibration_result->text());


    } else if (ui->checkBox_second_run_in->isChecked()){
        QAxObject* data = sheet->querySubObject("Cells(int, int)", 40, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_current_nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 40, 16);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_current_fact->text());

        data = sheet->querySubObject("Cells(int, int)", 40, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_current_result->text());

        data = sheet->querySubObject("Cells(int, int)", 41, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_losses_nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 41, 16);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_losses_fact->text());

        data = sheet->querySubObject("Cells(int, int)", 41, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_losses_result->text());

        data = sheet->querySubObject("Cells(int, int)", 42, 16);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_vibrationBQ1_fact->text());

        data = sheet->querySubObject("Cells(int, int)", 42, 17);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_vibrationBQ2_fact->text());

        data = sheet->querySubObject("Cells(int, int)", 42, 18);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_vibrationBQ3_fact->text());

        data = sheet->querySubObject("Cells(int, int)", 43, 16);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_vibrationBQ4_fact->text());

        data = sheet->querySubObject("Cells(int, int)", 43, 17);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_vibrationBQ5_fact->text());

        data = sheet->querySubObject("Cells(int, int)", 43, 18);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_vibrationBQ6_fact->text());

        data = sheet->querySubObject("Cells(int, int)", 42, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_vibration_result->text());

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
    emit signalExperienceSevenIsEnded();
}

void window_experience_7::closeEvent(QCloseEvent *)
{
    ui->lineEdit_voltage_fact->clear();
    ui->lineEdit_current_fact->clear();
    ui->lineEdit_losses_fact->clear();
    ui->lineEdit_vibrationBQ1_fact->clear();
    ui->lineEdit_vibrationBQ2_fact->clear();
    ui->lineEdit_vibrationBQ3_fact->clear();
    ui->lineEdit_vibrationBQ4_fact->clear();
    ui->lineEdit_vibrationBQ5_fact->clear();
    ui->lineEdit_vibrationBQ6_fact->clear();
    ui->lineEdit_temperature->clear();
    ui->lineEdit_current_result->clear();
    ui->lineEdit_losses_result->clear();
    ui->lineEdit_vibration_result->clear();
    emit signal_stop();
}

void window_experience_7::on_pushButton_start_clicked()
{
    emit signal_start();
}

void window_experience_7::on_pushButton_stop_clicked()
{
    emit signal_stop();
}
