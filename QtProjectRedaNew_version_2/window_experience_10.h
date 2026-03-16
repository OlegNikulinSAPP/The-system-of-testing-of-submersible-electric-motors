#ifndef WINDOW_EXPERIENCE_10_H
#define WINDOW_EXPERIENCE_10_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class window_experience_10;
}

class window_experience_10 : public QDialog
{
    Q_OBJECT

public:
    explicit window_experience_10(QWidget *parent = nullptr);
    ~window_experience_10();

    QString fileProtocol; // в эту переменную принимаем номер протокола из основного окна


private slots:
    void on_pushButton_ResistanceIsolation_hot_start_clicked();
    void on_pushButton_ResistanceIsolation_hot_save_clicked();
    void on_pushButton_ResistanceIsolation_hot_stop_clicked();

    void closeEvent(QCloseEvent *); // ловим событие "закрыть окно"

public slots:
    void slotInExperienceOneNumberProtocol(QString numberProtocol);

    void slotFromMainTo_window_experience_10_ResistanceIsolation(float ResistanceIsolation, float U);

    void slotFromMainTo_window_experience_10_Parameters(float ResistanceIsolation);

signals:
    void signal_startMeasurementHotIsolationResistance(); // сигнал из десятого окна "Начать измерения"
    void signalExperienceTenIsEnded(); // сигнал из окна 10 опыта "опыт 10 закончен"
    void signal_stop_measuredHotIsolation(); // сигнал из окна 10 опыта "Остановить измерения"

private:
    Ui::window_experience_10 *ui;
};

#endif // WINDOW_EXPERIENCE_10_H
