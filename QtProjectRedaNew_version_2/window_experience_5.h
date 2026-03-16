#ifndef WINDOW_EXPERIENCE_5_H
#define WINDOW_EXPERIENCE_5_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class window_experience_5;
}

class window_experience_5 : public QDialog
{
    Q_OBJECT

public:
    explicit window_experience_5(QWidget *parent = nullptr);
    ~window_experience_5();

    QString fileProtocol; // в эту переменную принимаем номер протокола из основного окна
    float value_high_voltage; // в эту переменную принимаем значение высокого напряжения трансформатора

public slots:
    void slotFromMainTo_window_experience_5_Parameters(float VoltageShortCircuit, float CurrentShortCircuit, float LossesShortCircuit);
    void slotInExperienceOneNumberProtocol(QString numberProtocol); // принимает номер протокола из основного окна
    void slotInExperienceFive_high_voltage(float high_voltage); // принимает значение высокого напряжения трансформатора

    // текущие значения опыта КЗ
    void slotFromMainTo_window_experience_5_voltage(float VoltageShortCircuit);
    void slotFromMainTo_window_experience_5_current(float CurrentShortCircuit);
    void slotFromMainTo_window_experience_5_losses(float LossesShortCircuit);

private slots:
    void on_pushButton_LossesShortCircuit_save_clicked();
    void on_pushButton_startExperienceFive_clicked();
    void on_pushButton_stopExperienceFive_clicked();

    void closeEvent(QCloseEvent *); // ловим событие "закрыть окно"
signals:
    void signalExperienceFiveIsEnded(); // сигнал из окна 5 опыта "опыт 5 закончен"
    void signal_startExperienceFive(); // сигнал из 5 окна "Старт"
    void signal_stopExperienceFive(); // сигнал из 5 окна "Стоп"

private:
    Ui::window_experience_5 *ui;
};

#endif // WINDOW_EXPERIENCE_5_H
