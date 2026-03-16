#ifndef WINDOW_EXPERIENCE_15_H
#define WINDOW_EXPERIENCE_15_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class window_experience_15;
}

class window_experience_15 : public QDialog
{
    Q_OBJECT

public:
    explicit window_experience_15(QWidget *parent = nullptr);
    ~window_experience_15();

    QString fileProtocol; // в эту переменную принимаем номер протокола из основного окна

private slots:
    void on_pushButton_testTMS_clicked();
    void on_pushButton_saveTMS_clicked();
    void on_pushButton_STOPtestTMS_clicked();

    void closeEvent(QCloseEvent *); // ловим событие "закрыть окно"

public slots:
    void slotInExperienceOneNumberProtocol(QString numberProtocol);

    void slotFromMainTo_window_experience_15_temperature_oil(float temperature_oil);
    void slotFromMainTo_window_experience_15_temperature_winding(float temperature_winding);
    void slotFromMainTo_window_experience_15_connection_pressure(int connection_pressure);
    void slotFromMainTo_window_experience_15_connection_vibration(int connection_vibration);
    void slotFromMainTo_window_experience_15_connection_isolation(float connection_isolation);

signals:
    void signal_testTMS();
    void signal_stop_testTMS();
    void signalExperienceFifteenIsEnded(); // сигнал из окна первого опыта "опыт 15 закончен"

private:
    Ui::window_experience_15 *ui;
};

#endif // WINDOW_EXPERIENCE_15_H
