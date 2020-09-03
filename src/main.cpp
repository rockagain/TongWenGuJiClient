#include "mainwindow.h"
#include <QApplication>
#include "login.h"
#include "editboard.h"
#include "anotherlogin.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //这个语句是设置右键标签可以显示中文的
    //login l;
    AnotherLogin l;
    l.show();
   return a.exec();
}
