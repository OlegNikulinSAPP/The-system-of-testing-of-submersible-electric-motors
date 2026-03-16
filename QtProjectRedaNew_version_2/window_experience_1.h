#ifndef WINDOW_EXPERIENCE_1_H
#define WINDOW_EXPERIENCE_1_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class window_experience_1;
}

class window_experience_1 : public QDialog
{
    Q_OBJECT

public:
    explicit window_experience_1(QWidget *parent = nullptr);
    ~window_experience_1();

    QString fileProtocol; // в эту переменную принимаем номер протокола из основного окна

public slots:
    // номер текущего протокола (в какой протокол сохранять результат)
    void slotInExperienceOneNumberProtocol(QString numberProtocol);

signals:
    void signalExperienceOneIsEnded(); // сигнал из окна первого опыта "опыт 1 закончен"

private slots:
    void on_pushButton_oilBreakdownVoltageSave_clicked();
    void closeEvent(QCloseEvent *); // ловим событие "закрыть окно"

private:
    Ui::window_experience_1 *ui;
};

#endif // WINDOW_EXPERIENCE_1_H
