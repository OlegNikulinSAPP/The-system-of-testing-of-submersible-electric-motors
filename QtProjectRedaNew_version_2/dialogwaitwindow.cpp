#include "dialogwaitwindow.h"
#include "ui_dialogwaitwindow.h"

DialogWaitWindow::DialogWaitWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWaitWindow)
{
    ui->setupUi(this);
}

DialogWaitWindow::~DialogWaitWindow()
{
    delete ui;
}

void DialogWaitWindow::generateWait()
{
    emit emitWait();
}
