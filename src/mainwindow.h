#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QSplitter>
#include <QListWidget>

#include "editboard.h"
#include "proofboard.h"
#include "fullpicproof.h"
#include "clusterproof.h"

#include "mylistwidget.h"
#include "config.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString projectId,QWidget *parent = 0,QString nodecode ="",QString username="");
    ~MainWindow();

    void setBoardType(int value);

private:
    Ui::MainWindow *ui;
    //新加的网络需要传输的东西
    QString projectid;
    QString nodeCode;
    QString userName;
    QMenu* fileMenu;
    QMenu* aboutMenu;
    QAction* clearSqlAction;
    QAction* quitAction;
    QAction* aboutAction;
    int BoardType;
    //下面的是toolbar的东西
    QAction* requestAction;
    QAction* uploadAction;

    QAction* zoomOutAction;
    QAction* zoomInAction;
    QAction* setCoverAction;

    QAction* mainBodyAction;  //主体Action
    QAction* imageAction;
    QAction* annotationAction;
    QAction* tableAction;
    QAction* singleRowAction;
    QAction* doubleRowAction;
    QAction* bigCharAction;
    QAction* smallCharAction;


    //四个板子
    EditBoard* editboard;
    ProofBoard* proofboard;
    FullPicProof* wholepicboard;
    ClusterProof* clustproof;

    //下载多少图片的数量传递
    QLineEdit* broadcastNumberEdit;

    void startEditBoard();
    void startProofBoard();
    void startFullPicBoard();
    void startClusterBoard();

    void chooseWhatBoard();
    void addMenuAndBar();
    void addToolbarAction();
    void clearEditboardsItem();
    void clearProofboardsItem();
private slots:
    void clearSqlActionConnect();
    void showTheIntroduction();
    void requestActionResponse();
    void uploadActionResponse();
    void zoomOutActionResponse();
    void zoomInActionResponse();
    void setCoverActionResponse();
    void mainBodyActionResponse();
    void imageActionResponse();
    void smallCharActionResponse();
    void bigCharActionResponse();
    void doubleRowActionResponse();
    void singleRowActionResponse();
    void tableActionResponse();
    void annotationActionResponse();
    void showToolBarAction();
};

#endif // MAINWINDOW_H
