#ifndef WINDOW_EXPERIENCE_4_H
#define WINDOW_EXPERIENCE_4_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class window_experience_4;
}

class window_experience_4 : public QDialog
{
    Q_OBJECT

public:
    explicit window_experience_4(QWidget *parent = nullptr);
    ~window_experience_4();

    QString fileProtocol; // в эту переменную принимаем номер протокола из основного окна

public slots:
    void slotFromMainTo_window_experience_4_Parameters(float TorqueTurning);
    void slotInExperienceOneNumberProtocol(QString numberProtocol); // принимает номер протокола из основного окна

private slots:
    void on_pushButton_TorqueTurning_save_clicked();
    void closeEvent(QCloseEvent *); // ловим событие "закрыть окно"

signals:
    void signalExperienceFourIsEnded(); // сигнал из окна 4 опыта "опыт 4 закончен"

private:
    Ui::window_experience_4 *ui;
};

#endif // WINDOW_EXPERIENCE_4_H
