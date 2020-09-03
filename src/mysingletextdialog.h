#ifndef MYSINGLETEXTDIALOG_H
#define MYSINGLETEXTDIALOG_H

#include <QDialog>
#include <QKeyEvent>
#include <QString>

namespace Ui {
class MySingleTextDialog;
}

class MySingleTextDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MySingleTextDialog(QWidget *parent = 0);
    ~MySingleTextDialog();
    void showHasFocus();

private:
    Ui::MySingleTextDialog *ui;
signals:
    void sentChangedWord(QString text);
private slots:
    void on_OkButton_clicked();
protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // MYSINGLETEXTDIALOG_H
