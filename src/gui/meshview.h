#ifndef MESHVIEW_H
#define MESHVIEW_H

#include <QGraphicsView>

class MeshView : public QGraphicsView
{
public:
    explicit MeshView(QWidget *parent = nullptr);

    MeshView();

private:
    QGraphicsScene *scene_;
};

#endif // MESHVIEW_H
