#include "window_experience_8.h"
#include "ui_window_experience_8.h"

#include <QAxObject>
#include <QMessageBox>

window_experience_8::window_experience_8(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::window_experience_8)
{
    ui->setupUi(this);

    this->setWindowTitle("8. Измерение выбега ПЭД");

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint); // скрываем кнопку "?"

    // снимаем галочку в чек-боксе, если стоит в другом
    connect(ui->checkBox_second_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_first_run_in->setCheckState(Qt::Unchecked); });
    connect(ui->checkBox_first_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_second_run_in->setCheckState(Qt::Unchecked); });
}

window_experience_8::~window_experience_8()
{
    delete ui;
}

void window_experience_8::slotFromMainTo_window_experience_8_Parameters(float RunOut)
{
    ui->lineEdit_RunOut_nominal->setText(QString::number(RunOut));
}

void window_experience_8::slotInExperienceOneNumberProtocol(QString numberProtocol)
{
    fileProtocol = numberProtocol;
}

void window_experience_8::slotFromMainTo_window_experience_8_time(QString timeExperienceEight, int i)
{
    double time = timeExperienceEight.toDouble() + i;
    ui->lineEdit_RunOut_fact->setText(QString::number(time));
}

void window_experience_8::slotFromMainTo_window_experience_8_finish()
{
    if (ui->lineEdit_RunOut_fact->text().toFloat() > ui->lineEdit_RunOut_nominal->text().toFloat()){
        ui->lineEdit_RunOut_result->setText("Годен");
    } else {
        ui->lineEdit_RunOut_result->setText("Не годен");
    }
}

void window_experience_8::on_pushButton_save_clicked()
{
    if (ui->lineEdit_RunOut_fact->text() == NULL || ui->lineEdit_RunOut_result->text() == NULL){
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
        QAxObject* data = sheet->querySubObject("Cells(int, int)", 48, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_RunOut_nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 48, 13);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_RunOut_fact->text());

        data = sheet->querySubObject("Cells(int, int)", 48, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_RunOut_result->text());

    } else if (ui->checkBox_second_run_in->isChecked()){
        QAxObject* data = sheet->querySubObject("Cells(int, int)", 4, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_RunOut_nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 48, 16);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_RunOut_fact->text());

        data = sheet->querySubObject("Cells(int, int)", 48, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_RunOut_result->text());

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
    emit signalExperienceEightIsEnded();
}

void window_experience_8::closeEvent(QCloseEvent *)
{
    ui->lineEdit_RunOut_fact->clear();
    ui->lineEdit_RunOut_result->clear();
    emit signal_stopExperienceEight();
}

void window_experience_8::on_pushButton_startRunOut_clicked()
{
    ui->lineEdit_RunOut_fact->clear();
    ui->lineEdit_RunOut_result->clear();

    emit signal_startExperienceEight();
}

void window_experience_8::on_pushButton_stopRunOut_clicked()
{
    emit signal_stopExperienceEight();
}
