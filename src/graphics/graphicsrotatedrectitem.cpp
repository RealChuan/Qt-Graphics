#include "graphicsrotatedrectitem.h"
#include "geometrycache.hpp"
#include "graphicsutils.hpp"

#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QtMath>

namespace Graphics {

namespace {

auto createRotatedRect(const QPointF &p1, const QPointF &p2, const QPointF &c) -> RotatedRect
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

void updateRotatedRectByMovingCorner(RotatedRect &rect, int cornerIndex, const QPointF &newPos)
{
    if (cornerIndex < 0 || cornerIndex > 3)
        return;

    // 获取当前控制点
    const auto currentPoints = rect.controlPoints();
    const int oppositeIndex = (cornerIndex + 2) % 4;
    const QPointF fixedPoint = currentPoints[oppositeIndex];

    // 计算新的中心点（移动点和对角点的中点）
    rect.center = (newPos + fixedPoint) / 2;

    // 计算新点在局部坐标系中的位置
    const double radians = qDegreesToRadians(-rect.angle);
    const double cosA = std::cos(radians);
    const double sinA = std::sin(radians);

    QPointF localNewPos = newPos - rect.center;
    localNewPos = QPointF(localNewPos.x() * cosA - localNewPos.y() * sinA,
                          localNewPos.x() * sinA + localNewPos.y() * cosA);

    // 计算新的宽度和高度（取绝对值并乘以2）
    rect.width = std::max(0.0, 2 * std::abs(localNewPos.x()));
    rect.height = std::max(0.0, 2 * std::abs(localNewPos.y()));
}

void handleEdgeStretch(RotatedRect &rrt,
                       QPolygonF &controlPoints,
                       const QPointF &delta,
                       const QPointF &clickedPos,
                       QLineF &hoveredLine)
{
    // 计算移动距离
    const double moveDistance = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

    // 获取被拖动的边对应的两个端点
    const QPointF p1 = hoveredLine.p1();
    const QPointF p2 = hoveredLine.p2();

    // 找到这两个点在控制点数组中的索引
    int index1 = controlPoints.indexOf(p1);
    int index2 = controlPoints.indexOf(p2);

    if (index1 < 0 || index2 < 0) {
        return;
    }

    // 计算边的法向量方向（垂直于边的方向）
    QLineF normalVector = hoveredLine.normalVector();

    // 确定拉伸方向（根据鼠标移动方向判断是拉伸还是收缩）
    bool isStretching = QLineF(rrt.center, clickedPos).length()
                        < QLineF(rrt.center, clickedPos + delta).length();

    // 计算移动向量
    const QPointF moveVector = QPointF(normalVector.dx(), normalVector.dy()) * moveDistance
                               / normalVector.length() * (isStretching ? 1 : -1);

    // 移动边的两个端点，保持对边静止不动
    controlPoints.replace(index1, p1 + moveVector);
    controlPoints.replace(index2, p2 + moveVector);

    // 更新被拖动的边
    hoveredLine = QLineF(controlPoints[index1], controlPoints[index2]);

    // 重新计算旋转矩形的参数
    rrt.width = QLineF(controlPoints[0], controlPoints[1]).length();
    rrt.height = QLineF(controlPoints[0], controlPoints[3]).length();
    rrt.center = (controlPoints[0] + controlPoints[2]) / 2;
}

} // namespace

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

auto RotatedRect::rotationLine() const -> QLineF
{
    const auto points = controlPoints();
    const QPointF c1 = (points.at(1) + points.at(2)) / 2;
    QLineF line(center, c1);
    return QLineF(center, line.pointAt(0.9));
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

void GraphicsRotatedRectItem::drawContent(QPainter *painter)
{
    if (isValid()) {
        painter->drawPolygon(geometryCache()->controlPoints());
        if (isSelected()) {
            auto pen = painter->pen();
            pen.setColor(pen.color().darker());
            painter->setPen(pen);
            painter->drawLine(d_ptr->rotatedRect.rotationLine());
            painter->drawEllipse(d_ptr->rotatedRect.center, 5, 5);
        }
    } else {
        painter->drawPath(d_ptr->cachePath);
    }
}

void GraphicsRotatedRectItem::pointsChanged(const QPolygonF &ply)
{
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

void GraphicsRotatedRectItem::updateHoverPreview(const QPointF &scenePos)
{
    auto controlPoints = geometryCache()->controlPoints();
    if (controlPoints.size() != 2) {
        return;
    }

    auto rrt = createRotatedRect(controlPoints[0], controlPoints[1], scenePos);
    if (!rrt.isValid(margin())) {
        return;
    }
    d_ptr->cachePath.clear();
    d_ptr->cachePath.addPolygon(rrt.controlPoints());
    d_ptr->cachePath.closeSubpath();

    update();
}

GraphicsBasicItem::MouseRegion GraphicsRotatedRectItem::detectEdgeRegion(const QPointF &scenePos)
{
    const auto &controlPoints = geometryCache()->controlPoints();
    const double edgeMargin = margin() / 2.0;

    const QLineF rotationLine = d_ptr->rotatedRect.rotationLine();
    if (Utils::isPointNearEdge(scenePos, rotationLine, edgeMargin)) {
        d_ptr->rotatedHovered = true;
        setCursor(Utils::cursorForDirection(rotationLine.angle()));
        setMouseRegion(MouseRegion::EdgeArea);
        return MouseRegion::EdgeArea;
    }

    for (int i = 0; i < 4; ++i) {
        const QLineF edgeLine(controlPoints[i], controlPoints[(i + 1) % 4]);

        if (!Utils::isPointNearEdge(scenePos, edgeLine, edgeMargin)) {
            continue;
        }
        d_ptr->linehovered = true;
        d_ptr->hoveredLine = edgeLine;
        setCursor(Utils::cursorForDirection(edgeLine.angle()));
        setMouseRegion(MouseRegion::EdgeArea);
        return MouseRegion::EdgeArea;
    }

    d_ptr->rotatedHovered = false;
    d_ptr->linehovered = false;

    return MouseRegion::NoSelection;
}

void GraphicsRotatedRectItem::handleMouseMoveEvent(const QPointF &scenePos,
                                                   const QPointF &clickedPos,
                                                   const QPointF delta)
{
    auto controlPoints = geometryCache()->controlPoints();

    auto rrt = d_ptr->rotatedRect;
    switch (mouseRegion()) {
    case MouseRegion::EntireShape: rrt.center += delta; break;
    case MouseRegion::AnchorPoint:
        updateRotatedRectByMovingCorner(rrt, hoveredDotIndex(), scenePos);
        break;
    case MouseRegion::EdgeArea: {
        QLineF currentLine(rrt.center, scenePos);
        QLineF previousLine(rrt.center, clickedPos);

        if (d_ptr->rotatedHovered) {
            // 处理旋转操作
            double angleDelta = currentLine.angleTo(previousLine);
            rrt.angle += angleDelta;
            rrt.angle = Utils::normalizeAngle(rrt.angle);
            setCursor(Utils::cursorForDirection(360 - rrt.angle));
        } else if (d_ptr->linehovered) {
            // 处理边的拉伸操作
            handleEdgeStretch(rrt, controlPoints, delta, clickedPos, d_ptr->hoveredLine);
        }
    } break;
    default: return;
    }

    if (setRotatedRect(rrt)) {
        update();
    }
}

} // namespace Graphics
