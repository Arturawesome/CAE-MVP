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
        ui->viewMesh->SetMesh(mesh);

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
