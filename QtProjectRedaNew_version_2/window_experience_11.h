#ifndef WINDOW_EXPERIENCE_11_H
#define WINDOW_EXPERIENCE_11_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class window_experience_11;
}

class window_experience_11 : public QDialog
{
    Q_OBJECT

public:
    explicit window_experience_11(QWidget *parent = nullptr);
    ~window_experience_11();

    QString fileProtocol; // в эту переменную принимаем номер протокола из основного окна

public slots:
    void slotInExperienceOneNumberProtocol(QString numberProtocol);

private slots:
    void on_pushButton_oilBreakdownVoltageSave_clicked();

    void closeEvent(QCloseEvent *); // ловим событие "закрыть окно"

signals:
    void signalExperienceElevenIsEnded(); // сигнал из окна первого опыта "опыт 11 закончен"

private:
    Ui::window_experience_11 *ui;
};

#endif // WINDOW_EXPERIENCE_11_H
