
#include "meshview.h"

#include <QPen>
#include <QPolygonF>


namespace {
QPointF ScenePoint(const geometry::Point2D &p)
{
    return QPointF(p.x, -p.y);
}
}

MeshView::MeshView(QWidget *parent)
    : QGraphicsView(parent),
    scene_(new QGraphicsScene(this)) {

    // Создаём графическую сцену, которая будет хранить линии и контуры сетки.
    setScene(scene_);

    // сглаживание линий.
    setRenderHint(QPainter::Antialiasing, true);

    //Задаём цвет фона сцены.
    setBackgroundBrush(QColor(248, 250, 252));

    // привязка для преобразований (под курсором мышки)
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    //Включаем стандартный режим перетаскивания сцены.
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void MeshView::SetMesh(const mesh::Mesh &mesh){

}