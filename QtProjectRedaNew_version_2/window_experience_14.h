#ifndef WINDOW_EXPERIENCE_14_H
#define WINDOW_EXPERIENCE_14_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class window_experience_14;
}

class window_experience_14 : public QDialog
{
    Q_OBJECT

public:
    explicit window_experience_14(QWidget *parent = nullptr);
    ~window_experience_14();

    QString fileProtocol; // в эту переменную принимаем номер протокола из основного окна

public slots:
    void slotInExperienceOneNumberProtocol(QString numberProtocol);

private slots:
    void on_pushButton_save_clicked();

    void closeEvent(QCloseEvent *); // ловим событие "закрыть окно"

signals:
    void signalExperienceFourteenIsEnded(); // сигнал из окна первого опыта "опыт 14 закончен"

private:
    Ui::window_experience_14 *ui;
};

#endif // WINDOW_EXPERIENCE_14_H
