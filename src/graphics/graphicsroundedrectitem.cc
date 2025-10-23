#include "graphicsroundedrectitem.hpp"
#include "geometrycache.hpp"
#include "graphicsutils.hpp"

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>

namespace Graphics {

RoundedRect::RoundedRect(const QRectF &rect, qreal xRadius, qreal yRadius)
{
    this->rect = rect;
    this->xRadius = xRadius;
    this->yRadius = yRadius;
}

bool RoundedRect::isValid(double margin) const
{
    return rect.isValid() && xRadius >= 0 && yRadius >= 0 && xRadius < rect.width() / 2
           && yRadius < rect.height() / 2 && rect.x() >= 0 && rect.y() >= 0 && rect.width() > margin
           && rect.height() > margin;
}

class GraphicsRoundedRectItem::GraphicsRoundedRectItemPrivate
{
public:
    explicit GraphicsRoundedRectItemPrivate(GraphicsRoundedRectItem *q)
        : q_ptr(q)
    {
        qRegisterMetaType<Graphics::RoundedRect>("Graphics::RoundedRect");
    }

    GraphicsRoundedRectItem *q_ptr = nullptr;

    QLineF hoveredLine;
};

GraphicsRoundedRectItem::GraphicsRoundedRectItem(QGraphicsItem *parent)
    : GraphicsBasicItem(parent)
    , d_ptr(new GraphicsRoundedRectItemPrivate(this))
{}

GraphicsRoundedRectItem::GraphicsRoundedRectItem(const RoundedRect &roundedRect,
                                                 QGraphicsItem *parent)
    : GraphicsBasicItem(parent)
    , d_ptr(new GraphicsRoundedRectItemPrivate(this))
{
    setRoundedRect(roundedRect);
}

GraphicsRoundedRectItem::~GraphicsRoundedRectItem() {}

auto GraphicsRoundedRectItem::setRoundedRect(const RoundedRect &roundedRect) -> bool
{
    if (!roundedRect.isValid(margin())) {
        return false;
    }

    QPolygonF anchorPoints{roundedRect.rect.topLeft(), roundedRect.rect.bottomRight()};
    auto rect = Utils::createBoundingRect(anchorPoints, margin());
    if (!scene()->sceneRect().contains(rect)) {
        return false;
    }
    QPainterPath shape;
    shape.addRoundedRect(roundedRect.rect, roundedRect.xRadius, roundedRect.yRadius);

    prepareGeometryChange();
    m_roundedRect = roundedRect;
    geometryCache()->setGeometryData(anchorPoints,
                                     Utils::createBoundingRect(anchorPoints, 0),
                                     shape);
    emit roundedRectChanged(m_roundedRect);

    return true;
}

void GraphicsRoundedRectItem::drawContent(QPainter *painter)
{
    painter->drawRoundedRect(m_roundedRect.rect, m_roundedRect.xRadius, m_roundedRect.yRadius);
}

void GraphicsRoundedRectItem::pointsChanged(const QPolygonF &ply)
{
    switch (ply.size()) {
    case 1: geometryCache()->setControlPoints(ply); break;
    case 2:
        if (!setRoundedRect(RoundedRect(QRectF(ply[0], ply[1]).normalized(),
                                        m_roundedRect.xRadius,
                                        m_roundedRect.yRadius))) {
            return;
        }
        break;
    default: return;
    }
    update();
}

void GraphicsRoundedRectItem::updateHoverPreview(const QPointF &scenePos)
{
    auto controlPoints = geometryCache()->controlPoints();
    if (controlPoints.size() != 1) {
        return;
    }
    auto rrt = RoundedRect(QRectF(controlPoints[0], scenePos).normalized(),
                           m_roundedRect.xRadius,
                           m_roundedRect.yRadius);
    if (!rrt.isValid(margin())) {
        return;
    }
    m_roundedRect = rrt;
    update();
}

GraphicsBasicItem::MouseRegion GraphicsRoundedRectItem::detectEdgeRegion(const QPointF &scenePos)
{
    const QRectF &rect = m_roundedRect.rect;
    const double edgeMargin = margin() / 2.0;

    std::array<std::pair<Qt::Edge, QLineF>, 4> edges = {
        {{Qt::TopEdge, QLineF(rect.topLeft(), rect.topRight())},
         {Qt::RightEdge, QLineF(rect.topRight(), rect.bottomRight())},
         {Qt::BottomEdge, QLineF(rect.bottomRight(), rect.bottomLeft())},
         {Qt::LeftEdge, QLineF(rect.bottomLeft(), rect.topLeft())}}};

    for (const auto &[edgeType, edgeLine] : edges) {
        if (!Utils::isPointNearEdge(scenePos, edgeLine, edgeMargin)) {
            continue;
        }
        setMouseRegion(MouseRegion::EdgeArea);
        d_ptr->hoveredLine = edgeLine;
        setCursor(Utils::cursorForDirection(edgeLine.angle()));
        return MouseRegion::EdgeArea;
    }

    d_ptr->hoveredLine = QLineF();
    return MouseRegion::NoSelection; // 或者适当的默认值
}

void GraphicsRoundedRectItem::handleMouseMoveEvent(const QPointF &scenePos,
                                                   const QPointF &clickedPos,
                                                   const QPointF delta)
{
    auto controlPoints = geometryCache()->controlPoints();

    switch (mouseRegion()) {
    case MouseRegion::EntireShape: controlPoints.translate(delta); break;
    case MouseRegion::AnchorPoint: controlPoints.replace(hoveredDotIndex(), scenePos); break;
    case MouseRegion::EdgeArea: {
        QPointF p1 = d_ptr->hoveredLine.p1();
        QPointF p2 = d_ptr->hoveredLine.p2();

        // 获取矩形顶点
        QPolygonF rectVertices = QPolygonF(m_roundedRect.rect);
        int index1 = rectVertices.indexOf(p1);
        int index2 = rectVertices.indexOf(p2);

        if (index1 >= 0 && index2 >= 0) {
            // 根据边的方向移动
            if (qFuzzyCompare(p1.x(), p2.x())) {
                // 垂直线 - 水平移动
                rectVertices[index1].rx() += delta.x();
                rectVertices[index2].rx() += delta.x();
            } else {
                // 水平线 - 垂直移动
                rectVertices[index1].ry() += delta.y();
                rectVertices[index2].ry() += delta.y();
            }

            // 更新被拖动的边和控制点
            d_ptr->hoveredLine = QLineF(rectVertices[index1], rectVertices[index2]);
            controlPoints = {rectVertices[0], rectVertices[2]};
        }
    } break;
    default: return;
    }

    pointsChanged(controlPoints);
}

} // namespace Graphics
