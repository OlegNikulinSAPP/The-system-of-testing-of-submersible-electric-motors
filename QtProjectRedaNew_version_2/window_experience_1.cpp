#include "window_experience_1.h"
#include "ui_window_experience_1.h"
#include <QFile>
#include <QAxObject>
#include <QMessageBox>
#include <QIntValidator>



window_experience_1::window_experience_1(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::window_experience_1)
{
    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint); // скрываем кнопку "?"

    this->setWindowTitle("1. Измерение напряжения пробоя масла холодного ПЭД");

    // запрещаем ввод символов - только цифры
    ui->lineEdit_oilBreakdownVoltageNominal->setValidator(new QIntValidator);
    ui->lineEdit_oilBreakdownVoltage->setValidator(new QIntValidator);

    // снимаем галочку в чек-боксе, если стоит в другом
    connect(ui->checkBox_second_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_first_run_in->setCheckState(Qt::Unchecked); });
    connect(ui->checkBox_first_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_second_run_in->setCheckState(Qt::Unchecked); });

    ui->pushButton_oilBreakdownVoltageSave->setStyleSheet("*{background-color: rgb(255, 255, 127);}");
}

window_experience_1::~window_experience_1()
{
    delete ui;
}

void window_experience_1::on_pushButton_oilBreakdownVoltageSave_clicked()
{
    if (ui->lineEdit_oilBreakdownVoltageNominal->text() == NULL || ui->lineEdit_oilBreakdownVoltage->text() == NULL){
        QMessageBox::about(this, "Предупреждение!", "Не заполнено одно из полей!");
        return;
    }

    if (ui->lineEdit_oilBreakdownVoltageNominal->text().toInt() > ui->lineEdit_oilBreakdownVoltage->text().toInt()){
        ui->lineEdit_oilBreakdownVoltageResult->setText("Не годно!");
    } else {
        ui->lineEdit_oilBreakdownVoltageResult->setText("Годно");
    }

    ui->pushButton_oilBreakdownVoltageSave->setEnabled(false);
    ui->pushButton_oilBreakdownVoltageSave->setStyleSheet("background-color: rgb(51, 255, 153)");

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
    QAxObject* data = sheet->querySubObject("Cells(int, int)", 22, 19);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_oilBreakdownVoltageResult->text());

    data = sheet->querySubObject("Cells(int, int)", 22, 10);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_oilBreakdownVoltageNominal->text());

    if (ui->checkBox_first_run_in->isChecked()){
        data = sheet->querySubObject("Cells(int, int)", 22, 13);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_oilBreakdownVoltage->text());
    } else if (ui->checkBox_second_run_in->isChecked()){
        data = sheet->querySubObject("Cells(int, int)", 22, 16);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_oilBreakdownVoltage->text());
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
    emit signalExperienceOneIsEnded();
}

void window_experience_1::closeEvent(QCloseEvent *)
{
    ui->lineEdit_oilBreakdownVoltageResult->clear();
    ui->lineEdit_oilBreakdownVoltageNominal->clear();
    ui->lineEdit_oilBreakdownVoltage->clear();

    ui->pushButton_oilBreakdownVoltageSave->setEnabled(true);
    ui->pushButton_oilBreakdownVoltageSave->setStyleSheet("*{background-color: rgb(255, 255, 127);}");
}

void window_experience_1::slotInExperienceOneNumberProtocol(QString numberProtocol)
{
    fileProtocol = numberProtocol;
}

