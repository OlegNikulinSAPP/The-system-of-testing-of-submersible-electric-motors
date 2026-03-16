#include "window_experience_9.h"
#include "ui_window_experience_9.h"

#include <QAxObject>
#include <QMessageBox>

float ResistanceWinding_of_SEM_hot;
float temperatureUV_hot, temperatureVW_hot, temperatureUW_hot, averageTemperanure_hot;


window_experience_9::window_experience_9(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::window_experience_9)
{
    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint); // скрываем кнопку "?"

    this->setWindowTitle("9. Измерение сопротивления обмоток горячего ПЭД");

    // снимаем галочку в чек-боксе, если стоит в другом
    connect(ui->checkBox_second_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_first_run_in->setCheckState(Qt::Unchecked); });
    connect(ui->checkBox_first_run_in, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked)
            ui->checkBox_second_run_in->setCheckState(Qt::Unchecked); });
}

window_experience_9::~window_experience_9()
{
    delete ui;
}

void window_experience_9::slotInExperienceOneNumberProtocol(QString numberProtocol)
{
    fileProtocol = numberProtocol;
}

void window_experience_9::slotFromMainMeasurement(QString measurement)
{
    ui->label_measurement->setText(measurement);
}

void window_experience_9::slotFromMainTo_window_experience_9_windingResistance(float sumColdResistance,
                                                                               bool measuredColdResistanceIsFinished_UV,
                                                                               bool measuredColdResistanceIsFinished_VW,
                                                                               bool measuredColdResistanceIsFinished_UW,
                                                                               int temperatureColdResistance)
{
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
        temp = qRound(temp*100);
        temp = temp/100;
        ui->lineEdit_winding_UV_hot->setText(QString::number(temp));
        temperatureUV_hot =  -50 + (350 - (-50)) * (((float)temperatureColdResistance - 4000)/(20000 - 4000));
        if (temperatureUV_hot > 0)
        ui->lineEdit_temperatureResistance_hot->setText(QString::number(temperatureUV_hot));

    }

    if (measuredColdResistanceIsFinished_VW){
        float temp = sumColdResistance/3 - resistanceV_calibration;
        temp = qRound(temp*100);
        temp = temp/100;
        ui->lineEdit_winding_VW_hot->setText(QString::number(temp));
        temperatureVW_hot = -50 + (350 - (-50)) * (((float)temperatureColdResistance - 4000)/(20000 - 4000));
        ui->lineEdit_temperatureResistance_hot->setText(QString::number(temperatureVW_hot));
    }

    if (measuredColdResistanceIsFinished_UW){
        float temp = sumColdResistance/3 - resistanceW_calibration;
        temp = qRound(temp*100);
        temp = temp/100;
        ui->lineEdit_winding_UW_hot->setText(QString::number(temp));
        temperatureUW_hot = -50 + (350 - (-50)) * (((float)temperatureColdResistance - 4000)/(20000 - 4000));
        ui->lineEdit_temperatureResistance_hot->setText(QString::number(temperatureUW_hot));



    /*---------- Средняя температура после трех измерений ------------*/
    averageTemperanure_hot = (temperatureUV_hot + temperatureVW_hot + temperatureUW_hot)/3;
    averageTemperanure_hot = qRound(averageTemperanure_hot*100);
    averageTemperanure_hot = averageTemperanure_hot/100;
    ui->lineEdit_temperatureResistance_hot->setText(QString::number(averageTemperanure_hot));


    /*---------- Вычисляем фазные  и среднее сопротивлния ------------*/
    float resistanceUV = ui->lineEdit_winding_UV_hot->text().toFloat();
    float resistanceVW = ui->lineEdit_winding_VW_hot->text().toFloat();
    float resistanceUW = ui->lineEdit_winding_UW_hot->text().toFloat();
    float resistanceU, resistanceV, resistanceW, averageResistanceUVW, averageResistanceUVW20 = 0;
    float resistanceMin, resistanceMax, WindingUVWbalance;

    resistanceU = (resistanceUV + resistanceVW - resistanceUW) / 2;
    resistanceV = (resistanceUV + resistanceUW - resistanceVW) / 2;
    resistanceW = (resistanceVW + resistanceUW - resistanceUV) / 2;
    averageResistanceUVW = (resistanceU + resistanceV + resistanceW) / 3;
    averageResistanceUVW20 = averageResistanceUVW * ((235 + 20) / (235 + averageTemperanure_hot));


    resistanceU = qRound(resistanceU*100);
    resistanceU = resistanceU/100;
    ui->lineEdit_resistanceU_hot->setText(QString::number(resistanceU));

    resistanceV = qRound(resistanceV*100);
    resistanceV = resistanceV/100;
    ui->lineEdit_resistanceV_hot->setText(QString::number(resistanceV));

    resistanceW = qRound(resistanceW*100);
    resistanceW = resistanceW/100;
    ui->lineEdit_resistanceW_hot->setText(QString::number(resistanceW));

    averageResistanceUVW = qRound(averageResistanceUVW*100);
    averageResistanceUVW = averageResistanceUVW/100;
    ui->lineEdit_averageResistanceUVW_hot->setText(QString::number(averageResistanceUVW));
    //ui->lineEdit_averageResistanceUVW20->setText(QString::number(averageResistanceUVW20));


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
    WindingUVWbalance = qRound(WindingUVWbalance*100);
    WindingUVWbalance = WindingUVWbalance/100;
    ui->lineEdit_WindingUVWbalance_hot->setText(QString::number(WindingUVWbalance));
    if (WindingUVWbalance <= 2) {  // допустимый баланс (перекос) сопротивлений
       ui->lineEdit_WindingBalanceResult_hot->setText("Годен");
    }
    else {
       ui->lineEdit_WindingBalanceResult_hot->setText("Не годен");
    }

    /*Сравниваеи паспортное значение сопротивления фазы при 20 град с вычисленным*/
//    if (averageResistanceUVW20 >= (ResistanceWinding_of_SEM - ResistanceWinding_of_SEM * 0.10) &&
//        averageResistanceUVW20 <= (ResistanceWinding_of_SEM + ResistanceWinding_of_SEM * 0.05)) {
//        ui->lineEdit_ResistanceWinding_of_SEM_Result->setText("Годен");
//     }
//     else {
//        ui->lineEdit_ResistanceWinding_of_SEM_Result->setText("Не годен");
//     }
//        ui->pushButton_saveMeasurementColdWindingResistance->setEnabled(true);
//        ui->pushButton_stopMeasurementColdWindingResistance->setEnabled(false);
//        ui->pushButton_startMeasurementColdWindingResistance->setEnabled(true);
//        ui->pushButton_Calibration->setEnabled(true);

        ui->pushButton_saveMeasurementHotIsolationResistance->setEnabled(true);
        ui->pushButton_stopMeasurementHotIsolationResistance->setEnabled(false);
        ui->pushButton_startMeasurementHotIsolationResistance->setEnabled(true);
    }
}

void window_experience_9::on_pushButton_saveMeasurementHotIsolationResistance_clicked()
{
    if (ui->lineEdit_winding_UV_hot->text() == NULL || ui->lineEdit_winding_VW_hot->text() == NULL ||
            ui->lineEdit_winding_UW_hot->text() == NULL || ui->lineEdit_resistanceU_hot->text() == NULL ||
            ui->lineEdit_resistanceV_hot->text() == NULL || ui->lineEdit_resistanceW_hot->text() == NULL ||
            ui->lineEdit_averageResistanceUVW_hot->text() == NULL ||
            ui->lineEdit_WindingUVWbalance_hot->text() == NULL || ui->lineEdit_WindingBalanceResult_hot->text() == NULL){
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

    QAxObject* data = sheet->querySubObject("Cells(int, int)", 54, 2);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_winding_UV_hot->text());

    data = sheet->querySubObject("Cells(int, int)", 55, 2);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_winding_VW_hot->text());

    data = sheet->querySubObject("Cells(int, int)", 56, 2);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_winding_UW_hot->text());

    data = sheet->querySubObject("Cells(int, int)", 54, 5);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_resistanceU_hot->text());

    data = sheet->querySubObject("Cells(int, int)", 55, 5);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_resistanceV_hot->text());

    data = sheet->querySubObject("Cells(int, int)", 56, 5);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_resistanceW_hot->text());

    data = sheet->querySubObject("Cells(int, int)", 54, 8);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_temperatureResistance_hot->text());

    data = sheet->querySubObject("Cells(int, int)", 54, 19);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_WindingBalanceResult_hot->text());


    workbook->dynamicCall("Close(SaveChanges)",  true);
    excel->dynamicCall("Quit(void)");
    delete excel;

    close();
    emit signalExperienceNineIsEnded();
}

void window_experience_9::closeEvent(QCloseEvent *)
{
    ui->lineEdit_winding_UV_hot->clear();
    ui->lineEdit_winding_VW_hot->clear();
    ui->lineEdit_winding_UW_hot->clear();
    ui->lineEdit_resistanceU_hot->clear();
    ui->lineEdit_resistanceV_hot->clear();
    ui->lineEdit_resistanceW_hot->clear();
    ui->lineEdit_averageResistanceUVW_hot->clear();
    ui->lineEdit_WindingUVWbalance_hot->clear();
    ui->lineEdit_WindingBalanceResult_hot->clear();

    ui->pushButton_saveMeasurementHotIsolationResistance->setEnabled(false);
    ui->pushButton_stopMeasurementHotIsolationResistance->setEnabled(false);
    ui->pushButton_startMeasurementHotIsolationResistance->setEnabled(true);

    //emit signal_stopMeasurementHotWindingResistance();
}

void window_experience_9::on_pushButton_startMeasurementHotIsolationResistance_clicked()
{
    ui->pushButton_saveMeasurementHotIsolationResistance->setEnabled(false);
    ui->pushButton_stopMeasurementHotIsolationResistance->setEnabled(true);
    ui->pushButton_startMeasurementHotIsolationResistance->setEnabled(false);

    emit signal_startExperienceNine();
}

void window_experience_9::on_pushButton_stopMeasurementHotIsolationResistance_clicked()
{
    ui->pushButton_saveMeasurementHotIsolationResistance->setEnabled(false);
    ui->pushButton_stopMeasurementHotIsolationResistance->setEnabled(false);
    ui->pushButton_startMeasurementHotIsolationResistance->setEnabled(true);

    emit signal_stopMeasurementHotWindingResistance();
}
