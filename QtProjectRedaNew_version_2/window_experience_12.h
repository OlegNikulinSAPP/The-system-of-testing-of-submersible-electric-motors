#ifndef WINDOW_EXPERIENCE_12_H
#define WINDOW_EXPERIENCE_12_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class window_experience_12;
}

class window_experience_12 : public QDialog
{
    Q_OBJECT

public:
    explicit window_experience_12(QWidget *parent = nullptr);
    ~window_experience_12();

    QString fileProtocol; // в эту переменную принимаем номер протокола из основного окна

public slots:
    void slotInExperienceOneNumberProtocol(QString numberProtocol);

    void closeEvent(QCloseEvent *); // ловим событие "закрыть окно"

    void slotFromMainTo_window_experience_12_Parameters(float TestingIsolationInterTurn);

private slots:
    void on_pushButton_TestingIsolationInterTurn_of_SEM_save_clicked();

signals:
    void signalExperienceTwelveIsEnded(); // сигнал из окна первого опыта "опыт 12 закончен"

private:
    Ui::window_experience_12 *ui;
};

#endif // WINDOW_EXPERIENCE_12_H
