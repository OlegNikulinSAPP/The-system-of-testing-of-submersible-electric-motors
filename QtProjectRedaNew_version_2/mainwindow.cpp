#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QtGui>
#include <QAxObject>
#include <QAxWidget>
#include <QApplication>
#include <QMessageBox>
#include <QList>
#include <QFile>
#include <windows.h>
#include <QDir>
#include <QtCore>
#include <iostream>
#include <QCompleter>



//###############################################################################################################
QString file("C:\\pattern\\pattern.xlsx");  // создаем путь к файлу-шаблону протокола
QString fileProtocol; // путь к протоколу, его формируем при нажатии в главном окне Сохранить

// три измерения сопротивлений обмоток холодного ПЭД проведено;
bool measuredColdResistanceIsFinished_UV = false;
bool measuredColdResistanceIsFinished_VW = false;
bool measuredColdResistanceIsFinished_UW = false;

// три калибровки обмоток холодного ПЭД проведено;
bool measuredColdResistanceIsFinished_UV_Calibration = false;
bool measuredColdResistanceIsFinished_VW_Calibration = false;
bool measuredColdResistanceIsFinished_UW_Calibration = false;

int temperatureColdResistance; // температура корпуса ПЭД во время измерений сопротивлений обмоток холодного ПЭД (целочисленный - потому что в hex)

float sumColdResistance = 0; // сумма трех измерений сопротивлений обмоток холодного ПЭД (сопротивление каждой обмотки измеряется три раза)
float sumColdResistanceCalibration = 0; // то же при калибровке

bool stop_measuredIsolation = false; // нажата кнопка СТОП в опыте 3 или 10
bool stop_measuredResistance = false; // нажата кнопка СТОП в опытах 2 и 9
bool stopExperienceEight = false;
bool stop_run = false;
bool stopExperienceTMS = false;
bool stopExperienceFive = false;
bool stopExperienceSix = false;


bool request_run_current = false;
bool request_run_voltage = false;
bool request_run_losses = false;
bool request_run_BQ1 = false;
bool request_run_BQ2 = false;
bool request_run_BQ3 = false;
bool request_run_BQ4 = false;
bool request_run_BQ5 = false;
bool request_run_BQ6 = false;
bool request_run_temperature = false;

bool protocolIsSaved = false;

// опыты проведены
bool Experience_1_IsEnded = false;
bool Experience_2_IsEnded = false;
bool Experience_3_IsEnded = false;
bool Experience_4_IsEnded = false;
bool Experience_5_IsEnded = false;
bool Experience_6_IsEnded = false;
bool Experience_7_IsEnded = false;
bool Experience_8_IsEnded = false;
bool Experience_9_IsEnded = false;
bool Experience_10_IsEnded = false;
bool Experience_11_IsEnded = false;
bool Experience_12_IsEnded = false;
bool Experience_13_IsEnded = false;
bool Experience_14_IsEnded = false;
bool Experience_15_IsEnded = false;

bool Cheking7019;
bool request_current_ExperienceFive = false;
bool request_losses_ExperienceFive = false;
bool request_voltage_ExperienceFive = false;

int counter_request_voltage_ExperienceFive = 0; // счетчик числа запросов напряжения
int counter_request_current_ExperienceFive = 0;

bool temperature_oil = false;
bool temperature_winding = false;
bool connection_pressure = false;
bool connection_vibration = false;
bool connection_isolation = false;

int timer_Experience_6 = 0; // счетчик обращений при подсчете выбега

bool windowIsOpenWhithoutStart = true;

int time_PI = 10; // время измерения PI

bool the_tab_has_already_opened = false; // вкладка уже открывалась со списком протоколов



//#########################################################################
// функция задержки времени
void delay(int sec)
{
    QTime dieTime= QTime::currentTime().addSecs(sec);
    while (QTime::currentTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}
//#########################################################################





//################################################-НАЧАЛО-#################################################################
void delay_for_Meger(int sec)
{
    QTime dieTime= QTime::currentTime().addSecs(sec);
    while (QTime::currentTime() < dieTime) {
        if (stop_measuredIsolation ||
                stop_measuredResistance ||
                stopExperienceEight ||
                stop_run ||
                stopExperienceTMS ||
                stopExperienceFive ||
                stopExperienceSix) {break;}
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)/*,*/
      //modbusDevice(nullptr) // Предварительно инициализируем указатель "нулевым"
{
    ui->setupUi(this);

    this->setWindowTitle("Программный комплекс тестирования ПЭД");

    // Список операторов
    QStringList wordList;
    wordList << "Гимранов Р.Н." << "Гарифуллин Ф.Ф." << "Мингазетдинов Р.И." << "Латыпов Р.Ф."
             << "Саблин А.И" << "Гилязов И.И.";

    QCompleter *completer = new QCompleter(wordList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEdit_operator->setCompleter(completer);


    // Widget background color properties
        ui->tabWidget->setAttribute(Qt::WA_StyledBackground);

       // ui->tab_test->setStyleSheet("background-image: url(C:/background.png);"); // фоновое изображение

        ui->tab_test->setStyleSheet("QWidget#tab_test{"
                               "background-color:#FFEFD5;"
                               "border:1px solid #eeeeee;}");

    ui->pushButton_experience_1->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_2->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_3->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_4->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_5->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_6->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_7->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_8->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_9->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_10->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_11->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_12->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_13->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_14->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_15->setStyleSheet("*{background-color: rgb(135, 206, 250);}");

    // кнопки не активны пока не сохранен протокол
    ui->pushButton_experience_1->setDisabled(true); // ОТЛАДКА!
    ui->pushButton_experience_2->setDisabled(true); // ОТЛАДКА!
    ui->pushButton_experience_3->setDisabled(true); // ОТЛАДКА!
    ui->pushButton_experience_4->setDisabled(true); // ОТЛАДКА!
    ui->pushButton_experience_5->setDisabled(true); // ОТЛАДКА!
    ui->pushButton_experience_6->setDisabled(true); // ОТЛАДКА!
    ui->pushButton_experience_7->setDisabled(true); // ОТЛАДКА!
    ui->pushButton_experience_8->setDisabled(true); // ОТЛАДКА!
    ui->pushButton_experience_9->setDisabled(true); // ОТЛАДКА!
    ui->pushButton_experience_10->setDisabled(true); // ОТЛАДКА!
    ui->pushButton_experience_11->setDisabled(true); // ОТЛАДКА!
    ui->pushButton_experience_12->setDisabled(true); // ОТЛАДКА!
    ui->pushButton_experience_13->setDisabled(true); // ОТЛАДКА!
    ui->pushButton_experience_14->setDisabled(true); // ОТЛАДКА!
    ui->pushButton_experience_15->setDisabled(true); // ОТЛАДКА!

    //serial = new QSerialPort;


    password = new Password;
    experience_1 = new window_experience_1;
    experience_2 = new window_experience_2;
    experience_3 = new window_experience_3;
    experience_4 = new window_experience_4;
    experience_5 = new window_experience_5;
    experience_6 = new window_experience_6;
    experience_7 = new window_experience_7;
    experience_8 = new window_experience_8;
    experience_9 = new window_experience_9;
    experience_10 = new window_experience_10;
    experience_11 = new window_experience_11;
    experience_12 = new window_experience_12;
    experience_13 = new window_experience_13;
    experience_14 = new window_experience_14;
    experience_15 = new window_experience_15;


    // 1
    /******************************************************************************************************************************/
    // main ->> to child window
    // номер текущего протокола (в какой протокол сохранять результат)
    connect(this, &MainWindow::signalFromMainNumberProtocol, experience_1, &window_experience_1::slotInExperienceOneNumberProtocol);

    // child window ->> to main
    // опыт сохранен
    connect(experience_1, &window_experience_1::signalExperienceOneIsEnded, this, &MainWindow::slotExperienceOneIsEnded);
    /******************************************************************************************************************************/





    // 2
    /******************************************************************************************************************************/
    // main ->> to child window
    // номер текущего протокола (в какой протокол сохранять результат)
    connect(this, &MainWindow::signalFromMainNumberProtocol, experience_2, &window_experience_2::slotInExperienceOneNumberProtocol);
    // номинальные праметры
    connect(this, &MainWindow::signalResistanceWinding_of_SEM, experience_2, &window_experience_2::slotResistanceWinding_of_SEM);
    // текущее состояние измерения
    connect(this, &MainWindow::signalFromMainMeasurement, experience_2, &window_experience_2::slotFromMainMeasurement);
    // значения измеренных параметров
    connect(this, &MainWindow::signalFromMainTo_window_experience_2_windingResistance,
            experience_2, &window_experience_2::slotFromMainTo_window_experience_2_windingResistance);
    connect(this, &MainWindow::signalFromMainTo_window_experience_2_Calibration,
            experience_2, &window_experience_2::slotFromMainTo_window_experience_2_Calibration);

    // child window ->> to main
    // опыт сохранен
    connect(experience_2, &window_experience_2::signalExperienceTwoIsEnded, this, &MainWindow::slotExperienceTwoIsEnded);
    // старт
    connect(experience_2, &window_experience_2::signal_startMeasurementColdWindingResistance,
            this, &MainWindow::slot_startMeasurementColdWindingResistance);
    connect(experience_2, &window_experience_2::signal_Calibration,
            this, &MainWindow::slot_Calibration);
    // остановить измерения
    connect(experience_2, &window_experience_2::signal_stopMeasurementColdWindingResistance,
            this, &MainWindow::slot_stopMeasurementColdWindingResistance);
    /******************************************************************************************************************************/





    // 3
    /******************************************************************************************************************************/
    // main ->> to child window
    // номер текущего протокола (в какой протокол сохранять результат)
    connect(this, &MainWindow::signalFromMainNumberProtocol, experience_3, &window_experience_3::slotInExperienceOneNumberProtocol);
    // номинальные параметры
    connect(this, &MainWindow::signalFromMainTo_window_experience_3_Parameters,
            experience_3, &window_experience_3::slotlFromMainTo_window_experience_3_Parameters);
    // значения измеренных параметров
    connect(this, &MainWindow::signalFromMainTo_window_experience_3_ResistanceIsolation,
            experience_3, &window_experience_3::slotlFromMainTo_window_experience_3_ResistanceIsolation);

    // child window ->> to main
    // опыт сохранен
    connect(experience_3, &window_experience_3::signalExperienceThreeIsEnded, this, &MainWindow::slotExperienceThreeIsEnded);
    // старт
    connect(experience_3, &window_experience_3::signal_startMeasurementColdIsolationResistance,
            this, &MainWindow::slot_startMeasurementColdIsolationResistance);
    // остановить измерения
    connect(experience_3, &window_experience_3::signal_stop_measuredIsolation,
            this, &MainWindow::slot_stop_measuredIsolation);
    // время PI
    connect(experience_3, &window_experience_3::signal_time_PI,
            this, &MainWindow::slot_time_PI);
    /******************************************************************************************************************************/





    // 4
    /******************************************************************************************************************************/
    // main ->> to child window
    // номер текущего протокола (в какой протокол сохранять результат)
    connect(this, &MainWindow::signalFromMainNumberProtocol, experience_4, &window_experience_4::slotInExperienceOneNumberProtocol);
    // номинальные параметры
    connect(this, &MainWindow::signalFromMainTo_window_experience_4_Parameters,
            experience_4, &window_experience_4::slotFromMainTo_window_experience_4_Parameters);

    // child window ->> to main
    // опыт сохранен
    connect(experience_4, &window_experience_4::signalExperienceFourIsEnded, this, &MainWindow::slotExperienceFourIsEnded);
    /******************************************************************************************************************************/





    // 5
    /******************************************************************************************************************************/
    // main ->> to child window
    // номер текущего протокола (в какой протокол сохранять результат)
    connect(this, &MainWindow::signalFromMainNumberProtocol, experience_5, &window_experience_5::slotInExperienceOneNumberProtocol);
    // номинальные параметры
    connect(this, &MainWindow::signalFromMainTo_window_experience_5_Parameters,
            experience_5, &window_experience_5::slotFromMainTo_window_experience_5_Parameters);
    // уровень высоковольтного напряжения
    connect(this, &MainWindow::signalInExperience_high_voltage, experience_5, &window_experience_5::slotInExperienceFive_high_voltage);
    // значения измеренных параметров
    connect(this, &MainWindow::signalFromMainTo_window_experience_5_voltage,
            experience_5, &window_experience_5::slotFromMainTo_window_experience_5_voltage);
    connect(this, &MainWindow::signalFromMainTo_window_experience_5_current,
            experience_5, &window_experience_5::slotFromMainTo_window_experience_5_current);
    connect(this, &MainWindow::signalFromMainTo_window_experience_5_losses,
            experience_5, &window_experience_5::slotFromMainTo_window_experience_5_losses);

    // child window ->> to main
    // опыт сохранен
    connect(experience_5, &window_experience_5::signalExperienceFiveIsEnded, this, &MainWindow::slotExperienceFiveIsEnded);
    // старт
    connect(experience_5, &window_experience_5::signal_startExperienceFive, this, &MainWindow::slot_startExperienceFive);
    // остановить измерения
    connect(experience_5, &window_experience_5::signal_stopExperienceFive, this, &MainWindow::slot_stopExperienceFive);
    /******************************************************************************************************************************/





    // 6
    /******************************************************************************************************************************/
    // main ->> to child window
    // номер текущего протокола (в какой протокол сохранять результат)
    connect(this, &MainWindow::signalFromMainNumberProtocol, experience_6, &window_experience_6::slotInExperienceOneNumberProtocol);
    // номинальные параметры
    connect(this, &MainWindow::signalFromMainTo_window_experience_6_Parameters,
            experience_6, &window_experience_6::slotFromMainTo_window_experience_6_Parameters);
    // уровень высоковольтного напряжения
    connect(this, &MainWindow::signalInExperience_high_voltage, experience_6, &window_experience_6::slotInExperienceSix_high_voltage);
    // значения измеренных параметров
    connect(this, &MainWindow::signalFromMainTo_window_experience_6_voltage_starting,
            experience_6, &window_experience_6::slotFromMainTo_window_experience_6_voltage_starting);
    connect(this, &MainWindow::signalFromMainTo_window_experience_6_finish,
            experience_6, &window_experience_6::slotFromMainTo_window_experience_6_finish); // финиш

    // child window ->> to main
    // опыт сохранен
    connect(experience_6, &window_experience_6::signalExperienceSixIsEnded, this, &MainWindow::slotExperienceSixIsEnded);
    // старт
    connect(experience_6, &window_experience_6::signal_startExperienceSix, this, &MainWindow::slot_startExperienceSix);
    // остановить измерения
    connect(experience_6, &window_experience_6::signal_stopExperienceSix, this, &MainWindow::slot_stopExperienceSix);
    /******************************************************************************************************************************/





    // 7
    /******************************************************************************************************************************/
    // main ->> to child window
    // номер текущего протокола (в какой протокол сохранять результат)
    connect(this, &MainWindow::signalFromMainNumberProtocol, experience_7, &window_experience_7::slotInExperienceOneNumberProtocol);
    // номинальные параметры
    connect(this, &MainWindow::signalFromMainTo_window_experience_7_Parameters,
            experience_7, &window_experience_7::slotFromMainTo_window_experience_7_Parameters);
    // уровень высоковольтного напряжения
    connect(this, &MainWindow::signalInExperience_high_voltage, experience_7, &window_experience_7::slotInExperienceSeven_high_voltage);
    // значения измеренных параметров
    connect(this, &MainWindow::signalFromMainTo_window_experience_7_voltage_run,
            experience_7, &window_experience_7::slotFromMainTo_window_experience_7_voltage_run);
    connect(this, &MainWindow::signalFromMainTo_window_experience_7_current_run,
            experience_7, &window_experience_7::slotFromMainTo_window_experience_7_current_run);
    connect(this, &MainWindow::signalFromMainTo_window_experience_7_losses_run,
            experience_7, &window_experience_7::slotFromMainTo_window_experience_7_losses_run);
    connect(this, &MainWindow::signalFromMainTo_window_experience_7_BQ1_run,
            experience_7, &window_experience_7::slotFromMainTo_window_experience_7_BQ1_run);
    connect(this, &MainWindow::signalFromMainTo_window_experience_7_BQ2_run,
            experience_7, &window_experience_7::slotFromMainTo_window_experience_7_BQ2_run);
    connect(this, &MainWindow::signalFromMainTo_window_experience_7_BQ3_run,
            experience_7, &window_experience_7::slotFromMainTo_window_experience_7_BQ3_run);
    connect(this, &MainWindow::signalFromMainTo_window_experience_7_BQ4_run,
            experience_7, &window_experience_7::slotFromMainTo_window_experience_7_BQ4_run);
    connect(this, &MainWindow::signalFromMainTo_window_experience_7_BQ5_run,
            experience_7, &window_experience_7::slotFromMainTo_window_experience_7_BQ5_run);
    connect(this, &MainWindow::signalFromMainTo_window_experience_7_BQ6_run,
            experience_7, &window_experience_7::slotFromMainTo_window_experience_7_BQ6_run);
    connect(this, &MainWindow::signalFromMainTo_window_experience_7_temperature_run,
            experience_7, &window_experience_7::slotFromMainTo_window_experience_7_temperature_run);
    // обкатка остановлена или завершена
    connect(this, &MainWindow::signal_stop_run,
            experience_7, &window_experience_7::slot_stop_run);

    // child window ->> to main
    // опыт сохранен
    connect(experience_7, &window_experience_7::signalExperienceSevenIsEnded, this, &MainWindow::slotExperienceSevenIsEnded);
    // старт
    connect(experience_7, &window_experience_7::signal_start, this, &MainWindow::slot_start);
    // остановить измерения
    connect(experience_7, &window_experience_7::signal_stop, this, &MainWindow::slot_stop);
    /******************************************************************************************************************************/





    // 8
    /******************************************************************************************************************************/
    // main ->> to child window
    // номер текущего протокола (в какой протокол сохранять результат)
    connect(this, &MainWindow::signalFromMainNumberProtocol, experience_8, &window_experience_8::slotInExperienceOneNumberProtocol);
    // номинальные параметры
    connect(this, &MainWindow::signalFromMainTo_window_experience_8_Parameters,
            experience_8, &window_experience_8::slotFromMainTo_window_experience_8_Parameters);
    // значения измеренных параметров
    connect(this, &MainWindow::signalFromMainTo_window_experience_8_time,
            experience_8, &window_experience_8::slotFromMainTo_window_experience_8_time);
    connect(this, &MainWindow::signalFromMainTo_window_experience_8_finish,
            experience_8, &window_experience_8::slotFromMainTo_window_experience_8_finish);

    // child window ->> to main
    // опыт сохранен
    connect(experience_8, &window_experience_8::signalExperienceEightIsEnded, this, &MainWindow::slotExperienceEightIsEnded);
    // старт
    connect(experience_8, &window_experience_8::signal_startExperienceEight, this, &MainWindow::slot_startExperienceEight);
    // остановить измерения
    connect(experience_8, &window_experience_8::signal_stopExperienceEight, this, &MainWindow::slot_stopExperienceEight);
    /******************************************************************************************************************************/





    // 9
    /******************************************************************************************************************************/
    // main ->> to child window
    // номер текущего протокола (в какой протокол сохранять результат)
    connect(this, &MainWindow::signalFromMainNumberProtocol, experience_9, &window_experience_9::slotInExperienceOneNumberProtocol);
    // текущее состояние измерения
    connect(this, &MainWindow::signalFromMainMeasurement, experience_9, &window_experience_9::slotFromMainMeasurement);
    // значения измеренных параметров
    connect(this, &MainWindow::signalFromMainTo_window_experience_9_windingResistance,
            experience_9, &window_experience_9::slotFromMainTo_window_experience_9_windingResistance);


    // child window ->> to main
    // опыт сохранен
    connect(experience_9, &window_experience_9::signalExperienceNineIsEnded, this, &MainWindow::slotExperienceNineIsEnded);
    // старт
    connect(experience_9, &window_experience_9::signal_startExperienceNine, this, &MainWindow::slot_startExperienceNine);
    // остановить измерения
    connect(experience_9, &window_experience_9::signal_stopMeasurementHotWindingResistance,
            this, &MainWindow::slot_stopMeasurementHotWindingResistance);
    /******************************************************************************************************************************/





    // 10
    /******************************************************************************************************************************/
    // main ->> to child window
    // номер текущего протокола (в какой протокол сохранять результат)
    connect(this, &MainWindow::signalFromMainNumberProtocol, experience_10, &window_experience_10::slotInExperienceOneNumberProtocol);
    // номинальные параметры
    connect(this, &MainWindow::signalFromMainTo_window_experience_10_Parameters,
            experience_10, &window_experience_10::slotFromMainTo_window_experience_10_Parameters);
    // значения измеренных параметров
    connect(this, &MainWindow::signalFromMainTo_window_experience_10_ResistanceIsolation,
            experience_10, &window_experience_10::slotFromMainTo_window_experience_10_ResistanceIsolation);

    // child window ->> to main
    // опыт сохранен
    connect(experience_10, &window_experience_10::signalExperienceTenIsEnded, this, &MainWindow::slotExperienceTenIsEnded);
    // старт
    connect(experience_10, &window_experience_10::signal_startMeasurementHotIsolationResistance,
            this, &MainWindow::slot_startMeasurementHotIsolationResistance);
    // остановить измерения
    connect(experience_10, &window_experience_10::signal_stop_measuredHotIsolation,
            this, &MainWindow::slot_stop_measuredHotIsolation);
    /******************************************************************************************************************************/





    // 11
    /******************************************************************************************************************************/
    // main ->> to child window
    // номер текущего протокола (в какой протокол сохранять результат)
    connect(this, &MainWindow::signalFromMainNumberProtocol, experience_11, &window_experience_11::slotInExperienceOneNumberProtocol);

    // child window ->> to main
    // опыт сохранен
    connect(experience_11, &window_experience_11::signalExperienceElevenIsEnded, this, &MainWindow::slotExperienceElevenIsEnded);
    /******************************************************************************************************************************/





    // 12
    /******************************************************************************************************************************/
    // main ->> to child window
    // номер текущего протокола (в какой протокол сохранять результат)
    connect(this, &MainWindow::signalFromMainNumberProtocol, experience_12, &window_experience_12::slotInExperienceOneNumberProtocol);
    // номинальные параметры
    connect(this, &MainWindow::signalFromMainTo_window_experience_12_Parameters,
            experience_12, &window_experience_12::slotFromMainTo_window_experience_12_Parameters);

    // child window ->> to main
    // опыт сохранен
    connect(experience_12, &window_experience_12::signalExperienceTwelveIsEnded, this, &MainWindow::slotlExperienceTwelveIsEnded);
    /******************************************************************************************************************************/





    // 13
    /******************************************************************************************************************************/
    // main ->> to child window
    // номер текущего протокола (в какой протокол сохранять результат)
    connect(this, &MainWindow::signalFromMainNumberProtocol, experience_13, &window_experience_13::slotInExperienceOneNumberProtocol);
    // значения номинальных параметров
    connect(this, &MainWindow::signalFromMainTo_window_experience_13_Parameters,
            experience_13, &window_experience_13::slotFromMainTo_window_experience_13_Parameters);

    // child window ->> to main
    // опыт сохранен
    connect(experience_13, &window_experience_13::signalExperienceThirteenIsEnded, this, &MainWindow::slotlExperienceThirteenIsEnded);
    /******************************************************************************************************************************/





    // 14
    /******************************************************************************************************************************/
    // main ->> to child window
    // номер текущего протокола (в какой протокол сохранять результат)
    connect(this, &MainWindow::signalFromMainNumberProtocol, experience_14, &window_experience_14::slotInExperienceOneNumberProtocol);

    // child window ->> to main
    // опыт сохранен
    connect(experience_14, &window_experience_14::signalExperienceFourteenIsEnded, this, &MainWindow::slotlExperienceFourteenIsEnded);
    /******************************************************************************************************************************/





    // 15
    /******************************************************************************************************************************/
    // main ->> to child window
    // номер текущего протокола (в какой протокол сохранять результат)
    connect(this, &MainWindow::signalFromMainNumberProtocol, experience_15, &window_experience_15::slotInExperienceOneNumberProtocol);
    // значения измеренных параметров
    connect(this, &MainWindow::signalFromMainTo_window_experience_15_temperature_oil,
            experience_15, &window_experience_15::slotFromMainTo_window_experience_15_temperature_oil);
    connect(this, &MainWindow::signalFromMainTo_window_experience_15_temperature_winding,
            experience_15, &window_experience_15::slotFromMainTo_window_experience_15_temperature_winding);
    connect(this, &MainWindow::signalFromMainTo_window_experience_15_connection_pressure,
            experience_15, &window_experience_15::slotFromMainTo_window_experience_15_connection_pressure);
    connect(this, &MainWindow::signalFromMainTo_window_experience_15_connection_vibration,
            experience_15, &window_experience_15::slotFromMainTo_window_experience_15_connection_vibration);
    connect(this, &MainWindow::signalFromMainTo_window_experience_15_connection_isolation,
            experience_15, &window_experience_15::slotFromMainTo_window_experience_15_connection_isolation);

    // child window ->> to main
    // опыт сохранен
    connect(experience_15, &window_experience_15::signalExperienceFifteenIsEnded, this, &MainWindow::slotlExperienceFifteenIsEnded);
    // старт
    connect(experience_15, &window_experience_15::signal_testTMS,
            this, &MainWindow::slot_signal_testTMS);
    // остановить измерения
    connect(experience_15, &window_experience_15::signal_stop_testTMS,
            this, &MainWindow::slot_stop_signal_testTMS);
    /******************************************************************************************************************************/



    // сигнал из окна Пароль (при нажатии ОК) в основное окно
    connect(password, &Password::signal_password, this, &MainWindow::slot_password);



    //*************************************************************************************************************
    db = QSqlDatabase::addDatabase("QSQLITE"); // создаем базу данных
    db.setDatabaseName("./baseReda.db"); // путь к базе данных
    if (db.open()){
        qDebug("open");
    }
    else {
        qDebug("no open");
    }
    query = new QSqlQuery(db); // инициализируем Sql-запрос
    // формируем запрос - создаем таблицу (наименование столбцов)
    query->exec("CREATE TABLE Base(ID INT, Model TEXT, Power_nom INT, U_nom INT, I_nom FLOAT, Turning INT,"
                "R_ColdWinding FLOAT, R_Insul INT, U_accel INT, BoringMoment FLOAT, U_k_z INT, I_k_z INT,"
                "P_h_h FLOAT, I_Idling FLOAT, U_Idling FLOAT, P_k_z INT, Time_RunDown FLOAT, VibrLevel FLOAT,"
                "TurningMoment FLOAT, P_HeatedWaste FLOAT, U_MinInsulWinding INT, U_InsulWinding INT);");
    // инициализируем таблицу
    model = new QSqlTableModel(this, db);
    // название таблицы
    model->setTable("Base");
    // заполняем модель данными из таблицы, которая была задана с помощью setTable()
    model->select();
    // устанавливаем модель в tableView
    ui->tableView_Base->setModel(model);

    for (int i = 0; i < 80; i++) {
        ui->listWidget_typePED->addItem((model->itemData(model->index(i, 1))[0]).toString());
    }

    connect(ui->lineEdit_search, &QLineEdit::returnPressed, this, &MainWindow::searchIn_listWidget_typePED); // сигнал нажатия Enter в поисковой строке
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::searchIn_listWidget_typePED() // функция поиска по содержимому в поисковой строке
{
    QString searchText = ui->lineEdit_search->text();
    int listWidgetSize = ui->listWidget_typePED->count();

    for (int k1 = 0; k1 < listWidgetSize; k1++)
        {
        if (ui->listWidget_typePED->item(k1)->text().contains(searchText))
        {
             ui->listWidget_typePED->item(k1)->setHidden(false);
        }
        else
        {
             ui->listWidget_typePED->item(k1)->setHidden(true);
        }
    }
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
MainWindow::~MainWindow()
{
    /* При завершении работы окна вызываем функцию отключения порта,
        * если указатель на объект modbusDevice существует. Если соединение
        * отсутствует, то функция не сделает ничего.
        * После чего удаляем экземпляр объекта.
        */
        if(modbusDevice)
            modbusDevice->disconnectDevice();
        delete modbusDevice;

    delete ui;
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::on_listWidget_typePED_currentRowChanged(int currentRow) // запись в окна главного окна параметров
{                                                                        // выделенного в списке ПЭД
    if (!protocolIsSaved){
    ui->lineEdit_type_of_SEM->setText((model->itemData(model->index(currentRow, 1))[0]).toString());
    ui->lineEdit_power_of_SEM->setText((model->itemData(model->index(currentRow, 2))[0]).toString());
    ui->lineEdit_voltage_of_SEM->setText((model->itemData(model->index(currentRow, 3))[0]).toString());
    ui->lineEdit_current_of_SEM->setText((model->itemData(model->index(currentRow, 4))[0]).toString());
    ui->lineEdit_RoundPerMinute_of_SEM->setText((model->itemData(model->index(currentRow, 5))[0]).toString());
    ui->lineEdit_ResistanceWinding_of_SEM->setText((model->itemData(model->index(currentRow, 6))[0]).toString());
    ui->lineEdit_ResistanceIsolation_of_SEM->setText((model->itemData(model->index(currentRow, 7))[0]).toString());
    ui->lineEdit_AccelerationVoltage_of_SEM->setText((model->itemData(model->index(currentRow, 8))[0]).toString());
    ui->lineEdit_TorqueTurning_of_SEM->setText((model->itemData(model->index(currentRow, 9))[0]).toString());
    ui->lineEdit_VoltageShortCircuit_of_SEM->setText((model->itemData(model->index(currentRow, 10))[0]).toString());
    ui->lineEdit_CurrentShortCircuit_of_SEM->setText((model->itemData(model->index(currentRow, 11))[0]).toString());
    ui->lineEdit_LossesShortCircuit_of_SEM->setText((model->itemData(model->index(currentRow, 15))[0]).toString()); //
    ui->lineEdit_CurrentIdling_of_SEM->setText((model->itemData(model->index(currentRow, 13))[0]).toString());
    ui->lineEdit_VoltageIdling_of_SEM->setText((model->itemData(model->index(currentRow, 14))[0]).toString());
    ui->lineEdit_LossesIdling_of_SEM->setText((model->itemData(model->index(currentRow, 12))[0]).toString()); //
    ui->lineEdit_RunOut_of_SEM->setText((model->itemData(model->index(currentRow, 16))[0]).toString());
    ui->lineEdit_Vibration_of_SEM->setText((model->itemData(model->index(currentRow, 17))[0]).toString());
    ui->lineEdit_Torque_of_SEM->setText((model->itemData(model->index(currentRow, 18))[0]).toString());
    ui->lineEdit_LossesHot_of_SEM->setText((model->itemData(model->index(currentRow, 19))[0]).toString());
    ui->lineEdit_TestingIsolationInterTurn_of_SEM->setText((model->itemData(model->index(currentRow, 20))[0]).toString());
    ui->lineEdit_TestingIsolationBody_of_SEM->setText((model->itemData(model->index(currentRow, 21))[0]).toString());
    } else {
        return;
    }
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::on_pushButton_saveProtocol_clicked() // создаем и сохраняем новый протокол с активацией/деактивацией
{                                                     // кнопок/окон

    // *********************-проверяем заполненение полей-*************************
    if (ui->lineEdit_NumberProtocol->text() != "Отладка") // исключаем все провекри для отладки
    {

        if (ui->lineEdit_NumberProtocol->text() == NULL)
        {
            QMessageBox::about(this, "Предупреждение!", "Не заполнено поле: \"Номер протокола\"");
            return;
        }

        if (ui->comboBox_executionGroup->currentText() == NULL)
        {
           QMessageBox::about(this, "Предупреждение!", "Не заполнено поле: \"Группа исполнения\"");
         return;
        }

        if (ui->lineEdit_Number_of_SEM->text() == NULL)
        {
            QMessageBox::about(this, "Предупреждение!", "Не заполнено поле: \"Номер ЭД\"");
            return;
        }

        if (ui->comboBox_performanceCategory->currentText() == NULL)
        {
            QMessageBox::about(this, "Предупреждение!", "Не заполнено поле: \"Категория исполнения\"");
            return;
        }

        if (ui->comboBox_typeOil->currentText() == NULL)
        {
            QMessageBox::about(this, "Предупреждение!", "Не заполнено поле: \"Тип масла\"");
            return;
        }

        if (ui->lineEdit_typeCoupling->text() == NULL)
        {
            QMessageBox::about(this, "Предупреждение!", "Не заполнено поле: \"Тип муфты\"");
            return;
        }

        if (ui->comboBox_markingCheck->currentText() == NULL)
        {
            QMessageBox::about(this, "Предупреждение!", "Не заполнено поле: \"Проверка маркировки\"");
            return;
        }

        if (ui->lineEdit_beating->text() == NULL)
        {
           QMessageBox::about(this, "Предупреждение!", "Не заполнено поле: \"Радиальное биение...\"");
            return;
        }

         if (ui->comboBox_RTI->currentText() == NULL)
        {
            QMessageBox::about(this, "Предупреждение!", "Не заполнено поле: \"Тип РТИ\"");
            return;
        }

        if (ui->lineEdit_shaftDiameter->text() == NULL)
        {
            QMessageBox::about(this, "Предупреждение!", "Не заполнено поле: \"Диаметр вала\"");
            return;
        }

        if (ui->lineEdit_typeTMS->text() == NULL)
        {
            QMessageBox::about(this, "Предупреждение!", "Не заполнено поле: \"Тип ТМС\"");
            return;
        }

        if (ui->lineEdit_durationRun->text() == NULL)
        {
            QMessageBox::about(this, "Предупреждение!", "Не заполнено поле: \"Длительность обкатки\"");
            return;
        }

        if (ui->lineEdit_shaftDeparture->text() == NULL)
        {
            QMessageBox::about(this, "Предупреждение!", "Не заполнено поле: \"Вылет вала\"");
            return;
        }

        if (ui->comboBox_articulation->currentText() == NULL)
        {
            QMessageBox::about(this, "Предупреждение!", "Не заполнено поле: \"Сочленение шлицевых соединений\"");
            return;
        }

        if (ui->lineEdit_typeBlockTMS->text() == NULL)
        {
            QMessageBox::about(this, "Предупреждение!", "Не заполнено поле: \"Тип блока ТМС\"");
            return;
        }

        if (ui->lineEdit_firmwareVersion->text() == NULL)
        {
            QMessageBox::about(this, "Предупреждение!", "Не заполнено поле: \"Версия прошивки\"");
            return;
        }

        if (ui->lineEdit_operator->text() == NULL)
        {
            QMessageBox::about(this, "Предупреждение!", "Не заполнено поле: \"Оператор\"");
            return;
        }


        // **************-проверяем заполнены ли поля параметров ПЭД-*****************
        if (ui->lineEdit_type_of_SEM->text() == NULL || ui->lineEdit_power_of_SEM->text() == NULL ||
            ui->lineEdit_voltage_of_SEM->text() == NULL || ui->lineEdit_current_of_SEM->text() == NULL ||
            ui->lineEdit_RoundPerMinute_of_SEM->text() == NULL || ui->lineEdit_ResistanceWinding_of_SEM->text() == NULL ||
            ui->lineEdit_ResistanceIsolation_of_SEM->text() == NULL || ui->lineEdit_AccelerationVoltage_of_SEM->text() == NULL ||
            ui->lineEdit_TorqueTurning_of_SEM->text() == NULL || ui->lineEdit_VoltageShortCircuit_of_SEM->text() == NULL ||
            ui->lineEdit_CurrentShortCircuit_of_SEM->text() == NULL || ui->lineEdit_LossesShortCircuit_of_SEM->text() == NULL ||
            ui->lineEdit_CurrentIdling_of_SEM->text() == NULL || ui->lineEdit_VoltageIdling_of_SEM->text() == NULL ||
            ui->lineEdit_LossesIdling_of_SEM->text() == NULL || ui->lineEdit_RunOut_of_SEM->text() == NULL ||
            ui->lineEdit_Vibration_of_SEM->text() == NULL || ui->lineEdit_Torque_of_SEM->text() == NULL ||
            ui->lineEdit_LossesHot_of_SEM->text() == NULL || ui->lineEdit_TestingIsolationInterTurn_of_SEM->text() == NULL ||
            ui->lineEdit_TestingIsolationBody_of_SEM->text() == NULL || ui->lineEdit_high_voltage->text() == NULL)
        {
            QMessageBox::about(this, "Предупреждение!", "Не заполнено одно из полей параметров ПЭД");
            return;
        }

    } // конец условия исключения отладки


    fileProtocol = "C:\\reda\\" + ui->lineEdit_NumberProtocol->text() +
    "_" + ui->lineEdit_Number_of_SEM->text() + "_" + ui->lineEdit_type_of_SEM->text() + ".xlsx";

    // проверяем, не существует ли файл (протокол) с таким же именем
    QFileInfo check_file(fileProtocol);
        // check if file exists and if yes: Is it really a file and no directory?
        if (check_file.exists() && check_file.isFile()) {
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Предупреждение!",
                                                                      "Протокол с таким номером уже существует! \nПродолжить?",
                                                                      QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::No) {
                return;
              }
            }

            // деактивируем окна после сохранения протокола
            ui->pushButton_saveProtocol->hide();
            ui->lineEdit_NumberProtocol->setDisabled(true);
            ui->comboBox_executionGroup->setDisabled(true);
            ui->lineEdit_Number_of_SEM->setDisabled(true);
            ui->comboBox_performanceCategory->setDisabled(true);
            ui->comboBox_typeOil->setDisabled(true);
            ui->lineEdit_typeCoupling->setDisabled(true);
            ui->comboBox_markingCheck->setDisabled(true);
            ui->lineEdit_beating->setDisabled(true);
            ui->comboBox_RTI->setDisabled(true);
            ui->lineEdit_shaftDiameter->setDisabled(true);
            ui->lineEdit_typeTMS->setDisabled(true);
            ui->lineEdit_durationRun->setDisabled(true);
            ui->lineEdit_shaftDeparture->setDisabled(true);
            ui->comboBox_articulation->setDisabled(true);
            ui->lineEdit_typeBlockTMS->setDisabled(true);
            ui->lineEdit_firmwareVersion->setDisabled(true);
            ui->lineEdit_type_of_SEM->setDisabled(true);
            ui->lineEdit_power_of_SEM->setDisabled(true);
            ui->lineEdit_voltage_of_SEM->setDisabled(true);
            ui->lineEdit_current_of_SEM->setDisabled(true);
            ui->lineEdit_RoundPerMinute_of_SEM->setDisabled(true);
            ui->lineEdit_ResistanceWinding_of_SEM->setDisabled(true);
            ui->lineEdit_ResistanceIsolation_of_SEM->setDisabled(true);
            ui->lineEdit_AccelerationVoltage_of_SEM->setDisabled(true);
            ui->lineEdit_TorqueTurning_of_SEM->setDisabled(true);
            ui->lineEdit_VoltageShortCircuit_of_SEM->setDisabled(true);
            ui->lineEdit_CurrentShortCircuit_of_SEM->setDisabled(true);
            ui->lineEdit_LossesShortCircuit_of_SEM->setDisabled(true);
            ui->lineEdit_CurrentIdling_of_SEM->setDisabled(true);
            ui->lineEdit_VoltageIdling_of_SEM->setDisabled(true);
            ui->lineEdit_LossesIdling_of_SEM->setDisabled(true);
            ui->lineEdit_RunOut_of_SEM->setDisabled(true);
            ui->lineEdit_Vibration_of_SEM->setDisabled(true);
            ui->lineEdit_Torque_of_SEM->setDisabled(true);
            ui->lineEdit_LossesHot_of_SEM->setDisabled(true);
            ui->lineEdit_TestingIsolationInterTurn_of_SEM->setDisabled(true);
            ui->lineEdit_TestingIsolationBody_of_SEM->setDisabled(true);
            ui->lineEdit_high_voltage->setDisabled(true);
            ui->lineEdit_old_protocol->setDisabled(false);
            ui->pushButtonc->setDisabled(false);
            ui->lineEdit_operator->setDisabled(true);



        ui->statusbar->setStyleSheet("color: red");
        ui->statusbar->showMessage("Сохраняю!");
        repaint();


    QFile::copy(file, fileProtocol);

    QAxObject* excel = new QAxObject("Excel.Application", this);

    excel->dynamicCall ("SetVisible(bool)", false); //  отображать форму
    excel->setProperty ("DisplayAlerts", false); //  отображать какую-либо предупреждающую информацию, например, сохранять ли подсказку при закрытии

    QAxObject * workbooks = excel-> querySubObject ("Workbooks"); // Получить коллекцию книг (файлы excel)
    QAxObject* workbook = workbooks->querySubObject("Open(const QString&)", fileProtocol);
    QAxObject *sheets = workbook->querySubObject("Worksheets");

    QAxObject *sheet = sheets->querySubObject("Item(int)", 1); // 2 - лист, к которому обращаемся


    //=================================================================================
    // извлекаем и сохраняем в файл
    QAxObject* data = sheet->querySubObject("Cells( int, int)", 4, 12);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_NumberProtocol->text());

    data = sheet->querySubObject("Cells( int, int)", 7, 6);
    data->dynamicCall("SetValue(const QVariant&)", ui->comboBox_executionGroup ->currentText());

    data = sheet->querySubObject("Cells( int, int)", 8, 6);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_type_of_SEM->text());

    data = sheet->querySubObject("Cells( int, int)", 9, 6);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_Number_of_SEM->text());

    data = sheet->querySubObject("Cells( int, int)", 10, 6);
    data->dynamicCall("SetValue(const QVariant&)", ui->comboBox_performanceCategory ->currentText());

    data = sheet->querySubObject("Cells( int, int)", 11, 6);
    data->dynamicCall("SetValue(const QVariant&)", ui->comboBox_typeOil ->currentText());

    data = sheet->querySubObject("Cells( int, int)", 12, 6);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_typeCoupling->text());

    if (ui->lineEdit_timeRun->text() == NULL) {
    ui->lineEdit_timeRun->setText(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm"));
    }
    data = sheet->querySubObject("Cells( int, int)", 13, 6);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_timeRun->text());

    data = sheet->querySubObject("Cells( int, int)", 14, 6);
    data->dynamicCall("SetValue(const QVariant&)", ui->comboBox_markingCheck ->currentText());

    data = sheet->querySubObject("Cells( int, int)", 15, 6);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_beating->text());

    data = sheet->querySubObject("Cells( int, int)", 7, 14);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_power_of_SEM->text());

    data = sheet->querySubObject("Cells( int, int)", 8, 14);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_voltage_of_SEM->text());

    data = sheet->querySubObject("Cells( int, int)", 9, 14);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_current_of_SEM->text());

    data = sheet->querySubObject("Cells( int, int)", 10, 14);
    data->dynamicCall("SetValue(const QVariant&)", ui->comboBox_RTI ->currentText());

    data = sheet->querySubObject("Cells( int, int)", 11, 14);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_shaftDiameter->text());

    data = sheet->querySubObject("Cells( int, int)", 12, 14);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_typeTMS->text());

    data = sheet->querySubObject("Cells( int, int)", 13, 14);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_durationRun->text());

    data = sheet->querySubObject("Cells( int, int)", 14, 14);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_shaftDeparture->text());

    data = sheet->querySubObject("Cells( int, int)", 15, 14);
    data->dynamicCall("SetValue(const QVariant&)", ui->comboBox_articulation ->currentText());

    data = sheet->querySubObject("Cells( int, int)", 8, 20);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_typeBlockTMS->text());

    data = sheet->querySubObject("Cells( int, int)", 9, 20);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_firmwareVersion->text());

    data = sheet->querySubObject("Cells( int, int)", 66, 6);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_operator->text());

    data = sheet->querySubObject("Cells( int, int)", 64, 1);
    data->dynamicCall("SetValue(const QVariant&)", ui->lineEdit_high_voltage->text());


    workbook->dynamicCall("Close(SaveChanges)", true);
    excel->dynamicCall("Quit(void)");
    delete excel;

    // активируем кнопки после сохранения протокола
    ui->pushButton_experience_1->setDisabled(false);
    ui->pushButton_experience_2->setDisabled(false);
    ui->pushButton_experience_3->setDisabled(false);
    ui->pushButton_experience_4->setDisabled(false);
    ui->pushButton_experience_5->setDisabled(false);
    ui->pushButton_experience_6->setDisabled(false);
    ui->pushButton_experience_7->setDisabled(false);
    ui->pushButton_experience_8->setDisabled(false);
    ui->pushButton_experience_9->setDisabled(false);
    ui->pushButton_experience_10->setDisabled(false);
    ui->pushButton_experience_11->setDisabled(false);
    ui->pushButton_experience_12->setDisabled(false);
    ui->pushButton_experience_13->setDisabled(false);
    ui->pushButton_experience_14->setDisabled(false);
    ui->pushButton_experience_15->setDisabled(false);
    ui->lineEdit_old_protocol->setDisabled(true);
    ui->lineEdit_old_protocol->clear();
    ui->pushButtonc->setDisabled(true);
    repaint();

    ui->statusbar->clearMessage();

    protocolIsSaved = true;
}
//################################################-КОНЕЦ-##################################################################

void MainWindow::on_pushButtonc_clicked() // ищем протокол
{
    QString fileProtocol_old("C:\\reda\\" + ui->lineEdit_old_protocol->text() + ".xlsx");

    // проверяем, существует ли файл (протокол) с таким же именем
    QFileInfo check_file(fileProtocol_old);
        // check if file exists and if yes: Is it really a file and no directory?
        if (!(check_file.exists() && check_file.isFile())) {
            QMessageBox::about(this, "Предупреждение!", "Протокол с таким номером не существует!");
            return;
            }
        QAxObject* excel = new QAxObject("Excel.Application", this);

        excel->dynamicCall ("SetVisible(bool)", false); //  отображать форму
        excel->setProperty ("DisplayAlerts", false); //  отображать какую-либо предупреждающую информацию, например, сохранять ли подсказку при закрытии

        QAxObject * workbooks = excel-> querySubObject ("Workbooks"); // Получить коллекцию книг (файлы excel)
        QAxObject* workbook = workbooks->querySubObject("Open(const QString&)", fileProtocol_old);
        QAxObject *sheets = workbook->querySubObject("Worksheets");

        QAxObject *sheet = sheets->querySubObject("Item(int)", 1); // 2 - лист, к которому обращаемся


        //=================================================================================
        // извлекаем и сохраняем в файл
        QAxObject* cell = sheet->querySubObject("Cells(int, int)", 4, 12);
        QVariant value = cell->dynamicCall("Value()");
        ui->lineEdit_NumberProtocol->setText(value.toString());

        cell = sheet->querySubObject("Cells(int, int)", 7, 6);
        value = cell->dynamicCall("Value()");
        if (value.toString() == "Д1")
            ui->comboBox_executionGroup->setCurrentIndex(1);
        else if (value.toString() == "Д2")
            ui->comboBox_executionGroup->setCurrentIndex(2);
        else if (value.toString() == "Д3")
            ui->comboBox_executionGroup->setCurrentIndex(3);

        cell = sheet->querySubObject("Cells(int, int)", 8, 6);
        value = cell->dynamicCall("Value()");
        ui->lineEdit_type_of_SEM->setText(value.toString());

        cell = sheet->querySubObject("Cells(int, int)", 9, 6);
        value = cell->dynamicCall("Value()");
        ui->lineEdit_Number_of_SEM->setText(value.toString());

        cell = sheet->querySubObject("Cells(int, int)", 10, 6);
        value = cell->dynamicCall("Value()");
        if (value.toString() == "Изготовление")
            ui->comboBox_performanceCategory->setCurrentIndex(1);
        else if (value.toString() == "Капитальный ремонт")
            ui->comboBox_performanceCategory->setCurrentIndex(2);
        else if (value.toString() == "Текущий ремонт")
            ui->comboBox_performanceCategory->setCurrentIndex(3);
        else if (value.toString() == "Тест")
            ui->comboBox_performanceCategory->setCurrentIndex(4);
        else if (value.toString() == "Модернизация")
            ui->comboBox_performanceCategory->setCurrentIndex(5);

        cell = sheet->querySubObject("Cells(int, int)", 11, 6);
        value = cell->dynamicCall("Value()");
        if (value.toString() == "МДПН-С")
            ui->comboBox_typeOil->setCurrentIndex(1);
        else if (value.toString() == "МДПН")
            ui->comboBox_typeOil->setCurrentIndex(2);
        else if (value.toString() == "МДПНС-2")
            ui->comboBox_typeOil->setCurrentIndex(3);

        cell = sheet->querySubObject("Cells(int, int)", 12, 6);
        value = cell->dynamicCall("Value()");
        ui->lineEdit_typeCoupling->setText(value.toString());

        cell = sheet->querySubObject("Cells(int, int)", 13, 6);
        value = cell->dynamicCall("Value()");
        ui->lineEdit_timeRun->setText(value.toString());

        cell = sheet->querySubObject("Cells(int, int)", 14, 6);
        value = cell->dynamicCall("Value()");
        if (value.toString() == "Соответствует")
            ui->comboBox_markingCheck->setCurrentIndex(1);
        else if (value.toString() == "Не соответствует")
            ui->comboBox_markingCheck->setCurrentIndex(2);

        cell = sheet->querySubObject("Cells(int, int)", 15, 6);
        value = cell->dynamicCall("Value()");
        ui->lineEdit_beating->setText(value.toString());

        cell = sheet->querySubObject("Cells(int, int)", 7, 14);
        value = cell->dynamicCall("Value()");
        ui->lineEdit_power_of_SEM->setText(value.toString());

        cell = sheet->querySubObject("Cells(int, int)", 8, 14);
        value = cell->dynamicCall("Value()");
        ui->lineEdit_voltage_of_SEM->setText(value.toString());

        cell = sheet->querySubObject("Cells(int, int)", 9, 14);
        value = cell->dynamicCall("Value()");
        ui->lineEdit_current_of_SEM->setText(value.toString());

        cell = sheet->querySubObject("Cells(int, int)", 10, 14);
        value = cell->dynamicCall("Value()");
        if (value.toString() == "РС-3")
            ui->comboBox_RTI->setCurrentIndex(1);
        else if (value.toString() == "АФ-15")
            ui->comboBox_RTI->setCurrentIndex(2);

        cell = sheet->querySubObject("Cells(int, int)", 11, 14);
        value = cell->dynamicCall("Value()");
        ui->lineEdit_shaftDiameter->setText(value.toString());

        cell = sheet->querySubObject("Cells(int, int)", 12, 14);
        value = cell->dynamicCall("Value()");
        ui->lineEdit_typeTMS->setText(value.toString());

        cell = sheet->querySubObject("Cells(int, int)", 13, 14);
        value = cell->dynamicCall("Value()");
        ui->lineEdit_durationRun->setText(value.toString());

        cell = sheet->querySubObject("Cells(int, int)", 14, 14);
        value = cell->dynamicCall("Value()");
        ui->lineEdit_shaftDeparture->setText(value.toString());

        cell = sheet->querySubObject("Cells(int, int)", 15, 14);
        value = cell->dynamicCall("Value()");
        if (value.toString() == "Эвольвента")
            ui->comboBox_articulation->setCurrentIndex(1);
        else if (value.toString() == "Прямозубое")
            ui->comboBox_articulation->setCurrentIndex(2);

        cell = sheet->querySubObject("Cells(int, int)", 8, 20);
        value = cell->dynamicCall("Value()");
        ui->lineEdit_typeBlockTMS->setText(value.toString());

        cell = sheet->querySubObject("Cells(int, int)", 9, 20);
        value = cell->dynamicCall("Value()");
        ui->lineEdit_firmwareVersion->setText(value.toString());

        cell = sheet->querySubObject("Cells(int, int)", 66, 6);
        value = cell->dynamicCall("Value()");
        ui->lineEdit_operator->setText(value.toString());

        cell = sheet->querySubObject("Cells(int, int)", 64, 1);
        value = cell->dynamicCall("Value()");
        ui->lineEdit_high_voltage->setText(value.toString());


//        ui->lineEdit_RoundPerMinute_of_SEM->setText("-");
//        ui->lineEdit_ResistanceWinding_of_SEM->setText("-");
//        ui->lineEdit_ResistanceIsolation_of_SEM->setText("-");
//        ui->lineEdit_AccelerationVoltage_of_SEM->setText("-");
//        ui->lineEdit_TorqueTurning_of_SEM->setText("-");
//        ui->lineEdit_VoltageShortCircuit_of_SEM->setText("-");
//        ui->lineEdit_CurrentShortCircuit_of_SEM->setText("-");
//        ui->lineEdit_LossesShortCircuit_of_SEM->setText("-");
//        ui->lineEdit_CurrentIdling_of_SEM->setText("-");
//        ui->lineEdit_VoltageIdling_of_SEM->setText("-");
//        ui->lineEdit_LossesIdling_of_SEM->setText("-");
//        ui->lineEdit_RunOut_of_SEM->setText("-");
//        ui->lineEdit_Vibration_of_SEM->setText("-");
//        ui->lineEdit_Torque_of_SEM->setText("-");
//        ui->lineEdit_LossesHot_of_SEM->setText("-");
//        ui->lineEdit_TestingIsolationInterTurn_of_SEM->setText("-");
//        ui->lineEdit_TestingIsolationBody_of_SEM->setText("-");


        cell = sheet->querySubObject("Cells(int, int)", 22, 19);
        value = cell->dynamicCall("Value()");
        if (value.toString() != NULL) {
            ui->pushButton_experience_1->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
            Experience_1_IsEnded = true;
        } else {
            ui->pushButton_experience_1->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
            Experience_1_IsEnded = false;
        }

        cell = sheet->querySubObject("Cells(int, int)", 30, 19);
        value = cell->dynamicCall("Value()");
        if (value.toString() != NULL) {
            ui->pushButton_experience_2->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
            Experience_2_IsEnded = true;
        } else {
            ui->pushButton_experience_2->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
            Experience_2_IsEnded = false;
        }

        cell = sheet->querySubObject("Cells(int, int)", 23, 19);
        value = cell->dynamicCall("Value()");
        if (value.toString() != NULL) {
            ui->pushButton_experience_3->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
            Experience_3_IsEnded = true;
        } else {
            ui->pushButton_experience_3->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
            Experience_3_IsEnded = false;
        }

        cell = sheet->querySubObject("Cells(int, int)", 32, 19);
        value = cell->dynamicCall("Value()");
        if (value.toString() != NULL) {
            ui->pushButton_experience_4->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
            Experience_4_IsEnded = true;
        } else {
            ui->pushButton_experience_4->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
            Experience_4_IsEnded = false;
        }

        cell = sheet->querySubObject("Cells(int, int)", 34, 19);
        value = cell->dynamicCall("Value()");
        if (value.toString() != NULL) {
            ui->pushButton_experience_5->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
            Experience_5_IsEnded = true;
        } else {
            ui->pushButton_experience_5->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
            Experience_5_IsEnded = false;
        }

        cell = sheet->querySubObject("Cells(int, int)", 37, 19);
        value = cell->dynamicCall("Value()");
        if (value.toString() != NULL) {
            ui->pushButton_experience_6->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
            Experience_6_IsEnded = true;
        } else {
            ui->pushButton_experience_6->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
            Experience_6_IsEnded = false;
        }

        cell = sheet->querySubObject("Cells(int, int)", 40, 19);
        value = cell->dynamicCall("Value()");
        if (value.toString() != NULL) {
            ui->pushButton_experience_7->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
            Experience_7_IsEnded = true;
        } else {
            ui->pushButton_experience_7->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
            Experience_7_IsEnded = false;
        }

        cell = sheet->querySubObject("Cells(int, int)", 48, 19);
        value = cell->dynamicCall("Value()");
        if (value.toString() != NULL) {
            ui->pushButton_experience_8->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
            Experience_8_IsEnded = true;
        } else {
            ui->pushButton_experience_8->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
            Experience_8_IsEnded = false;
        }

        cell = sheet->querySubObject("Cells(int, int)", 54, 19);
        value = cell->dynamicCall("Value()");
        if (value.toString() != NULL) {
            ui->pushButton_experience_9->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
            Experience_9_IsEnded = true;
        } else {
            ui->pushButton_experience_9->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
            Experience_9_IsEnded = false;
        }

        cell = sheet->querySubObject("Cells(int, int)", 44, 19);
        value = cell->dynamicCall("Value()");
        if (value.toString() != NULL) {
            ui->pushButton_experience_10->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
            Experience_10_IsEnded = true;
        } else {
            ui->pushButton_experience_10->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
            Experience_10_IsEnded = false;
        }

        cell = sheet->querySubObject("Cells(int, int)", 45, 19);
        value = cell->dynamicCall("Value()");
        if (value.toString() != NULL) {
            ui->pushButton_experience_11->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
            Experience_11_IsEnded = true;
        } else {
            ui->pushButton_experience_11->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
            Experience_11_IsEnded = false;
        }

        cell = sheet->querySubObject("Cells(int, int)", 46, 19);
        value = cell->dynamicCall("Value()");
        if (value.toString() != NULL) {
            ui->pushButton_experience_12->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
            Experience_12_IsEnded = true;
        } else {
            ui->pushButton_experience_12->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
            Experience_12_IsEnded = false;
        }

        cell = sheet->querySubObject("Cells(int, int)", 49, 19);
        value = cell->dynamicCall("Value()");
        if (value.toString() != NULL) {
            ui->pushButton_experience_13->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
            Experience_13_IsEnded = true;
        } else {
            ui->pushButton_experience_13->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
            Experience_13_IsEnded = false;
        }

        cell = sheet->querySubObject("Cells(int, int)", 51, 19);
        value = cell->dynamicCall("Value()");
        if (value.toString() != NULL) {
            ui->pushButton_experience_14->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
            Experience_14_IsEnded = true;
        } else {
            ui->pushButton_experience_14->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
            Experience_14_IsEnded = false;
        }

        cell = sheet->querySubObject("Cells(int, int)", 58, 19);
        value = cell->dynamicCall("Value()");
        if (value.toString() != NULL) {
            ui->pushButton_experience_15->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
            Experience_15_IsEnded = true;
        } else {
            ui->pushButton_experience_15->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
            Experience_15_IsEnded = false;
        }


        workbook->dynamicCall("Close(SaveChanges)", true);
        excel->dynamicCall("Quit(void)");
        delete excel;
}



//################################################-НАЧАЛО-#################################################################
void MainWindow::on_pushButton_newTest_clicked() // новый тест
{
    ui->pushButton_saveProtocol->show();
    ui->lineEdit_NumberProtocol->setDisabled(false);
    ui->comboBox_executionGroup->setDisabled(false);
    ui->lineEdit_Number_of_SEM->setDisabled(false);
    ui->comboBox_performanceCategory->setDisabled(false);
    ui->comboBox_typeOil->setDisabled(false);
    ui->lineEdit_typeCoupling->setDisabled(false);
    ui->comboBox_markingCheck->setDisabled(false);
    ui->lineEdit_beating->setDisabled(false);
    ui->comboBox_RTI->setDisabled(false);
    ui->lineEdit_shaftDiameter->setDisabled(false);
    ui->lineEdit_typeTMS->setDisabled(false);
    ui->lineEdit_durationRun->setDisabled(false);
    ui->lineEdit_shaftDeparture->setDisabled(false);
    ui->comboBox_articulation->setDisabled(false);
    ui->lineEdit_typeBlockTMS->setDisabled(false);
    ui->lineEdit_firmwareVersion->setDisabled(false);
    ui->lineEdit_type_of_SEM->setDisabled(false);
    ui->lineEdit_power_of_SEM->setDisabled(false);
    ui->lineEdit_voltage_of_SEM->setDisabled(false);
    ui->lineEdit_current_of_SEM->setDisabled(false);
    ui->lineEdit_RoundPerMinute_of_SEM->setDisabled(false);
    ui->lineEdit_ResistanceWinding_of_SEM->setDisabled(false);
    ui->lineEdit_ResistanceIsolation_of_SEM->setDisabled(false);
    ui->lineEdit_AccelerationVoltage_of_SEM->setDisabled(false);
    ui->lineEdit_TorqueTurning_of_SEM->setDisabled(false);
    ui->lineEdit_VoltageShortCircuit_of_SEM->setDisabled(false);
    ui->lineEdit_CurrentShortCircuit_of_SEM->setDisabled(false);
    ui->lineEdit_LossesShortCircuit_of_SEM->setDisabled(false);
    ui->lineEdit_CurrentIdling_of_SEM->setDisabled(false);
    ui->lineEdit_VoltageIdling_of_SEM->setDisabled(false);
    ui->lineEdit_LossesIdling_of_SEM->setDisabled(false);
    ui->lineEdit_RunOut_of_SEM->setDisabled(false);
    ui->lineEdit_Vibration_of_SEM->setDisabled(false);
    ui->lineEdit_Torque_of_SEM->setDisabled(false);
    ui->lineEdit_LossesHot_of_SEM->setDisabled(false);
    ui->lineEdit_TestingIsolationInterTurn_of_SEM->setDisabled(false);
    ui->lineEdit_TestingIsolationBody_of_SEM->setDisabled(false);
    ui->lineEdit_high_voltage->setDisabled(false);
    ui->lineEdit_operator->setDisabled(false);

    ui->lineEdit_old_protocol->setDisabled(false);
    ui->pushButtonc->setDisabled(false);

    ui->lineEdit_old_protocol->clear();
    ui->lineEdit_high_voltage->clear();

    ui->lineEdit_timeRun->clear();

    ui->lineEdit_NumberProtocol->clear();

    ui->comboBox_executionGroup->clear();
        ui->comboBox_executionGroup->addItem("");
        ui->comboBox_executionGroup->addItem("Д1");
        ui->comboBox_executionGroup->addItem("Д2");
        ui->comboBox_executionGroup->addItem("Д3");

    ui->lineEdit_Number_of_SEM->clear();

    ui->comboBox_performanceCategory->clear();
        ui->comboBox_performanceCategory->addItem("");
        ui->comboBox_performanceCategory->addItem("Изготовление");
        ui->comboBox_performanceCategory->addItem("Капитальный ремонт");
        ui->comboBox_performanceCategory->addItem("Текущий ремонт");
        ui->comboBox_performanceCategory->addItem("Тест");
        ui->comboBox_performanceCategory->addItem("Модернизация");


    ui->comboBox_typeOil->clear();
        ui->comboBox_typeOil->addItem("");
        ui->comboBox_typeOil->addItem("МДПН-С");
        ui->comboBox_typeOil->addItem("МДПН");
        ui->comboBox_typeOil->addItem("МДПНС-2");


    ui->lineEdit_typeCoupling->clear();

    ui->comboBox_markingCheck->clear();
        ui->comboBox_markingCheck->addItem("");
        ui->comboBox_markingCheck->addItem("Соответствует");
        ui->comboBox_markingCheck->addItem("Не соответствует");


    ui->lineEdit_beating->clear();

    ui->comboBox_RTI->clear();
        ui->comboBox_RTI->addItem("");
        ui->comboBox_RTI->addItem("РС-3");
        ui->comboBox_RTI->addItem("АФ-15");


    ui->lineEdit_shaftDiameter->clear();
    ui->lineEdit_typeTMS->clear();
    ui->lineEdit_durationRun->clear();
    ui->lineEdit_shaftDeparture->clear();

    ui->comboBox_articulation->clear();
        ui->comboBox_articulation->addItem("");
        ui->comboBox_articulation->addItem("Эвольвента");
        ui->comboBox_articulation->addItem("Прямозубое");


    ui->lineEdit_typeBlockTMS->clear();
    ui->lineEdit_firmwareVersion->clear();
    ui->lineEdit_type_of_SEM->clear();
    ui->lineEdit_power_of_SEM->clear();
    ui->lineEdit_voltage_of_SEM->clear();
    ui->lineEdit_current_of_SEM->clear();
    ui->lineEdit_RoundPerMinute_of_SEM->clear();
    ui->lineEdit_ResistanceWinding_of_SEM->clear();
    ui->lineEdit_ResistanceIsolation_of_SEM->clear();
    ui->lineEdit_AccelerationVoltage_of_SEM->clear();
    ui->lineEdit_TorqueTurning_of_SEM->clear();
    ui->lineEdit_VoltageShortCircuit_of_SEM->clear();
    ui->lineEdit_CurrentShortCircuit_of_SEM->clear();
    ui->lineEdit_LossesShortCircuit_of_SEM->clear();
    ui->lineEdit_CurrentIdling_of_SEM->clear();
    ui->lineEdit_VoltageIdling_of_SEM->clear();
    ui->lineEdit_LossesIdling_of_SEM->clear();
    ui->lineEdit_RunOut_of_SEM->clear();
    ui->lineEdit_Vibration_of_SEM->clear();
    ui->lineEdit_Torque_of_SEM->clear();
    ui->lineEdit_LossesHot_of_SEM->clear();
    ui->lineEdit_TestingIsolationInterTurn_of_SEM->clear();
    ui->lineEdit_TestingIsolationBody_of_SEM->clear();

    ui->pushButton_experience_1->setDisabled(true);
    ui->pushButton_experience_2->setDisabled(true);
    ui->pushButton_experience_3->setDisabled(true);
    ui->pushButton_experience_4->setDisabled(true);
    ui->pushButton_experience_5->setDisabled(true);
    ui->pushButton_experience_6->setDisabled(true);
    ui->pushButton_experience_7->setDisabled(true);
    ui->pushButton_experience_8->setDisabled(true);
    ui->pushButton_experience_9->setDisabled(true);
    ui->pushButton_experience_10->setDisabled(true);
    ui->pushButton_experience_11->setDisabled(true);
    ui->pushButton_experience_12->setDisabled(true);
    ui->pushButton_experience_13->setDisabled(true);
    ui->pushButton_experience_14->setDisabled(true);
    ui->pushButton_experience_15->setDisabled(true);

    ui->pushButton_experience_1->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_2->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_3->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_4->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_5->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_6->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_7->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_8->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_9->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_10->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_11->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_12->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_13->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_14->setStyleSheet("*{background-color: rgb(135, 206, 250);}");
    ui->pushButton_experience_15->setStyleSheet("*{background-color: rgb(135, 206, 250);}");

    protocolIsSaved = false;
    Experience_1_IsEnded = false;
    Experience_2_IsEnded = false;
    Experience_3_IsEnded = false;
    Experience_4_IsEnded = false;
    Experience_5_IsEnded = false;
    Experience_6_IsEnded = false;
    Experience_7_IsEnded = false;
    Experience_8_IsEnded = false;
    Experience_9_IsEnded = false;
    Experience_10_IsEnded = false;
    Experience_11_IsEnded = false;
    Experience_12_IsEnded = false;
    Experience_13_IsEnded = false;
    Experience_14_IsEnded = false;
    Experience_15_IsEnded = false;
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::on_pushButton_experience_1_clicked() // кнопка Опыт 1
{
    if (Experience_1_IsEnded) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Предупреждение!",
                                                                  "Данный опыт уже проведен! \nПродолжить?",
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
          }
    }
        emit signalFromMainNumberProtocol(fileProtocol); // сигнал от основного окна с номером текущего протокола
        experience_1->exec();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slotExperienceOneIsEnded() // слот: опыт 1 завершен
{
    ui->pushButton_experience_1->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
    Experience_1_IsEnded = true;
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::on_pushButton_experience_2_clicked() // кнопка Опыт 2
{
    // отправляем во второе окно сопротивление обмоток ПЭД из основного окна
    QString ResistanceWinding_of_SEM = ui->lineEdit_ResistanceWinding_of_SEM->text();
    for (unsigned int i = 0; i < sizeof(ResistanceWinding_of_SEM); i++) {
      if (ResistanceWinding_of_SEM[i] == 44) { ResistanceWinding_of_SEM[i] = 46; } // меняем запятую на точку
    }
    emit signalResistanceWinding_of_SEM(ResistanceWinding_of_SEM.toFloat());

    if (Experience_2_IsEnded) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Предупреждение!",
                                                                  "Данный опыт уже проведен! \nПродолжить?",
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
          }
    }

    // сигнал от основного окна с номером текущего протокола
    emit signalFromMainNumberProtocol(experience_2->fileProtocol = fileProtocol);
    experience_2->exec(); // открываем окно
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slotExperienceTwoIsEnded() // слот: опыт 2 завершен
{
    ui->pushButton_experience_2->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
    Experience_2_IsEnded = true;
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_startMeasurementColdWindingResistance() // слот: Старт Опыта 2 Измерение сопртивления
                                                              // обмоток холодного ПЭД
{
    windowIsOpenWhithoutStart = false; // окно открыто, кнопка нажата!

    stop_measuredIsolation = false;
    stop_measuredResistance = false;
    stopExperienceEight = false;
    stop_run = false;
    stopExperienceTMS = false;
    stopExperienceFive = false;
    stopExperienceSix= false;
    //**************************************************************************************************************
    /*
        * Создаем экземпляр класса QModbusRtuSerialMaster
        * Связываем сигнал errorOccurred с методом класса errorString(),
        * обрабатывающим ошибки.
        */
    modbusDevice = new QModbusRtuSerialMaster(this);
    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
                statusBar()->showMessage(modbusDevice->errorString(), 5000);
            });
    /*
        * Также соединяем сигнал изменения состояния объекта modbusDevice
        * с методом onStateChanged, где мы будем обрабатывать изменения состояния (подключение или отключение порта)
        */
    if(modbusDevice) {
        device = "Миллиомметр";
        connect(modbusDevice, &QModbusClient::stateChanged,
                this, &MainWindow::onStateChanged);
    }
    /*
        * Если экземпляр объекта по каким-либо причинам не существует,
        * то во избежание критических ошибок и проблем покидаем функцию
        */
    if (!modbusDevice)
        return;
    /*
        * Если порт не подключен, то настраиваем его и пытаемся установить соединение.
        * Функция setConnectionParameter принимает 2 параметра:
        * 1) название устанавливаемого параметра
        * 2) значение параметра
        */
    statusBar()->clearMessage();
    if (modbusDevice->state() != QModbusDevice::ConnectedState) {
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, "COM7");
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, ui->lineEdit_PARITY_miko->text().toInt());
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, ui->lineEdit_BAUDS_miko->text().toInt());
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, ui->lineEdit_DATA_BITS_miko->text().toInt());
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, ui->lineEdit_STOP_BITS_miko->text().toInt());
        modbusDevice->setTimeout(ui->lineEdit_RESPONSE_TIME_miko->text().toInt());
        modbusDevice->setNumberOfRetries(1);
        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
        }
    } else {
        modbusDevice->disconnectDevice();
    }

    if (modbusDevice->state() != QModbusDevice::ConnectedState) {
        QMessageBox::about(this, "Уведомление", "Отсутствует подключение!");
        return;
    }


//    // отправляем запрос в модуль 7019
//    QModbusReply *lastRequest = nullptr;
//    QModbusDataUnit readUnitCheck(QModbusDataUnit::HoldingRegisters, ui->lineEdit_int_addres_6->text().toInt(),
//                                  ui->lineEdit_quint16_size->text().toInt());
//    lastRequest = modbusDevice->sendReadRequest(readUnitCheck, ui->lineEdit_int_server_Addres->text().toInt());
//    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCheking);
//    else lastRequest->deleteLater();

//    delay(5);

//    if (!Cheking7019) {
//        modbusDevice->disconnectDevice();

//        modbusDevice->deleteLater();


//        modbusDevice = new QModbusRtuSerialMaster(this);
//        connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
//                    statusBar()->showMessage(modbusDevice->errorString(), 5000);
//                });

//        if(modbusDevice) {
//            connect(modbusDevice, &QModbusClient::stateChanged,
//                    this, &MainWindow::onStateChanged);
//        }

//        if (!modbusDevice)
//            return;

//        statusBar()->clearMessage(); // Подключаемся на скорости ПЧ Danfoss 38400
//        if (modbusDevice->state() != QModbusDevice::ConnectedState) {
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, ui->lineEdit_PORT_miko->text());
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, ui->lineEdit_PARITY_miko->text().toInt());
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, ui->lineEdit_BAUDS_Danfoss->text().toInt());
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, ui->lineEdit_DATA_BITS_miko->text().toInt());
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, ui->lineEdit_STOP_BITS_miko->text().toInt());
//            modbusDevice->setTimeout(ui->lineEdit_RESPONSE_TIME_miko->text().toInt());
//            modbusDevice->setNumberOfRetries(1);
//            if (!modbusDevice->connectDevice()) {
//                statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
//            }
//        } else {
//            modbusDevice->disconnectDevice();
//        }

//        // отправляем запрос в модуль 7019
//        QModbusDataUnit readUnitCheck(QModbusDataUnit::HoldingRegisters, ui->lineEdit_int_addres_6->text().toInt(),
//                                      ui->lineEdit_quint16_size->text().toInt());
//        lastRequest = modbusDevice->sendReadRequest(readUnitCheck, ui->lineEdit_int_server_Addres->text().toInt());
//        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCheking);
//        else lastRequest->deleteLater();

//        // настраиваем скорость 115200 в модуле 7019
//        auto cmd = QModbusPdu::FunctionCode(0x46);
//        auto request = QModbusRequest(cmd, QByteArray::fromHex("06000A000000010000"));
//        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
//        lastRequest = modbusDevice->sendRawRequest(request, ui->lineEdit_int_server_Addres->text().toInt());
//        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
//        else lastRequest->deleteLater();

//        delay(1);

//        modbusDevice->disconnectDevice();

//        modbusDevice->deleteLater();


//        modbusDevice = new QModbusRtuSerialMaster(this);
//        connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
//                    statusBar()->showMessage(modbusDevice->errorString(), 5000);
//                });

//        if(modbusDevice) {
//            connect(modbusDevice, &QModbusClient::stateChanged,
//                    this, &MainWindow::onStateChanged);
//        }

//        if (!modbusDevice)
//            return;

//        statusBar()->clearMessage(); // Подключаемся на скорости МИКО 115200
//        if (modbusDevice->state() != QModbusDevice::ConnectedState) {
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, ui->lineEdit_PORT_miko->text());
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, ui->lineEdit_PARITY_miko->text().toInt());
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, ui->lineEdit_BAUDS_miko->text().toInt());
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, ui->lineEdit_DATA_BITS_miko->text().toInt());
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, ui->lineEdit_STOP_BITS_miko->text().toInt());
//            modbusDevice->setTimeout(ui->lineEdit_RESPONSE_TIME_miko->text().toInt());
//            modbusDevice->setNumberOfRetries(1);
//            if (!modbusDevice->connectDevice()) {
//                statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
//            }
//        } else {
//            modbusDevice->disconnectDevice();
//        }

//        delay(1);

//        // включаем дискретный выход 7 ОВЕН МУ110-224.8Р для сброса питания модуля 7019
//        cmd = QModbusPdu::FunctionCode(0x06);
//        request = QModbusRequest(cmd, QByteArray::fromHex("000603E8"));
//        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
//        lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
//        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
//        else lastRequest->deleteLater();

//        delay(10);

//        // отключаем выход 7 ОВЕН МУ110-224.8Р для восстановления питания модуля 7019
//        cmd = QModbusPdu::FunctionCode(0x06);
//        request = QModbusRequest(cmd, QByteArray::fromHex("00060000"));
//        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
//        lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
//        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
//        else lastRequest->deleteLater();


//    }

    // отправляем команду отключения реле 3
    QModbusReply *lastRequest = nullptr;
    auto cmd = QModbusPdu::FunctionCode(0x06);
    auto request = QModbusRequest(cmd, QByteArray::fromHex("00020000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0a);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем

    // отправляем команду на останов ПЧ
    cmd = QModbusPdu::FunctionCode(0x0f);
    request = QModbusRequest(cmd, QByteArray::fromHex("000000200474049919"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x06);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFive);
    else lastRequest->deleteLater();


    int timeOfMeasurementColdWindingResistance = 20; // время измерения
    //***********************************************************************
    // включаем реле переключения RS-485
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000603E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(2); // ждем

    // включаем контактор мико
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000103E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(2); // ждем

    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Запущено измерение сопротивления обмотки UV");
    // отправляем команду на включение выхода 4
    //QModbusReply *lastRequest = nullptr;
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000303E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(5); // ждем



    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет программирование МИКО-9А");
    // программируем
    cmd = QModbusPdu::FunctionCode(0x43);
    request = QModbusRequest(cmd, QByteArray::fromHex("0101"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(5); // ждем



    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет первое измерение обмотки UV");
    // измеряем, замер 1
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет второе измерение обмотки UV");
    // измеряем, замер 2
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished())  connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем



    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет третье измерение обмотки UV");
    // измеряем, замер 3
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int { return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    measuredColdResistanceIsFinished_UV = true; // измерение обмотки UV завершено
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос датчика температуры");
    // температура
    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters, 0x06, 0x01);
    lastRequest = modbusDevice->sendReadRequest(readUnit, 0x57);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceTemperature);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Измерение сопротивления обмотки UV завершено");
    // отправляем команду на отключение выхода 4
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00030000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();

    measuredColdResistanceIsFinished_UV = false; // чтобы сигнал signalFromMainTo_window_experience_2_windingResistance не передал предыдущие значения
    sumColdResistance = 0; // обнуляем
    delay(5); // ждем



    //***********************************************************************
    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Запущено измерение сопротивления обмотки VW");
    // отправляем команду на включение выхода 5
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000403E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет первое измерение обмотки VW");
    // измеряем, замер 1
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished())  connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет второе измерение обмотки VW");
    // измеряем, замер 2
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет третье измерение обмотки VW");
    // измеряем, замер 3
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
        QModbusResponse::registerDataSizeCalculator( cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    measuredColdResistanceIsFinished_VW = true;
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос датчика температуры");
    // температура
    //QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters, 0x07, 0x01); // это уже задано выше
    lastRequest = modbusDevice->sendReadRequest(readUnit, 0x57);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceTemperature);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Измерение сопротивления обмотки VW завершено");
    // отправляем команду на отключение выхода 5
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00040000"));
    QModbusResponse::registerDataSizeCalculator( cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    measuredColdResistanceIsFinished_VW = false;
    sumColdResistance = 0;
    delay(5); // ждем



    //***********************************************************************
    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Запущено измерение сопротивления обмотки UW");
    // отправляем команду на включение выхода 6
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000503E8"));
    QModbusResponse::registerDataSizeCalculator( cmd, [](const QModbusResponse &)->int { return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет первое измерение обмотки UW");
    // измеряем, замер 1
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished())  connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет второе измерение обмотки UW");
    // измеряем, замер 2
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10; });
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет третье измерение обмотки UW");
    // измеряем, замер 3
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    measuredColdResistanceIsFinished_UW = true;
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос датчика температуры");
    // температура
    //QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters, 0x07, 0x01);
    lastRequest = modbusDevice->sendReadRequest(readUnit, 0x57);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceTemperature);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Измерение сопротивления обмоток завершено");
    // отправляем команду на отключение выхода 6
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00050000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    measuredColdResistanceIsFinished_UW = false;
    sumColdResistance = 0;
    delay(5); // ждем


    // отключаем контактор мико
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00010000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(2); // ждем

    // отключаем реле переключения RS-485
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00060000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем

    QMessageBox::about(this, "Уведомление", "Измерение завершено");

    //modbusDevice->deleteLater();
    modbusDevice->disconnectDevice();
    delete modbusDevice;
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::responceResistance() // обработка ответа от реле и МИКО-9А
{
    QModbusReply* lastRequest = qobject_cast<QModbusReply*>(sender());
    if (!lastRequest) return;
    if (lastRequest->error() == QModbusDevice::NoError){
        QModbusResponse unit = lastRequest->rawResult();
    } else {
        ui->statusbar->showMessage("Ошибка чтения!", 1000);
    }
    lastRequest->deleteLater();

    // если ответ от реле включения контактора мегаомметра
    if (lastRequest->serverAddress() == 0x10 && QString::fromUtf8(lastRequest->rawResult().data().toHex()) == "020010") {
        ui->label_statusRele->setText("Контактор магаомметра включен!");
    }

    // если ответ от МИКО-9А
    if (lastRequest->serverAddress() == 0x55) {
        //ui->lineEdit_Number_of_SEM->setText(QString::fromUtf8(lastRequest->rawResult().data().toHex()));
        QString number, res;
        res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
        for (int i = 4; i < 12; i++) {
            number += res[i];
        }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true

        float y = *reinterpret_cast<float *>(&hex);

        if (y != 0)
            sumColdResistance += y;
        if (measuredColdResistanceIsFinished_UV)
        emit signalFromMainTo_window_experience_2_windingResistance(sumColdResistance, measuredColdResistanceIsFinished_UV,
                                                                    measuredColdResistanceIsFinished_VW, measuredColdResistanceIsFinished_UW,
                                                                    temperatureColdResistance);

        if (measuredColdResistanceIsFinished_VW)
        emit signalFromMainTo_window_experience_2_windingResistance(sumColdResistance, measuredColdResistanceIsFinished_UV,
                                                                    measuredColdResistanceIsFinished_VW, measuredColdResistanceIsFinished_UW,
                                                                    temperatureColdResistance);

        if (measuredColdResistanceIsFinished_UW)
        emit signalFromMainTo_window_experience_2_windingResistance(sumColdResistance, measuredColdResistanceIsFinished_UV,
                                                                    measuredColdResistanceIsFinished_VW, measuredColdResistanceIsFinished_UW,
                                                                    temperatureColdResistance);
    }
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::responceTemperature() // обработка ответа от датчика температуры при измерении сопротивлений
                                       // обмоток холодного ПЭД
{
    QModbusReply* lastRequest = qobject_cast<QModbusReply*>(sender());
    if (!lastRequest) return;
    if (lastRequest->error() == QModbusDevice::NoError){
        QModbusDataUnit unit = lastRequest->result();
        //ui->lineEdit_type_of_SEM->setText(QString::number(unit.value(0)));
    } else {
        ui->statusbar->showMessage("Ошибка чтения!");
    }
    lastRequest->deleteLater();

    // собираем в number символы из ответа - со второго по пятый символы
    QString number, res;
    res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
    for (int i = 2; i < 6; i++) {
        number += res[i];
    }
    // преобразуем из строкового типа в целочисленный шестнадцатиричный
    bool ok;
    int hex = number.toInt(&ok, 16); // hex == 255, ok == true
    temperatureColdResistance = hex; // температура в шестнадцатиричном формате
    // передаем температуру во второе окно
    emit signalFromMainTo_window_experience_2_windingResistance(sumColdResistance, measuredColdResistanceIsFinished_UV,
                                                                measuredColdResistanceIsFinished_VW, measuredColdResistanceIsFinished_UW,
                                                                temperatureColdResistance);
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_stopMeasurementColdWindingResistance() // остановить измерения во втором окне
{
    stop_measuredResistance = true;

    if (windowIsOpenWhithoutStart == true)  // если пытаемся закрыть окно без старта
        return;

    if (!modbusDevice)
        return;

    if (modbusDevice->state() != QModbusDevice::ConnectedState) {
        return;
    } else {

    QModbusReply *lastRequest = nullptr;

    auto cmd = QModbusPdu::FunctionCode(0x06);
    auto request = QModbusRequest(cmd, QByteArray::fromHex("00030000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();

    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00040000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();

    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00050000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();

    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00060000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();

    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00010000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();

    emit signalFromMainMeasurement("Измерения остановлены пользователем");

    }
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_Calibration()
{
    windowIsOpenWhithoutStart = false; // окно открыто, кнопка нажата!

    modbusDevice = new QModbusRtuSerialMaster(this);
    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
                statusBar()->showMessage(modbusDevice->errorString(), 5000);
            });

    if(modbusDevice) {
        connect(modbusDevice, &QModbusClient::stateChanged,
                this, &MainWindow::onStateChanged);
    }

    if (!modbusDevice)
        return;

    statusBar()->clearMessage();
    if (modbusDevice->state() != QModbusDevice::ConnectedState) {
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, "COM7");
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, PARITY);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, 115200);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, DATA_BITS);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, STOP_BITS);
        modbusDevice->setTimeout(RESPONSE_TIME);
        modbusDevice->setNumberOfRetries(1);
        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
        }
    } else {
        modbusDevice->disconnectDevice();
    }

        if (modbusDevice->state() != QModbusDevice::ConnectedState) {
            QMessageBox::about(this, "Уведомление", "Отсутствует подключение!");
            return;
        }

    int timeOfMeasurementColdWindingResistance = 25; // время измерения
    //***********************************************************************
    // отправляем команду отключения реле 3
    QModbusReply *lastRequest = nullptr;
    auto cmd = QModbusPdu::FunctionCode(0x06);
    auto request = QModbusRequest(cmd, QByteArray::fromHex("00020000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0a);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем

    // отправляем команду на останов ПЧ
    cmd = QModbusPdu::FunctionCode(0x0f);
    request = QModbusRequest(cmd, QByteArray::fromHex("000000200474049919"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x06);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFive);
    else lastRequest->deleteLater();


    // включаем реле переключения RS-485
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000603E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(2); // ждем

    // включаем контактор мико
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000103E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(2); // ждем

    emit signalFromMainMeasurement("Запущена калибровка UV");
    // отправляем команду на включение выхода 1 (реле 1)
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000303E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(5); // ждем



    emit signalFromMainMeasurement("Идет программирование МИКО-9А");
    // программируем
    cmd = QModbusPdu::FunctionCode(0x43);
    request = QModbusRequest(cmd, QByteArray::fromHex("0101"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(5); // ждем



    emit signalFromMainMeasurement("Идет первая калибровка UV");
    // измеряем, замер 1
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(5); // ждем


    emit signalFromMainMeasurement("Идет вторая калибровка UV");
    // измеряем, замер 2
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished())  connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(5); // ждем


    emit signalFromMainMeasurement("Идет третья калибровка UV");
    // измеряем, замер 3
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int { return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    measuredColdResistanceIsFinished_UV_Calibration = true; // измерение обмотки UV завершено
    delay(5); // ждем


    emit signalFromMainMeasurement("Калибровка UV завершена");
    // отправляем команду на отключение выхода 1 (реле 1)
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00030000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();

    measuredColdResistanceIsFinished_UV_Calibration = false; // чтобы сигнал signalFromMainTo_window_experience_2_windingResistance не передал предыдущие значения
    sumColdResistanceCalibration = 0; // обнуляем
    delay(5); // ждем



    //***********************************************************************

    emit signalFromMainMeasurement("Запущена калибровка VW");
    // отправляем команду на включение выхода 2 (реле 2)
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000403E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(5); // ждем


    emit signalFromMainMeasurement("Идет первая калибровка VW");
    // измеряем, замер 1
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished())  connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(5); // ждем


    emit signalFromMainMeasurement("Идет вторая калибровка VW");
    // измеряем, замер 2
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(5); // ждем


    emit signalFromMainMeasurement("Идет третья калибровка VW");
    // измеряем, замер 3
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
        QModbusResponse::registerDataSizeCalculator( cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    measuredColdResistanceIsFinished_VW_Calibration = true;
    delay(5); // ждем


    emit signalFromMainMeasurement("Калибровка VW завершена");
    // отправляем команду на отключение выхода 2 (реле 2)
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00040000"));
    QModbusResponse::registerDataSizeCalculator( cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    measuredColdResistanceIsFinished_VW_Calibration = false;
    sumColdResistanceCalibration = 0;
    delay(5); // ждем



    //***********************************************************************

    emit signalFromMainMeasurement("Запущена калибровка UW");
    // отправляем команду на включение выхода 3 (реле 3)
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000503E8"));
    QModbusResponse::registerDataSizeCalculator( cmd, [](const QModbusResponse &)->int { return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(5); // ждем


    emit signalFromMainMeasurement("Идет первая калибровка UW");
    // измеряем, замер 1
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished())  connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(5); // ждем


    emit signalFromMainMeasurement("Идет вторая калибровка UW");
    // измеряем, замер 2
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10; });
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(5); // ждем


    emit signalFromMainMeasurement("Идет третья калибровка UW");
    // измеряем, замер 3
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    measuredColdResistanceIsFinished_UW_Calibration = true;
    delay(5); // ждем


    emit signalFromMainMeasurement("Калибровка завершена");
    // отправляем команду на отключение выхода 3 (реле 3)
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00050000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCalibration);
    else lastRequest->deleteLater();
    measuredColdResistanceIsFinished_UW_Calibration = false;
    sumColdResistanceCalibration = 0;
    delay(5); // ждем


    // отключаем контактор мико
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00010000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем

    // отключаем реле переключения RS-485
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00060000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем

    QMessageBox::about(this, "Уведомление", "Калибровка завершена");

    modbusDevice->deleteLater();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::responceCalibration() // обработка ответа от реле и МИКО-9А при калибровке
{
    QModbusReply* lastRequest = qobject_cast<QModbusReply*>(sender());
    if (!lastRequest) return;
    if (lastRequest->error() == QModbusDevice::NoError){
        QModbusResponse unit = lastRequest->rawResult();
    } else {
        ui->statusbar->showMessage("Ошибка чтения!", 1000);
    }
    lastRequest->deleteLater();

    // если ответ от реле включения контактора мегаомметра
    if (lastRequest->serverAddress() == 0x10 && QString::fromUtf8(lastRequest->rawResult().data().toHex()) == "020010") {
        ui->label_statusRele->setText("Контактор мегаомметра включен!");
    }

    // если ответ от МИКО-9А
    if (lastRequest->serverAddress() == 0x55) {
        //ui->lineEdit_Number_of_SEM->setText(QString::fromUtf8(lastRequest->rawResult().data().toHex()));
        QString number, res;
        res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
        for (int i = 4; i < 12; i++) {
            number += res[i];
        }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true

        float y = *reinterpret_cast<float *>(&hex);

        if (y != 0)
            sumColdResistanceCalibration += y;
        if (measuredColdResistanceIsFinished_UV_Calibration)
        emit signalFromMainTo_window_experience_2_Calibration(sumColdResistanceCalibration,
                                                              measuredColdResistanceIsFinished_UV_Calibration,
                                                              measuredColdResistanceIsFinished_VW_Calibration,
                                                              measuredColdResistanceIsFinished_UW_Calibration);

        if (measuredColdResistanceIsFinished_VW_Calibration)
        emit signalFromMainTo_window_experience_2_Calibration(sumColdResistanceCalibration,
                                                              measuredColdResistanceIsFinished_UV_Calibration,
                                                              measuredColdResistanceIsFinished_VW_Calibration,
                                                              measuredColdResistanceIsFinished_UW_Calibration);

        if (measuredColdResistanceIsFinished_UW_Calibration)
        emit signalFromMainTo_window_experience_2_Calibration(sumColdResistanceCalibration,
                                                              measuredColdResistanceIsFinished_UV_Calibration,
                                                              measuredColdResistanceIsFinished_VW_Calibration,
                                                              measuredColdResistanceIsFinished_UW_Calibration);
    }
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::on_pushButton_experience_3_clicked() // кнопка Опыт 3
{
    // отправляем в третье окно сопротивление изоляции ПЭД из основного окна
    QString ResistanceIsolation_of_SEM = ui->lineEdit_ResistanceIsolation_of_SEM->text();
    for (unsigned int i = 0; i < sizeof(ResistanceIsolation_of_SEM); i++) {
        if (ResistanceIsolation_of_SEM[i] == 44) { ResistanceIsolation_of_SEM[i] = 46; } // меняем запятую на точку
    }
    emit signalFromMainTo_window_experience_3_Parameters(ResistanceIsolation_of_SEM.toFloat());

    if (Experience_3_IsEnded) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Предупреждение!",
                                                                  "Данный опыт уже проведен! \nПродолжить?",
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
          }
    }

    // сигнал от основного окна с номером текущего протокола
    emit signalFromMainNumberProtocol(experience_2->fileProtocol = fileProtocol);
    experience_3->exec();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slotExperienceThreeIsEnded() // слот: опыт 3 завершен
{
    ui->pushButton_experience_3->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
    Experience_3_IsEnded = true;
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_startMeasurementColdIsolationResistance() // слот: Старт Опыта 3 Измерение сопртивления
                                                                // изоляции холодного ПЭД
{
    stop_measuredIsolation = false;
    stop_measuredResistance = false;
    stopExperienceEight = false;
    stop_run = false;
    stopExperienceTMS = false;
    stopExperienceFive = false;
    stopExperienceSix= false;

    int counter = 0;
    const int counter_meger = 1000;

    modbusDevice = new QModbusRtuSerialMaster(this);
    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
                statusBar()->showMessage(modbusDevice->errorString(), 5000);
            });

    if(modbusDevice) {
        device = "Мегаомметр";
        connect(modbusDevice, &QModbusClient::stateChanged,
                this, &MainWindow::onStateChanged);
    }
    if (!modbusDevice)
        return;

    statusBar()->clearMessage();
    if (modbusDevice->state() != QModbusDevice::ConnectedState) {
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, PORT);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, PARITY);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, 115200);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, DATA_BITS);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, STOP_BITS);
        modbusDevice->setTimeout(RESPONSE_TIME);
        modbusDevice->setNumberOfRetries(1);
        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
        }
    } else {
        modbusDevice->disconnectDevice();
    }


    // отправляем команду отключения реле 3
    QModbusReply *lastRequest = nullptr;
    auto cmd = QModbusPdu::FunctionCode(0x06);
    auto request = QModbusRequest(cmd, QByteArray::fromHex("00020000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0a);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем

    // отправляем команду на останов ПЧ
    cmd = QModbusPdu::FunctionCode(0x0f);
    request = QModbusRequest(cmd, QByteArray::fromHex("000000200474049919"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x06);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFive);
    else lastRequest->deleteLater();


    // отправляем команду на включение выхода 0 Овен - включение контактора мегаомметра
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000003E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(2); // ждем

    // отправляем запрос в модуль дискретных входов ОВЕН
    cmd = QModbusPdu::FunctionCode(0x03);
    request = QModbusRequest(cmd, QByteArray::fromHex("00330001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 3;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x10);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем

    COMPORT = new QSerialPort();
    COMPORT->setPortName("COM1");
    COMPORT->setBaudRate(QSerialPort::BaudRate::Baud9600);
    COMPORT->setParity(QSerialPort::Parity::NoParity);
    COMPORT->setDataBits(QSerialPort::DataBits::Data8);
    COMPORT->setStopBits(QSerialPort::StopBits::OneStop);
    COMPORT->setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    COMPORT->open(QIODevice::ReadWrite);

    if (COMPORT->isOpen()){
        qDebug() << "Serial Port Is Connected";
        qDebug() << COMPORT->error();
    } else {
        qDebug() << "Serial Port Is Not Connected";
        qDebug() << COMPORT->error();
    }
    COMPORT->flush();

    float ResistanceIsolation = 0, PI = 0, U = 0;

    if (COMPORT->isOpen()){

        COMPORT->clear();


    //***********************************************************************
        if (!stop_measuredIsolation) {
         ui->statusbar->showMessage("Процесс: Rn_one");
         delay(1);
         char buf_Rn_one[6];
         do {
             counter++;
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Rn\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Rn_one, sizeof(buf_Rn_one));
             qDebug() << buf_Rn_one;
             if (counter == counter_meger) {
                 ui->statusbar->showMessage("Что-то пошло не так! Перезагрузите прибор");
                 QMessageBox::about(this, "Уведомление", "Что-то пошло не так! Перезагрузите прибор");
                 stop_measuredIsolation = true;
                 break;
             }
         } while (buf_Rn_one[2] != 79 && buf_Rn_one[3] != 75 && buf_Rn_one[4] != 13 && buf_Rn_one[5] != 10/*!COMPORT->isReadable()*/);
        }
    //***********************************************************************
        if (!stop_measuredIsolation) {
         ui->statusbar->showMessage("Процесс: Fs_one");
         delay(1);
         char buf_Fs_one[6];
         counter = 0;
         do {
             counter++;
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Fs03\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Fs_one, sizeof(buf_Fs_one));
             qDebug() << buf_Fs_one;
             if (counter == counter_meger) {
                 ui->statusbar->showMessage("Что-то пошло не так! Перезагрузите прибор");
                 QMessageBox::about(this, "Уведомление", "Что-то пошло не так! Перезагрузите прибор");
                 stop_measuredIsolation = true;
                 break;
             }
         } while (buf_Fs_one[2] != 79 && buf_Fs_one[3] != 75 && buf_Fs_one[4] != 13 && buf_Fs_one[5] != 10);
        }
    //***********************************************************************
        if (!stop_measuredIsolation) {
         ui->statusbar->showMessage("Процесс: Is");
         delay(1);
         char buf_Is[6];
         counter = 0;
         do {
             counter++;
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Is030\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Is, sizeof(buf_Is));
             qDebug() << buf_Is;
             if (counter == counter_meger) {
                 ui->statusbar->showMessage("Что-то пошло не так! Перезагрузите прибор");
                 QMessageBox::about(this, "Уведомление", "Что-то пошло не так! Перезагрузите прибор");
                 stop_measuredIsolation = true;
                 break;
             }
         } while (buf_Is[2] != 79 && buf_Is[3] != 75 && buf_Is[4] != 13 && buf_Is[5] != 10);
        }
    //***********************************************************************
        if (!stop_measuredIsolation) {
         ui->statusbar->showMessage("Процесс: Bd_one");
         delay(1);
         char buf_Bd_one[6];
         counter = 0;
         do {
             counter++;
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Bd\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Bd_one, sizeof(buf_Bd_one));
             qDebug() << buf_Bd_one;
             if (counter == counter_meger) {
                 ui->statusbar->showMessage("Что-то пошло не так! Перезагрузите прибор");
                 QMessageBox::about(this, "Уведомление", "Что-то пошло не так! Перезагрузите прибор");
                 stop_measuredIsolation = true;
                 break;
             }
         } while (buf_Bd_one[2] != 79 && buf_Bd_one[3] != 75 && buf_Bd_one[4] != 13 && buf_Bd_one[5] != 10);
        }
    //***********************************************************************
        timer = new QTimer();
        time.setHMS(0, 1, 0); // обратный отсчет
        connect(timer, SIGNAL(timeout()), this, SLOT(updateCountdown()));
        timer->start(1000);

        delay_for_Meger(60);


    //***********************************************************************
         ui->statusbar->showMessage("Процесс: Bu_one");
         delay_for_Meger(1);
         char buf_Bu_one[7];
         do {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Bu\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Bu_one, sizeof(buf_Bu_one));
             qDebug() << buf_Bu_one << "STOP";
             delay(2);
         } while (buf_Bu_one[2] != 79 && buf_Bu_one[3] != 75 && buf_Bu_one[4] != 13 && buf_Bu_one[5] != 10/*!COMPORT->isReadable()*/);
    //***********************************************************************
         // проверяем остановилось ли измерение
         ui->statusbar->showMessage("Процесс: St");
         delay_for_Meger(1);
         char buf_St[16];
         while (buf_St[2] != 49) {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("St\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_St, sizeof(buf_St));
             qDebug() << buf_St;

            if (buf_St[0] == 50 || buf_St[3] == 50) {
                 do {
                    if (stop_measuredIsolation) {break;}
                   COMPORT->write("Bu\r\n");
                   COMPORT->waitForBytesWritten();
                   COMPORT->waitForReadyRead(500);
                   COMPORT->readLine(buf_Bu_one, sizeof(buf_Bu_one));
                   qDebug() << buf_Bu_one << "STOP";
                 } while (!COMPORT->isReadable());
            }
         }
    //***********************************************************************
         if (!stop_measuredIsolation) {
         ui->statusbar->showMessage("Процесс: Df_R");
         delay(1);
         char buf_Df_R[7];
         counter = 0;
         do {
             counter++;
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Df0040\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Df_R, sizeof(buf_Df_R));
             qDebug() << buf_Df_R;
             if (counter == counter_meger) {
                 ui->statusbar->showMessage("Что-то пошло не так! Перезагрузите прибор");
                 QMessageBox::about(this, "Уведомление", "Что-то пошло не так! Перезагрузите прибор");
                 stop_measuredIsolation = true;
                 break;
             }
        } while (!(buf_Df_R[2] == 79
                 && buf_Df_R[3] == 75 && buf_Df_R[4] == 13 && buf_Df_R[5] == 10));
         }
    //***********************************************************************
         if (!stop_measuredIsolation) {
         ui->statusbar->showMessage("Процесс: Dg_R");
         delay(1);
         char buf_Dg_R[16];
         counter = 0;
         do {
             counter++;
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Dg\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Dg_R, sizeof(buf_Dg_R));
             qDebug() << buf_Dg_R;
             if (counter == counter_meger) {
                 ui->statusbar->showMessage("Что-то пошло не так! Перезагрузите прибор");
                 QMessageBox::about(this, "Уведомление", "Что-то пошло не так! Перезагрузите прибор");
                 stop_measuredIsolation = true;
                 break;
             }
               char buffer[sizeof(buf_Dg_R)];

               if (buf_Dg_R[0] == 17  && buf_Dg_R[1] == 19 && buf_Dg_R[2] == 32) {
                   for (unsigned int i = 0; i < sizeof(buf_Dg_R) - 2; i++) {
                       buffer[i] = buf_Dg_R[i+2];
                   }
               }
               else if (buf_Dg_R[2] == 79  && buf_Dg_R[3] == 32 && buf_Dg_R[4] == 50) {
                   for (unsigned int i = 0; i < sizeof(buf_Dg_R) - 3; i++) {
                       buffer[i] = buf_Dg_R[i+3];
                   }
               }

             QString num = buffer;
             ResistanceIsolation = num.toFloat() / 1000000;
         } while (!((buf_Dg_R[0] == 17  && buf_Dg_R[1] == 19 && buf_Dg_R[2] == 32) ||
                    (buf_Dg_R[2] == 79  && buf_Dg_R[3] == 32 && buf_Dg_R[4] == 50)));
         }
    //***********************************************************************
         if (!stop_measuredIsolation) {
         ui->statusbar->showMessage("Процесс: Df_U");
         delay(1);
         char buf_Df_U[16];
         counter = 0;
         do {
             counter++;
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Df0100\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Df_U, sizeof(buf_Df_U));
             qDebug() << buf_Df_U;
             if (counter == counter_meger) {
                 ui->statusbar->showMessage("Что-то пошло не так! Перезагрузите прибор");
                 QMessageBox::about(this, "Уведомление", "Что-то пошло не так! Перезагрузите прибор");
                 stop_measuredIsolation = true;
                 break;
             }
         } while (!(buf_Df_U[2] == 79
                    && buf_Df_U[3] == 75 && buf_Df_U[4] == 13 && buf_Df_U[5] == 10));
         }
    //***********************************************************************
         if (!stop_measuredIsolation) {
         ui->statusbar->showMessage("Процесс: Dg_U");
         delay(1);
         char buf_Dg_U[16];
         counter = 0;
         do {
             counter++;
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Dg\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Dg_U, sizeof(buf_Dg_U));
             qDebug() << buf_Dg_U;
             if (counter == counter_meger) {
                 ui->statusbar->showMessage("Что-то пошло не так! Перезагрузите прибор");
                 QMessageBox::about(this, "Уведомление", "Что-то пошло не так! Перезагрузите прибор");
                 stop_measuredIsolation = true;
                 break;
             }
               char buffer[sizeof(buf_Dg_U)];
               for (unsigned int i = 0; i < sizeof(buf_Dg_U) - 2; i++) {
                   buffer[i] = buf_Dg_U[i+2];
               }
             QString num = buffer;
             U = num.toFloat();
         } while (!(buf_Dg_U[2] == 32 && buf_Dg_U[3] == 43));
        }



    delay_for_Meger(2);
    //***********************************************************************
    if (!stop_measuredIsolation) {
         ui->statusbar->showMessage("Процесс: R*");
         delay(1);
         char buf_R[6];
         do {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("R*\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_R, sizeof(buf_R));
             qDebug() << buf_R;
         } while (!COMPORT->isReadable());
    }
    //***********************************************************************
    if (!stop_measuredIsolation) {
         ui->statusbar->showMessage("Процесс: @");
         delay(1);
         char buf_flush_PI[6];
         do {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("@\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_flush_PI, sizeof(buf_flush_PI));
             qDebug() << buf_flush_PI;
         } while (buf_flush_PI[1] != 64);
    }
    //***********************************************************************
    if (!stop_measuredIsolation) {
         ui->statusbar->showMessage("Процесс: #");
         delay(1);
         char buf_erase_PI[6];
         do {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("#\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_erase_PI, sizeof(buf_erase_PI));
             qDebug() << buf_erase_PI;
         } while (!COMPORT->isReadable());
    }
    //***********************************************************************
    if (!stop_measuredIsolation) {
         ui->statusbar->showMessage("Процесс: Rn_two");
         delay(1);
         char buf_Rn_two[6];
         do {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Rn\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Rn_two, sizeof(buf_Rn_two));
             qDebug() << buf_Rn_two;
         } while (!COMPORT->isReadable());
      }
    }

    //***********************************************************************
    if (!stop_measuredIsolation) {
         ui->statusbar->showMessage("Процесс: Fs_one");
         delay(1);
         char buf_Fs_one[6];
         do {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Fs03\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Fs_one, sizeof(buf_Fs_one));
             qDebug() << buf_Fs_one;
         } while (buf_Fs_one[2] != 79 && buf_Fs_one[3] != 75 && buf_Fs_one[4] != 13 && buf_Fs_one[5] != 10);
    }
    //***********************************************************************
    if (!stop_measuredIsolation) {
         ui->statusbar->showMessage("Процесс: Is");
         delay(1);
         char buf_Is[6];
         do {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Is030\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Is, sizeof(buf_Is));
             qDebug() << buf_Is;
         } while (buf_Is[2] != 79 && buf_Is[3] != 75 && buf_Is[4] != 13 && buf_Is[5] != 10);
    }
    //***********************************************************************
    if (!stop_measuredIsolation) {
         ui->statusbar->showMessage("Процесс: Bd_one");
         delay(1);
         char buf_Bd_one[6];
         do {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Bd\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Bd_one, sizeof(buf_Bd_one));
             qDebug() << buf_Bd_one;
         } while (buf_Bd_one[2] != 79 && buf_Bd_one[3] != 75 && buf_Bd_one[4] != 13 && buf_Bd_one[5] != 10);
    }
    //***********************************************************************
        timer = new QTimer();
        time.setHMS(0, time_PI, 0); // обратный отсчет
        connect(timer, SIGNAL(timeout()), this, SLOT(updateCountdown()));
        timer->start(1000);

        delay_for_Meger(time_PI*60);


    //***********************************************************************
         ui->statusbar->showMessage("Процесс: Bu_one");
         delay_for_Meger(1);
         char buf_Bu_one[7];
         do {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Bu\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Bu_one, sizeof(buf_Bu_one));
             qDebug() << buf_Bu_one << "STOP";
             delay(2);
         } while (buf_Bu_one[2] != 79 && buf_Bu_one[3] != 75 && buf_Bu_one[4] != 13 && buf_Bu_one[5] != 10/*!COMPORT->isReadable()*/);
    //***********************************************************************
         // проверяем остановилось ли измерение
         ui->statusbar->showMessage("Процесс: St");
         delay_for_Meger(1);
         char buf_St[16];
         while (buf_St[2] != 49) {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("St\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_St, sizeof(buf_St));
             qDebug() << buf_St;

            if (buf_St[0] == 50 || buf_St[3] == 50) {
                 do {
                    if (stop_measuredIsolation) {break;}
                   COMPORT->write("Bu\r\n");
                   COMPORT->waitForBytesWritten();
                   COMPORT->waitForReadyRead(500);
                   COMPORT->readLine(buf_Bu_one, sizeof(buf_Bu_one));
                   qDebug() << buf_Bu_one << "STOP";
                 } while (!COMPORT->isReadable());
            }
         }
    //***********************************************************************
         if (!stop_measuredIsolation) {
         ui->statusbar->showMessage("Процесс: Df_R");
         delay(1);
         char buf_Df_R[7];
         counter = 0;
         do {
             counter++;
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Df0040\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Df_R, sizeof(buf_Df_R));
             qDebug() << buf_Df_R;
             if (counter == counter_meger) {
                 ui->statusbar->showMessage("Что-то пошло не так! Перезагрузите прибор");
                 QMessageBox::about(this, "Уведомление", "Что-то пошло не так! Перезагрузите прибор");
                 stop_measuredIsolation = true;
                 break;
             }
        } while (!(buf_Df_R[2] == 79
                 && buf_Df_R[3] == 75 && buf_Df_R[4] == 13 && buf_Df_R[5] == 10));
         }
    //***********************************************************************
         if (!stop_measuredIsolation) {
         ui->statusbar->showMessage("Процесс: Dg_R");
         delay(1);
         char buf_Dg_R[16];
         counter = 0;
         do {
             counter++;
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Dg\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Dg_R, sizeof(buf_Dg_R));
             qDebug() << buf_Dg_R;
             if (counter == counter_meger) {
                 ui->statusbar->showMessage("Что-то пошло не так! Перезагрузите прибор");
                 QMessageBox::about(this, "Уведомление", "Что-то пошло не так! Перезагрузите прибор");
                 stop_measuredIsolation = true;
                 break;
             }
               char buffer[sizeof(buf_Dg_R)];

               if (buf_Dg_R[0] == 17  && buf_Dg_R[1] == 19 && buf_Dg_R[2] == 32) {
                   for (unsigned int i = 0; i < sizeof(buf_Dg_R) - 2; i++) {
                       buffer[i] = buf_Dg_R[i+2];
                   }
               }
               else if (buf_Dg_R[2] == 79  && buf_Dg_R[3] == 32 && buf_Dg_R[4] == 50) {
                   for (unsigned int i = 0; i < sizeof(buf_Dg_R) - 3; i++) {
                       buffer[i] = buf_Dg_R[i+3];
                   }
               }

             QString num = buffer;
             PI/*ResistanceIsolation*/ = (num.toFloat() / 1000000) / ResistanceIsolation;
         } while (!((buf_Dg_R[0] == 17  && buf_Dg_R[1] == 19 && buf_Dg_R[2] == 32) ||
                    (buf_Dg_R[2] == 79  && buf_Dg_R[3] == 32 && buf_Dg_R[4] == 50)));
         }
    //***********************************************************************

         if (stop_measuredIsolation) {
             do {
                 COMPORT->write("Bu\r\n");
                 COMPORT->waitForBytesWritten();
                 char buf[28];
                 COMPORT->waitForReadyRead(500);
                 COMPORT->readLine(buf, sizeof(buf));
                 qDebug() << buf;
            } while (!COMPORT->isReadable());
            delay(1); // ждем
            // отправляем команду отключения реле
            cmd = QModbusPdu::FunctionCode(0x06);
            request = QModbusRequest(cmd, QByteArray::fromHex("00000000"));
            QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
            lastRequest = modbusDevice->sendRawRequest(request, 0x0a);
            if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
            else lastRequest->deleteLater();
        }

    delay(1); // ждем
    // отправляем команду отключения реле
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00000000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0a);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем

    QMessageBox::about(this, "Уведомление", "Измерение завершено");

    emit signalFromMainTo_window_experience_3_ResistanceIsolation(ResistanceIsolation, PI, U);

    //COMPORT->deleteLater();
    //modbusDevice->deleteLater();

    delete COMPORT;
    delete modbusDevice;

    ui->label_statusRele->clear();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::updateCountdown() // счетчик оставшегося времени при измерении изоляции (общий с 10 опытом)
{
    time = time.addSecs(-1);
    ui->statusbar->showMessage("Оставшееся время: " + time.toString("m:ss"));
    if (time.toString("m:ss") == "0:00" || stop_measuredIsolation == true) {
        ui->statusbar->clearMessage();
        timer->deleteLater();
        return;
    }
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_stop_measuredIsolation() // остановить измерения в третьем окне
{
    stop_measuredIsolation = true;
    //QMessageBox::about(this, "Уведомление", "Прервано пользователем!");
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_time_PI(int arg1)
{
    time_PI = arg1;
}
//################################################-КОНЕЦ-##################################################################



//################################################-НАЧАЛО-#################################################################
void MainWindow::on_pushButton_experience_4_clicked() // кнопка Опыт 4
{
    // отправляем в четвертое окно момент проворачивания из основного окна
    QString TorqueTurning = ui->lineEdit_TorqueTurning_of_SEM->text();
    for (unsigned int i = 0; i < sizeof(TorqueTurning); i++) {
        if (TorqueTurning[i] == 44) { TorqueTurning[i] = 46; } // меняем запятую на точку
    }
    emit signalFromMainTo_window_experience_4_Parameters(TorqueTurning.toFloat());

    if (Experience_4_IsEnded) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Предупреждение!",
                                                                  "Данный опыт уже проведен! \nПродолжить?",
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
          }
    }

    // сигнал от основного окна с номером текущего протокола
    emit signalFromMainNumberProtocol(experience_4->fileProtocol = fileProtocol);
    experience_4->exec(); // открываем окно
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slotExperienceFourIsEnded() // слот: опыт 4 завершен
{
    ui->pushButton_experience_4->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
    Experience_4_IsEnded = true;
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::on_pushButton_experience_5_clicked() // кнопка Опыт 5
{
    // отправляем в пятое окно
    QString VoltageShortCircuit = ui->lineEdit_VoltageShortCircuit_of_SEM->text();
    for (unsigned int i = 0; i < sizeof(VoltageShortCircuit); i++) {
        if (VoltageShortCircuit[i] == 44) { VoltageShortCircuit[i] = 46; } // меняем запятую на точку
    }

    QString CurrentShortCircuit = ui->lineEdit_CurrentShortCircuit_of_SEM->text();
    for (unsigned int i = 0; i < sizeof(CurrentShortCircuit); i++) {
        if (CurrentShortCircuit[i] == 44) { CurrentShortCircuit[i] = 46; } // меняем запятую на точку
    }

    QString LossesShortCircuit = ui->lineEdit_LossesShortCircuit_of_SEM->text();
    for (unsigned int i = 0; i < sizeof(LossesShortCircuit); i++) {
        if (LossesShortCircuit[i] == 44) { LossesShortCircuit[i] = 46; } // меняем запятую на точку
    }

    emit signalFromMainTo_window_experience_5_Parameters(VoltageShortCircuit.toFloat(), CurrentShortCircuit.toFloat(), LossesShortCircuit.toFloat());

    if (Experience_5_IsEnded) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Предупреждение!",
                                                                  "Данный опыт уже проведен! \nПродолжить?",
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
          }
    }

    // сигнал от основного окна с номером текущего протокола
    emit signalFromMainNumberProtocol(experience_5->fileProtocol = fileProtocol);

    // сигнал из основного окна со значением высокого напряжения трансформатора
    emit signalInExperience_high_voltage(ui->lineEdit_high_voltage->text().toFloat());

    QMessageBox::about(this, "Уведомление", "Установите напряжение на трансформаторе 704В!");

    experience_5->exec();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slotExperienceFiveIsEnded() // слот: опыт 5 завершен
{
    ui->pushButton_experience_5->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
    Experience_5_IsEnded = true;
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_startExperienceFive() // слот: Старт Опыта 5. Опыт короткого замыкания
{
    stop_measuredIsolation = false;
    stop_measuredResistance = false;
    stopExperienceEight = false;
    stop_run = false;
    stopExperienceTMS = false;
    stopExperienceFive = false;
    stopExperienceSix= false;

    modbusDevice = new QModbusRtuSerialMaster(this);
    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
                statusBar()->showMessage(modbusDevice->errorString(), 5000);
            });

    if(modbusDevice) {
        device = "ПЧ";
        connect(modbusDevice, &QModbusClient::stateChanged,
                this, &MainWindow::onStateChanged);
    }

    if (!modbusDevice)
        return;

    statusBar()->clearMessage();
    if (modbusDevice->state() != QModbusDevice::ConnectedState) {
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, PORT);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, PARITY);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, 115200);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, DATA_BITS);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, STOP_BITS);
        modbusDevice->setTimeout(RESPONSE_TIME);
        modbusDevice->setNumberOfRetries(1);
        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
        }
    } else {
        modbusDevice->disconnectDevice();
    }


    //***********************************************************************
    // отправляем команду отключения реле 1
    QModbusReply *lastRequest = nullptr;
    auto cmd = QModbusPdu::FunctionCode(0x06);
    auto request = QModbusRequest(cmd, QByteArray::fromHex("00000000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0a);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем

    // отправляем команду отключения реле 2
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00010000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0a);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем


    // отправляем команду на включение выхода 3 Овен - включение контактора ПЧ
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000203E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFive);
    else lastRequest->deleteLater();
    delay(2); // ждем

    // отправляем команду на пуск ПЧ
    cmd = QModbusPdu::FunctionCode(0x0f);
    request = QModbusRequest(cmd, QByteArray::fromHex("00000020047c049919"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x06);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFive);
    else lastRequest->deleteLater();
    delay(1); // ждем

//    QTimer *timer = new QTimer(this);
//    timer->start(100);
//    connect(timer, SIGNAL(timeout()), this, SLOT(readModbusVFD()));




    // цикл опроса
    while (stopExperienceFive == false) {
        if (stopExperienceFive == true) break;
        request_current_ExperienceFive = true;
        cmd = QModbusPdu::FunctionCode(0x03);
        request = QModbusRequest(cmd, QByteArray::fromHex("3f0b0002"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 5;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x06);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFive);
        else lastRequest->deleteLater();
        //QCoreApplication::processEvents(QEventLoop::AllEvents, 200);
        //Sleep(500);
        delay(1); // ждем
        request_current_ExperienceFive = false;

        if (stopExperienceFive == true) break;
        request_voltage_ExperienceFive = true;
        cmd = QModbusPdu::FunctionCode(0x03);
        request = QModbusRequest(cmd, QByteArray::fromHex("3ef70002"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 5;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x06);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFive);
        else lastRequest->deleteLater();
        delay(1); // ждем
        //QCoreApplication::processEvents(QEventLoop::AllEvents, 500);
        request_voltage_ExperienceFive = false;

////        if (stopExperienceFive == true) break;
////        request_losses_ExperienceFive = true;
////        cmd = QModbusPdu::FunctionCode(0x03);
////        request = QModbusRequest(cmd, QByteArray::fromHex("3ee30002"));
////        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 5;});
////        lastRequest = modbusDevice->sendRawRequest(request, 0x06);
////        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFive);
////        else lastRequest->deleteLater();
////        //QCoreApplication::processEvents(QEventLoop::AllEvents, 200);
////        //Sleep(500);
////        delay(1); // ждем
////        request_losses_ExperienceFive = false;
    }

    if (stopExperienceFive == true) {
        // отправляем команду на останов ПЧ
        cmd = QModbusPdu::FunctionCode(0x0f);
        request = QModbusRequest(cmd, QByteArray::fromHex("000000200474049919"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x06);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFive);
        else lastRequest->deleteLater();

        // отправляем команду на отключение выхода 3 Овен - отключение контактора ПЧ
        cmd = QModbusPdu::FunctionCode(0x06);
        request = QModbusRequest(cmd, QByteArray::fromHex("00020000"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
        else lastRequest->deleteLater();
        delay(2); // ждем
    }

    QMessageBox::about(this, "Уведомление", "Опыт завершен!");

    delay(1); // ждем
    modbusDevice->deleteLater();
}
//################################################-КОНЕЦ-##################################################################

void MainWindow::readModbusVFD()
{
//    QModbusReply *lastRequest = nullptr;
//    // цикл опроса
//    if (stopExperienceFive == false) {
//        request_current_ExperienceFive = true;
//        auto cmd = QModbusPdu::FunctionCode(0x03);
//        auto request = QModbusRequest(cmd, QByteArray::fromHex("3f0b0002"));
//        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 5;});
//        lastRequest = modbusDevice->sendRawRequest(request, 0x06);
//        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFive);
//        else lastRequest->deleteLater();
//        //QCoreApplication::processEvents(QEventLoop::AllEvents, 200);
//        //Sleep(500);
//        delay(1); // ждем
//        request_current_ExperienceFive = false;

//        request_voltage_ExperienceFive = true;
//        cmd = QModbusPdu::FunctionCode(0x03);
//        request = QModbusRequest(cmd, QByteArray::fromHex("3ef70002"));
//        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 5;});
//        lastRequest = modbusDevice->sendRawRequest(request, 0x06);
//        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFive);
//        else lastRequest->deleteLater();
//        delay(1); // ждем
//        //QCoreApplication::processEvents(QEventLoop::AllEvents, 500);
//        request_voltage_ExperienceFive = false;

//        if (stopExperienceFive == true) break;
//        request_losses_ExperienceFive = true;
//        cmd = QModbusPdu::FunctionCode(0x03);
//        request = QModbusRequest(cmd, QByteArray::fromHex("3ee30002"));
//        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 5;});
//        lastRequest = modbusDevice->sendRawRequest(request, 0x06);
//        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFive);
//        else lastRequest->deleteLater();
//        //QCoreApplication::processEvents(QEventLoop::AllEvents, 200);
//        //Sleep(500);
//        delay(1); // ждем
//        request_losses_ExperienceFive = false;
  //  }
}

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_stopExperienceFive() // остановить измерения в пятом окне
{
    stopExperienceFive = true;
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::responceExperienceFive()
{
    QModbusReply* lastRequest = qobject_cast<QModbusReply*>(sender());
    if (!lastRequest) return;
    if (lastRequest->error() == QModbusDevice::NoError){
        QModbusResponse unit = lastRequest->rawResult();
    } else {
        ui->statusbar->showMessage("Ошибка чтения!");
    }
    lastRequest->deleteLater();

    // если ответ от ...
    if (lastRequest->serverAddress() == 0x06 && request_voltage_ExperienceFive) {

        QString number, res;
        res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
        for (int i = 2; i < 6; i++) { // 2  6
            number += res[i];
        }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true

          float k = hex;

          if (k != 0)
          emit signalFromMainTo_window_experience_5_voltage(k/10);
          else if (k == 0) counter_request_voltage_ExperienceFive++;

          if (counter_request_voltage_ExperienceFive > 2) {
              stopExperienceFive = true;
              ui->statusbar->showMessage("Что-то пошло не так! Опыт не удался...");
              return;
          }
    }

    if (lastRequest->serverAddress() == 0x06 && request_current_ExperienceFive) {
        QString number, res;
        res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
        for (int i = 6; i < 10; i++) {
            number += res[i];
        }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true
          float k = hex;

          if (k > 0) {
           emit signalFromMainTo_window_experience_5_current(k/(100));
          }
          else if (k == 0) counter_request_current_ExperienceFive++;

          if (counter_request_current_ExperienceFive > 2) {
              stopExperienceFive = true;
              ui->statusbar->showMessage("Что-то пошло не так! Опыт не удался...");
              return;
          }
    }

//    if (lastRequest->serverAddress() == 0x06 && request_losses_ExperienceFive) {
//        QString number, res;
//        res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
//        for (int i = 6; i < 10; i++) {
//            number += res[i];
//        }

//          bool ok;
//          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true

//          float k = hex;

//          if (k > 0) {
//           emit signalFromMainTo_window_experience_5_losses(k/(100*1.73));
//          }

//    }
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::on_pushButton_experience_6_clicked() // кнопка Опыт 6
{
    // отправляем в пятое окно
    QString AccelerationVoltage = ui->lineEdit_AccelerationVoltage_of_SEM->text();
    for (unsigned int i = 0; i < sizeof(AccelerationVoltage); i++) {
        if (AccelerationVoltage[i] == 44) { AccelerationVoltage[i] = 46; } // меняем запятую на точку
    }

    emit signalFromMainTo_window_experience_6_Parameters(AccelerationVoltage.toFloat());

    if (Experience_6_IsEnded) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Предупреждение!",
                                                                  "Данный опыт уже проведен! \nПродолжить?",
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
          }
    }

    // сигнал от основного окна с номером текущего протокола
    emit signalFromMainNumberProtocol(experience_6->fileProtocol = fileProtocol);

    // сигнал из основного окна со значением высокого напряжения трансформатора
    emit signalInExperience_high_voltage(ui->lineEdit_high_voltage->text().toFloat());

    experience_6->exec();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slotExperienceSixIsEnded() // слот: опыт 6 завершен
{
    ui->pushButton_experience_6->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
    Experience_6_IsEnded = true;
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_startExperienceSix() // слот: Старт Опыта 6. Определение напряжения трогания
{
    stop_measuredIsolation = false;
    stop_measuredResistance = false;
    stopExperienceEight = false;
    stop_run = false;
    stopExperienceTMS = false;
    stopExperienceFive = false;
    stopExperienceSix= false;

    modbusDevice = new QModbusRtuSerialMaster(this);
    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
                statusBar()->showMessage(modbusDevice->errorString(), 5000);
            });

    if(modbusDevice) {
        device = "ПЧ";
        connect(modbusDevice, &QModbusClient::stateChanged,
                this, &MainWindow::onStateChanged);
    }

    if (!modbusDevice)
        return;

    statusBar()->clearMessage();
    if (modbusDevice->state() != QModbusDevice::ConnectedState) {
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, ui->lineEdit_PORT_Danfoss->text());
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, ui->lineEdit_PARITY_Danfoss->text().toInt());
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, 115200);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, ui->lineEdit_DATA_BITS_Danfoss->text().toInt());
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, ui->lineEdit_STOP_BITS_Danfoss->text().toInt());
        modbusDevice->setTimeout(ui->lineEdit_RESPONSE_TIME_Danfoss->text().toInt());
        modbusDevice->setNumberOfRetries(1);
        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
        }
    } else {
        modbusDevice->disconnectDevice();
    }


    // отправляем команду отключения реле 1
    QModbusReply *lastRequest = nullptr;
    auto cmd = QModbusPdu::FunctionCode(0x06);
    auto request = QModbusRequest(cmd, QByteArray::fromHex("00000000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0a);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем

    // отправляем команду отключения реле 2
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00010000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0a);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем


    // отправляем команду на включение выхода 3 Овен - включение контактора ПЧ
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000203E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
    else lastRequest->deleteLater();
    delay(2); // ждем

    // отправляем команду на пуск ПЧ
    cmd = QModbusPdu::FunctionCode(0x0f);
    request = QModbusRequest(cmd, QByteArray::fromHex("00000020047c049919"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x06);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceSix);
    else lastRequest->deleteLater();
    delay(1); // ждем

    while(stopExperienceSix == false) {
        if (stopExperienceSix == true) break; // опрос дискретного модуля ICP CON 7080
        QModbusDataUnit readUnit(QModbusDataUnit::InputRegisters, 0x02, 0x02);
        lastRequest = modbusDevice->sendReadRequest(readUnit, 0x58);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceSix);
        else lastRequest->deleteLater();
        delay(1);

//        QModbusDataUnit readUnitVFD(QModbusDataUnit::InputRegisters, 0x3ef7, 0x02);
//        lastRequest = modbusDevice->sendReadRequest(readUnitVFD, 0x06);
//        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceSix);
//        else lastRequest->deleteLater();
//        delay(1);

        // запрос текущего напряжения из ПЧ
        if (stopExperienceSix == true) break;
        cmd = QModbusPdu::FunctionCode(0x03);
        request = QModbusRequest(cmd, QByteArray::fromHex("3ef70002"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 5;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x06);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceSix);
        else lastRequest->deleteLater();
        //Sleep(1000);
        delay(1); // ждем
    }

    //if (stopExperienceFive == true) {
        // отправляем команду на останов ПЧ
        cmd = QModbusPdu::FunctionCode(0x0f);
        request = QModbusRequest(cmd, QByteArray::fromHex("000000200474049919"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x06);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceSix);
        else lastRequest->deleteLater();
        delay(1); // ждем

        // отправляем команду на отключение выхода 3 Овен - отключение контактора ПЧ
        cmd = QModbusPdu::FunctionCode(0x06);
        request = QModbusRequest(cmd, QByteArray::fromHex("00020000"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
        else lastRequest->deleteLater();
        delay(2); // ждем
    //}

    QMessageBox::about(this, "Уведомление", "Опыт завершен");

    delay(1); // ждем
    modbusDevice->deleteLater();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::responceExperienceSix() // обработка сигналов Модбас опыта 6
{
    QModbusReply* lastRequest = qobject_cast<QModbusReply*>(sender());
    if (!lastRequest) return;
    if (lastRequest->error() == QModbusDevice::NoError){
        QModbusResponse unit = lastRequest->rawResult();
    } else {
        ui->statusbar->showMessage("Ошибка чтения!");
    }
    lastRequest->deleteLater();

    // если ответ от ...
    if (lastRequest->serverAddress() == 0x06) {
        QString number, res;
        res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
        for (int i = 2; i < 6; i++) { // 2  6
            number += res[i];
        }

        bool ok;
        int hex = number.toInt(&ok, 16);       // hex == 255, ok == true
        float voltage_starting = hex;

        if (hex > 0)
          emit signalFromMainTo_window_experience_6_voltage_starting(voltage_starting/10);
    }

    if (lastRequest->serverAddress() == 0x58) {
          QString number, res;
          res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
         for (int i = 2; i < 6; i++) {
             number += res[i];
            }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true
          float starting_frequency = hex;

          if (starting_frequency > 0) {
              stopExperienceSix = true;
              emit signalFromMainTo_window_experience_6_finish();
          }
    }
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_stopExperienceSix() // остановить измерения в 6 окне
{
    stopExperienceSix = true;

//    // отправляем команду на останов ПЧ
//    QModbusReply *lastRequest = nullptr;
//    auto cmd = QModbusPdu::FunctionCode(0x0f);
//    auto request = QModbusRequest(cmd, QByteArray::fromHex("000000200474049919"));
//    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
//    lastRequest = modbusDevice->sendRawRequest(request, 0x06);
//    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFive);
//    else lastRequest->deleteLater();

//    // отправляем команду на отключение выхода 3 Овен - отключение контактора ПЧ
//    cmd = QModbusPdu::FunctionCode(0x06);
//    request = QModbusRequest(cmd, QByteArray::fromHex("00020000"));
//    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
//    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
//    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
//    else lastRequest->deleteLater();
//    delay(2); // ждем

    //QMessageBox::about(this, "Уведомление", "Прервано пользователем!");
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::on_pushButton_experience_7_clicked() // кнопка Опыт 7
{
    // отправляем в пятое окно
    QString Voltage = ui->lineEdit_VoltageIdling_of_SEM->text();
    for (unsigned int i = 0; i < sizeof(Voltage); i++) {
        if (Voltage[i] == 44) { Voltage[i] = 46; } // меняем запятую на точку
    }

    QString Current = ui->lineEdit_CurrentIdling_of_SEM->text();
    for (unsigned int i = 0; i < sizeof(Current); i++) {
        if (Current[i] == 44) { Current[i] = 46; } // меняем запятую на точку
    }

    QString Losses = ui->lineEdit_LossesIdling_of_SEM->text();
    for (unsigned int i = 0; i < sizeof(Losses); i++) {
        if (Losses[i] == 44) { Losses[i] = 46; } // меняем запятую на точку
    }

    QString Vibration = ui->lineEdit_Vibration_of_SEM->text();
    for (unsigned int i = 0; i < sizeof(Vibration); i++) {
        if (Vibration[i] == 44) { Vibration[i] = 46; } // меняем запятую на точку
    }

    emit signalFromMainTo_window_experience_7_Parameters(Voltage.toFloat(), Current.toFloat(), Losses.toFloat(), Vibration.toFloat());

    if (Experience_7_IsEnded) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Предупреждение!",
                                                                  "Данный опыт уже проведен! \nПродолжить?",
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
          }
    }

    // сигнал от основного окна с номером текущего протокола
    emit signalFromMainNumberProtocol(experience_7->fileProtocol = fileProtocol);

    // сигнал из основного окна со значением высокого напряжения трансформатора
    emit signalInExperience_high_voltage(ui->lineEdit_high_voltage->text().toFloat());

    experience_7->exec();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slotExperienceSevenIsEnded() // слот: опыт 7 завершен
{
    ui->pushButton_experience_7->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
    Experience_7_IsEnded = true;
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::responceCheking() //
{
    QModbusReply* lastRequest = qobject_cast<QModbusReply*>(sender());
    if (!lastRequest) return;
    if (lastRequest->error() == QModbusDevice::NoError){
        ui->statusbar->showMessage("Модуль 7019 найден");
        Cheking7019 = true;
    } else {
        ui->statusbar->showMessage("Не вижу модуль 7019");
        Cheking7019 = false;
    }
    lastRequest->deleteLater();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_start() // слот: старт обкатки
{
    stop_measuredIsolation = false;
    stop_measuredResistance = false;
    stopExperienceEight = false;
    stop_run = false;
    stopExperienceTMS = false;
    stopExperienceFive = false;
    stopExperienceSix= false;

    //**************************************************************************************************************
    /*
        * Создаем экземпляр класса QModbusRtuSerialMaster
        * Связываем сигнал errorOccurred с методом класса errorString(),
        * обрабатывающим ошибки.
        */
    modbusDevice = new QModbusRtuSerialMaster(this);
    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
                statusBar()->showMessage(modbusDevice->errorString(), 5000);
            });
    /*
        * Также соединяем сигнал изменения состояния объекта modbusDevice
        * с методом onStateChanged, где мы будем обрабатывать изменения состояния (подключение или отключение порта)
        */
    if(modbusDevice) {
        device = "ПЧ";
        connect(modbusDevice, &QModbusClient::stateChanged,
                this, &MainWindow::onStateChanged);
    }
    /*
        * Если экземпляр объекта по каким-либо причинам не существует,
        * то во избежание критических ошибок и проблем покидаем функцию
        */
    if (!modbusDevice)
        return;
    /*
        * Если порт не подключен, то настраиваем его и пытаемся установить соединение.
        * Функция setConnectionParameter принимает 2 параметра:
        * 1) название устанавливаемого параметра
        * 2) значение параметра
        */
    statusBar()->clearMessage();
    if (modbusDevice->state() != QModbusDevice::ConnectedState) {
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, PORT);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, PARITY);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, 115200);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, DATA_BITS);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, STOP_BITS);
        modbusDevice->setTimeout(RESPONSE_TIME);
        modbusDevice->setNumberOfRetries(1);
        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
        }
    } else {
        modbusDevice->disconnectDevice();
    }


    //***********************************************************************
    // отправляем команду отключения реле 1
    QModbusReply *lastRequest = nullptr;
    auto cmd = QModbusPdu::FunctionCode(0x06);
    auto request = QModbusRequest(cmd, QByteArray::fromHex("00000000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0a);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем

    // отправляем команду отключения реле 2
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00010000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0a);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем

    // отправляем команду на включение выхода 3 Овен - включение контактора ПЧ
    //QModbusReply *lastRequest = nullptr;
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000203E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
    else lastRequest->deleteLater();
    delay(1); // ждем

    // отправляем команду на пуск ПЧ
    //QModbusReply *lastRequest = nullptr;
    cmd = QModbusPdu::FunctionCode(0x0f);
    request = QModbusRequest(cmd, QByteArray::fromHex("00000020047c049919"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x06);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
    else lastRequest->deleteLater();
    delay(1); // ждем

    timer = new QTimer();
    // запускаем таймер в Статусбар
    if (stop_run == false) {
        time.setHMS(0, 0, 0); // прямой отсчет
        connect(timer, SIGNAL(timeout()), this, SLOT(updateCountdownRun()));
        timer->start(1000);
    } else {
        timer->stop();
        timer->deleteLater();
    }

    // цикл опроса
    while (stop_run == false) {
        if (stop_run == true) break;
        request_run_voltage = true;
        cmd = QModbusPdu::FunctionCode(0x03);
        request = QModbusRequest(cmd, QByteArray::fromHex("3ef70002"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 5;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x06);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
        else lastRequest->deleteLater();
        //Sleep(1000);
        delay(1); // ждем
        request_run_voltage = false;

        if (stop_run == true) break;
        request_run_current = true;
        cmd = QModbusPdu::FunctionCode(0x03);
        request = QModbusRequest(cmd, QByteArray::fromHex("3f0b0002"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 5;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x06);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
        else lastRequest->deleteLater();
        //Sleep(1000);
        delay(1); // ждем
        request_run_current = false;

        if (stop_run == true) break;
        request_run_losses = true;
        cmd = QModbusPdu::FunctionCode(0x03);
        request = QModbusRequest(cmd, QByteArray::fromHex("3ee30002"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 5;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x06);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
        else lastRequest->deleteLater();
        //Sleep(1000);
        delay(1); // ждем
        request_run_losses = false;

        if (stop_run == true) break;
        request_run_BQ1 = true;
        cmd = QModbusPdu::FunctionCode(0x03);
        request = QModbusRequest(cmd, QByteArray::fromHex("00000001"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 3;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x57);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
        else lastRequest->deleteLater();
        delay(1); // ждем
        request_run_BQ1 = false;

        if (stop_run == true) break;
        request_run_BQ2 = true;
        cmd = QModbusPdu::FunctionCode(0x03);
        request = QModbusRequest(cmd, QByteArray::fromHex("00010001"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 3;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x57);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
        else lastRequest->deleteLater();
        delay(1); // ждем
        request_run_BQ2 = false;

        if (stop_run == true) break;
        request_run_BQ3 = true;
        cmd = QModbusPdu::FunctionCode(0x03);
        request = QModbusRequest(cmd, QByteArray::fromHex("00020001"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 3;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x57);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
        else lastRequest->deleteLater();
        delay(1); // ждем
        request_run_BQ3 = false;

        if (stop_run == true) break;
        request_run_BQ4 = true;
        cmd = QModbusPdu::FunctionCode(0x03);
        request = QModbusRequest(cmd, QByteArray::fromHex("00030001"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 3;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x57);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
        else lastRequest->deleteLater();
        delay(1); // ждем
        request_run_BQ4 = false;

        if (stop_run == true) break;
        request_run_BQ5 = true;
        cmd = QModbusPdu::FunctionCode(0x03);
        request = QModbusRequest(cmd, QByteArray::fromHex("00040001"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 3;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x57);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
        else lastRequest->deleteLater();
        delay(1); // ждем
        request_run_BQ5 = false;

        if (stop_run == true) break;
        request_run_BQ6 = true;
        cmd = QModbusPdu::FunctionCode(0x03);
        request = QModbusRequest(cmd, QByteArray::fromHex("00050001"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 3;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x57);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
        else lastRequest->deleteLater();
        delay(1); // ждем
        request_run_BQ6 = false;

        if (stop_run == true) break;
        request_run_temperature = true;
        cmd = QModbusPdu::FunctionCode(0x03);
        request = QModbusRequest(cmd, QByteArray::fromHex("00060001"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 3;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x57);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
        else lastRequest->deleteLater();
        delay(1); // ждем
        request_run_temperature = false;
    }


    if (stop_run == true) {
        // отправляем команду на отключение выхода 3 Овен - отключение контактора ПЧ
        auto cmd = QModbusPdu::FunctionCode(0x06);
        auto request = QModbusRequest(cmd, QByteArray::fromHex("00020000"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
        else lastRequest->deleteLater();
        delay(2); // ждем

        // отправляем команду на останов ПЧ
        cmd = QModbusPdu::FunctionCode(0x0f);
        request = QModbusRequest(cmd, QByteArray::fromHex("000000200474049919"));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
        lastRequest = modbusDevice->sendRawRequest(request, 0x06);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
        else lastRequest->deleteLater();
    }

    QMessageBox::about(this, "Уведомление", "Обкатка завершена");
    emit signal_stop_run();

    delay(1); // ждем
    modbusDevice->deleteLater();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::responceReleStart() // обработка сигналов Модбас
{
    QModbusReply* lastRequest = qobject_cast<QModbusReply*>(sender());
    if (!lastRequest) return;
    if (lastRequest->error() == QModbusDevice::NoError){
        QModbusResponse unit = lastRequest->rawResult();
    } else {
        ui->statusbar->showMessage("Ошибка чтения!");
    }
    lastRequest->deleteLater();

    // если ответ от ...
    if (lastRequest->serverAddress() == 0x06 && request_run_voltage) { // 6
        QString number, res;
        res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
        for (int i = 2; i < 6; i++) { // 2  6
            number += res[i];
        }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true
          float voltage = hex;

        emit signalFromMainTo_window_experience_7_voltage_run(voltage/10);

    } else if (lastRequest->serverAddress() == 0x06 && request_run_current) {
        QString number, res;
        res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
        for (int i = 2; i < 10; i++) {
            number += res[i];
        }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true
          float current = hex;

          //if (current/100 < 50) {
             emit signalFromMainTo_window_experience_7_current_run(current/100/*/1.73*/);
          //}


    } else if (lastRequest->serverAddress() == 0x06 && request_run_losses) {
        QString number, res;
        res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
        for (int i = 2; i < 10; i++) {
            number += res[i];
        }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true
          float losses = hex;

        emit signalFromMainTo_window_experience_7_losses_run(losses/100/1.73);

    } else if (lastRequest->serverAddress() == 0x57 && request_run_BQ1) {
        QString number, res;
        res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
        for (int i = 2; i < 6; i++) {
            number += res[i];
        }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true
          float BQ1 = hex;


        emit signalFromMainTo_window_experience_7_BQ1_run(BQ1);

    } else if (lastRequest->serverAddress() == 0x57 && request_run_BQ2) {
        QString number, res;
        res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
        for (int i = 2; i < 6; i++) {
            number += res[i];
        }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true
          float BQ2 = hex;


        emit signalFromMainTo_window_experience_7_BQ2_run(BQ2);

    } else if (lastRequest->serverAddress() == 0x57 && request_run_BQ3) {
        QString number, res;
        res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
        for (int i = 2; i < 6; i++) {
            number += res[i];
        }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true
          float BQ3 = hex;

        emit signalFromMainTo_window_experience_7_BQ3_run(BQ3);

    } else if (lastRequest->serverAddress() == 0x57 && request_run_BQ4) {
        QString number, res;
        res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
        for (int i = 2; i < 6; i++) {
            number += res[i];
        }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true
          float BQ4 = hex;

        emit signalFromMainTo_window_experience_7_BQ4_run(BQ4);

    } else if (lastRequest->serverAddress() == 0x57 && request_run_BQ5) {
        QString number, res;
        res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
        for (int i = 2; i < 6; i++) {
            number += res[i];
        }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true
          float BQ5 = hex;

        emit signalFromMainTo_window_experience_7_BQ5_run(BQ5);

    } else if (lastRequest->serverAddress() == 0x57 && request_run_BQ6) {
        QString number, res;
        res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
        for (int i = 2; i < 6; i++) {
            number += res[i];
        }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true
          float BQ6 = hex;

        emit signalFromMainTo_window_experience_7_BQ6_run(BQ6);

    } else if (lastRequest->serverAddress() == 0x57 && request_run_temperature) {
        QString number, res;
        res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
        for (int i = 2; i < 6; i++) {
            number += res[i];
        }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true
          float temperature = hex;

        emit signalFromMainTo_window_experience_7_temperature_run(temperature);
    }
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::updateCountdownRun() // время обкатки в Статусбар
{
    if (stop_run == false) {
        time = time.addSecs(1);
        ui->statusbar->showMessage("Время обкатки: " + time.toString("h:mm:ss"));
        if (time.toString("h:mm:ss") == "1:00:00") {
            stop_run = true;
            ui->statusbar->showMessage("Обкатка завершена!", 1000);
            //QMessageBox::about(this, "Уведомление", "Обкатка завершена");
            //ui->statusbar->clearMessage();
            emit signal_stop_run();
            timer->deleteLater();
            return;
        }
    } else {
        timer->stop();
        timer->deleteLater();
    }
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_stop() // остановить обкатку
{
    stop_run = true;
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::on_pushButton_experience_8_clicked() // кнопка Опыт 8
{
    // отправляем в пятое окно
    QString RunOut = ui->lineEdit_RunOut_of_SEM->text();
    for (unsigned int i = 0; i < sizeof(RunOut); i++) {
        if (RunOut[i] == 44) { RunOut[i] = 46; } // меняем запятую на точку
    }

    emit signalFromMainTo_window_experience_8_Parameters(RunOut.toFloat());

    if (Experience_8_IsEnded) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Предупреждение!",
                                                                  "Данный опыт уже проведен! \nПродолжить?",
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
          }
    }

    // сигнал от основного окна с номером текущего протокола
    emit signalFromMainNumberProtocol(experience_8->fileProtocol = fileProtocol);

    experience_8->exec();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slotExperienceEightIsEnded() // слот: опыт 8 завершен
{
    ui->pushButton_experience_8->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
    Experience_8_IsEnded = true;
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_startExperienceEight() // слот: Старт Опыта 8. Измерение выбега ПЭД
{
    stop_measuredIsolation = false;
    stop_measuredResistance = false;
    stopExperienceEight = false;
    stop_run = false;
    stopExperienceTMS = false;
    stopExperienceFive = false;
    stopExperienceSix= false;

    modbusDevice = new QModbusRtuSerialMaster(this);
    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
                statusBar()->showMessage(modbusDevice->errorString(), 5000);
            });

    if(modbusDevice) {
        device = "ПЧ";
        connect(modbusDevice, &QModbusClient::stateChanged,
                this, &MainWindow::onStateChanged);
    }

    if (!modbusDevice)
        return;

    statusBar()->clearMessage();
    if (modbusDevice->state() != QModbusDevice::ConnectedState) {
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, ui->lineEdit_PORT_Danfoss->text());
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, ui->lineEdit_PARITY_Danfoss->text().toInt());
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, ui->lineEdit_BAUDS_Danfoss->text().toInt());
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, ui->lineEdit_DATA_BITS_Danfoss->text().toInt());
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, ui->lineEdit_STOP_BITS_Danfoss->text().toInt());
        modbusDevice->setTimeout(ui->lineEdit_RESPONSE_TIME_Danfoss->text().toInt());
        modbusDevice->setNumberOfRetries(1);
        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
        }
    } else {
        modbusDevice->disconnectDevice();
    }


    // отправляем команду отключения реле 1
    QModbusReply *lastRequest = nullptr;
    auto cmd = QModbusPdu::FunctionCode(0x06);
    auto request = QModbusRequest(cmd, QByteArray::fromHex("00000000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0a);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем

    // отправляем команду отключения реле 2
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00010000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0a);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем


    // отправляем команду на включение выхода 3 Овен - включение контактора ПЧ
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000203E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
    else lastRequest->deleteLater();
    delay(2); // ждем

    // отправляем команду на пуск ПЧ
    cmd = QModbusPdu::FunctionCode(0x0f);
    request = QModbusRequest(cmd, QByteArray::fromHex("00000020047c049919"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x06);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceEight);
    else lastRequest->deleteLater();
    delay_for_Meger(60); // ждем разгона ПЭД

    // отправляем команду на останов ПЧ
    cmd = QModbusPdu::FunctionCode(0x0f);
    request = QModbusRequest(cmd, QByteArray::fromHex("000000200474049919"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x06);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceEight);
    else lastRequest->deleteLater();



    timer = new QTimer();
    // запускаем таймер в Статусбар
    if (stopExperienceEight == false) {
        time.setHMS(0, 0, 0); // прямой отсчет
        connect(timer, SIGNAL(timeout()), this, SLOT(updateCountdownExperienceEight()));
        timer->start();
    } else {
        timer->stop();
        timer->deleteLater();
    }

    timer_Experience_6 = 0;
    while(!stopExperienceEight) {
        if (stopExperienceEight == true) break; // опрос дискретного модуля ICP CON 7080
        QModbusReply *lastRequest = nullptr;
        QModbusDataUnit readUnit(QModbusDataUnit::InputRegisters, 0x02, 0x02);
        lastRequest = modbusDevice->sendReadRequest(readUnit, 0x58);
        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceEight);
        else lastRequest->deleteLater();
        delay(1);
        timer_Experience_6++;
    }

    // отправляем команду на отключение выхода 3 Овен - отключение контактора ПЧ
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00020000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceReleStart);
    else lastRequest->deleteLater();
    delay(2); // ждем

    QMessageBox::about(this, "Уведомление", "Опыт завершен");

    modbusDevice->deleteLater();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::responceExperienceEight() // обработка сигналов Модбас опыта 8
{
    QModbusReply* lastRequest = qobject_cast<QModbusReply*>(sender());
    if (!lastRequest) return;
    if (lastRequest->error() == QModbusDevice::NoError){
        QModbusResponse unit = lastRequest->rawResult();
    } else {
        ui->statusbar->showMessage("Ошибка чтения!");
    }
    lastRequest->deleteLater();

    // если ответ от ...
    if (lastRequest->serverAddress() == 0x58) {
          QString number, res;
          res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
         for (int i = 2; i < 6; i++) {
             number += res[i];
            }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true
          float starting_frequency = hex;

          if (starting_frequency == 0) {
              stopExperienceEight = true;
              emit signalFromMainTo_window_experience_8_finish();
          }
    }
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::updateCountdownExperienceEight() // время обкатки в Статусбар
{
    if (stopExperienceEight == false) {
        time = time.addMSecs(1);
        emit signalFromMainTo_window_experience_8_time(time.toString("mm.ss"), timer_Experience_6);
        if (stopExperienceEight == true) {
            timer->deleteLater();
            return;
        }
    } else {
        timer->stop();
        timer->deleteLater();
    }
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_stopExperienceEight() // остановить измерения в 8 окне
{
    stopExperienceEight = true;
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::on_pushButton_experience_9_clicked() // кнопка Опыт 9
{
    if (Experience_9_IsEnded) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Предупреждение!",
                                                                  "Данный опыт уже проведен! \nПродолжить?",
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
          }
    }

    // сигнал от основного окна с номером текущего протокола
    emit signalFromMainNumberProtocol(experience_9->fileProtocol = fileProtocol);

    experience_9->exec();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slotExperienceNineIsEnded() // слот: опыт 9 завершен
{
    ui->pushButton_experience_9->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
    Experience_9_IsEnded = true;
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_startExperienceNine() // слот: Старт Опыта 9. Измерение сопротивления обмоток горячего ПЭД
{
    stop_measuredIsolation = false;
    stop_measuredResistance = false;
    stopExperienceEight = false;
    stop_run = false;
    stopExperienceTMS = false;
    stopExperienceFive = false;
    stopExperienceSix= false;
    //**************************************************************************************************************
    /*
        * Создаем экземпляр класса QModbusRtuSerialMaster
        * Связываем сигнал errorOccurred с методом класса errorString(),
        * обрабатывающим ошибки.
        */
    modbusDevice = new QModbusRtuSerialMaster(this);
    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
                statusBar()->showMessage(modbusDevice->errorString(), 5000);
            });
    /*
        * Также соединяем сигнал изменения состояния объекта modbusDevice
        * с методом onStateChanged, где мы будем обрабатывать изменения состояния (подключение или отключение порта)
        */
    if(modbusDevice) {
        device = "Миллиомметр";
        connect(modbusDevice, &QModbusClient::stateChanged,
                this, &MainWindow::onStateChanged);
    }
    /*
        * Если экземпляр объекта по каким-либо причинам не существует,
        * то во избежание критических ошибок и проблем покидаем функцию
        */
    if (!modbusDevice)
        return;
    /*
        * Если порт не подключен, то настраиваем его и пытаемся установить соединение.
        * Функция setConnectionParameter принимает 2 параметра:
        * 1) название устанавливаемого параметра
        * 2) значение параметра
        */
    statusBar()->clearMessage();
    if (modbusDevice->state() != QModbusDevice::ConnectedState) {
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, "COM7");
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, ui->lineEdit_PARITY_miko->text().toInt());
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, ui->lineEdit_BAUDS_miko->text().toInt());
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, ui->lineEdit_DATA_BITS_miko->text().toInt());
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, ui->lineEdit_STOP_BITS_miko->text().toInt());
        modbusDevice->setTimeout(ui->lineEdit_RESPONSE_TIME_miko->text().toInt());
        modbusDevice->setNumberOfRetries(1);
        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
        }
    } else {
        modbusDevice->disconnectDevice();
    }




//    // отправляем запрос в модуль 7019
//    QModbusReply *lastRequest = nullptr;
//    QModbusDataUnit readUnitCheck(QModbusDataUnit::HoldingRegisters, ui->lineEdit_int_addres_6->text().toInt(),
//                                  ui->lineEdit_quint16_size->text().toInt());
//    lastRequest = modbusDevice->sendReadRequest(readUnitCheck, ui->lineEdit_int_server_Addres->text().toInt());
//    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCheking);
//    else lastRequest->deleteLater();

//    delay(5);

//    if (!Cheking7019) {
//        modbusDevice->disconnectDevice();

//        modbusDevice->deleteLater();


//        modbusDevice = new QModbusRtuSerialMaster(this);
//        connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
//                    statusBar()->showMessage(modbusDevice->errorString(), 5000);
//                });

//        if(modbusDevice) {
//            connect(modbusDevice, &QModbusClient::stateChanged,
//                    this, &MainWindow::onStateChanged);
//        }

//        if (!modbusDevice)
//            return;

//        statusBar()->clearMessage(); // Подключаемся на скорости ПЧ Danfoss 38400
//        if (modbusDevice->state() != QModbusDevice::ConnectedState) {
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, ui->lineEdit_PORT_miko->text());
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, ui->lineEdit_PARITY_miko->text().toInt());
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, ui->lineEdit_BAUDS_Danfoss->text().toInt());
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, ui->lineEdit_DATA_BITS_miko->text().toInt());
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, ui->lineEdit_STOP_BITS_miko->text().toInt());
//            modbusDevice->setTimeout(ui->lineEdit_RESPONSE_TIME_miko->text().toInt());
//            modbusDevice->setNumberOfRetries(1);
//            if (!modbusDevice->connectDevice()) {
//                statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
//            }
//        } else {
//            modbusDevice->disconnectDevice();
//        }

//        // отправляем запрос в модуль 7019
//        QModbusDataUnit readUnitCheck(QModbusDataUnit::HoldingRegisters, ui->lineEdit_int_addres_6->text().toInt(),
//                                      ui->lineEdit_quint16_size->text().toInt());
//        lastRequest = modbusDevice->sendReadRequest(readUnitCheck, ui->lineEdit_int_server_Addres->text().toInt());
//        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceCheking);
//        else lastRequest->deleteLater();

//        // настраиваем скорость 115200 в модуле 7019
//        auto cmd = QModbusPdu::FunctionCode(0x46);
//        auto request = QModbusRequest(cmd, QByteArray::fromHex("06000A000000010000"));
//        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
//        lastRequest = modbusDevice->sendRawRequest(request, ui->lineEdit_int_server_Addres->text().toInt());
//        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
//        else lastRequest->deleteLater();

//        delay(1);

//        modbusDevice->disconnectDevice();

//        modbusDevice->deleteLater();


//        modbusDevice = new QModbusRtuSerialMaster(this);
//        connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
//                    statusBar()->showMessage(modbusDevice->errorString(), 5000);
//                });

//        if(modbusDevice) {
//            connect(modbusDevice, &QModbusClient::stateChanged,
//                    this, &MainWindow::onStateChanged);
//        }

//        if (!modbusDevice)
//            return;

//        statusBar()->clearMessage(); // Подключаемся на скорости МИКО 115200
//        if (modbusDevice->state() != QModbusDevice::ConnectedState) {
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, ui->lineEdit_PORT_miko->text());
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, ui->lineEdit_PARITY_miko->text().toInt());
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, ui->lineEdit_BAUDS_miko->text().toInt());
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, ui->lineEdit_DATA_BITS_miko->text().toInt());
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, ui->lineEdit_STOP_BITS_miko->text().toInt());
//            modbusDevice->setTimeout(ui->lineEdit_RESPONSE_TIME_miko->text().toInt());
//            modbusDevice->setNumberOfRetries(1);
//            if (!modbusDevice->connectDevice()) {
//                statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
//            }
//        } else {
//            modbusDevice->disconnectDevice();
//        }

//        delay(1);

//        // включаем дискретный выход 7 ОВЕН МУ110-224.8Р для сброса питания модуля 7019
//        cmd = QModbusPdu::FunctionCode(0x06);
//        request = QModbusRequest(cmd, QByteArray::fromHex("000603E8"));
//        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
//        lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
//        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
//        else lastRequest->deleteLater();

//        delay(10);

//        // отключаем выход 7 ОВЕН МУ110-224.8Р для восстановления питания модуля 7019
//        cmd = QModbusPdu::FunctionCode(0x06);
//        request = QModbusRequest(cmd, QByteArray::fromHex("00060000"));
//        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
//        lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
//        if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
//        else lastRequest->deleteLater();


//    }

    // отправляем команду отключения реле 3
    QModbusReply *lastRequest = nullptr;
    auto cmd = QModbusPdu::FunctionCode(0x06);
    auto request = QModbusRequest(cmd, QByteArray::fromHex("00020000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0a);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем

    // отправляем команду на останов ПЧ
    cmd = QModbusPdu::FunctionCode(0x0f);
    request = QModbusRequest(cmd, QByteArray::fromHex("000000200474049919"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x06);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFive);
    else lastRequest->deleteLater();


    int timeOfMeasurementColdWindingResistance = 10; // время измерения
    //***********************************************************************
    // включаем реле переключения RS-485
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000603E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(2); // ждем

    // включаем контактор мико
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000103E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(2); // ждем

    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Запущено измерение сопротивления обмотки UV");
    // отправляем команду на включение выхода 4
    //QModbusReply *lastRequest = nullptr;
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000303E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(5); // ждем



    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет программирование МИКО-9А");
    // программируем
    cmd = QModbusPdu::FunctionCode(0x43);
    request = QModbusRequest(cmd, QByteArray::fromHex("0101"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(5); // ждем



    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет первое измерение обмотки UV");
    // измеряем, замер 1
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
        QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет второе измерение обмотки UV");
    // измеряем, замер 2
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished())  connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем



    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет третье измерение обмотки UV");
    // измеряем, замер 3
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int { return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    measuredColdResistanceIsFinished_UV = true; // измерение обмотки UV завершено
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос датчика температуры");
    // температура
    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters, 0x06, 0x01);
    lastRequest = modbusDevice->sendReadRequest(readUnit, 0x57);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceTemperature_hot);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Измерение сопротивления обмотки UV завершено");
    // отправляем команду на отключение выхода 4
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00030000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();

    measuredColdResistanceIsFinished_UV = false; // чтобы сигнал signalFromMainTo_window_experience_2_windingResistance не передал предыдущие значения
    sumColdResistance = 0; // обнуляем
    delay(5); // ждем



    //***********************************************************************
    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Запущено измерение сопротивления обмотки VW");
    // отправляем команду на включение выхода 5
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000403E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет первое измерение обмотки VW");
    // измеряем, замер 1
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished())  connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет второе измерение обмотки VW");
    // измеряем, замер 2
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет третье измерение обмотки VW");
    // измеряем, замер 3
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
        QModbusResponse::registerDataSizeCalculator( cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    measuredColdResistanceIsFinished_VW = true;
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос датчика температуры");
    // температура
    //QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters, 0x07, 0x01); // это уже задано выше
    lastRequest = modbusDevice->sendReadRequest(readUnit, 0x57);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceTemperature_hot);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Измерение сопротивления обмотки VW завершено");
    // отправляем команду на отключение выхода 5
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00040000"));
    QModbusResponse::registerDataSizeCalculator( cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    measuredColdResistanceIsFinished_VW = false;
    sumColdResistance = 0;
    delay(5); // ждем



    //***********************************************************************
    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Запущено измерение сопротивления обмотки UW");
    // отправляем команду на включение выхода 6
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000503E8"));
    QModbusResponse::registerDataSizeCalculator( cmd, [](const QModbusResponse &)->int { return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет первое измерение обмотки UW");
    // измеряем, замер 1
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished())  connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет второе измерение обмотки UW");
    // измеряем, замер 2
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10; });
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Идет третье измерение обмотки UW");
    // измеряем, замер 3
    cmd = QModbusPdu::FunctionCode(0x41);
    request = QModbusRequest(cmd, QByteArray::fromHex("0001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(timeOfMeasurementColdWindingResistance); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос МИКО-9А");
    // опрос
    cmd = QModbusPdu::FunctionCode(0x44);
    request = QModbusRequest(cmd, QByteArray::fromHex(""));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 10;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x55);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    measuredColdResistanceIsFinished_UW = true;
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Опрос датчика температуры");
    // температура
    //QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters, 0x07, 0x01);
    lastRequest = modbusDevice->sendReadRequest(readUnit, 0x57);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceTemperature_hot);
    else lastRequest->deleteLater();
    delay(5); // ждем


    if (stop_measuredResistance) {
        modbusDevice->deleteLater();
        return;}
    emit signalFromMainMeasurement("Измерение сопротивления обмоток завершено");
    // отправляем команду на отключение выхода 6
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00050000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    measuredColdResistanceIsFinished_UW = false;
    sumColdResistance = 0;
    delay(5); // ждем

    // отключаем контактор мико
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00010000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(2); // ждем

    // отключаем реле переключения RS-485
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00060000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();
    delay(1); // ждем

    QMessageBox::about(this, "Уведомление", "Измерение завершено");

    modbusDevice->deleteLater();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_stopMeasurementHotWindingResistance()
{
    stop_measuredResistance = true;

    if (!modbusDevice)
        return;

    QModbusReply *lastRequest = nullptr;

    auto cmd = QModbusPdu::FunctionCode(0x06);
    auto request = QModbusRequest(cmd, QByteArray::fromHex("00030000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();

    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00040000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();

    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00050000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();

    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00010000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance_hot);
    else lastRequest->deleteLater();

    emit signalFromMainMeasurement("Измерения остановлены пользователем");
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::responceResistance_hot() // обработка ответа от реле и МИКО-9А
{
    QModbusReply* lastRequest = qobject_cast<QModbusReply*>(sender());
    if (!lastRequest) return;
    if (lastRequest->error() == QModbusDevice::NoError){
        QModbusResponse unit = lastRequest->rawResult();
    } else {
        ui->statusbar->showMessage("Ошибка чтения!", 1000);
    }
    lastRequest->deleteLater();

    // если ответ от реле включения контактора мегаомметра
    if (lastRequest->serverAddress() == 0x10 && QString::fromUtf8(lastRequest->rawResult().data().toHex()) == "020010") {
        ui->label_statusRele->setText("Контактор мегаомметра включен!");
    }

    // если ответ от МИКО-9А
    if (lastRequest->serverAddress() == 0x55) {
        //ui->lineEdit_Number_of_SEM->setText(QString::fromUtf8(lastRequest->rawResult().data().toHex()));
        QString number, res;
        res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
        for (int i = 4; i < 12; i++) {
            number += res[i];
        }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true

        float y = *reinterpret_cast<float *>(&hex);

        if (y != 0)
            sumColdResistance += y;
        if (measuredColdResistanceIsFinished_UV)
        emit signalFromMainTo_window_experience_9_windingResistance(sumColdResistance, measuredColdResistanceIsFinished_UV,
                                                                    measuredColdResistanceIsFinished_VW, measuredColdResistanceIsFinished_UW,
                                                                    temperatureColdResistance);

        if (measuredColdResistanceIsFinished_VW)
        emit signalFromMainTo_window_experience_9_windingResistance(sumColdResistance, measuredColdResistanceIsFinished_UV,
                                                                    measuredColdResistanceIsFinished_VW, measuredColdResistanceIsFinished_UW,
                                                                    temperatureColdResistance);

        if (measuredColdResistanceIsFinished_UW)
        emit signalFromMainTo_window_experience_9_windingResistance(sumColdResistance, measuredColdResistanceIsFinished_UV,
                                                                    measuredColdResistanceIsFinished_VW, measuredColdResistanceIsFinished_UW,
                                                                    temperatureColdResistance);
    }
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::responceTemperature_hot() // обработка ответа от датчика температуры при измерении сопротивлений
                                       // обмоток ПЭД
{
    QModbusReply* lastRequest = qobject_cast<QModbusReply*>(sender());
    if (!lastRequest) return;
    if (lastRequest->error() == QModbusDevice::NoError){
        QModbusDataUnit unit = lastRequest->result();
        //ui->lineEdit_type_of_SEM->setText(QString::number(unit.value(0)));
    } else {
        ui->statusbar->showMessage("Ошибка чтения!");
    }
    lastRequest->deleteLater();

    // собираем в number символы из ответа - со второго по пятый символы
    QString number, res;
    res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
    for (int i = 2; i < 6; i++) {
        number += res[i];
    }
    // преобразуем из строкового типа в целочисленный шестнадцатиричный
    bool ok;
    int hex = number.toInt(&ok, 16); // hex == 255, ok == true
    temperatureColdResistance = hex; // температура в шестнадцатиричном формате
    // передаем температуру в 9 окно
    emit signalFromMainTo_window_experience_9_windingResistance(sumColdResistance, measuredColdResistanceIsFinished_UV,
                                                                measuredColdResistanceIsFinished_VW, measuredColdResistanceIsFinished_UW,
                                                                temperatureColdResistance);
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::on_pushButton_experience_10_clicked() // кнопка Опыт 10
{
    // отправляем в пятое окно
    QString ResistanceIsolation = ui->lineEdit_ResistanceIsolation_of_SEM->text();
    for (unsigned int i = 0; i < sizeof(ResistanceIsolation); i++) {
        if (ResistanceIsolation[i] == 44) { ResistanceIsolation[i] = 46; } // меняем запятую на точку
    }

    emit signalFromMainTo_window_experience_10_Parameters(ResistanceIsolation.toFloat());

    if (Experience_10_IsEnded) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Предупреждение!",
                                                                  "Данный опыт уже проведен! \nПродолжить?",
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
          }
    }

    // сигнал от основного окна с номером текущего протокола
    emit signalFromMainNumberProtocol(experience_10->fileProtocol = fileProtocol);

    experience_10->exec();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slotExperienceTenIsEnded() // слот: опыт 10 завершен
{
    ui->pushButton_experience_10->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
    Experience_10_IsEnded = true;
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_startMeasurementHotIsolationResistance() // слот: Старт Опыта 10. Измерение сопротивления изоляции
                                                               // горячего ПЭД
{
    stop_measuredIsolation = false;
    stop_measuredResistance = false;
    stopExperienceEight = false;
    stop_run = false;
    stopExperienceTMS = false;
    stopExperienceFive = false;
    stopExperienceSix= false;

    modbusDevice = new QModbusRtuSerialMaster(this);
    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
                statusBar()->showMessage(modbusDevice->errorString(), 5000);
            });

    if(modbusDevice) {
        device = "Мегаомметр";
        connect(modbusDevice, &QModbusClient::stateChanged,
                this, &MainWindow::onStateChanged);
    }
    if (!modbusDevice)
        return;

    statusBar()->clearMessage();
    if (modbusDevice->state() != QModbusDevice::ConnectedState) {
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, PORT);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, PARITY);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, 115200);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, DATA_BITS);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, STOP_BITS);
        modbusDevice->setTimeout(RESPONSE_TIME);
        modbusDevice->setNumberOfRetries(1);
        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
        }
    } else {
        modbusDevice->disconnectDevice();
    }

    // отправляем команду отключения реле 3
    QModbusReply *lastRequest = nullptr;
    auto cmd = QModbusPdu::FunctionCode(0x06);
    auto request = QModbusRequest(cmd, QByteArray::fromHex("00020000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0a);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем

    // отправляем команду на останов ПЧ
    cmd = QModbusPdu::FunctionCode(0x0f);
    request = QModbusRequest(cmd, QByteArray::fromHex("000000200474049919"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x06);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFive);
    else lastRequest->deleteLater();



    // отправляем команду на включение выхода 0 Овен - включение контактора мегаомметра
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000003E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(2); // ждем

    // отправляем запрос в модуль дискретных входов ОВЕН
    cmd = QModbusPdu::FunctionCode(0x03);
    request = QModbusRequest(cmd, QByteArray::fromHex("00330001"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 3;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x10);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем

    COMPORT = new QSerialPort();
    COMPORT->setPortName("COM1");
    COMPORT->setBaudRate(QSerialPort::BaudRate::Baud9600);
    COMPORT->setParity(QSerialPort::Parity::NoParity);
    COMPORT->setDataBits(QSerialPort::DataBits::Data8);
    COMPORT->setStopBits(QSerialPort::StopBits::OneStop);
    COMPORT->setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    COMPORT->open(QIODevice::ReadWrite);

    if (COMPORT->isOpen()){
        qDebug() << "Serial Port Is Connected";
        qDebug() << COMPORT->error();
    } else {
        qDebug() << "Serial Port Is Not Connected";
        qDebug() << COMPORT->error();
    }
    COMPORT->flush();

    float ResistanceIsolation = 0, U = 0;

    if (COMPORT->isOpen()){

        COMPORT->clear();


    //***********************************************************************
         ui->statusbar->showMessage("Процесс: Rn_one");
         delay(1);
         char buf_Rn_one[6];
         do {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Rn\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Rn_one, sizeof(buf_Rn_one));
             qDebug() << buf_Rn_one;
         } while (buf_Rn_one[2] != 79 && buf_Rn_one[3] != 75 && buf_Rn_one[4] != 13 && buf_Rn_one[5] != 10/*!COMPORT->isReadable()*/);
    //***********************************************************************
         ui->statusbar->showMessage("Процесс: Fs_one");
         delay(1);
         char buf_Fs_one[6];
         do {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Fs04\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Fs_one, sizeof(buf_Fs_one));
             qDebug() << buf_Fs_one;
         } while (buf_Fs_one[2] != 79 && buf_Fs_one[3] != 75 && buf_Fs_one[4] != 13 && buf_Fs_one[5] != 10);
    //***********************************************************************
         ui->statusbar->showMessage("Процесс: Is");
         delay(1);
         char buf_Is[6];
         do {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Is030\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Is, sizeof(buf_Is));
             qDebug() << buf_Is;
         } while (buf_Is[2] != 79 && buf_Is[3] != 75 && buf_Is[4] != 13 && buf_Is[5] != 10);
    //***********************************************************************
         ui->statusbar->showMessage("Процесс: Bd_one");
         delay(1);
         char buf_Bd_one[6];
         do {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Bd\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Bd_one, sizeof(buf_Bd_one));
             qDebug() << buf_Bd_one;
         } while (buf_Bd_one[2] != 79 && buf_Bd_one[3] != 75 && buf_Bd_one[4] != 13 && buf_Bd_one[5] != 10);
    //***********************************************************************
        timer = new QTimer();
        time.setHMS(0, 1, 0); // обратный отсчет
        connect(timer, SIGNAL(timeout()), this, SLOT(updateCountdown()));
        timer->start(1000);

        delay_for_Meger(60);


    //***********************************************************************
         ui->statusbar->showMessage("Процесс: Bu_one");
         delay(1);
         char buf_Bu_one[7];
         do {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Bu\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Bu_one, sizeof(buf_Bu_one));
             qDebug() << buf_Bu_one << "STOP";
             delay(2);
         } while (buf_Bu_one[2] != 79 && buf_Bu_one[3] != 75 && buf_Bu_one[4] != 13 && buf_Bu_one[5] != 10/*!COMPORT->isReadable()*/);
    //***********************************************************************
         // проверяем остановилось ли измерение
         ui->statusbar->showMessage("Процесс: St");
         delay(1);
         char buf_St[16];
         while (buf_St[2] != 49) {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("St\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_St, sizeof(buf_St));
             qDebug() << buf_St;

            if (buf_St[0] == 50 || buf_St[3] == 50) {
                 do {
                    if (stop_measuredIsolation) {break;}
                   COMPORT->write("Bu\r\n");
                   COMPORT->waitForBytesWritten();
                   COMPORT->waitForReadyRead(500);
                   COMPORT->readLine(buf_Bu_one, sizeof(buf_Bu_one));
                   qDebug() << buf_Bu_one << "STOP";
                 } while (!COMPORT->isReadable());
            }
         }
    //***********************************************************************
         ui->statusbar->showMessage("Процесс: Df_R");
         delay(1);
         char buf_Df_R[7];
         do {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Df0040\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Df_R, sizeof(buf_Df_R));
             qDebug() << buf_Df_R;
        } while (!(buf_Df_R[2] == 79
                 && buf_Df_R[3] == 75 && buf_Df_R[4] == 13 && buf_Df_R[5] == 10));
    //***********************************************************************
         ui->statusbar->showMessage("Процесс: Dg_R");
         delay(1);
         char buf_Dg_R[16];
         do {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Dg\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Dg_R, sizeof(buf_Dg_R));
             qDebug() << buf_Dg_R;
               char buffer[sizeof(buf_Dg_R)];

               if (buf_Dg_R[0] == 17  && buf_Dg_R[1] == 19 && buf_Dg_R[2] == 32) {
                   for (unsigned int i = 0; i < sizeof(buf_Dg_R) - 2; i++) {
                       buffer[i] = buf_Dg_R[i+2];
                   }
               }
               else if (buf_Dg_R[2] == 79  && buf_Dg_R[3] == 32 && buf_Dg_R[4] == 50) {
                   for (unsigned int i = 0; i < sizeof(buf_Dg_R) - 3; i++) {
                       buffer[i] = buf_Dg_R[i+3];
                   }
               }

             QString num = buffer;
             ResistanceIsolation = num.toFloat() / 1000000;
         } while (!((buf_Dg_R[0] == 17  && buf_Dg_R[1] == 19 && buf_Dg_R[2] == 32) ||
                    (buf_Dg_R[2] == 79  && buf_Dg_R[3] == 32 && buf_Dg_R[4] == 50)));
    //***********************************************************************
         ui->statusbar->showMessage("Процесс: Df_U");
         delay(1);
         char buf_Df_U[16];
         do {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Df0100\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Df_U, sizeof(buf_Df_U));
             qDebug() << buf_Df_U;
         } while (!(buf_Df_U[2] == 79
                    && buf_Df_U[3] == 75 && buf_Df_U[4] == 13 && buf_Df_U[5] == 10));
    //***********************************************************************
         ui->statusbar->showMessage("Процесс: Dg_U");
         delay(1);
         char buf_Dg_U[16];
         do {
             if (stop_measuredIsolation) {break;}
             COMPORT->write("Dg\r\n");
             COMPORT->waitForBytesWritten();
             COMPORT->waitForReadyRead(500);
             COMPORT->readLine(buf_Dg_U, sizeof(buf_Dg_U));
             qDebug() << buf_Dg_U;
               char buffer[sizeof(buf_Dg_U)];
               for (unsigned int i = 0; i < sizeof(buf_Dg_U) - 2; i++) {
                   buffer[i] = buf_Dg_U[i+2];
               }
             QString num = buffer;
             U = num.toFloat();
         } while (!(buf_Dg_U[2] == 32 && buf_Dg_U[3] == 43));

    delay(2);
    }


    //***********************************************************************


    emit signalFromMainTo_window_experience_10_ResistanceIsolation(ResistanceIsolation, U);

    // отправляем команду отключения реле
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00000000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice->sendRawRequest(request, 0x0a);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем

    QMessageBox::about(this, "Уведомление", "Измерение завершено");

    COMPORT->deleteLater();
    modbusDevice->deleteLater();

    ui->label_statusRele->clear();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_stop_measuredHotIsolation() // остановить измерения в 10 окне
{
    stop_measuredIsolation = true;
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::on_pushButton_experience_11_clicked() // кнопка Опыт 11
{
    if (Experience_11_IsEnded) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Предупреждение!",
                                                                  "Данный опыт уже проведен! \nПродолжить?",
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
          }
    }
    // сигнал от основного окна с номером текущего протокола
    emit signalFromMainNumberProtocol(experience_11->fileProtocol = fileProtocol);
    experience_11->exec();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slotExperienceElevenIsEnded() // слот: опыт 11 завершен
{
    ui->pushButton_experience_11->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
    Experience_11_IsEnded = true;
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::on_pushButton_experience_12_clicked() // кнопка Опыт 12
{
    QString TestingIsolationInterTurn = ui->lineEdit_TestingIsolationInterTurn_of_SEM->text();
    for (unsigned int i = 0; i < sizeof(TestingIsolationInterTurn); i++) {
        if (TestingIsolationInterTurn[i] == 44) { TestingIsolationInterTurn[i] = 46; } // меняем запятую на точку
    }

    emit signalFromMainTo_window_experience_12_Parameters(TestingIsolationInterTurn.toFloat());

    if (Experience_12_IsEnded) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Предупреждение!",
                                                                  "Данный опыт уже проведен! \nПродолжить?",
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
          }
    }

    emit signalFromMainNumberProtocol(fileProtocol); // сигнал от основного окна с номером текущего протокола
    experience_12->exec();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slotlExperienceTwelveIsEnded() // слот: опыт 12 завершен
{
    ui->pushButton_experience_12->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
    Experience_12_IsEnded = true;
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::on_pushButton_experience_13_clicked() // кнопка Опыт 13
{
    QString TestingIsolationBody = ui->lineEdit_TestingIsolationBody_of_SEM->text();
    for (unsigned int i = 0; i < sizeof(TestingIsolationBody); i++) {
        if (TestingIsolationBody[i] == 44) { TestingIsolationBody[i] = 46; } // меняем запятую на точку
    }

    emit signalFromMainTo_window_experience_13_Parameters(TestingIsolationBody.toFloat());

    if (Experience_13_IsEnded) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Предупреждение!",
                                                                  "Данный опыт уже проведен! \nПродолжить?",
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
          }
    }

    emit signalFromMainNumberProtocol(fileProtocol); // сигнал от основного окна с номером текущего протокола
    experience_13->exec();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slotlExperienceThirteenIsEnded() // слот: опыт 13 завершен
{
    ui->pushButton_experience_13->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
    Experience_13_IsEnded = true;
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::on_pushButton_experience_14_clicked() // кнопка Опыт 14
{
    if (Experience_14_IsEnded) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Предупреждение!",
                                                                  "Данный опыт уже проведен! \nПродолжить?",
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
          }
    }

    emit signalFromMainNumberProtocol(fileProtocol); // сигнал от основного окна с номером текущего протокола
    experience_14->exec();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slotlExperienceFourteenIsEnded() // слот: опыт 14 завершен
{
    ui->pushButton_experience_14->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
    Experience_14_IsEnded = true;
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::on_pushButton_experience_15_clicked() // кнопка Опыт 15
{
    if (Experience_15_IsEnded) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Предупреждение!",
                                                                  "Данный опыт уже проведен! \nПродолжить?",
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
          }
    }

    emit signalFromMainNumberProtocol(fileProtocol); // сигнал от основного окна с номером текущего протокола
    experience_15->exec();
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slotlExperienceFifteenIsEnded()
{
    ui->pushButton_experience_15->setStyleSheet("*{background-color: rgb(51, 255, 153);}");
    Experience_15_IsEnded = true;
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_signal_testTMS()
{
    stop_measuredIsolation = false;
    stop_measuredResistance = false;
    stopExperienceEight = false;
    stop_run = false;
    stopExperienceTMS = false;
    stopExperienceFive = false;
    stopExperienceSix= false;

    modbusDevice_Oven = new QModbusRtuSerialMaster(this);
    connect(modbusDevice_Oven, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
                statusBar()->showMessage(modbusDevice_Oven->errorString(), 5000);
            });

    if(modbusDevice_Oven) {
        device = "ТМС";
        connect(modbusDevice_Oven, &QModbusClient::stateChanged,
                this, &MainWindow::onStateChanged);
    }

    if (!modbusDevice_Oven)
        return;

    statusBar()->clearMessage();
    if (modbusDevice_Oven->state() != QModbusDevice::ConnectedState) {
        modbusDevice_Oven->setConnectionParameter(QModbusDevice::SerialPortNameParameter, PORT);
        modbusDevice_Oven->setConnectionParameter(QModbusDevice::SerialParityParameter, PARITY);
        modbusDevice_Oven->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, 115200);
        modbusDevice_Oven->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, DATA_BITS);
        modbusDevice_Oven->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, STOP_BITS);
        modbusDevice_Oven->setTimeout(RESPONSE_TIME);
        modbusDevice_Oven->setNumberOfRetries(1);
        if (!modbusDevice_Oven->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice_Oven->errorString(), 5000);
        }
    } else {
        modbusDevice_Oven->disconnectDevice();
    }

    // отправляем команду отключения реле 1
    QModbusReply *lastRequest = nullptr;
    auto cmd = QModbusPdu::FunctionCode(0x06);
    auto request = QModbusRequest(cmd, QByteArray::fromHex("00000000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice_Oven->sendRawRequest(request, 0x0a);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем

    // отправляем команду отключения реле 2
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("00010000"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice_Oven->sendRawRequest(request, 0x0a);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceResistance);
    else lastRequest->deleteLater();
    delay(1); // ждем


    // отправляем команду на включение выхода 3 Овен - включение контактора ПЧ
    cmd = QModbusPdu::FunctionCode(0x06);
    request = QModbusRequest(cmd, QByteArray::fromHex("000203E8"));
    QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
    lastRequest = modbusDevice_Oven->sendRawRequest(request, 0x0A);
    if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFifteen);
    else lastRequest->deleteLater();

    delay(1); // ждем
    modbusDevice_Oven->disconnectDevice();
    modbusDevice_Oven->deleteLater();

    //**************************************************************************************************

    modbusDevice = new QModbusRtuSerialMaster(this);
    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
                statusBar()->showMessage(modbusDevice->errorString(), 5000);
            });

    if(modbusDevice) {
        connect(modbusDevice, &QModbusClient::stateChanged,
                this, &MainWindow::onStateChanged);
    }

    if (!modbusDevice)
        return;

    statusBar()->clearMessage();
    if (modbusDevice->state() != QModbusDevice::ConnectedState) {
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, PORT);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, PARITY);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, 19200);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, DATA_BITS);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, STOP_BITS);
        modbusDevice->setTimeout(RESPONSE_TIME);
        modbusDevice->setNumberOfRetries(1);
        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
        }
    } else {
        modbusDevice->disconnectDevice();
    }


        int i = 0;
        while (stopExperienceTMS == false) {
            i++;
            qDebug() << "Спрашиваю: " << i;
            temperature_oil = true;
            cmd = QModbusPdu::FunctionCode(0x04);
            request = QModbusRequest(cmd, QByteArray::fromHex("061b0001")); // Температура масла двигателя
            QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 3;});
            lastRequest = modbusDevice->sendRawRequest(request, 0x0f);
            if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFifteen);
            else lastRequest->deleteLater();
            delay_for_Meger(1); // ждем
            temperature_oil = false;

            temperature_winding = true;
            cmd = QModbusPdu::FunctionCode(0x04);
            request = QModbusRequest(cmd, QByteArray::fromHex("06090001")); // Температура обмоток двигателя
            QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 3;});
            lastRequest = modbusDevice->sendRawRequest(request, 0x0f);
            if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFifteen);
            else lastRequest->deleteLater();
            delay_for_Meger(1); // ждем
            temperature_winding = false;

            connection_pressure = true;
            cmd = QModbusPdu::FunctionCode(0x04);
            request = QModbusRequest(cmd, QByteArray::fromHex("060a0001")); // Давление внешней среды
            QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 3;});
            lastRequest = modbusDevice->sendRawRequest(request, 0x0f);
            if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFifteen);
            else lastRequest->deleteLater();
            delay_for_Meger(1); // ждем
            connection_pressure = false;

            connection_vibration = true;
            cmd = QModbusPdu::FunctionCode(0x04);
            request = QModbusRequest(cmd, QByteArray::fromHex("06050001")); // Амплитуда вибрации по X
            QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 3;});
            lastRequest = modbusDevice->sendRawRequest(request, 0x0f);
            if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFifteen);
            else lastRequest->deleteLater();
            delay_for_Meger(1); // ждем
            connection_vibration = false;

            connection_isolation = true;
            cmd = QModbusPdu::FunctionCode(0x04);
            request = QModbusRequest(cmd, QByteArray::fromHex("06000001")); // Сопротивление изоляции
            QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 3;});
            lastRequest = modbusDevice->sendRawRequest(request, 0x0f);
            if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFifteen);
            else lastRequest->deleteLater();
            delay_for_Meger(1); // ждем
            connection_isolation = false;
    }

        delay(1); // ждем
        modbusDevice->disconnectDevice();
        modbusDevice->deleteLater();

        // ***************************************************************************************************************
        if (stopExperienceTMS) {
            modbusDevice_Oven = new QModbusRtuSerialMaster(this);
            connect(modbusDevice_Oven, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
                        statusBar()->showMessage(modbusDevice_Oven->errorString(), 5000);
                    });

            if(modbusDevice_Oven) {
                connect(modbusDevice_Oven, &QModbusClient::stateChanged,
                        this, &MainWindow::onStateChanged);
            }

            if (!modbusDevice_Oven)
                return;

            statusBar()->clearMessage();
            if (modbusDevice_Oven->state() != QModbusDevice::ConnectedState) {
                modbusDevice_Oven->setConnectionParameter(QModbusDevice::SerialPortNameParameter, PORT);
                modbusDevice_Oven->setConnectionParameter(QModbusDevice::SerialParityParameter, PARITY);
                modbusDevice_Oven->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, 115200);
                modbusDevice_Oven->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, DATA_BITS);
                modbusDevice_Oven->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, STOP_BITS);
                modbusDevice_Oven->setTimeout(RESPONSE_TIME);
                modbusDevice_Oven->setNumberOfRetries(1);
                if (!modbusDevice_Oven->connectDevice()) {
                    statusBar()->showMessage(tr("Connect failed: ") + modbusDevice_Oven->errorString(), 5000);
                }
            } else {
                modbusDevice_Oven->disconnectDevice();
            }

            QModbusReply *lastRequest = nullptr;

            // отправляем команду на отключение выхода 3 Овен - отключение контактора ПЧ
            auto cmd = QModbusPdu::FunctionCode(0x06);
            auto request = QModbusRequest(cmd, QByteArray::fromHex("00020000"));
            QModbusResponse::registerDataSizeCalculator(cmd, [](const QModbusResponse &)->int {return 4;});
            lastRequest = modbusDevice_Oven->sendRawRequest(request, 0x0A);
            if (!lastRequest->isFinished()) connect(lastRequest, &QModbusReply::finished, this, &MainWindow::responceExperienceFifteen);
            else lastRequest->deleteLater();

            delay(1); // ждем
            modbusDevice_Oven->deleteLater();
        }

}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::responceExperienceFifteen() // обработка сигналов Модбас опыта 15
{
    QModbusReply* lastRequest = qobject_cast<QModbusReply*>(sender());
    if (!lastRequest) return;
    if (lastRequest->error() == QModbusDevice::NoError){
        QModbusResponse unit = lastRequest->rawResult();
    } else {
        ui->statusbar->showMessage("Ошибка чтения!");
    }
    lastRequest->deleteLater();

    // если ответ от ...
    if (lastRequest->serverAddress() == 0x0f && temperature_oil) {
          QString number, res;
          res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
          qDebug() << lastRequest->rawResult().data().toHex();
         for (int i = 2; i < 6; i++) {
             number += res[i];
            }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true
          float temperature_oil = hex;

          if (hex > 0)
          emit signalFromMainTo_window_experience_15_temperature_oil(temperature_oil/100);
    }

    if (lastRequest->serverAddress() == 0x0f && temperature_winding) {
          QString number, res;
          res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
         for (int i = 2; i < 6; i++) {
             number += res[i];
            }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true
          float temperature_winding = hex;

          if (hex > 0)
          emit signalFromMainTo_window_experience_15_temperature_winding(temperature_winding/100);
    }

    if (lastRequest->serverAddress() == 0x0f && connection_pressure) {
          QString number, res;
          res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
         for (int i = 0; i < 2; i++) {
             number += res[i];
            }

         int connection_pressure = number.toInt(); // проверяем, что получено в ответе два байта

         emit signalFromMainTo_window_experience_15_connection_pressure(connection_pressure);
    }

    if (lastRequest->serverAddress() == 0x0f && connection_vibration) {
          QString number, res;
          res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
         for (int i = 0; i < 2; i++) {
             number += res[i];
            }

          int connection_vibration = number.toInt(); // проверяем, что получено в ответе два байта

          emit signalFromMainTo_window_experience_15_connection_vibration(connection_vibration);
    }

    if (lastRequest->serverAddress() == 0x0f && connection_isolation) {
          QString number, res;
          res = QString::fromUtf8(lastRequest->rawResult().data().toHex());
         for (int i = 2; i < 6; i++) {
             number += res[i];
            }

          bool ok;
          int hex = number.toInt(&ok, 16);       // hex == 255, ok == true
          float connection_isolation = hex;

          if (hex > 0)
          emit signalFromMainTo_window_experience_15_connection_isolation(connection_isolation);
    }
}
//################################################-КОНЕЦ-##################################################################

//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_stop_signal_testTMS()
{
    stopExperienceTMS = true;
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::onStateChanged(int state) {
    if (state == QModbusDevice::UnconnectedState)
        ui->statusbar->showMessage("Отключено!");
    else if (state == QModbusDevice::ConnectedState)
        ui->statusbar->showMessage("Соединение установлено для прибора: " + device);
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::on_pushButton_addBase_clicked()
{
    model->insertRow(model->rowCount()); // по нажатию на кнопку будет создаваться строка в базе данных
}
//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::slot_password()
{
    ui->tabWidget->setTabEnabled(0, false); // если пароль введен не верно - деактивируем вкладку
}


//################################################-КОНЕЦ-##################################################################





//################################################-НАЧАЛО-#################################################################
void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    if (index == 0){ // индекс первой вкладки с базой = 0
        password->exec();
    }

    if (index == 3 && the_tab_has_already_opened == false){

        QString fileProtocols = "C:\\reda\\";
        QDir dir;  //объявляем объект работы с папками
        dir.cd(fileProtocols);
        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);   //устанавливаем фильтр выводимых файлов/папок (см ниже)
        dir.setSorting(QDir::Size | QDir::Reversed);   //устанавливаем сортировку "от меньшего к большему"
        QFileInfoList list = dir.entryInfoList();     //получаем список файлов директории

        for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        ui->listWidget_list_protocols->addItem(QString("%1").arg(fileInfo.fileName()));
        }

        the_tab_has_already_opened = true;

        connect(ui->lineEdit_search_protocols, &QLineEdit::returnPressed, this, &MainWindow::searchIn_listWidget_protocols); // сигнал нажатия Enter в поисковой строке
    }

}
//################################################-КОНЕЦ-##################################################################



//################################################-НАЧАЛО-#################################################################
void MainWindow::searchIn_listWidget_protocols() // функция поиска по содержимому в поисковой строке протоколов
{
QString searchText = ui->lineEdit_search_protocols->text();
int listWidgetSize = ui->listWidget_list_protocols->count();

for (int k1 = 0; k1 < listWidgetSize; k1++)
    {
    if (ui->listWidget_list_protocols->item(k1)->text().contains(searchText))
    {
         ui->listWidget_list_protocols->item(k1)->setHidden(false);
    }
    else
    {
         ui->listWidget_list_protocols->item(k1)->setHidden(true);
    }
}
}
//################################################-КОНЕЦ-##################################################################

void MainWindow::on_pushButton_open_protocol_clicked()
{
    QString current_protocol = "C:\\reda\\" + ui->listWidget_list_protocols->currentItem()->text();

    QAxObject* excel = new QAxObject("Excel.Application", this);

    excel->dynamicCall ("SetVisible(bool)", true); //  отображать форму
    excel->setProperty ("DisplayAlerts", true); //  отображать какую-либо предупреждающую информацию, например, сохранять ли подсказку при закрытии

    QAxObject * workbooks = excel-> querySubObject ("Workbooks"); // Получить коллекцию книг (файлы excel)
    workbooks->querySubObject("Open(const QString&)", current_protocol);

    delete excel;
}

void MainWindow::on_pushButton_work_protocol_clicked()
{
    QString current_protocol = ui->listWidget_list_protocols->currentItem()->text();

    int pos = current_protocol.lastIndexOf(QChar('.'));
    ui->lineEdit_old_protocol->setText(current_protocol.left(pos));

    ui->tabWidget->setCurrentIndex(2);

    on_pushButtonc_clicked(); // "нажимаем" кнопку Найти протокол

    connect(ui->lineEdit_search, &QLineEdit::textChanged, this, &MainWindow::searchIn_listWidget_typePED); // сигнал изменения текста в поисковой строке
    ui->lineEdit_search->setText(ui->lineEdit_type_of_SEM->text());

    int i = 0;
    for (i = 0; i < 80; i++) {
        if (
        (model->itemData(model->index(i, 1))[0]).toString() ==
                ui->lineEdit_type_of_SEM->text()
                ) {
            break;
        }

    }
    on_listWidget_typePED_currentRowChanged(i);

    fileProtocol = "C:\\reda\\" + ui->lineEdit_NumberProtocol->text() +
    "_" + ui->lineEdit_Number_of_SEM->text() + "_" + ui->lineEdit_type_of_SEM->text() + ".xlsx";

    // деактивируем окна после применения выбранного протокола
    ui->pushButton_saveProtocol->hide();
    ui->lineEdit_NumberProtocol->setDisabled(true);
    ui->comboBox_executionGroup->setDisabled(true);
    ui->lineEdit_Number_of_SEM->setDisabled(true);
    ui->comboBox_performanceCategory->setDisabled(true);
    ui->comboBox_typeOil->setDisabled(true);
    ui->lineEdit_typeCoupling->setDisabled(true);
    ui->comboBox_markingCheck->setDisabled(true);
    ui->lineEdit_beating->setDisabled(true);
    ui->comboBox_RTI->setDisabled(true);
    ui->lineEdit_shaftDiameter->setDisabled(true);
    ui->lineEdit_typeTMS->setDisabled(true);
    ui->lineEdit_durationRun->setDisabled(true);
    ui->lineEdit_shaftDeparture->setDisabled(true);
    ui->comboBox_articulation->setDisabled(true);
    ui->lineEdit_typeBlockTMS->setDisabled(true);
    ui->lineEdit_firmwareVersion->setDisabled(true);
    ui->lineEdit_type_of_SEM->setDisabled(true);
    ui->lineEdit_power_of_SEM->setDisabled(true);
    ui->lineEdit_voltage_of_SEM->setDisabled(true);
    ui->lineEdit_current_of_SEM->setDisabled(true);
    ui->lineEdit_RoundPerMinute_of_SEM->setDisabled(true);
    ui->lineEdit_ResistanceWinding_of_SEM->setDisabled(true);
    ui->lineEdit_ResistanceIsolation_of_SEM->setDisabled(true);
    ui->lineEdit_AccelerationVoltage_of_SEM->setDisabled(true);
    ui->lineEdit_TorqueTurning_of_SEM->setDisabled(true);
    ui->lineEdit_VoltageShortCircuit_of_SEM->setDisabled(true);
    ui->lineEdit_CurrentShortCircuit_of_SEM->setDisabled(true);
    ui->lineEdit_LossesShortCircuit_of_SEM->setDisabled(true);
    ui->lineEdit_CurrentIdling_of_SEM->setDisabled(true);
    ui->lineEdit_VoltageIdling_of_SEM->setDisabled(true);
    ui->lineEdit_LossesIdling_of_SEM->setDisabled(true);
    ui->lineEdit_RunOut_of_SEM->setDisabled(true);
    ui->lineEdit_Vibration_of_SEM->setDisabled(true);
    ui->lineEdit_Torque_of_SEM->setDisabled(true);
    ui->lineEdit_LossesHot_of_SEM->setDisabled(true);
    ui->lineEdit_TestingIsolationInterTurn_of_SEM->setDisabled(true);
    ui->lineEdit_TestingIsolationBody_of_SEM->setDisabled(true);
    ui->lineEdit_high_voltage->setDisabled(true);
    ui->lineEdit_old_protocol->setDisabled(false);
    ui->pushButtonc->setDisabled(false);
    ui->lineEdit_operator->setDisabled(true);

    // активируем кнопки после применения выбранного протокола
    ui->pushButton_experience_1->setDisabled(false);
    ui->pushButton_experience_2->setDisabled(false);
    ui->pushButton_experience_3->setDisabled(false);
    ui->pushButton_experience_4->setDisabled(false);
    ui->pushButton_experience_5->setDisabled(false);
    ui->pushButton_experience_6->setDisabled(false);
    ui->pushButton_experience_7->setDisabled(false);
    ui->pushButton_experience_8->setDisabled(false);
    ui->pushButton_experience_9->setDisabled(false);
    ui->pushButton_experience_10->setDisabled(false);
    ui->pushButton_experience_11->setDisabled(false);
    ui->pushButton_experience_12->setDisabled(false);
    ui->pushButton_experience_13->setDisabled(false);
    ui->pushButton_experience_14->setDisabled(false);
    ui->pushButton_experience_15->setDisabled(false);
    ui->lineEdit_old_protocol->setDisabled(true);
    ui->lineEdit_old_protocol->clear();
    ui->pushButtonc->setDisabled(true);
    repaint();
}
