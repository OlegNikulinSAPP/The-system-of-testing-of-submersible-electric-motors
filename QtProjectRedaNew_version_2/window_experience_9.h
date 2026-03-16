#ifndef WINDOW_EXPERIENCE_9_H
#define WINDOW_EXPERIENCE_9_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class window_experience_9;
}

class window_experience_9 : public QDialog
{
    Q_OBJECT

public:
    explicit window_experience_9(QWidget *parent = nullptr);
    ~window_experience_9();

    QString fileProtocol; // в эту переменную принимаем номер протокола из основного окна

public slots:
    void slotInExperienceOneNumberProtocol(QString numberProtocol);
    void slotFromMainMeasurement(QString measurement);
    void slotFromMainTo_window_experience_9_windingResistance(float sumColdResistance, bool measuredColdResistanceIsFinished_UV,
                                                              bool measuredColdResistanceIsFinished_VW,
                                                              bool measuredColdResistanceIsFinished_UW,
                                                              int temperatureColdResistance); // принимаем сигнал из основного окна - значение сопротивления


private slots:
    void on_pushButton_saveMeasurementHotIsolationResistance_clicked();
    void on_pushButton_startMeasurementHotIsolationResistance_clicked();
    void on_pushButton_stopMeasurementHotIsolationResistance_clicked();

    void closeEvent(QCloseEvent *); // ловим событие "закрыть окно"

signals:
    void signalExperienceNineIsEnded(); // сигнал из окна 9 опыта "опыт 9 закончен"
    void signal_startExperienceNine(); // сигнал из 9 окна "Старт"
    void signal_stopMeasurementHotWindingResistance(); // сигнал из окна 9 опыта "Стоп"

private:
    Ui::window_experience_9 *ui;
};

#endif // WINDOW_EXPERIENCE_9_H
