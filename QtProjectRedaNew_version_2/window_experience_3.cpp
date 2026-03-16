#include "window_experience_3.h"
#include "ui_window_experience_3.h"

#include <QAxObject>
#include <QMessageBox>


window_experience_3::window_experience_3(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::window_experience_3)
{
    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint); // скрываем кнопку "?"

    this->setWindowTitle("3. Измерение сопртивления изоляции холодного ПЭД");

    // снимаем галочку в чек-боксе, если стоит в другом
    connect(ui->checkBox_second_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_first_run_in->setCheckState(Qt::Unchecked); });
    connect(ui->checkBox_first_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_second_run_in->setCheckState(Qt::Unchecked); });
}

window_experience_3::~window_experience_3()
{
    delete ui;
}

void window_experience_3::on_pushButton_startMeasurementColdIsolationResistance_clicked()
{
    ui->pushButton_startMeasurementColdIsolationResistance->setEnabled(false);
    ui->pushButton_stopMeasurementColdIsolationResistance->setEnabled(true);
    ui->pushButton_save_experience_3->setEnabled(false);

    ui->lineEdit_U->clear();
    ui->lineEdit_ResistanceIsolation->clear();
    ui->lineEdit_PI->clear();
    ui->lineEdit_ResistanceIsolation_Result->clear();
    ui->lineEdit_PI_Result->clear();

    emit signal_startMeasurementColdIsolationResistance();
}

void window_experience_3::closeEvent(QCloseEvent *)
{
    ui->lineEdit_U->clear();
    ui->lineEdit_ResistanceIsolation->clear();
    ui->lineEdit_PI->clear();
    ui->lineEdit_ResistanceIsolation_Result->clear();
    ui->lineEdit_PI_Result->clear();

    ui->pushButton_startMeasurementColdIsolationResistance->setEnabled(true);
    ui->pushButton_stopMeasurementColdIsolationResistance->setEnabled(false);
    ui->pushButton_save_experience_3->setEnabled(false);

    emit signal_stop_measuredIsolation();
}

void window_experience_3::slotlFromMainTo_window_experience_3_ResistanceIsolation(float ResistanceIsolation, float PI, float U)
{
    ResistanceIsolation = qRound(ResistanceIsolation*100);
    ResistanceIsolation = ResistanceIsolation/100;
    ui->lineEdit_ResistanceIsolation->setText(QString::number(ResistanceIsolation));

    PI = qRound(PI*100);
    PI = PI/100;
    ui->lineEdit_PI->setText(QString::number(PI));

    U = qRound(U*100);
    U = U/100;
    ui->lineEdit_U->setText(QString::number(U));

    if (ui->lineEdit_ResistanceIsolation->text().toFloat() > ui->lineEdit_ResistanceIsolation_Nominal->text().toFloat()) {
        ui->lineEdit_ResistanceIsolation_Result->setText("Годен");
    } else {
          ui->lineEdit_ResistanceIsolation_Result->setText("Не годен");
        }

    if (ui->lineEdit_PI->text().toFloat() > ui->lineEdit_PI_Nominal->text().toFloat()) {
        ui->lineEdit_PI_Result->setText("Годен");
    } else {
          ui->lineEdit_PI_Result->setText("Не годен");
        }

    ui->pushButton_startMeasurementColdIsolationResistance->setEnabled(true);
    ui->pushButton_stopMeasurementColdIsolationResistance->setEnabled(false);
    ui->pushButton_save_experience_3->setEnabled(true);
}

void window_experience_3::slotlFromMainTo_window_experience_3_Parameters(float ResistanceIsolation_of_SEM)
{
    ui->lineEdit_ResistanceIsolation_Nominal->setText(QString::number(ResistanceIsolation_of_SEM));
}

void window_experience_3::slotInExperienceOneNumberProtocol(QString numberProtocol)
{
    fileProtocol = numberProtocol;
}

void window_experience_3::on_pushButton_save_experience_3_clicked()
{
    if (ui->lineEdit_U->text() == NULL || ui->lineEdit_ResistanceIsolation->text() == NULL ||
            ui->lineEdit_PI->text() == NULL || ui->lineEdit_U_Nominal->text() == NULL ||
            ui->lineEdit_ResistanceIsolation_Nominal->text() == NULL || ui->lineEdit_PI_Nominal->text() == NULL ||
            ui->lineEdit_ResistanceIsolation_Result->text() == NULL || ui->lineEdit_PI_Result->text() == NULL){
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
        QAxObject* data = sheet->querySubObject("Cells(int, int)", 23, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_ResistanceIsolation_Nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 23, 13);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_ResistanceIsolation->text());

        data = sheet->querySubObject("Cells(int, int)", 23, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_ResistanceIsolation_Result->text());

        data = sheet->querySubObject("Cells(int, int)", 25, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_PI_Nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 25, 13);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_PI->text());

        data = sheet->querySubObject("Cells(int, int)", 25, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_PI_Result->text());

        float Resistance_10_minutes = ui->lineEdit_ResistanceIsolation->text().toFloat() *
                ui->lineEdit_PI->text().toFloat();
        Resistance_10_minutes = qRound(Resistance_10_minutes*100);
        Resistance_10_minutes = Resistance_10_minutes/100;
        data = sheet->querySubObject("Cells(int, int)", 24, 13);
        data->dynamicCall("SetValue(const QVariant&)", Resistance_10_minutes);

        data = sheet->querySubObject("Cells(int, int)", 24, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_PI_Result->text());

    } else if (ui->checkBox_second_run_in->isChecked()){
        QAxObject* data = sheet->querySubObject("Cells(int, int)", 23, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_ResistanceIsolation_Nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 23, 16);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_ResistanceIsolation->text());

        data = sheet->querySubObject("Cells(int, int)", 23, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_ResistanceIsolation_Result->text());

        data = sheet->querySubObject("Cells(int, int)", 25, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_PI_Nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 25, 16);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_PI->text());

        data = sheet->querySubObject("Cells(int, int)", 25, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_PI_Result->text());

        float Resistance_10_minutes = ui->lineEdit_ResistanceIsolation->text().toFloat() *
                ui->lineEdit_PI->text().toFloat();
        Resistance_10_minutes = qRound(Resistance_10_minutes*100);
        Resistance_10_minutes = Resistance_10_minutes/100;
        data = sheet->querySubObject("Cells(int, int)", 24, 16);
        data->dynamicCall("SetValue(const QVariant&)", Resistance_10_minutes);

        data = sheet->querySubObject("Cells(int, int)", 24, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_PI_Result->text());

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
    emit signalExperienceThreeIsEnded();
}

void window_experience_3::on_pushButton_stopMeasurementColdIsolationResistance_clicked()
{
    ui->pushButton_stopMeasurementColdIsolationResistance->setEnabled(false);
    emit signal_stop_measuredIsolation();
}

void window_experience_3::on_spinBox_valueChanged(int arg1)
{
    emit signal_time_PI(arg1);
    //ui->spinBox->text().toInt();
}
