#pragma once
#include "mesh/Mesh2D.h"
#include <QString>
#include <QObject>
#include <memory>

using MeshPtr = std::shared_ptr<mesh::Mesh>;

class FileLoader : public QObject
{
    Q_OBJECT

public:
    explicit FileLoader(QString filename);

public slots:
    void Load();

signals:
    void Finished(MeshPtr mesh);
    void Error(const QString &message);

private:
    QString filename_;

};
