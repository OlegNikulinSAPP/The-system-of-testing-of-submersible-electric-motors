#ifndef WINDOW_EXPERIENCE_6_H
#define WINDOW_EXPERIENCE_6_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class window_experience_6;
}

class window_experience_6 : public QDialog
{
    Q_OBJECT

public:
    explicit window_experience_6(QWidget *parent = nullptr);
    ~window_experience_6();

    QString fileProtocol; // в эту переменную принимаем номер протокола из основного окна
    float value_high_voltage; // в эту переменную принимаем значение высокого напряжения трансформатора

public slots:
    void slotInExperienceOneNumberProtocol(QString numberProtocol); // принимает номер протокола из основного окна
    void slotFromMainTo_window_experience_6_Parameters(float AccelerationVoltage);
    void slotInExperienceSix_high_voltage(float high_voltage); // принимает значение высокого напряжения трансформатора
    void slotFromMainTo_window_experience_6_voltage_starting (float voltage_starting); // текущее напряжение
    void slotFromMainTo_window_experience_6_finish (); // опыт завершен

private slots:
    void on_pushButton_AccelerationVoltage_save_clicked();
    void on_pushButton_AccelerationVoltage_start_clicked();
    void on_pushButton_AccelerationVoltage_stop_clicked();

    void closeEvent(QCloseEvent *); // ловим событие "закрыть окно"

signals:
    void signalExperienceSixIsEnded(); // сигнал из окна 6 опыта "опыт 6 закончен"
    void signal_startExperienceSix(); // сигнал из 5 окна "Старт"
    void signal_stopExperienceSix(); // сигнал из 5 окна "Стоп"

private:
    Ui::window_experience_6 *ui;
};

#endif // WINDOW_EXPERIENCE_6_H
