#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "app/FileLoader.h"

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
    if (loadThead_) {
        loadThead_->quit();
        loadThead_->wait();
    }

    delete ui;
}

void MainWindow::on_actionOpenVTK_triggered() {
    if (loadThead_) {
        return; // loading in porcerss
    }

    QString filename = QFileDialog::getOpenFileName(
        this,
        "Open VTK Mesh",
        "",
        "VTK files (*.vtk)"
        );

    if (filename.isEmpty()) {
        return;
    }


    auto *thread = new QThread(this);
    auto *worker = new FileLoader(filename);

    ui->actionOpenVTK->setEnabled(false);

    loadThead_ = thread;
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &FileLoader::Load);

    connect(worker, &FileLoader::Finished, this, [this](MeshPtr loaded) {
        project_.SetMesh(std::move(*loaded));
        const mesh::Mesh &mesh = project_.GetMesh();
        ui->viewMesh->SetMesh(mesh);
        statusBar()->showMessage(QString(" %1 nodes, %2 triangles, %3 boundary edges")
                                     .arg(mesh.NumNodes())
                                     .arg(mesh.NumEl())
                                     .arg(mesh.NumBoundEdges()));
    });

    connect(worker, &FileLoader::Error, this, [this](const QString &message) {
        QMessageBox::critical(this, "Load error", message);
    });


    connect(worker, &FileLoader::Finished, thread, &QThread::quit);
    connect(worker, &FileLoader::Error, thread, &QThread::quit);

    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    connect(thread, &QThread::finished, this, [this]() {
        loadThead_ = nullptr;
        ui->actionOpenVTK->setEnabled(true);
    });
    statusBar()->showMessage("Loading mesh...");
    thread->start();
}

void MainWindow::on_actionSaveVTK_triggered() {}

void MainWindow::on_treeProject_itemClicked(QTreeWidgetItem *item, int column) {

        if (item->text(0) == "Heat Transfer") {
            qDebug() << "Heat Transfer selected";
            ui->stackedProperties->setCurrentWidget(ui->pageHeat);
        }
        else if (item->text(0) == "Fluid Dynamics") {
            qDebug() << "Fluid Dynamics selected";
            ui->stackedProperties->setCurrentWidget(ui->pageHydro);
        }
        else if (item->text(0) == "Solid Mechanics") {
            qDebug() << "Solid Mechanics selected";
            ui->stackedProperties->setCurrentWidget(ui->pageDeformation);
        }
}


