#include "graphicsrotatedrectitem.h"
#include "geometrycache.hpp"
#include "graphicsutils.hpp"

#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QtMath>

namespace Graphics {

inline auto createRotatedRect(const QPointF &p1, const QPointF &p2, const QPointF &c) -> RotatedRect
{
    RotatedRect rect;

    // 情况1：p1和p2是矩形的一条边上的两个点，c是另一条边上的点
    // 计算p1和p2的中点作为可能的中心点
    QPointF mid = (p1 + p2) * 0.5;

    // 计算p1到p2的向量
    QPointF edge = p2 - p1;
    double edgeLength = std::sqrt(edge.x() * edge.x() + edge.y() * edge.y());

    if (edgeLength < 1e-10) {
        // p1和p2重合，退化为普通矩形
        rect.center = (p1 + c) * 0.5;
        rect.width = std::abs(p1.x() - c.x());
        rect.height = std::abs(p1.y() - c.y());
        rect.angle = 0;
        return rect;
    }

    // 计算边的角度（弧度）
    double edgeAngle = std::atan2(edge.y(), edge.x());

    // 计算c到边的垂直距离（高度的两倍）
    QPointF toC = c - mid;

    // 将toC旋转-edgeAngle到边所在的坐标系
    double cosAngle = std::cos(-edgeAngle);
    double sinAngle = std::sin(-edgeAngle);
    QPointF rotatedC(toC.x() * cosAngle - toC.y() * sinAngle,
                     toC.x() * sinAngle + toC.y() * cosAngle);

    // 现在rotatedC的y坐标就是高度的一半
    double halfHeight = std::abs(rotatedC.y());

    // 设置旋转矩形参数
    rect.center = mid
                  + QPointF(-rotatedC.y() * std::sin(edgeAngle), rotatedC.y() * std::cos(edgeAngle))
                        * 0.5;
    rect.width = edgeLength;
    rect.height = 2.0 * halfHeight;
    rect.angle = edgeAngle * 180.0 / M_PI; // 转换为度

    return rect;
}

auto RotatedRect::isValid(double margin) const -> bool
{
    return width > 0 && height > 0 && width > margin && height > margin;
}

auto RotatedRect::boundingRect(double margin) const -> QRectF
{
    if (!isValid(margin)) {
        return QRectF{};
    }
    return Utils::createBoundingRect(controlPoints(), margin);
}

auto RotatedRect::controlPoints() const -> QPolygonF
{
    const double halfWidth = width * 0.5;
    const double halfHeight = height * 0.5;

    // 矩形的四个角点（在局部坐标系中）
    QPolygonF polygon;
    polygon << QPointF(-halfWidth, -halfHeight) << QPointF(halfWidth, -halfHeight)
            << QPointF(halfWidth, halfHeight) << QPointF(-halfWidth, halfHeight);

    // 旋转并移动到正确位置
    const double radians = qDegreesToRadians(angle);
    const double cosA = std::cos(radians);
    const double sinA = std::sin(radians);

    for (auto &point : polygon) {
        // 旋转
        const double x = point.x() * cosA - point.y() * sinA;
        const double y = point.x() * sinA + point.y() * cosA;
        // 平移
        point = center + QPointF(x, y);
    }

    return polygon;
}

class GraphicsRotatedRectItem::GraphicsRotatedRectItemPrivate
{
public:
    explicit GraphicsRotatedRectItemPrivate(GraphicsRotatedRectItem *q)
        : q_ptr(q)
    {}

    GraphicsRotatedRectItem *q_ptr;

    RotatedRect rotatedRect;
    QPainterPath cachePath;
    bool rotatedHovered = false;
    bool linehovered = false;
    QLineF hoveredLine;
};

GraphicsRotatedRectItem::GraphicsRotatedRectItem(QGraphicsItem *parent)
    : GraphicsBasicItem(parent)
    , d_ptr(new GraphicsRotatedRectItemPrivate(this))
{}

GraphicsRotatedRectItem::GraphicsRotatedRectItem(const RotatedRect &rotatedRect,
                                                 QGraphicsItem *parent)
    : GraphicsBasicItem(parent)
    , d_ptr(new GraphicsRotatedRectItemPrivate(this))
{
    setRotatedRect(rotatedRect);
}

GraphicsRotatedRectItem::~GraphicsRotatedRectItem() {}

auto GraphicsRotatedRectItem::setRotatedRect(const RotatedRect &rotatedRect) -> bool
{
    if (!rotatedRect.isValid(margin())) {
        return false;
    }
    auto anchorPoints = rotatedRect.controlPoints();
    auto rect = Utils::createBoundingRect(anchorPoints, margin());
    if (!scene()->sceneRect().contains(rect)) {
        return false;
    }
    rect = Utils::createBoundingRect(anchorPoints, 0);

    QPainterPath shape;
    shape.addPolygon(anchorPoints);
    shape.closeSubpath();

    prepareGeometryChange();

    d_ptr->rotatedRect = rotatedRect;

    geometryCache()->setControlPoints(anchorPoints, rect, shape);

    return true;
}

auto GraphicsRotatedRectItem::rotatedRect() const -> RotatedRect
{
    return d_ptr->rotatedRect;
}

auto GraphicsRotatedRectItem::type() const -> int
{
    return Shape::ROTATEDRECT;
}

void GraphicsRotatedRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) == 0 || !isValid()) {
        return;
    }
    if (!isSelected()) {
        setSelected(true);
    }
    QPointF point = event->scenePos();
    QPointF dp = point - clickedPos();

    RotatedRect rrt = d_ptr->rotatedRect;
    QPointF c = rrt.center;
    auto pts_tmp = geometryCache()->controlPoints();
    QLineF l0(c, point);
    QLineF l1(c, clickedPos());
    double m = qSqrt(dp.x() * dp.x() + dp.y() * dp.y());
    if (d_ptr->rotatedHovered) {
        double angle = l0.angleTo(l1);
        rrt.angle += angle;
        rrt.angle = Utils::normalizeAngle(rrt.angle);
        setCursor(Utils::cursorForDirection(360 - rrt.angle));
    } else if (d_ptr->linehovered) {
        QLineF dl = d_ptr->hoveredLine.normalVector();
        QPointF p1 = d_ptr->hoveredLine.p1();
        QPointF p2 = d_ptr->hoveredLine.p2();

        int index0 = pts_tmp.indexOf(p1);
        int index1 = pts_tmp.indexOf(p2);
        if (index0 < 0 || index1 < 0) {
            return;
        }
        bool strech = l0.length() > l1.length();
        QPointF p3 = p1 + QPointF(dl.dx() * m, dl.dy() * m) / dl.length() * (strech ? 1 : -1);
        QPointF p4 = p2 + QPointF(dl.dx() * m, dl.dy() * m) / dl.length() * (strech ? 1 : -1);

        pts_tmp.replace(index0, p3);
        pts_tmp.replace(index1, p4);
        d_ptr->hoveredLine = QLineF(p3, p4);

        rrt.width = QLineF(pts_tmp.at(0), pts_tmp.at(1)).length();
        rrt.height = QLineF(pts_tmp.at(0), pts_tmp.at(3)).length();
        rrt.center = (pts_tmp.at(0) + pts_tmp.at(2)) / 2;
    } else {
        switch (mouseRegion()) {
        case MouseRegion::DotRegion: break;
        case MouseRegion::All: rrt.center += dp; break;
        default: return;
        }
    }
    if (setRotatedRect(rrt)) {
        update();
    }

    setClickedPos(point);
}

void GraphicsRotatedRectItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    auto pts_tmp = geometryCache()->controlPoints();
    QPointF point = event->scenePos();
    if (pts_tmp.size() == 2) {
        pts_tmp.append(point);
        showHoverRotatedRect(pts_tmp);
    }
    if (!isValid()) {
        return;
    }
    QPointF center = d_ptr->rotatedRect.center;

    QPointF c1 = (pts_tmp[1] + pts_tmp[2]) / 2;
    QLineF l = QLineF(center, c1);
    l = QLineF(center, l.pointAt(0.9));

    pts_tmp = Utils::boundingFromLine(l, margin() / 4);
    if (pts_tmp.containsPoint(point, Qt::OddEvenFill)) {
        d_ptr->rotatedHovered = true;
        setCursor(Utils::cursorForDirection(l.angle()));
        return;
    }

    pts_tmp = geometryCache()->controlPoints();
    for (int i = 0; i < pts_tmp.count(); ++i) {
        QLineF pl(pts_tmp.at(i), pts_tmp.at((i + 1) % 4));
        QPolygonF tmp = Utils::boundingFromLine(pl, margin() / 4);
        if (tmp.containsPoint(point, Qt::OddEvenFill)) {
            d_ptr->linehovered = true;
            d_ptr->hoveredLine = pl;
            setCursor(Utils::cursorForDirection(pl.angle()));
            return;
        }
    }

    d_ptr->rotatedHovered = false;
    d_ptr->linehovered = false;
    GraphicsBasicItem::hoverMoveEvent(event);
}

void GraphicsRotatedRectItem::drawContent(QPainter *painter)
{
    if (isValid()) {
        auto center = d_ptr->rotatedRect.center;
        auto anchorPoints = geometryCache()->controlPoints();
        painter->drawPolygon(anchorPoints);
        if (isSelected()) {
            auto pen = painter->pen();
            pen.setColor(pen.color().darker());
            painter->setPen(pen);
            QPointF c1 = (anchorPoints.at(1) + anchorPoints.at(2)) / 2;
            QLineF l = QLineF(center, c1);
            l = QLineF(center, l.pointAt(0.9));
            painter->drawLine(l);
            painter->drawEllipse(center, 5, 5);
        }
    } else {
        painter->drawPath(d_ptr->cachePath);
    }
}

void GraphicsRotatedRectItem::pointsChanged(const QPolygonF &ply)
{
    auto rect = scene()->sceneRect();
    if (!rect.contains(ply.last())) {
        return;
    }

    switch (ply.size()) {
    case 1:
    case 2: geometryCache()->setControlPoints(ply); break;
    case 3:
        if (setRotatedRect(createRotatedRect(ply[0], ply[1], ply[2]))) {
            update();
        }
        break;
    default: return;
    }
}

void GraphicsRotatedRectItem::showHoverRotatedRect(const QPolygonF &ply)
{
    switch (ply.size()) {
    case 3: {
        auto rrt = createRotatedRect(ply[0], ply[1], ply[2]);
        if (!rrt.isValid(margin())) {
            return;
        }
        if (!scene()->sceneRect().contains(rrt.boundingRect(margin()))) {
            return;
        }
        d_ptr->cachePath.clear();
        d_ptr->cachePath.addPolygon(rrt.controlPoints());
        d_ptr->cachePath.closeSubpath();
    }; break;
    default: return;
    }

    update();
}

} // namespace Graphics
