#ifndef WINDOW_EXPERIENCE_13_H
#define WINDOW_EXPERIENCE_13_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class window_experience_13;
}

class window_experience_13 : public QDialog
{
    Q_OBJECT

public:
    explicit window_experience_13(QWidget *parent = nullptr);
    ~window_experience_13();

    QString fileProtocol; // в эту переменную принимаем номер протокола из основного окна

public slots:
    void slotInExperienceOneNumberProtocol(QString numberProtocol);

    void closeEvent(QCloseEvent *); // ловим событие "закрыть окно"

    void slotFromMainTo_window_experience_13_Parameters(float TestingIsolationBody);

private slots:
    void on_pushButton_TestingIsolationBody_of_SEM_save_clicked();

signals:
    void signalExperienceThirteenIsEnded(); // сигнал из окна первого опыта "опыт 13 закончен"

private:
    Ui::window_experience_13 *ui;
};

#endif // WINDOW_EXPERIENCE_13_H
