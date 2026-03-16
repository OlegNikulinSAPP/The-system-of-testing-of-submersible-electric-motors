#include "window_experience_12.h"
#include "ui_window_experience_12.h"

#include <QAxObject>
#include <QMessageBox>

window_experience_12::window_experience_12(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::window_experience_12)
{
    ui->setupUi(this);

    this->setWindowTitle("12. Ипытание межвитк. изо-ции обмотки на эл. прочн. (1,3Uн)");

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

window_experience_12::~window_experience_12()
{
    delete ui;
}

void window_experience_12::slotInExperienceOneNumberProtocol(QString numberProtocol)
{
    fileProtocol = numberProtocol;
}

void window_experience_12::closeEvent(QCloseEvent *)
{
    ui->checkBox_TestingIsolationInterTurn_of_SEM_result->setCheckState(Qt::Unchecked);
}

void window_experience_12::slotFromMainTo_window_experience_12_Parameters(float TestingIsolationInterTurn)
{
    ui->lineEdit_TestingIsolationInterTurn_of_SEM_Nominal->setText(QString::number(TestingIsolationInterTurn));
}

void window_experience_12::on_pushButton_TestingIsolationInterTurn_of_SEM_save_clicked()
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
    QAxObject* data = sheet->querySubObject("Cells(int, int)", 46, 10);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_TestingIsolationInterTurn_of_SEM_Nominal->text()); // номинальное значение в протокол

    if (ui->checkBox_TestingIsolationInterTurn_of_SEM_result->isChecked()){
        data = sheet->querySubObject("Cells(int, int)", 46, 19);
        data->dynamicCall("SetValue(const QVariant&)", "Годен");
            if (ui->checkBox_first_run_in->isChecked()){
                    data = sheet->querySubObject("Cells(int, int)", 46, 13);
                    data->dynamicCall("SetValue(const QVariant&)", "Испытание выдержал");
                } else if (ui->checkBox_second_run_in->isChecked()){
                    data = sheet->querySubObject("Cells(int, int)", 46, 16);
                    data->dynamicCall("SetValue(const QVariant&)", "Испытание выдержал");
                }
    } else {
        data = sheet->querySubObject("Cells(int, int)", 46, 19);
        data->dynamicCall("SetValue(const QVariant&)", "Не годен");
        if (ui->checkBox_first_run_in->isChecked()){
                data = sheet->querySubObject("Cells(int, int)", 46, 13);
                data->dynamicCall("SetValue(const QVariant&)", "Испытание не выдержал");
            } else if (ui->checkBox_second_run_in->isChecked()){
                data = sheet->querySubObject("Cells(int, int)", 46, 16);
                data->dynamicCall("SetValue(const QVariant&)", "Испытание не выдержал");
            }
    }

    workbook->dynamicCall("Close(SaveChanges)",  true);
    excel->dynamicCall("Quit(void)");
    delete excel;

    close();
    emit signalExperienceTwelveIsEnded();
}
