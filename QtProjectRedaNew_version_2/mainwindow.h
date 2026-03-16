#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <window_experience_1.h>
#include <window_experience_2.h>
#include <window_experience_3.h>
#include <window_experience_4.h>
#include <window_experience_5.h>
#include "window_experience_6.h"
#include "window_experience_7.h"
#include "window_experience_8.h"
#include "window_experience_9.h"
#include "window_experience_10.h"
#include "window_experience_11.h"
#include "window_experience_12.h"
#include "window_experience_13.h"
#include "window_experience_14.h"
#include "window_experience_15.h"
#include "password.h"


#include <QModbusDataUnit>
#include <QModbusRtuSerialMaster>
#include <QSerialPort>
#include <QDateTime>
#include <QTime>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QFileInfo> // для проверки существования файла
#include <QLabel>

static QByteArray sum_data;


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_pushButton_addBase_clicked();
    void on_pushButton_experience_1_clicked();
    void on_listWidget_typePED_currentRowChanged(int currentRow);
    void on_pushButton_saveProtocol_clicked();
    void on_pushButton_experience_2_clicked();
    void on_pushButton_experience_3_clicked();
    void on_pushButton_experience_4_clicked();
    void on_pushButton_experience_5_clicked();
    void on_pushButton_experience_6_clicked();
    void on_pushButton_experience_7_clicked();
    void on_pushButton_experience_8_clicked();
    void on_pushButton_experience_9_clicked();
    void on_pushButton_experience_10_clicked();
    void on_pushButton_experience_11_clicked();
    void on_pushButton_experience_12_clicked();
    void on_pushButton_experience_13_clicked();
    void on_pushButton_experience_14_clicked();
    void on_pushButton_experience_15_clicked();
    void on_pushButton_newTest_clicked();

    //******************************************
    void onStateChanged(int state);

    void on_tabWidget_tabBarClicked(int index);


    void on_pushButtonc_clicked();

    void on_pushButton_open_protocol_clicked();

    void on_pushButton_work_protocol_clicked();

signals:
    // этот сигнал передается во все дочерние окна, извлекается номер протокола из lineEdit_NumberProtocol
    void signalFromMainNumberProtocol(QString numberProtocol);
    // номинальные праметры
    void signalResistanceWinding_of_SEM(float ResistanceWinding_of_SEM); // во 2-е окно
    void signalFromMainTo_window_experience_3_Parameters(float ResistanceIsolation_of_SEM);
    void signalFromMainTo_window_experience_4_Parameters(float TorqueTurning);
    void signalFromMainTo_window_experience_5_Parameters(float VoltageShortCircuit, float CurrentShortCircuit, float LossesShortCircuit);
    void signalFromMainTo_window_experience_6_Parameters(float AccelerationVoltage);
    void signalFromMainTo_window_experience_7_Parameters(float Voltage, float Current, float Losses, float Vibration);
    void signalFromMainTo_window_experience_8_Parameters(float RunOut);
    void signalFromMainTo_window_experience_10_Parameters(float ResistanceIsolation);
    void signalFromMainTo_window_experience_12_Parameters(float TestingIsolationInterTurn);
    void signalFromMainTo_window_experience_13_Parameters(float TestingIsolationBody);
    // текущее состояние измерения
    void signalFromMainMeasurement(QString measurement); // во 2-е окно
    // уровень высоковольтного напряжения
    void signalInExperience_high_voltage(float high_voltage);
    // значения измеренных параметров
    void signalFromMainTo_window_experience_2_windingResistance(float sumColdResistance, bool measuredColdResistanceIsFinished_UV,
                                                                bool measuredColdResistanceIsFinished_VW,
                                                                bool measuredColdResistanceIsFinished_UW,
                                                                int temperatureColdResistance);
    void signalFromMainTo_window_experience_2_Calibration(float sumColdResistanceCalibration,
                                                          bool measuredColdResistanceIsFinished_UV_Calibration,
                                                          bool measuredColdResistanceIsFinished_VW_Calibration,
                                                          bool measuredColdResistanceIsFinished_UW_Calibration);
    void signalFromMainTo_window_experience_3_ResistanceIsolation(float ResistanceIsolation, float PI, float U);
    void signalFromMainTo_window_experience_5_voltage (float VoltageShortCircuit); // текущее напряжение КЗ
    void signalFromMainTo_window_experience_5_current (float CurrentShortCircuit); // текущий ток КЗ
    void signalFromMainTo_window_experience_5_losses (float LossesShortCircuit); // текущие потери КЗ
    void signalFromMainTo_window_experience_6_voltage_starting (float voltage_starting); // текущее напряжение
    void signalFromMainTo_window_experience_6_finish (); // опыт завершен
    void signalFromMainTo_window_experience_7_voltage_run (float voltage); // текущее напряжения во время обкатки
    void signalFromMainTo_window_experience_7_current_run (float current); // текущее тока во время обкатки
    void signalFromMainTo_window_experience_7_losses_run (float losses); // текущие потери мощности во время обкатки
    void signalFromMainTo_window_experience_7_BQ1_run (float BQ1); // текущая вибрация BQ1 во время обкатки
    void signalFromMainTo_window_experience_7_BQ2_run (float BQ2);
    void signalFromMainTo_window_experience_7_BQ3_run (float BQ3);
    void signalFromMainTo_window_experience_7_BQ4_run (float BQ4);
    void signalFromMainTo_window_experience_7_BQ5_run (float BQ5);
    void signalFromMainTo_window_experience_7_BQ6_run (float BQ6);
    void signalFromMainTo_window_experience_7_temperature_run (float temperature);
    void signalFromMainTo_window_experience_8_time (QString timeExperienceEight, int i); // время выбега +  число циклов в 1 сек
    void signalFromMainTo_window_experience_8_finish(); // опыт завершен
    void signalFromMainTo_window_experience_9_windingResistance(float sumColdResistance, bool measuredColdResistanceIsFinished_UV,
                                                                bool measuredColdResistanceIsFinished_VW,
                                                                bool measuredColdResistanceIsFinished_UW,
                                                                int temperatureColdResistance);
    void signalFromMainTo_window_experience_10_ResistanceIsolation(float ResistanceIsolation, float U);
    void signalFromMainTo_window_experience_15_temperature_oil(float temperature_oil);
    void signalFromMainTo_window_experience_15_temperature_winding(float temperature_winding);
    void signalFromMainTo_window_experience_15_connection_pressure(int connection_pressure);
    void signalFromMainTo_window_experience_15_connection_vibration(int connection_vibration);
    void signalFromMainTo_window_experience_15_connection_isolation(float connection_isolation);



    void signal_stop_run(); // сигнал об остановке обкатки в опыте 7

public slots:
    void slotExperienceOneIsEnded(); // слот для сигнала из окна первого опыта "опыт 1 закончен"
    void slotExperienceTwoIsEnded(); // слот для сигнала из окна второго опыта "опыт 2 закончен"
    void slotExperienceThreeIsEnded(); // слот для сигнала из окна третьего опыта "опыт 3 закончен"
    void slotExperienceFourIsEnded(); // слот для сигнала из окна 4 опыта "опыт 4 закончен"
    void slotExperienceFiveIsEnded(); // слот для сигнала из окна 5 опыта "опыт 5 закончен"
    void slotExperienceSixIsEnded(); // слот для сигнала из окна 6 опыта "опыт 6 закончен"
    void slotExperienceSevenIsEnded(); // слот для сигнала из окна 7 опыта "опыт 7 закончен"
    void slotExperienceEightIsEnded(); // слот для сигнала из окна 8 опыта "опыт 8 закончен"
    void slotExperienceNineIsEnded(); // слот для сигнала из окна 9 опыта "опыт 9 закончен"
    void slotExperienceTenIsEnded(); // слот для сигнала из окна 10 опыта "опыт 10 закончен"
    void slotExperienceElevenIsEnded(); // слот для сигнала из окна первого опыта "опыт 11 закончен"
    void slotlExperienceTwelveIsEnded(); // слот для сигнала из окна первого опыта "опыт 12 закончен"
    void slotlExperienceThirteenIsEnded(); // слот для сигнала из окна первого опыта "опыт 13 закончен"
    void slotlExperienceFourteenIsEnded(); // слот для сигнала из окна первого опыта "опыт 14 закончен"
    void slotlExperienceFifteenIsEnded(); // слот для сигнала из окна первого опыта "опыт 15 закончен"

    // старт
    void slot_startMeasurementColdWindingResistance(); // слот для сигнала из окна второго опыта "Начать измерения"
    void slot_Calibration(); // слотдля сигнала из окна второго опыта "Калибровка"
    void slot_startMeasurementColdIsolationResistance(); // слот для сигнала из третьего окна "Начать измерения"
    void slot_startExperienceFive(); // слот для сигнала из 5 окна "Старт 5 опыта"
    void slot_startExperienceSix(); // слот для сигнала из 6 окна "Старт 6 опыта"
    void slot_start(); // слот для сигнала из седьмого окна "Старт"
    void slot_startExperienceEight(); // слот для сигнала из 8 окна "Старт"
    void slot_startExperienceNine(); // слот для сигнала из 9 окна "Старт"
    void slot_startMeasurementHotIsolationResistance(); // слот для сигнала из десятого окна "Начать измерения"
    void slot_signal_testTMS();

    // остановить измерения
    void slot_stopMeasurementColdWindingResistance(); // слот для сигнала из окна второго опыта "Стоп"
    void slot_stop_measuredIsolation(); // слот для сигнала из 3 окна "Стоп"
    void slot_stopExperienceFive(); // слот для сигнала из 5 окна "Стоп 5 опыта"
    void slot_stopExperienceSix(); // слот для сигнала из 6 окна "Стоп"
    void slot_stop(); // слот для сигнала из седьмого окна "Стоп"
    void slot_stopExperienceEight(); // слот для сигнала из 8 окна "Стоп"
    void slot_stopMeasurementHotWindingResistance(); // слот для сигнала из 9 окна "Стоп"
    void slot_stop_measuredHotIsolation(); // слот для сигнала из 10 окна "Стоп"
    void slot_stop_signal_testTMS();

    void slot_password();

    void slot_time_PI(int arg1); // время измерения индекса поляризации

    void updateCountdown();
    void updateCountdownRun(); // время обкатки в Статусбар
    void updateCountdownExperienceEight(); // время выбега


    void responceTemperature();
    void responceResistance();
    void responceCalibration();
    void responceReleStart();
    void responceCheking();
    void responceExperienceFive();
    void responceExperienceSix();
    void responceExperienceEight();
    void responceExperienceFifteen();
    void responceResistance_hot();
    void responceTemperature_hot();

    void readModbusVFD(); // читаем в опыте 5 из ПЧ

    void searchIn_listWidget_typePED(); // функция поиска по содержимому в поисковой строке
    void searchIn_listWidget_protocols();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db; // объект базы данных
    QSqlQuery* query; // объект для Sql-запросов к базе данных
    QSqlTableModel* model; // отобразить таблицу в tableView

    window_experience_1 *experience_1;
    window_experience_2 *experience_2;
    window_experience_3 *experience_3;
    window_experience_4 *experience_4;
    window_experience_5 *experience_5;
    window_experience_6 *experience_6;
    window_experience_7 *experience_7;
    window_experience_8 *experience_8;
    window_experience_9 *experience_9;
    window_experience_10 *experience_10;
    window_experience_11 *experience_11;
    window_experience_12 *experience_12;
    window_experience_13 *experience_13;
    window_experience_14 *experience_14;
    window_experience_15 *experience_15;


    Password* password;

    QModbusClient *modbusDevice;
    QModbusClient *modbusDevice_Oven;
    //QSerialPort *serial;

    QTime time;
    QTimer *timer;

    QString device;
    const int BAUDS = /*115200*//*38400*/9600;
    const int STOP_BITS = 1;
    const int DATA_BITS = 8;
    const QString PORT = "COM7";
    const int PARITY = QSerialPort::NoParity;
    const int RESPONSE_TIME = 1000;


    QSerialPort* COMPORT;

    QMenu *menu;

};
#endif // MAINWINDOW_H
