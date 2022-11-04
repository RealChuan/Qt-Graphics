#include "graphicsarcitem.h"
#include "graphics.h"

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtMath>
#include <QDebug>

namespace Graphics {

bool Arc::isValid() const
{
    return maxRadius > minRadius;
}

/*
 *
 */

bool inTop(const QPointF linePt0, const QPointF linePt1, const QPointF pt)
{
    if(linePt0.x() - linePt1.x() == 0.0){
        if(pt.x() > linePt0.x())
            return false;
        return true;
    }

    bool isInTop = true;
    double k = 0;
    double b = 0;
    k = (linePt0.y() - linePt1.y()) / (linePt0.x() - linePt1.x());
    b = linePt0.y() - k * linePt0.x();
    if((pt.x() * k + b - pt.y()) < 0)
        isInTop = false;
    return isInTop;
}

void calculateArc(QPointF center,
                  double radius,
                  QPointF p1,
                  QPointF p2,
                  bool isInTop,
                  QPainterPath &path,
                  double &startAngle,
                  double &endAngle,
                  double &deltaAngle)
{
    // 先确定起点和终点 当两个点X相等时，起始点无所谓是哪个
    QPointF startPoint ;
    QPointF endPoint ;
    if(p1.x() > p2.x()) {
        startPoint = p1;
        endPoint = p2;
    } else {
        startPoint = p2;
        endPoint = p1;
    }

    // 计算角度，保证终点角度大于起点角度
    startAngle = QLineF(center, startPoint).angle();
    endAngle = QLineF(center, endPoint).angle();

    if (startAngle > endAngle)
        endAngle += 360;

    deltaAngle = isInTop ? (endAngle - startAngle) : (endAngle - startAngle - 360);

    QRectF rect(QPointF(center.x() - radius, center.y() - radius),
                QPointF(center.x() + radius, center.y() + radius));
    QPainterPath pathTemp(startPoint);
    path = pathTemp;
    // deltaAngle 为 负数时，顺时针
    path.arcTo(rect, startAngle, deltaAngle);
}

bool calculateAllArc(const QPolygonF &ply,
                     QPainterPath &path,
                     QPainterPath &shape,
                     const double margin)
{
    if(ply.size() != 4)
        return false;

    QPainterPath path_temp;
    QPointF center;
    double radius0 = 0;
    double radius1 = 0;
    double startAngle = 0;
    double endAngle = 0;
    double deltaAngle = 0;

    QPolygonF pts_tmp = ply;
    QPointF endPt = pts_tmp[3];
    pts_tmp.pop_back();
    // 计算方向得出旋转角度
    bool isInTop = inTop(ply[0], ply[1], endPt);
    Graphics::calculateCircle(pts_tmp, center, radius0);
    calculateArc(center,
                 radius0,
                 ply[0],
                 ply[1],
                 isInTop,
                 path_temp,
                 startAngle,
                 endAngle,
                 deltaAngle);

    if (deltaAngle <= 0) {
        if(endAngle < 0){
            endAngle += 360;
            startAngle += 360;
        }
        qSwap(startAngle, endAngle);
        deltaAngle = -deltaAngle;
    }

    radius1 = Graphics::distance(endPt, center);

    double radiusIn = qMin(radius0, radius1);
    double radiusOut = qMax(radius0, radius1);
    double num = margin * qSqrt(2) / 2;

    if ((radiusIn < num) || ((radiusOut - radiusIn) < num))
        return false;

    QRectF rect0(center.x() - radiusIn, center.y() - radiusIn, radiusIn * 2, radiusIn * 2);
    QRectF rect1(center.x() - radiusOut, center.y() - radiusOut, radiusOut * 2, radiusOut * 2);

    path.clear();
    path.moveTo(center);
    path.arcTo(rect1, startAngle, deltaAngle);

    QPainterPath subPath1;
    subPath1.addEllipse(rect0);
    path -= subPath1;

    //shape
    radiusIn -= num;
    radiusOut += num;

    QRectF rect00(center.x() - radiusIn, center.y() - radiusIn, radiusIn * 2, radiusIn * 2);
    QRectF rect11(center.x() - radiusOut, center.y() - radiusOut, radiusOut * 2, radiusOut * 2);

    double addAngle = qAsin(num / (radiusIn + num)) * 180 / M_PI;
    startAngle -= addAngle;
    deltaAngle += 2 * addAngle;

    shape.clear();
    shape.setFillRule(Qt::WindingFill);
    shape.moveTo(center);
    shape.arcTo(rect11, startAngle, deltaAngle);

    QPainterPath subPath;
    subPath.addEllipse(rect00);
    shape -= subPath;

    return true;
}

struct GraphicsArcItem::GraphicsArcItemPrivate
{
    Arc arch;
    QPainterPath arcPath;
    QPainterPath cachePath;
    QPainterPath shape;
    GraphicsArcItem::MouseRegion mouseRegion = GraphicsArcItem::None;
};

GraphicsArcItem::GraphicsArcItem(QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d_ptr(new GraphicsArcItemPrivate)
{
}

GraphicsArcItem::GraphicsArcItem(const Arc &arc, QGraphicsItem *parent)
    : BasicGraphicsItem(parent)
    , d_ptr(new GraphicsArcItemPrivate)
{
    setArc(arc);
    calculateAllArc(cache(), d_ptr->arcPath, d_ptr->shape, margin());
}

GraphicsArcItem::~GraphicsArcItem(){}

inline QPointF findAnotherPtOfLine(const QPointF basePt, const double r, const double angle)
{
    QLineF line;
    line.setP1(basePt);
    line.setLength(r);
    line.setAngle(angle);
    return line.p2();
}

inline QPolygonF calculateCache(const Arc &arch)
{
    QPointF pt00 = findAnotherPtOfLine(arch.center, arch.minRadius, arch.startAngle);
    QPointF pt01 = findAnotherPtOfLine(arch.center, arch.minRadius, arch.endAngle);
    QPointF pt10 = findAnotherPtOfLine(arch.center, arch.minRadius,
                                       (arch.startAngle + arch.endAngle) / 2.0);
    QPointF pt11 = findAnotherPtOfLine(arch.center,
                                       arch.maxRadius,
                                       (arch.startAngle + arch.endAngle) / 2.0);

    QPolygonF pts;
    pts.push_back(pt00);
    pts.push_back(pt01);
    pts.push_back(pt10);
    pts.push_back(pt11);
    return pts;
}

inline bool checkArc(const Arc &arch, const double margin)
{
    double minLen = margin * qSqrt(2) / 2;
    return arch.minRadius >= minLen && arch.maxRadius - arch.minRadius >= minLen;
}

void GraphicsArcItem::setArc(const Arc &arc)
{
    if (!checkArc(arc, margin())) {
        return;
    }

    d_ptr->arch = arc;
    QPolygonF pts = calculateCache(d_ptr->arch);
    setCache(pts);
}

Arc GraphicsArcItem::arch() const
{
    return d_ptr->arch;
}

bool GraphicsArcItem::isValid() const
{
    return checkArc(d_ptr->arch, margin());
}

int GraphicsArcItem::type() const
{
    return ARC;
}

QRectF GraphicsArcItem::boundingRect() const
{
    if (!isValid()) {
        return BasicGraphicsItem::boundingRect();
    }

    QPolygonF pts = d_ptr->arcPath.toFillPolygon() + cache();
    QRectF rectF = pts.boundingRect();
    double addLen = margin() * qSqrt(2) / 2;
    rectF.adjust(-addLen, -addLen, addLen, addLen);
    return rectF;
}

QPainterPath GraphicsArcItem::shape() const
{
    if (isValid()) {
        return d_ptr->shape;
    }
    return BasicGraphicsItem::shape();
}

void GraphicsArcItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
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

inline QPointF lineSetLength(const QPointF p1, const QPointF p2, const double len)
{
    QLineF line(p1, p2);
    line.setLength(len);
    return line.p2();
}

void GraphicsArcItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!isValid()) {
        return;
    }

    QPointF point = event->scenePos();
    QPointF dp = point - clickedPos();
    setClickedPos(event->scenePos());
    QPolygonF pts_tmp = cache();
    double distance = Graphics::distance(d_ptr->arch.center, point);

    switch (mouseRegion()) {
    case BasicGraphicsItem::All: pts_tmp.translate(dp); break;
    case BasicGraphicsItem::None: {
        switch (d_ptr->mouseRegion) {
        case InEdge0:
            setMyCursor(d_ptr->arch.center, event->scenePos());
            for (int i = 0; i < 3; ++i) {
                pts_tmp[i] = lineSetLength(d_ptr->arch.center, pts_tmp[i], distance);
            }
            break;
        case InEdge1:
            setMyCursor(d_ptr->arch.center, event->scenePos());
            pts_tmp[3] = lineSetLength(d_ptr->arch.center, pts_tmp[3], distance);
            break;
        case InEdgeH:
        case InEdgeL:{
            Arc arch = d_ptr->arch;
            if(d_ptr->mouseRegion == InEdgeL){
                arch.startAngle = QLineF(arch.center, event->scenePos()).angle();
                setCursor(Graphics::curorFromAngle(arch.startAngle));
            }else{
                arch.endAngle = QLineF(arch.center, event->scenePos()).angle();
                setCursor(Graphics::curorFromAngle(arch.endAngle));
            }
            while (arch.startAngle > arch.endAngle) {
                arch.endAngle += 360;
            }
            if (arch.endAngle - arch.startAngle > 360) {
                arch.endAngle -= 360;
            }
            pts_tmp = calculateCache(arch);
        } break;
        default: return;
        }break;
    }
    case BasicGraphicsItem::DotRegion:
        pts_tmp[hoveredDotIndex()] += dp;
        break;
    default: return;
    }

    pointsChanged(pts_tmp);
}

void GraphicsArcItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPolygonF pts_tmp = cache();
    QPointF point = event->scenePos();
    if (pts_tmp.size() == 2 || pts_tmp.size() == 3) {
        pts_tmp.append(point);
        showHoverArc(pts_tmp);
    }
    if (!isValid()) {
        return;
    }
    BasicGraphicsItem::hoverMoveEvent(event);
    if(mouseRegion() == DotRegion)
        return;
    setMouseRegion(BasicGraphicsItem::None);

    QPointF p1 = findAnotherPtOfLine(d_ptr->arch.center,
                                     d_ptr->arch.maxRadius,
                                     d_ptr->arch.startAngle);
    QPointF p2 = findAnotherPtOfLine(d_ptr->arch.center,
                                     d_ptr->arch.maxRadius,
                                     d_ptr->arch.endAngle);
    QLineF line1(p1, cache().at(0));
    QLineF line2(p2, cache().at(1));
    if (qAbs(Graphics::distance(point, d_ptr->arch.center) - d_ptr->arch.minRadius) < margin() / 3) {
        d_ptr->mouseRegion = InEdge0;
        setMyCursor(d_ptr->arch.center, point);
    } else if (qAbs(Graphics::distance(point, d_ptr->arch.center) - d_ptr->arch.maxRadius)
               < margin() / 3) {
        d_ptr->mouseRegion = InEdge1;
        setMyCursor(d_ptr->arch.center, point);
    } else if (Graphics::boundingFromLine(line1, margin() / 4).containsPoint(point, Qt::OddEvenFill)) {
        d_ptr->mouseRegion = InEdgeL;
        setCursor(Graphics::curorFromAngle(line1.angle()));
    } else if (Graphics::boundingFromLine(line2, margin() / 4).containsPoint(point, Qt::OddEvenFill)) {
        d_ptr->mouseRegion = InEdgeH;
        setCursor(Graphics::curorFromAngle(line2.angle()));
    } else if (d_ptr->arcPath.contains(point)) {
        setMouseRegion(BasicGraphicsItem::All);
        setCursor(Qt::SizeAllCursor);
    } else {
        unsetCursor();
    }
}

void GraphicsArcItem::paint(QPainter *painter,
                            const QStyleOptionGraphicsItem *option,
                            QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);
    double linew = 2 * pen().widthF() / painter->transform().m11();
    painter->setPen(QPen(LineColor, linew));
    setMargin(painter->transform().m11());

    if (isValid()) {
        painter->drawPath(d_ptr->arcPath);
    } else {
        painter->drawPath(d_ptr->cachePath);
    }
    if (option->state & QStyle::State_Selected) {
        drawAnchor(painter);
    }
}

inline bool calculateHalfArc(const QPolygonF &ply, QPainterPath &path)
{
    if (ply.size() != 3) {
        return false;
    }

    QPointF center;
    double radius0 = 0;
    double startAngle = 0;
    double endAngle = 0;
    double deltaAngle = 0;

    QPointF endPt = ply[2];
    // 计算方向得出旋转角度
    bool isInTop = inTop(ply[0], ply[1], endPt);
    Graphics::calculateCircle(ply, center, radius0);
    calculateArc(center, radius0, ply[0], ply[1], isInTop, path, startAngle, endAngle, deltaAngle);
    return true;
}

inline Arc calucateFinnalArch(const QPolygonF& ply)
{
    double startAngle_ = 0;
    double endAngle_ = 0;

    QPointF center(0, 0);
    double radius0 = 0;
    double radius1 = 0;
    double startAngle = 0;
    double endAngle = 0;
    double deltaAngle = 0;

    QPolygonF pts_tmp = ply;
    QPointF endPt = pts_tmp[3];
    pts_tmp.pop_back();
    // 计算方向得出旋转角度
    bool isInTop = inTop(ply[0], ply[1], endPt);
    Graphics::calculateCircle(pts_tmp, center, radius0);
    QPainterPath path1;
    calculateArc(center, radius0, ply[0], ply[1], isInTop, path1, startAngle, endAngle, deltaAngle);
    radius1 = Graphics::distance(endPt, center);

    endAngle = startAngle + deltaAngle;
    if(deltaAngle > 0) {
        startAngle_ = startAngle;
        endAngle_ = endAngle;
    } else {
        startAngle_ = endAngle + 360;
        endAngle_ = startAngle + 360;
    }

    return Arc{center, qMin(radius0, radius1), qMax(radius0, radius1), startAngle_, endAngle_};
}

void GraphicsArcItem::pointsChanged(const QPolygonF& ply)
{
    QRectF rect = scene()->sceneRect();
    if (!rect.contains(ply.last())) {
        return;
    }

    if (ply.size() > 2) {
        double deltaAngle = QLineF(ply[0], ply[1]).angleTo(QLineF(ply[0], ply[2]));
        if (deltaAngle < 0.00001 || deltaAngle > 355.99999) {
            return;
        }
    }

    switch (ply.size()) {
    case 1:
    case 2:
        setCache(ply);
        break;
    case 3:{
        if (!calculateHalfArc(ply, d_ptr->cachePath)) {
            return;
        }
        QPolygonF polygon = d_ptr->cachePath.toFillPolygon() + ply;
        if (!rect.contains(polygon.boundingRect())) {
            return;
        }
        setCache(ply);
    } break;
    case 4:{
        if (!calculateAllArc(ply, d_ptr->arcPath, d_ptr->shape, margin())) {
            return;
        }
        QPolygonF polygon = d_ptr->shape.toFillPolygon() + ply;
        if (!rect.contains(polygon.boundingRect())) {
            return;
        }
        Arc arc = calucateFinnalArch(ply);
        if (!checkArc(arc, margin())) {
            return;
        }
        setArc(arc);
    } break;
    default: return;
    }

    update();
}

void GraphicsArcItem::showHoverArc(const QPolygonF &ply)
{
    switch (ply.size()) {
    case 3:
        // QPainterPath::arcTo: Adding point with invalid coordinates, ignoring call
        if (Graphics::distance(ply[1], ply[2]) < margin()) {
            return;
        }
        calculateHalfArc(ply, d_ptr->cachePath);
        break;
    case 4: calculateAllArc(ply, d_ptr->cachePath, d_ptr->shape, margin()); break;
    default: return;
    }

    update();
}

}
