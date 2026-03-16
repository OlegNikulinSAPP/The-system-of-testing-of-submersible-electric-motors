#ifndef DIALOGWAITWINDOW_H
#define DIALOGWAITWINDOW_H

#include <QDialog>

namespace Ui {
class DialogWaitWindow;
}

class DialogWaitWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWaitWindow(QWidget *parent = nullptr);
    ~DialogWaitWindow();

public slots:
    void generateWait();

signals:
    void emitWait();

private:
    Ui::DialogWaitWindow *ui;
};

#endif // DIALOGWAITWINDOW_H
