#include "procedureselect.h"
#include "ui_procedureselect.h"

ProcedureSelect::ProcedureSelect(QMap<QString,QString>nodeId,QMap<QString,QString>projectId,QWidget *parent,QString username) :
    QDialog(parent),
    ui(new Ui::ProcedureSelect)
{
    userName = username;
    nodeCode = "";
    ui->setupUi(this);
    for(auto i:projectId.values()) {
        ui->projectIdBox->addItem(i);
    }
    //把这俩参数本地化了
    nodeIdMap = nodeId;
    projectIdMap = projectId;
    for(auto i:nodeIdMap.keys()) {
        if(i == "hb" || i == "bmjd"|| i == "jljd" || i == "zyjd"){
            ui->nodeIdBox->addItem(nodeIdMap.value(i));
        }
    }

}

ProcedureSelect::~ProcedureSelect()
{
    delete ui;
}

void ProcedureSelect::on_yesButton_clicked()
{
    nodeCode =  nodeIdMap.key( ui->nodeIdBox->currentText());
    //本地QString 提供参数
    QString projectId = projectIdMap.key(ui->projectIdBox->currentText());

    MainWindow *window = new MainWindow(projectId,0,nodeCode,userName);
    this->hide();
    window->show();
}
