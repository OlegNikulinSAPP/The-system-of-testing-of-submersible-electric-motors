#ifndef WINDOW_EXPERIENCE_7_H
#define WINDOW_EXPERIENCE_7_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class window_experience_7;
}

class window_experience_7 : public QDialog
{
    Q_OBJECT

public:
    explicit window_experience_7(QWidget *parent = nullptr);
    ~window_experience_7();

    QString fileProtocol; // в эту переменную принимаем номер протокола из основного окна
    float value_high_voltage; // в эту переменную принимаем значение высокого напряжения трансформатора

    // вычесленная вибрация по току от датчика
    float temp_vibrationBQ1, temp_vibrationBQ2, temp_vibrationBQ3, temp_vibrationBQ4,
    temp_vibrationBQ5, temp_vibrationBQ6;

public slots:
    void slotFromMainTo_window_experience_7_Parameters(float Voltage, float Current, float Losses, float Vibration);
    void slotInExperienceOneNumberProtocol(QString numberProtocol);
    void slotInExperienceSeven_high_voltage(float high_voltage); // принимает значение высокого напряжения трансформатора
    void slotFromMainTo_window_experience_7_voltage_run (float voltage); // текущее напряжение во время обкатки
    void slotFromMainTo_window_experience_7_current_run (float current); // текущее тока во время обкатки
    void slotFromMainTo_window_experience_7_losses_run (float losses); // текущие потери мощности во время обкатки
    void slotFromMainTo_window_experience_7_BQ1_run (float BQ1); // текущая вибрация BQ1 во время обкатки
    void slotFromMainTo_window_experience_7_BQ2_run (float BQ2);
    void slotFromMainTo_window_experience_7_BQ3_run (float BQ3);
    void slotFromMainTo_window_experience_7_BQ4_run (float BQ4);
    void slotFromMainTo_window_experience_7_BQ5_run (float BQ5);
    void slotFromMainTo_window_experience_7_BQ6_run (float BQ6);
    void slotFromMainTo_window_experience_7_temperature_run (float temperature);

    void slot_stop_run(); // остановка обкатки в опыте 7

private slots:
    void on_pushButton_save_clicked();
    void on_pushButton_start_clicked();
    void on_pushButton_stop_clicked();

    void closeEvent(QCloseEvent *); // ловим событие "закрыть окно"

signals:
    void signalExperienceSevenIsEnded(); // сигнал из окна 7 опыта "опыт 7 закончен"
    void signal_start(); // сигнал из седьмого окна "Старт"
    void signal_stop(); // сигнал из седьмого окна "Стоп"

private:
    Ui::window_experience_7 *ui;
};

#endif // WINDOW_EXPERIENCE_7_H
