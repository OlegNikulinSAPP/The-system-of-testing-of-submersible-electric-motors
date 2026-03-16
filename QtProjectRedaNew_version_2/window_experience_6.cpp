#include "window_experience_6.h"
#include "ui_window_experience_6.h"

#include <QAxObject>
#include <QMessageBox>

window_experience_6::window_experience_6(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::window_experience_6)
{
    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint); // скрываем кнопку "?"

    this->setWindowTitle("6. Определение напряжения трогания");

    // снимаем галочку в чек-боксе, если стоит в другом
    connect(ui->checkBox_second_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_first_run_in->setCheckState(Qt::Unchecked); });
    connect(ui->checkBox_first_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_second_run_in->setCheckState(Qt::Unchecked); });

    ui->pushButton_AccelerationVoltage_stop->setEnabled(false);
    ui->pushButton_AccelerationVoltage_save->setEnabled(false);
}

window_experience_6::~window_experience_6()
{
    delete ui;
}

void window_experience_6::slotInExperienceOneNumberProtocol(QString numberProtocol)
{
    fileProtocol = numberProtocol;
}

void window_experience_6::slotFromMainTo_window_experience_6_Parameters(float AccelerationVoltage)
{
    AccelerationVoltage = qRound(AccelerationVoltage*100);
    AccelerationVoltage = AccelerationVoltage/100;
    ui->lineEdit_AccelerationVoltage_Nominal->setText(QString::number(AccelerationVoltage));
}

void window_experience_6::slotInExperienceSix_high_voltage(float high_voltage)
{
    value_high_voltage = high_voltage;
    ui->lineEdit_high_voltage->setText(QString::number(value_high_voltage));
}

void window_experience_6::slotFromMainTo_window_experience_6_voltage_starting(float voltage_starting)
{
    ui->lineEdit_AccelerationVoltage_Fact->setText(QString::number(voltage_starting*(value_high_voltage/380)));
}

void window_experience_6::slotFromMainTo_window_experience_6_finish()
{
    //emit signal_stopExperienceSix();

    if (ui->lineEdit_AccelerationVoltage_Fact->text().toFloat() <= ui->lineEdit_AccelerationVoltage_Nominal->text().toFloat()) {
        ui->lineEdit_AccelerationVoltage_Result->setText("Годен");
    } else
        ui->lineEdit_AccelerationVoltage_Result->setText("Не годен");

    ui->pushButton_AccelerationVoltage_stop->setEnabled(false);
    ui->pushButton_AccelerationVoltage_start->setEnabled(true);
    ui->pushButton_AccelerationVoltage_save->setEnabled(true);
}

void window_experience_6::on_pushButton_AccelerationVoltage_save_clicked()
{

    if (ui->lineEdit_AccelerationVoltage_Fact->text() == NULL || ui->comboBox_AccelerationVoltage_direction->currentText() == NULL){
        QMessageBox::about(this, "Предупреждение!", "Не заполнено одно из полей!");
        return;
    }

    ui->pushButton_AccelerationVoltage_stop->setEnabled(false);
    ui->pushButton_AccelerationVoltage_start->setEnabled(false);
    ui->pushButton_AccelerationVoltage_save->setEnabled(false);
    repaint();

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
        QAxObject* data = sheet->querySubObject("Cells(int, int)", 37, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_AccelerationVoltage_Nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 37, 13);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_AccelerationVoltage_Fact->text());

        data = sheet->querySubObject("Cells(int, int)", 37, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_AccelerationVoltage_Result->text());

        data = sheet->querySubObject("Cells(int, int)", 38, 13);
        data->dynamicCall("SetValue(const QVariant&)", ui->comboBox_AccelerationVoltage_direction->currentText());

        data = sheet->querySubObject("Cells(int, int)", 38, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->comboBox_AccelerationVoltage_direction->currentText());

        data = sheet->querySubObject("Cells(int, int)", 38, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_AccelerationVoltage_Result->text());

    } else if (ui->checkBox_second_run_in->isChecked()){
        QAxObject* data = sheet->querySubObject("Cells(int, int)", 37, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_AccelerationVoltage_Nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 37, 16);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_AccelerationVoltage_Fact->text());

        data = sheet->querySubObject("Cells(int, int)", 37, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_AccelerationVoltage_Result->text());

        data = sheet->querySubObject("Cells(int, int)", 38, 16);
        data->dynamicCall("SetValue(const QVariant&)", ui->comboBox_AccelerationVoltage_direction->currentText());

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
    emit signalExperienceSixIsEnded();
}

void window_experience_6::on_pushButton_AccelerationVoltage_start_clicked()
{
    ui->lineEdit_AccelerationVoltage_Fact->clear();

    ui->comboBox_AccelerationVoltage_direction->clear();
        ui->comboBox_AccelerationVoltage_direction->addItem("");
        ui->comboBox_AccelerationVoltage_direction->addItem("Правое");
        ui->comboBox_AccelerationVoltage_direction->addItem("Левое");

    ui->lineEdit_AccelerationVoltage_Result->clear();

    ui->pushButton_AccelerationVoltage_stop->setEnabled(true);
    ui->pushButton_AccelerationVoltage_start->setEnabled(false);
    ui->pushButton_AccelerationVoltage_save->setEnabled(false);

    emit signal_startExperienceSix();
}

void window_experience_6::on_pushButton_AccelerationVoltage_stop_clicked()
{
    ui->pushButton_AccelerationVoltage_stop->setEnabled(false);
    ui->pushButton_AccelerationVoltage_start->setEnabled(true);
    ui->pushButton_AccelerationVoltage_save->setEnabled(false);

    emit signal_stopExperienceSix();
}

void window_experience_6::closeEvent(QCloseEvent *)
{
    ui->lineEdit_AccelerationVoltage_Fact->clear();
    ui->lineEdit_AccelerationVoltage_Result->clear();
    ui->comboBox_AccelerationVoltage_direction->clear();

    ui->pushButton_AccelerationVoltage_stop->setEnabled(false);
    ui->pushButton_AccelerationVoltage_start->setEnabled(true);
    ui->pushButton_AccelerationVoltage_save->setEnabled(false);

    emit signal_stopExperienceSix();
}
