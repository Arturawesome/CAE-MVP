#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QStatusBar>
#include <stdexcept>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpenVTK, &QAction::triggered, this,  &MainWindow::on_actionOpenVTK_triggered);

    connect(ui->actionSaveVTK, &QAction::triggered, this, [this]() {
        QString filename = QFileDialog::getSaveFileName(this,
                                                        "Save VTK mesh",
                                                        "",
                                                        "VTK files (*.vtk)");
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpenVTK_triggered() {
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Open VTK Mesh",
        "",
        "VTK files (*.vtk)"
        );

    if (filename.isEmpty())
        return;
    try {
        project_.LoadMesh(filename.toStdString());
        const mesh::Mesh &mesh = project_.GetMesh();
        statusBar()->showMessage(
            QString("%1 — %2 nodes, %3 triangles, %4 boundary edges")
                .arg(QFileInfo(filename).fileName())
                .arg(mesh.NumNodes())
                .arg(mesh.NumEl())
                .arg(mesh.NumBoundEdges()));

    } catch (const std::exception& error){
        QMessageBox::critical(this, "VTK load error", QString::fromUtf8(error.what()));
    }

    statusBar()->showMessage(filename);
}

void MainWindow::on_actionSaveVTK_triggered() {}
