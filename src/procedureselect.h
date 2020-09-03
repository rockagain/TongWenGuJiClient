#ifndef PROCEDURESELECT_H
#define PROCEDURESELECT_H

#include <QDialog>
#include <QString>

#include "mainwindow.h"

namespace Ui {
class ProcedureSelect;
}

class ProcedureSelect : public QDialog
{
    Q_OBJECT

public:
    explicit ProcedureSelect(QMap<QString,QString>nodeId
                             ,QMap<QString,QString>projectId
                             ,QWidget *parent = 0
                             ,QString username = ""); ~ProcedureSelect();
    QString nodeCode;
    QString userName;
    QMap<QString,QString> nodeIdMap;
    QMap<QString,QString> projectIdMap;

public slots:
    void on_yesButton_clicked();
private:
    Ui::ProcedureSelect *ui;
};

#endif // PROCEDURESELECT_H
