#include "window_experience_10.h"
#include "ui_window_experience_10.h"

#include <QAxObject>
#include <QMessageBox>

window_experience_10::window_experience_10(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::window_experience_10)
{
    ui->setupUi(this);

    this->setWindowTitle("10. Измерение сопротивления изоляции горячего ПЭД");

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint); // скрываем кнопку "?"
    //this->setWindowFlags(this->windowFlags() & ~Qt::WindowCloseButtonHint);// скрываем кнопку "Х"

    // снимаем галочку в чек-боксе, если стоит в другом
    connect(ui->checkBox_second_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_first_run_in->setCheckState(Qt::Unchecked); });
    connect(ui->checkBox_first_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_second_run_in->setCheckState(Qt::Unchecked); });
}

window_experience_10::~window_experience_10()
{
    delete ui;
}

void window_experience_10::on_pushButton_ResistanceIsolation_hot_start_clicked()
{
    ui->pushButton_ResistanceIsolation_hot_start->setEnabled(false);
    ui->pushButton_ResistanceIsolation_hot_stop->setEnabled(true);
    ui->pushButton_ResistanceIsolation_hot_save->setEnabled(false);

    ui->lineEdit_U_hot->clear();
    ui->lineEdit_ResistanceIsolation_hot->clear();
    ui->lineEdit_ResistanceIsolation_Result_hot->clear();

    emit signal_startMeasurementHotIsolationResistance();
}

void window_experience_10::closeEvent(QCloseEvent *)
{
    ui->lineEdit_U_hot->clear();
    ui->lineEdit_ResistanceIsolation_hot->clear();
    ui->lineEdit_ResistanceIsolation_Result_hot->clear();
    emit signal_stop_measuredHotIsolation();
}

void window_experience_10::slotFromMainTo_window_experience_10_ResistanceIsolation(float ResistanceIsolation, float U)
{
    ui->lineEdit_ResistanceIsolation_hot->setText(QString::number(ResistanceIsolation));
    ui->lineEdit_U_hot->setText(QString::number(U));

    if (ui->lineEdit_ResistanceIsolation_hot->text().toFloat() > ui->lineEdit_ResistanceIsolation_Nominal_hot->text().toFloat()) {
        ui->lineEdit_ResistanceIsolation_Result_hot->setText("Годен");
    } else {
          ui->lineEdit_ResistanceIsolation_Result_hot->setText("Не годен");
        }

    ui->pushButton_ResistanceIsolation_hot_start->setEnabled(true);
    ui->pushButton_ResistanceIsolation_hot_stop->setEnabled(false);
    ui->pushButton_ResistanceIsolation_hot_save->setEnabled(true);
}

void window_experience_10::slotFromMainTo_window_experience_10_Parameters(float ResistanceIsolation)
{
    ui->lineEdit_ResistanceIsolation_Nominal_hot->setText(QString::number(ResistanceIsolation));
}

void window_experience_10::on_pushButton_ResistanceIsolation_hot_save_clicked()
{
    if (ui->lineEdit_U_hot->text() == NULL || ui->lineEdit_ResistanceIsolation_hot->text() == NULL ||
            ui->lineEdit_ResistanceIsolation_Result_hot->text() == NULL ){
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
        QAxObject* data = sheet->querySubObject("Cells(int, int)", 44, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_ResistanceIsolation_Nominal_hot->text());

        data = sheet->querySubObject("Cells(int, int)", 44, 13);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_ResistanceIsolation_hot->text());

        data = sheet->querySubObject("Cells(int, int)", 44, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_ResistanceIsolation_Result_hot->text());

    } else if (ui->checkBox_second_run_in->isChecked()){
        QAxObject* data = sheet->querySubObject("Cells(int, int)", 44, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_ResistanceIsolation_Nominal_hot->text());

        data = sheet->querySubObject("Cells(int, int)", 44, 13);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_ResistanceIsolation_hot->text());

        data = sheet->querySubObject("Cells(int, int)", 44, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_ResistanceIsolation_Result_hot->text());

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
    emit signalExperienceTenIsEnded();
}

void window_experience_10::slotInExperienceOneNumberProtocol(QString numberProtocol)
{
    fileProtocol = numberProtocol;
}

void window_experience_10::on_pushButton_ResistanceIsolation_hot_stop_clicked()
{
    ui->pushButton_ResistanceIsolation_hot_stop->setEnabled(false);
    emit signal_stop_measuredHotIsolation();
}
