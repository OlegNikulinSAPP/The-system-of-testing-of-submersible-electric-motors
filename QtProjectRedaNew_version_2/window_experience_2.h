#ifndef WINDOW_EXPERIENCE_2_H
#define WINDOW_EXPERIENCE_2_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class window_experience_2;
}

class window_experience_2 : public QDialog
{
    Q_OBJECT

public:
    explicit window_experience_2(QWidget *parent = nullptr);
    ~window_experience_2();

    QString fileProtocol; // в эту переменную принимаем номер протокола из основного окна

public slots:
    // номер текущего протокола (в какой протокол сохранять результат)
    void slotFromMainMeasurement(QString measurement);

    void slotResistanceWinding_of_SEM(float ResistanceWinding_of_SEM); // принимает сопротивление обмоток ПЭД из основного окна
    void slotFromMainTo_window_experience_2_windingResistance(float sumColdResistance, bool measuredColdResistanceIsFinished_UV,
                                                              bool measuredColdResistanceIsFinished_VW,
                                                              bool measuredColdResistanceIsFinished_UW,
                                                              int temperatureColdResistance); // принимаем сигнал из основного окна - значение сопротивления
    void slotFromMainTo_window_experience_2_Calibration(float sumColdResistanceCalibration,
                                                        bool measuredColdResistanceIsFinished_UV_Calibration,
                                                        bool measuredColdResistanceIsFinished_VW_Calibration,
                                                        bool measuredColdResistanceIsFinished_UW_Calibration); // принимаем сигнал из основного окна - значение сопротивления при калибровке

    void slotInExperienceOneNumberProtocol(QString numberProtocol); // принимает номер протокола из основного окна

private slots:
    void on_pushButton_startMeasurementColdWindingResistance_clicked();
    void on_pushButton_stopMeasurementColdWindingResistance_clicked();
    void on_pushButton_saveMeasurementColdWindingResistance_clicked();
    void on_pushButton_Calibration_clicked();

    void closeEvent(QCloseEvent *); // ловим событие "закрыть окно"

signals:
    void signal_startMeasurementColdWindingResistance(); // сигнал из окна второго опыта "Начать измерения"
    void signal_stopMeasurementColdWindingResistance(); // сигнал из окна второго опыта "Стоп"
    void signalExperienceTwoIsEnded(); // сигнал из окна второго опыта "опыт 2 закончен"
    void signal_Calibration(); // сигнал из окна второго опыта "Калибровка"

private:
    Ui::window_experience_2 *ui;
};

#endif // WINDOW_EXPERIENCE_2_H
