#include "window_experience_2.h"
#include "ui_window_experience_2.h"

#include <QMessageBox>
#include <QFile>
#include <QAxObject>


float ResistanceWinding_of_SEM_here;
float temperatureUV, temperatureVW, temperatureUW, averageTemperanure;

window_experience_2::window_experience_2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::window_experience_2)
{
    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint); // скрываем кнопку "?"

    this->setWindowTitle("2. Измерение сопртивления обмоток холодного ПЭД");

    // снимаем галочку в чек-боксе, если стоит в другом
    connect(ui->checkBox_second_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_first_run_in->setCheckState(Qt::Unchecked); });
    connect(ui->checkBox_first_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_second_run_in->setCheckState(Qt::Unchecked); });

    ui->pushButton_Calibration->setEnabled(true);
    ui->pushButton_startMeasurementColdWindingResistance->setEnabled(true);
    ui->pushButton_stopMeasurementColdWindingResistance->setEnabled(false);
    ui->pushButton_saveMeasurementColdWindingResistance->setEnabled(false);
}

window_experience_2::~window_experience_2()
{
    delete ui;
}

void window_experience_2::on_pushButton_startMeasurementColdWindingResistance_clicked()
{
    ui->pushButton_Calibration->setEnabled(false);
    ui->pushButton_startMeasurementColdWindingResistance->setEnabled(false);
    ui->pushButton_stopMeasurementColdWindingResistance->setEnabled(true);

    ui->lineEdit_winding_UV->clear();
    ui->lineEdit_winding_VW->clear();
    ui->lineEdit_winding_UW->clear();
    ui->lineEdit_resistanceU->clear();
    ui->lineEdit_resistanceV->clear();
    ui->lineEdit_resistanceW->clear();
    ui->lineEdit_averageResistanceUVW->clear();
    ui->lineEdit_averageResistanceUVW20->clear();
    ui->lineEdit_temperatureColdResistance->clear();
    ui->lineEdit_WindingUVWbalance->clear();
    ui->lineEdit_WindingBalanceResult->clear();
    ui->lineEdit_ResistanceWinding_of_SEM_Result->clear();
    ui->label_measurement->clear();

    emit signal_startMeasurementColdWindingResistance();
}

void window_experience_2::closeEvent(QCloseEvent *)
{
    ui->lineEdit_winding_UV->clear();
    ui->lineEdit_winding_VW->clear();
    ui->lineEdit_winding_UW->clear();
    ui->lineEdit_resistanceU->clear();
    ui->lineEdit_resistanceV->clear();
    ui->lineEdit_resistanceW->clear();
    ui->lineEdit_averageResistanceUVW->clear();
    ui->lineEdit_averageResistanceUVW20->clear();
    ui->lineEdit_temperatureColdResistance->clear();
    ui->lineEdit_WindingUVWbalance->clear();
    ui->lineEdit_WindingBalanceResult->clear();
    ui->lineEdit_ResistanceWinding_of_SEM_Result->clear();

    ui->pushButton_Calibration->setEnabled(true);
    ui->pushButton_startMeasurementColdWindingResistance->setEnabled(true);
    ui->pushButton_stopMeasurementColdWindingResistance->setEnabled(false);

    emit signal_stopMeasurementColdWindingResistance();
}

void window_experience_2::on_pushButton_stopMeasurementColdWindingResistance_clicked()
{
    ui->pushButton_Calibration->setEnabled(true);
    ui->pushButton_startMeasurementColdWindingResistance->setEnabled(true);
    ui->pushButton_stopMeasurementColdWindingResistance->setEnabled(false);

    emit signal_stopMeasurementColdWindingResistance();
}

void window_experience_2::slotFromMainMeasurement(QString measurement)
{
    ui->label_measurement->setText(measurement);
}

void window_experience_2::slotFromMainTo_window_experience_2_windingResistance(float sumColdResistance, bool measuredColdResistanceIsFinished_UV,
                                                                               bool measuredColdResistanceIsFinished_VW,
                                                                               bool measuredColdResistanceIsFinished_UW,
                                                                               int temperatureColdResistance)
{
    //float temperatureUV, temperatureVW, temperatureUW, averageTemperanure;


    QAxObject * excel = new QAxObject("Excel.Application",this);

    excel->dynamicCall ("SetVisible(bool)", false); //  отображать форму
    excel->setProperty ("DisplayAlerts", false); //  отображать какую-либо предупреждающую информацию, например, сохранять ли подсказку при закрытии

    QAxObject * workbooks = excel-> querySubObject ("Workbooks"); // Получить коллекцию книг (файлы excel)
    QAxObject* workbook = workbooks->querySubObject("Open(const QString&)", "C:\\pattern\\calibration.xlsx");
    QAxObject *sheets = workbook->querySubObject("Worksheets");

    QAxObject *sheet = sheets->querySubObject("Item(int)", 1); // 2 - лист, к которому обращаемся


    //=================================================================================
    // извлекаем и сохраняем в файл
    QAxObject* cell = sheet->querySubObject("Cells(int, int)", 1, 1);
    QVariant value = cell->dynamicCall("Value()");
    //ui->lineEdit_resistanceU->setText(value.toString());
    float resistanceU_calibration = value.toFloat();

    cell = sheet->querySubObject("Cells(int, int)", 1, 2);
    value = cell->dynamicCall("Value()");
    //ui->lineEdit_resistanceV->setText(value.toString());
    float resistanceV_calibration = value.toFloat();

    cell = sheet->querySubObject("Cells(int, int)", 1, 3);
    value = cell->dynamicCall("Value()");
    //ui->lineEdit_resistanceW->setText(value.toString());
    float resistanceW_calibration = value.toFloat();

    workbook->dynamicCall("Close(SaveChanges)",  true);
    excel->dynamicCall("Quit(void)");
    delete excel;



    if (measuredColdResistanceIsFinished_UV){
        float temp = sumColdResistance/3 - resistanceU_calibration;
        temp = qRound(temp*1000);
        temp = temp/1000;
        ui->lineEdit_winding_UV->setText(QString::number(temp));
        temperatureUV =  -50 + (350 - (-50)) * (((float)temperatureColdResistance - 4000)/(20000 - 4000));
        if (temperatureUV > 0)
        ui->lineEdit_temperatureColdResistance->setText(QString::number(temperatureUV));

    }

    if (measuredColdResistanceIsFinished_VW){
        float temp = sumColdResistance/3 - resistanceV_calibration;
        temp = qRound(temp*1000);
        temp = temp/1000;
        ui->lineEdit_winding_VW->setText(QString::number(temp));
        temperatureVW = -50 + (350 - (-50)) * (((float)temperatureColdResistance - 4000)/(20000 - 4000));
        ui->lineEdit_temperatureColdResistance->setText(QString::number(temperatureVW));
    }

    if (measuredColdResistanceIsFinished_UW){
        float temp = sumColdResistance/3 - resistanceW_calibration;
        temp = qRound(temp*1000);
        temp = temp/1000;
        ui->lineEdit_winding_UW->setText(QString::number(temp));
        temperatureUW = -50 + (350 - (-50)) * (((float)temperatureColdResistance - 4000)/(20000 - 4000));
        ui->lineEdit_temperatureColdResistance->setText(QString::number(temperatureUW));



    /*---------- Средняя температура после трех измерений ------------*/
    averageTemperanure = (temperatureUV + temperatureVW + temperatureUW)/3;
    averageTemperanure = qRound(averageTemperanure*100);
    averageTemperanure = averageTemperanure/100;
    ui->lineEdit_temperatureColdResistance->setText(QString::number(averageTemperanure));


    /*---------- Вычисляем фазные  и среднее сопротивлния ------------*/
    float resistanceUV = ui->lineEdit_winding_UV->text().toFloat();
    float resistanceVW = ui->lineEdit_winding_VW->text().toFloat();
    float resistanceUW = ui->lineEdit_winding_UW->text().toFloat();
    float resistanceU, resistanceV, resistanceW, averageResistanceUVW, averageResistanceUVW20 = 0;
    float resistanceMin, resistanceMax, WindingUVWbalance;

    resistanceU = (resistanceUV + resistanceVW - resistanceUW) / 2;
    resistanceV = (resistanceUV + resistanceUW - resistanceVW) / 2;
    resistanceW = (resistanceVW + resistanceUW - resistanceUV) / 2;
    averageResistanceUVW = (resistanceU + resistanceV + resistanceW) / 3;
    averageResistanceUVW20 = averageResistanceUVW * ((235 + 20) / (235 + averageTemperanure));

    resistanceU = qRound(resistanceU*1000);
    resistanceU = resistanceU/1000;
    ui->lineEdit_resistanceU->setText(QString::number(resistanceU));

    resistanceV = qRound(resistanceV*1000);
    resistanceV = resistanceV/1000;
    ui->lineEdit_resistanceV->setText(QString::number(resistanceV));

    resistanceW = qRound(resistanceW*1000);
    resistanceW = resistanceW/1000;
    ui->lineEdit_resistanceW->setText(QString::number(resistanceW));

    averageResistanceUVW = qRound(averageResistanceUVW*1000);
    averageResistanceUVW = averageResistanceUVW/1000;
    ui->lineEdit_averageResistanceUVW->setText(QString::number(averageResistanceUVW));

    averageResistanceUVW20 = qRound(averageResistanceUVW20*1000);
    averageResistanceUVW20 = averageResistanceUVW20/1000;
    ui->lineEdit_averageResistanceUVW20->setText(QString::number(averageResistanceUVW20));


    /*---------- Определяем минимальное сопротивление -----------------*/

    if (resistanceUV < resistanceVW && resistanceUV < resistanceUW) {
        resistanceMin = resistanceUV;
    }
    else if (resistanceVW < resistanceUV && resistanceVW < resistanceUW) {
        resistanceMin = resistanceVW;
    }
    else {
        resistanceMin = resistanceUW;
    }

    /*----------- Определяем максимальное сопротивление -----------------*/

    if (resistanceUV > resistanceVW && resistanceUV > resistanceUW) {
        resistanceMax = resistanceUV;
    }
    else if (resistanceVW > resistanceUV && resistanceVW > resistanceUW) {
        resistanceMax = resistanceVW;
    }
    else {
        resistanceMax = resistanceUW;
    }

    /*----------- Определяем баланс сопротивлений ------------------------*/
    WindingUVWbalance = (1 - (resistanceMin / resistanceMax)) * 100;
    WindingUVWbalance = qRound(WindingUVWbalance*1000);
    WindingUVWbalance = WindingUVWbalance/1000;
    ui->lineEdit_WindingUVWbalance->setText(QString::number(WindingUVWbalance));
    if (WindingUVWbalance <= 2) {  // допустимый баланс (перекос) сопротивлений
       ui->lineEdit_WindingBalanceResult->setText("Годен");
    }
    else {
       ui->lineEdit_WindingBalanceResult->setText("Не годен");
    }

    /*Сравниваеи паспортное значение сопротивления фазы при 20 град с вычисленным*/
    if (averageResistanceUVW20 >= (ResistanceWinding_of_SEM_here - ResistanceWinding_of_SEM_here * 0.10) &&
        averageResistanceUVW20 <= (ResistanceWinding_of_SEM_here + ResistanceWinding_of_SEM_here * 0.05)) {
        ui->lineEdit_ResistanceWinding_of_SEM_Result->setText("Годен");
     }
     else {
        ui->lineEdit_ResistanceWinding_of_SEM_Result->setText("Не годен");
     }
        ui->pushButton_saveMeasurementColdWindingResistance->setEnabled(true);
        ui->pushButton_stopMeasurementColdWindingResistance->setEnabled(false);
        ui->pushButton_startMeasurementColdWindingResistance->setEnabled(true);
        ui->pushButton_Calibration->setEnabled(true);
    }
}



void window_experience_2::slotFromMainTo_window_experience_2_Calibration(float sumColdResistanceCalibration,
                                                                         bool measuredColdResistanceIsFinished_UV_Calibration,
                                                                         bool measuredColdResistanceIsFinished_VW_Calibration,
                                                                         bool measuredColdResistanceIsFinished_UW_Calibration)
{
    if (measuredColdResistanceIsFinished_UV_Calibration){
        ui->lineEdit_winding_UV->setText(QString::number(sumColdResistanceCalibration/3));
    }

    if (measuredColdResistanceIsFinished_VW_Calibration){
        ui->lineEdit_winding_VW->setText(QString::number(sumColdResistanceCalibration/3));
    }

    if (measuredColdResistanceIsFinished_UW_Calibration){
    ui->lineEdit_winding_UW->setText(QString::number(sumColdResistanceCalibration/3));


    QAxObject * excel = new QAxObject("Excel.Application",this);

    excel->dynamicCall ("SetVisible(bool)", false); //  отображать форму
    excel->setProperty ("DisplayAlerts", false); //  отображать какую-либо предупреждающую информацию, например, сохранять ли подсказку при закрытии

    QAxObject * workbooks = excel-> querySubObject ("Workbooks"); // Получить коллекцию книг (файлы excel)
    QAxObject* workbook = workbooks->querySubObject("Open(const QString&)", "C:\\pattern\\calibration.xlsx");
    QAxObject *sheets = workbook->querySubObject("Worksheets");

    QAxObject *sheet = sheets->querySubObject("Item(int)", 1); // 2 - лист, к которому обращаемся


    //=================================================================================
    // извлекаем и сохраняем в файл
    QAxObject* data = sheet->querySubObject("Cells(int, int)", 1, 1);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_winding_UV->text());

    data = sheet->querySubObject("Cells(int, int)", 1, 2);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_winding_VW->text());

    data = sheet->querySubObject("Cells(int, int)", 1, 3);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_winding_UW->text());

    workbook->dynamicCall("Close(SaveChanges)",  true);
    excel->dynamicCall("Quit(void)");
    delete excel;

    ui->pushButton_saveMeasurementColdWindingResistance->setEnabled(false);
    ui->pushButton_stopMeasurementColdWindingResistance->setEnabled(false);
    ui->pushButton_startMeasurementColdWindingResistance->setEnabled(true);
    ui->pushButton_Calibration->setEnabled(true);
    }
}



void window_experience_2::slotResistanceWinding_of_SEM(float ResistanceWinding_of_SEM)
{
    // принимаем из основного окна сопротивление обмоток ПЭД
    ui->lineEdit_ResistanceWinding_of_SEM->setText(QString::number(ResistanceWinding_of_SEM));
    ResistanceWinding_of_SEM_here = ResistanceWinding_of_SEM;
}

void window_experience_2::slotInExperienceOneNumberProtocol(QString numberProtocol)
{
    fileProtocol = numberProtocol;
}

void window_experience_2::on_pushButton_saveMeasurementColdWindingResistance_clicked()
{
    ui->pushButton_Calibration->setEnabled(false);
    ui->pushButton_startMeasurementColdWindingResistance->setEnabled(false);
    ui->pushButton_stopMeasurementColdWindingResistance->setEnabled(false);
    ui->pushButton_saveMeasurementColdWindingResistance->setEnabled(false);
    repaint();

    if (ui->lineEdit_winding_UV->text() == NULL || ui->lineEdit_winding_VW->text() == NULL ||
            ui->lineEdit_winding_UW->text() == NULL || ui->lineEdit_resistanceU->text() == NULL ||
            ui->lineEdit_resistanceV->text() == NULL || ui->lineEdit_resistanceW->text() == NULL ||
            ui->lineEdit_averageResistanceUVW->text() == NULL || ui->lineEdit_averageResistanceUVW20->text() == NULL ||
            ui->lineEdit_temperatureColdResistance->text() == NULL || ui->lineEdit_WindingUVWbalance->text() == NULL ||
            ui->lineEdit_ResistanceWinding_of_SEM->text() == NULL){
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
    QAxObject* data = sheet->querySubObject("Cells(int, int)", 29, 2);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_winding_UV->text());

    data = sheet->querySubObject("Cells(int, int)", 30, 2);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_winding_VW->text());

    data = sheet->querySubObject("Cells(int, int)", 31, 2);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_winding_UW->text());

    data = sheet->querySubObject("Cells(int, int)", 29, 5);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_resistanceU->text());

    data = sheet->querySubObject("Cells(int, int)", 30, 5);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_resistanceV->text());

    data = sheet->querySubObject("Cells(int, int)", 31, 5);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_resistanceW->text());

    data = sheet->querySubObject("Cells(int, int)", 29, 8);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_WindingUVWbalance->text());

    data = sheet->querySubObject("Cells(int, int)", 30, 10);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_ResistanceWinding_of_SEM->text());

    if (ui->checkBox_first_run_in->isChecked()){
        data = sheet->querySubObject("Cells(int, int)", 30, 13);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_averageResistanceUVW20->text());
    } else if (ui->checkBox_second_run_in->isChecked()){
        data = sheet->querySubObject("Cells(int, int)", 30, 16);
        data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_averageResistanceUVW20->text());
    } else {
        QMessageBox::about(this, "Предупреждение!", "Выберите номер обкатки!");
        workbook->dynamicCall("Close(SaveChanges)",  false);
        excel->dynamicCall("Quit(void)");
        delete excel;
        return;
    }

    if (ui->lineEdit_WindingBalanceResult->text() == "Годен" && ui->lineEdit_ResistanceWinding_of_SEM_Result->text() == "Годен"){
        data = sheet->querySubObject("Cells(int, int)", 30, 19);
        data->dynamicCall("SetValue(const QVariant&)", "Годен");
    } else {
        data = sheet->querySubObject("Cells(int, int)", 30, 19);
        data->dynamicCall("SetValue(const QVariant&)", "Не годен");
    }


    workbook->dynamicCall("Close(SaveChanges)",  true);
    excel->dynamicCall("Quit(void)");
    delete excel;

    close();

    ui->pushButton_Calibration->setEnabled(true);
    ui->pushButton_startMeasurementColdWindingResistance->setEnabled(true);
    ui->pushButton_stopMeasurementColdWindingResistance->setEnabled(false);
    ui->pushButton_saveMeasurementColdWindingResistance->setEnabled(false);

    ui->lineEdit_winding_UV->clear();
    ui->lineEdit_winding_VW->clear();
    ui->lineEdit_winding_UW->clear();
    ui->lineEdit_resistanceU->clear();
    ui->lineEdit_resistanceV->clear();
    ui->lineEdit_resistanceW->clear();
    ui->lineEdit_averageResistanceUVW->clear();
    ui->lineEdit_averageResistanceUVW20->clear();
    ui->lineEdit_temperatureColdResistance->clear();
    ui->lineEdit_WindingUVWbalance->clear();
    ui->lineEdit_WindingBalanceResult->clear();
    ui->lineEdit_ResistanceWinding_of_SEM_Result->clear();
    ui->label_measurement->clear();

    emit signalExperienceTwoIsEnded();
}

void window_experience_2::on_pushButton_Calibration_clicked()
{
    ui->pushButton_Calibration->setEnabled(false);
    ui->pushButton_startMeasurementColdWindingResistance->setEnabled(false);
    ui->pushButton_stopMeasurementColdWindingResistance->setEnabled(false);
    ui->pushButton_saveMeasurementColdWindingResistance->setEnabled(false);

    ui->lineEdit_winding_UV->clear();
    ui->lineEdit_winding_VW->clear();
    ui->lineEdit_winding_UW->clear();
    ui->lineEdit_resistanceU->clear();
    ui->lineEdit_resistanceV->clear();
    ui->lineEdit_resistanceW->clear();
    ui->lineEdit_averageResistanceUVW->clear();
    ui->lineEdit_averageResistanceUVW20->clear();
    ui->lineEdit_temperatureColdResistance->clear();
    ui->lineEdit_WindingUVWbalance->clear();
    ui->lineEdit_WindingBalanceResult->clear();
    ui->lineEdit_ResistanceWinding_of_SEM_Result->clear();
    ui->label_measurement->clear();

    emit signal_Calibration();
}
