#include "graphicsrotatedrectitem.h"
#include "graphics.h"

#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtMath>

namespace Graphics {

auto RotatedRect::isValid() const -> bool
{
    return width > 0 && height > 0;
}

struct GraphicsRotatedRectItem::GraphicsRotatedRectItemPrivate
{
    RotatedRect rotatedRect;
    bool rotatedHovered = false;
    bool linehovered = false;
    QLineF hoveredLine;
};

GraphicsRotatedRectItem::GraphicsRotatedRectItem(QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d_ptr(new GraphicsRotatedRectItemPrivate)
{
    setAcceptHoverEvents(true);
}

GraphicsRotatedRectItem::GraphicsRotatedRectItem(const RotatedRect &rotatedRect,
                                                 QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d_ptr(new GraphicsRotatedRectItemPrivate)
{
    setAcceptHoverEvents(true);
    setRotatedRect(rotatedRect);
}

GraphicsRotatedRectItem::~GraphicsRotatedRectItem() {}

inline auto checkRotatedRect(const RotatedRect &rotatedRect, const double margin) -> bool
{
    return rotatedRect.width > margin && rotatedRect.height > margin;
}

inline auto rotate(const QPointF &p, double angle) -> QPointF
{
    double _angle = Graphics::ConvertTo360(angle) * M_PI * 2 / 360.0;
    double cos = std::cos(_angle);
    double sin = std::sin(_angle);
    double _x = cos * p.x() - sin * p.y();
    double _y = sin * p.x() + cos * p.y();
    return QPointF(_x, _y);
}

inline auto cacheFromRotated(const RotatedRect &rotated) -> QPolygonF
{
    QPointF p0 = QPointF(-rotated.width / 2, -rotated.height / 2);
    QPointF p1 = QPointF(rotated.width / 2, -rotated.height / 2);
    QPointF p2 = QPointF(rotated.width / 2, rotated.height / 2);
    QPointF p3 = QPointF(-rotated.width / 2, rotated.height / 2);

    QPolygonF pts;
    pts << rotated.center + rotate(p0, rotated.angle);
    pts << rotated.center + rotate(p1, rotated.angle);
    pts << rotated.center + rotate(p2, rotated.angle);
    pts << rotated.center + rotate(p3, rotated.angle);

    return pts;
}

void GraphicsRotatedRectItem::setRotatedRect(const RotatedRect &rotatedRect)
{
    if (!checkRotatedRect(rotatedRect, margin())) {
        return;
    }
    prepareGeometryChange();
    d_ptr->rotatedRect = rotatedRect;
    QPolygonF pts = cacheFromRotated(rotatedRect);
    setCache(pts);
}

auto GraphicsRotatedRectItem::rotatedRect() const -> RotatedRect
{
    return d_ptr->rotatedRect;
}

auto GraphicsRotatedRectItem::isValid() const -> bool
{
    return checkRotatedRect(d_ptr->rotatedRect, margin());
}

auto GraphicsRotatedRectItem::type() const -> int
{
    return ROTATEDRECT;
}

void GraphicsRotatedRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }
    setClickedPos(event->scenePos());
    if (isValid()) {
        return;
    }
    QPointF point = event->pos();
    QPolygonF pts_tmp = cache();
    pts_tmp.append(point);
    pointsChanged(pts_tmp);
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
    QPolygonF pts_tmp = cache();
    QLineF l0(c, point);
    QLineF l1(c, clickedPos());
    double m = qSqrt(dp.x() * dp.x() + dp.y() * dp.y());
    if (d_ptr->rotatedHovered) {
        double angle = l0.angleTo(l1);
        rrt.angle += angle;
        rrt.angle = Graphics::ConvertTo360(rrt.angle);
        setCursor(Graphics::curorFromAngle(360 - rrt.angle));
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
        case DotRegion: break;
        case All: rrt.center += dp; break;
        default: return;
        }
    }

    pts_tmp = cacheFromRotated(rrt);
    if (scene()->sceneRect().contains(pts_tmp.boundingRect()) && checkRotatedRect(rrt, margin())) {
        setRotatedRect(rrt);
        update();
    }

    setClickedPos(point);
}

void GraphicsRotatedRectItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if (!isValid()) {
        return;
    }
    QPointF point = event->scenePos();
    QPointF center = d_ptr->rotatedRect.center;
    QPolygonF pts_tmp = cache();

    QPointF c1 = (pts_tmp[1] + pts_tmp[2]) / 2;
    QLineF l = QLineF(center, c1);
    l = QLineF(center, l.pointAt(0.9));

    pts_tmp = Graphics::boundingFromLine(l, margin() / 4);
    if (pts_tmp.containsPoint(point, Qt::OddEvenFill)) {
        d_ptr->rotatedHovered = true;
        setCursor(Graphics::curorFromAngle(l.angle()));
        return;
    }

    pts_tmp = cache();
    for (int i = 0; i < pts_tmp.count(); ++i) {
        QLineF pl(pts_tmp.at(i), pts_tmp.at((i + 1) % 4));
        QPolygonF tmp = Graphics::boundingFromLine(pl, margin() / 4);
        if (tmp.containsPoint(point, Qt::OddEvenFill)) {
            d_ptr->linehovered = true;
            d_ptr->hoveredLine = pl;
            setCursor(Graphics::curorFromAngle(pl.angle()));
            return;
        }
    }

    d_ptr->rotatedHovered = false;
    d_ptr->linehovered = false;
    BasicGraphicsItem::hoverMoveEvent(event);
}

void GraphicsRotatedRectItem::paint(QPainter *painter,
                                    const QStyleOptionGraphicsItem *option,
                                    QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    double linew = 2 * pen().widthF() / painter->transform().m11();
    painter->setPen(QPen(LineColor, linew));
    setMargin(painter->transform().m11());

    if (isValid()) {
        QPointF c = d_ptr->rotatedRect.center;
        painter->drawPolygon(cache());
        if (option->state & QStyle::State_Selected) {
            painter->setPen(QPen(pen().color().darker(), linew));
            QPointF c1 = (cache().at(1) + cache().at(2)) / 2;
            QLineF l = QLineF(c, c1);
            l = QLineF(c, l.pointAt(0.9));
            painter->drawLine(l);
            painter->drawEllipse(c, 5, 5);
        }
    }

    if (!isValid() || (option->state & QStyle::State_Selected)) {
        drawAnchor(painter);
        drawBoundingRect(painter);
    }
}

inline auto showRotatedFromCache(const QPolygonF &ply) -> RotatedRect
{
    if (ply.count() < 3) {
        return RotatedRect{};
    }
    RotatedRect rrt;
    rrt.width = QLineF(ply[0], ply[1]).length();
    rrt.height = QLineF(ply[1], ply[2]).length();
    rrt.center = (ply[0] + ply[2]) / 2;
    rrt.angle = Graphics::ConvertTo360(QLineF(ply[0], ply[1]).angle());
    return rrt;
}

void GraphicsRotatedRectItem::pointsChanged(const QPolygonF &ply)
{
    QRectF rect = scene()->sceneRect();
    if (!rect.contains(ply.last())) {
        return;
    }
    double width = 0;
    double height = 0;
    switch (ply.size()) {
    case 1:
    case 2: setCache(ply); break;
    case 3: {
        QLineF l = QLineF(ply[0], ply[1]);
        QLineF l1 = l.normalVector();
        QPointF p;
        l.intersects(l1, &p);
        width = l.length();
        height = QLineF(p, ply[2]).length();
        if (width < margin() || height < margin()) {
            return;
        }
        RotatedRect rotatedRect = showRotatedFromCache(ply);
        if (!checkRotatedRect(rotatedRect, margin())) {
            return;
        }
        setRotatedRect(rotatedRect);
    } break;
    default: return;
    }
    update();
}

} // namespace Graphics
