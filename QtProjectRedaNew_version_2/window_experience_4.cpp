#include "window_experience_4.h"
#include "ui_window_experience_4.h"

#include <QAxObject>
#include <QMessageBox>

window_experience_4::window_experience_4(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::window_experience_4)
{
    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint); // скрываем кнопку "?"

    this->setWindowTitle("4. Измерение момента проворачивания вала");

    // снимаем галочку в чек-боксе, если стоит в другом
    connect(ui->checkBox_second_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_first_run_in->setCheckState(Qt::Unchecked); });
    connect(ui->checkBox_first_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_second_run_in->setCheckState(Qt::Unchecked); });
}

window_experience_4::~window_experience_4()
{
    delete ui;
}

void window_experience_4::slotFromMainTo_window_experience_4_Parameters(float TorqueTurning)
{
    ui->lineEdit_TorqueTurning_Nominal->setText(QString::number(TorqueTurning));
}

void window_experience_4::slotInExperienceOneNumberProtocol(QString numberProtocol)
{
    fileProtocol = numberProtocol;
}

void window_experience_4::on_pushButton_TorqueTurning_save_clicked()
{
    if (ui->lineEdit_TorqueTurning_Fact->text() == NULL || ui->lineEdit_TorqueTurning_Nominal->text() == NULL){
        QMessageBox::about(this, "Предупреждение!", "Не заполнено одно из полей!");
        return;
    }

    if (ui->lineEdit_TorqueTurning_Fact->text().toInt() > ui->lineEdit_TorqueTurning_Nominal->text().toInt()){
        ui->lineEdit_TorqueTurning_Result->setText("Не годно!");
    } else {
        ui->lineEdit_TorqueTurning_Result->setText("Годно");
    }

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
        QAxObject* data = sheet->querySubObject("Cells(int, int)", 32, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_TorqueTurning_Nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 32, 13);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_TorqueTurning_Fact->text());

        data = sheet->querySubObject("Cells(int, int)", 32, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_TorqueTurning_Result->text());

    } else if (ui->checkBox_second_run_in->isChecked()){
        QAxObject* data = sheet->querySubObject("Cells(int, int)", 32, 10);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_TorqueTurning_Nominal->text());

        data = sheet->querySubObject("Cells(int, int)", 32, 16);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_TorqueTurning_Fact->text());

        data = sheet->querySubObject("Cells(int, int)", 32, 19);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_TorqueTurning_Result->text());

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
    emit signalExperienceFourIsEnded();
}

void window_experience_4::closeEvent(QCloseEvent *)
{
    ui->lineEdit_TorqueTurning_Fact->clear();
    ui->lineEdit_TorqueTurning_Result->clear();
}
