
#include "meshview.h"

#include <QPen>
#include <QPolygonF>
#include "mesh/Mesh2D.h"
#include <QPainterPath>
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

void MeshView::SetMesh(const mesh::Mesh &mesh)
{
    QPainterPath triangles;
    for (const auto &element : mesh.Elements()) {
        const auto &ids = element.nodes_id;

        //Перемещает текущую точку пути к первому узлу, не рисуя линию
        triangles.moveTo(ScenePoint(mesh.NodeById(ids[0]).position));

        //Добавляет отрезок
        triangles.lineTo(ScenePoint(mesh.NodeById(ids[1]).position));
        triangles.lineTo(ScenePoint(mesh.NodeById(ids[2]).position));

        //Добавляет замыкающий отрезок
        triangles.closeSubpath();
    }

    QPainterPath boundaries;
    for (const auto &edje : mesh.BoundaryEdges()) {
        const auto qPOint = ScenePoint(mesh.NodeById(edje.nodes_id.first).position);
        boundaries.moveTo(qPOint);
        boundaries.lineTo(ScenePoint(mesh.NodeById(edje.nodes_id.second).position));
    }


    double minX = std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();

    double maxX = std::numeric_limits<double>::lowest();
    double maxY = std::numeric_limits<double>::lowest();

    for (const auto &node : mesh.Nodes()) {
        const QPointF p = ScenePoint(node.position);

        minX = std::min(minX, p.x());
        minY = std::min(minY, p.y());

        maxX = std::max(maxX, p.x());
        maxY = std::max(maxY, p.y());
    }

    scene_->clear();
    QPen trainglePen(QColor(135, 148, 163));
    trainglePen.setWidth(0);
    scene_->addPath(triangles, trainglePen);


    QPen boundaryPen(QColor(0, 150, 130));
    boundaryPen.setWidth(2);
    // setCosmetic беспечивает постоянную экранную толщину границ — два пикселя.
    boundaryPen.setCosmetic(true);
    scene_->addPath(boundaries, boundaryPen);

    const QRectF bounds(QPointF(minX, minY), QPointF(maxX, maxY));
    const double padding = std::max(0.05 * std::max(bounds.width(), bounds.height()), 1e-9);
    scene_->setSceneRect(bounds.adjusted(-padding, -padding, padding, padding));
    FitMesh();

}

void MeshView::FitMesh() {
    if (!scene_->items().isEmpty()) {
        resetTransform();
        fitInView(scene_->sceneRect(), Qt::KeepAspectRatio);
    }
}




















