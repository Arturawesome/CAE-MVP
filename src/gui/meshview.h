#ifndef MESHVIEW_H
#define MESHVIEW_H

#include <QGraphicsView>


namespace mesh {
class Mesh;
}


class MeshView : public QGraphicsView
{
public:
    explicit MeshView(QWidget *parent = nullptr);
    MeshView();

    void SetMesh(const mesh::Mesh &mesh);
    void FitMesh();

private:
    QGraphicsScene *scene_;
};

#endif // MESHVIEW_H
