#ifndef WINDOW_EXPERIENCE_3_H
#define WINDOW_EXPERIENCE_3_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class window_experience_3;
}

class window_experience_3 : public QDialog
{
    Q_OBJECT

public:
    explicit window_experience_3(QWidget *parent = nullptr);
    ~window_experience_3();

    QString fileProtocol; // в эту переменную принимаем номер протокола из основного окна

void close_window_experience_3(QCloseEvent* event); // функция закрытия окна, вызывающая сигнал о его закрытии

private slots:
    void on_pushButton_startMeasurementColdIsolationResistance_clicked();
    void on_pushButton_save_experience_3_clicked();
    void on_pushButton_stopMeasurementColdIsolationResistance_clicked();

    void closeEvent(QCloseEvent *); // ловим событие "закрыть окно"

    void on_spinBox_valueChanged(int arg1);

public slots:
    void slotlFromMainTo_window_experience_3_ResistanceIsolation(float ResistanceIsolation, float PI, float U);
    void slotlFromMainTo_window_experience_3_Parameters(float ResistanceIsolation_of_SEM);
    void slotInExperienceOneNumberProtocol(QString numberProtocol); // принимает номер протокола из основного окна

signals:
    void signal_startMeasurementColdIsolationResistance(); // сигнал из третьего окна "Начать измерения"
    void signalExperienceThreeIsEnded(); // сигнал из окна третьего опыта "опыт 3 закончен"
    void signal_stop_measuredIsolation(); // сигнал из окна третьего опыта "Остановить измерения"
    void signal_time_PI(int arg1); // время измерения индекса поляризации


private:
    Ui::window_experience_3 *ui;
};

#endif // WINDOW_EXPERIENCE_3_H
