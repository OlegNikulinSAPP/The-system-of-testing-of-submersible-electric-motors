#include "window_experience_14.h"
#include "ui_window_experience_14.h"

#include <QAxObject>
#include <QMessageBox>

window_experience_14::window_experience_14(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::window_experience_14)
{
    ui->setupUi(this);

    this->setWindowTitle("14. Проверка ЭД на герметичность маслом");

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

window_experience_14::~window_experience_14()
{
    delete ui;
}

void window_experience_14::slotInExperienceOneNumberProtocol(QString numberProtocol)
{
    fileProtocol = numberProtocol;
}

void window_experience_14::on_pushButton_save_clicked()
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
    QAxObject* data;
    if (ui->checkBox_result->isChecked()){
        data = sheet->querySubObject("Cells(int, int)", 51, 19);
        data->dynamicCall("SetValue(const QVariant&)", "Годен");
            if (ui->checkBox_first_run_in->isChecked()){
                    data = sheet->querySubObject("Cells(int, int)", 51, 13);
                    data->dynamicCall("SetValue(const QVariant&)", "Испытание выдержал");
                } else if (ui->checkBox_second_run_in->isChecked()){
                    data = sheet->querySubObject("Cells(int, int)", 51, 16);
                    data->dynamicCall("SetValue(const QVariant&)", "Испытание выдержал");
                }
    } else {
        data = sheet->querySubObject("Cells(int, int)", 51, 19);
        data->dynamicCall("SetValue(const QVariant&)", "Не годен");
        if (ui->checkBox_first_run_in->isChecked()){
                data = sheet->querySubObject("Cells(int, int)", 51, 13);
                data->dynamicCall("SetValue(const QVariant&)", "Испытание не выдержал");
            } else if (ui->checkBox_second_run_in->isChecked()){
                data = sheet->querySubObject("Cells(int, int)", 51, 16);
                data->dynamicCall("SetValue(const QVariant&)", "Испытание не выдержал");
            }
    }

    workbook->dynamicCall("Close(SaveChanges)",  true);
    excel->dynamicCall("Quit(void)");
    delete excel;

    close();
    emit signalExperienceFourteenIsEnded();
}

void window_experience_14::closeEvent(QCloseEvent *)
{
    ui->checkBox_result->setCheckState(Qt::Unchecked);
}
