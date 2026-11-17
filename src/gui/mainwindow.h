#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include "app/ProjectController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_actionOpenVTK_triggered();
    void on_actionSaveVTK_triggered();

    void on_treeProject_itemClicked(QTreeWidgetItem *item, int column);

private:
    ProjectController project_;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
