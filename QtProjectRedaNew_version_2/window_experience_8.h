#ifndef WINDOW_EXPERIENCE_8_H
#define WINDOW_EXPERIENCE_8_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class window_experience_8;
}

class window_experience_8 : public QDialog
{
    Q_OBJECT

public:
    explicit window_experience_8(QWidget *parent = nullptr);
    ~window_experience_8();

    QString fileProtocol; // в эту переменную принимаем номер протокола из основного окна

public slots:
    void slotFromMainTo_window_experience_8_Parameters(float RunOut);
    void slotInExperienceOneNumberProtocol(QString numberProtocol);

    void slotFromMainTo_window_experience_8_time (QString timeExperienceEight, int i); // время выбега
    void slotFromMainTo_window_experience_8_finish(); // опыт завершен

private slots:
    void on_pushButton_save_clicked();
    void on_pushButton_startRunOut_clicked();
    void on_pushButton_stopRunOut_clicked();

    void closeEvent(QCloseEvent *); // ловим событие "закрыть окно"

signals:
    void signalExperienceEightIsEnded(); // сигнал из окна 8 опыта "опыт 8 закончен"
    void signal_startExperienceEight(); // сигнал из 8 окна "Старт"
    void signal_stopExperienceEight(); // сигнал из 8 окна "Стоп"

private:
    Ui::window_experience_8 *ui;
};

#endif // WINDOW_EXPERIENCE_8_H
