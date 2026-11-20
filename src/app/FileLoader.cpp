#include "FileLoader.h"
#include "io/VTKReader.h"
#include <QThread>

FileLoader::FileLoader(QString filename)
    : filename_(std::move(filename))
{}

void FileLoader::Load()
{
    try {
        auto mesh = std::make_shared<mesh::Mesh>(
            VTKReader::ReadUstructuredGrid(filename_.toStdString()));

        if (mesh->NumNodes() == 0 || mesh->NumEl() == 0) {
            throw std::runtime_error("VTK mesh contains no nodes or triangles");
        }
        mesh->CreateBoundaryEdges();
        QThread::sleep(2);
        emit Finished(mesh);
    } catch (const std::exception &error) {
        emit Error(QString::fromUtf8(error.what()));
    }

}