#include "mysingletextdialog.h"
#include "ui_mysingletextdialog.h"

MySingleTextDialog::MySingleTextDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MySingleTextDialog)
{
    ui->setupUi(this);
    QFont font;
    font.setPixelSize(60);
    ui->lineEdit->setFont(font);
    this->setWindowTitle(QStringLiteral("改字框"));
}

MySingleTextDialog::~MySingleTextDialog()
{
    delete ui;
}

void MySingleTextDialog::on_OkButton_clicked()
{
    if(ui->lineEdit->text().count() == 1) {
        sentChangedWord(ui->lineEdit->text());
        ui->lineEdit->clear();
        this->hide();
    }
}

void MySingleTextDialog::keyPressEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Return)||(event->key() == Qt::Key_Enter)) {
        if(ui->lineEdit->text().count() == 1){
            sentChangedWord(ui->lineEdit->text());
            ui->lineEdit->clear();
            this->hide();
        }
    }
}

/**
 * @brief MySingleTextDialog::showHasFocus 设置这个lineEdit 有focus
 */
void MySingleTextDialog::showHasFocus()
{
    ui->lineEdit->setFocus();
}
