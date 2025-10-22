#include "graphicsutils.hpp"

#include <QDebug>
#include <QPainterPath>
#include <QtMath>

namespace Graphics::Utils {

auto calculateCircle(const QPolygonF &pts, QPointF &center, double &radius) -> bool
{
    // 输入验证
    if (pts.size() != 3) {
        qWarning() << "calculateCircle requires exactly 3 points, got" << pts.size();
        return false;
    }

    const QPointF &A = pts[0];
    const QPointF &B = pts[1];
    const QPointF &C = pts[2];

    // 检查点是否重合或共线
    const double epsilon = 1e-10;

    // 检查是否有重合点
    if (distance(A, B) < epsilon || distance(B, C) < epsilon || distance(C, A) < epsilon) {
        qWarning() << "Points are too close or coincident";
        return false;
    }

    // 检查三点是否共线（使用叉积）
    const double crossProduct = (B.x() - A.x()) * (C.y() - A.y())
                                - (B.y() - A.y()) * (C.x() - A.x());

    if (qAbs(crossProduct) < epsilon) {
        qWarning() << "Points are collinear, cannot form a circle";
        return false;
    }

    // 使用几何方法计算圆心和半径
    // 圆心是两条垂直平分线的交点

    // 计算AB和AC的中点
    const QPointF midAB((A.x() + B.x()) * 0.5, (A.y() + B.y()) * 0.5);
    const QPointF midAC((A.x() + C.x()) * 0.5, (A.y() + C.y()) * 0.5);

    // 计算AB和AC的垂直向量
    const QPointF perpAB(B.y() - A.y(), A.x() - B.x()); // AB的垂直向量
    const QPointF perpAC(C.y() - A.y(), A.x() - C.x()); // AC的垂直向量

    // 解线性方程组求圆心
    // 直线1: midAB + t * perpAB
    // 直线2: midAC + s * perpAC
    // 求交点

    const double denom = perpAB.x() * perpAC.y() - perpAB.y() * perpAC.x();

    if (qAbs(denom) < epsilon) {
        qWarning() << "Parallel perpendiculars, points are collinear";
        return false;
    }

    const double dx = midAC.x() - midAB.x();
    const double dy = midAC.y() - midAB.y();

    const double t = (dx * perpAC.y() - dy * perpAC.x()) / denom;

    // 计算圆心坐标
    center.setX(midAB.x() + t * perpAB.x());
    center.setY(midAB.y() + t * perpAB.y());

    // 计算半径（圆心到任意点的距离）
    radius = distance(center, A);

    // 验证结果
    const double radiusB = distance(center, B);
    const double radiusC = distance(center, C);

    // 检查三个半径是否一致（允许一定的浮点误差）
    const double radiusTolerance = 1e-6;
    if (qAbs(radius - radiusB) > radiusTolerance || qAbs(radius - radiusC) > radiusTolerance) {
        qWarning() << "Circle calculation inconsistency, possible numerical issues";
        return false;
    }

    return true;
}

auto createBoundingRect(const QPolygonF &ply, double margin) -> QRectF
{
    double addLen = margin / 2;
    return ply.boundingRect().adjusted(-addLen, -addLen, addLen, addLen);
}

auto normalizeAngle(double angle) -> double
{
    angle = std::fmod(angle, 360.0);
    return angle < 0.0 ? angle + 360.0 : angle;
}

auto distance(QPointF pos, QPointF center) -> double
{
    return QLineF(center, pos).length();
}

auto cursorForDirection(double angle) -> QCursor
{
    angle = normalizeAngle(angle);
    const int sector = static_cast<int>((angle + 22.5) / 45.0) % 8;
    static constexpr std::array<Qt::CursorShape, 8> cursors = {Qt::SizeVerCursor,
                                                               Qt::SizeFDiagCursor,
                                                               Qt::SizeHorCursor,
                                                               Qt::SizeBDiagCursor,
                                                               Qt::SizeVerCursor,
                                                               Qt::SizeFDiagCursor,
                                                               Qt::SizeHorCursor,
                                                               Qt::SizeBDiagCursor};

    return cursors[sector];
}

auto expandAndUnitePath(const QPainterPath &path, qreal strokeWidth) -> QPainterPath
{
    QPainterPathStroker stroker;
    stroker.setWidth(strokeWidth);
    QPainterPath expandedPath = stroker.createStroke(path);
    return expandedPath.united(path);
}

auto calculateHalfArc(const QPolygonF &ply, QPainterPath &path) -> bool
{
    if (ply.size() != 3) {
        return false;
    }

    const QPointF &A = ply[0];
    const QPointF &B = ply[1];
    const QPointF &C = ply[2];

    // 计算圆心和半径
    QPointF center;
    double radius = 0;
    if (!calculateCircle(ply, center, radius)) {
        return false;
    }

    // 计算三个点相对于圆心的角度
    const double angleA = QLineF(center, A).angle();
    const double angleB = QLineF(center, B).angle();
    const double angleC = QLineF(center, C).angle();

    // 确定圆弧方向（使用第三点C来判断）
    // 如果C在从A到B的逆时针方向上，则为逆时针弧，否则为顺时针弧
    const bool isCounterClockwise = isPointOnCounterClockwiseSide(A, B, C, center);

    // 计算起始角度和扫过角度
    double startAngle, sweepAngle;
    calculateArcAngles(angleA, angleB, angleC, isCounterClockwise, startAngle, sweepAngle);

    // 创建圆弧路径
    const QRectF rect(center.x() - radius, center.y() - radius, radius * 2, radius * 2);
    path.clear();
    path.moveTo(A);
    path.arcTo(rect, startAngle, sweepAngle);

    return true;
}

auto isPointOnCounterClockwiseSide(const QPointF &A,
                                   const QPointF &B,
                                   const QPointF &C,
                                   const QPointF &center) -> bool
{
    // 使用向量叉积判断方向
    const QPointF vecOA = A - center;
    const QPointF vecOB = B - center;
    const QPointF vecOC = C - center;

    // 计算从OA到OB的旋转方向
    const double crossAB = vecOA.x() * vecOB.y() - vecOA.y() * vecOB.x();

    // 计算从OA到OC的旋转方向
    const double crossAC = vecOA.x() * vecOC.y() - vecOA.y() * vecOC.x();

    // 如果C与B在同一旋转方向上，则使用逆时针
    return (crossAB * crossAC) >= 0;
}

void calculateArcAngles(double angleA,
                        double angleB,
                        double angleC,
                        bool isCounterClockwise,
                        double &startAngle,
                        double &sweepAngle)
{
    angleA = normalizeAngle(angleA);
    angleB = normalizeAngle(angleB);
    angleC = normalizeAngle(angleC);

    if (isCounterClockwise) {
        // 逆时针圆弧
        startAngle = angleA;

        // 确保结束角度大于起始角度
        if (angleB < angleA) {
            angleB += 360.0;
        }

        sweepAngle = angleB - angleA;

        // 验证第三点C是否在圆弧上
        double angleCAdjusted = angleC;
        if (angleC < angleA) {
            angleCAdjusted += 360.0;
        }

        if (angleCAdjusted < angleA || angleCAdjusted > angleB) {
            // 如果C不在A和B之间，可能需要调整方向
            sweepAngle -= 360.0;
        }
    } else {
        // 顺时针圆弧
        startAngle = angleA;

        // 确保结束角度小于起始角度
        if (angleB > angleA) {
            angleB -= 360.0;
        }

        sweepAngle = angleB - angleA;

        // 验证第三点C是否在圆弧上
        double angleCAdjusted = angleC;
        if (angleC > angleA) {
            angleCAdjusted -= 360.0;
        }

        if (angleCAdjusted > angleA || angleCAdjusted < angleB) {
            // 如果C不在A和B之间，可能需要调整方向
            sweepAngle += 360.0;
        }
    }

    // 确保扫过角度在合理范围内
    if (qAbs(sweepAngle) > 360.0) {
        sweepAngle = std::copysign(360.0, sweepAngle);
    }
}

auto calculateAllArc(const QPolygonF &ply, QPainterPath &path, const double margin) -> bool
{
    if (ply.size() != 4) {
        return false;
    }

    const QPointF &A = ply[0];
    const QPointF &B = ply[1];
    const QPointF &C = ply[2];
    const QPointF &D = ply[3];

    // 计算圆心和半径（使用前三个点）
    QPointF center;
    double innerRadius = 0;
    if (!calculateCircle(ply.mid(0, 3), center, innerRadius)) {
        return false;
    }

    // 计算外半径（第四个点到圆心的距离）
    const double outerRadius = QLineF(center, D).length();

    // 确定内外半径
    const double radiusIn = qMin(innerRadius, outerRadius);
    const double radiusOut = qMax(innerRadius, outerRadius);

    // 检查半径有效性
    const double minRadius = margin * qSqrt(2) * 0.5;
    if (radiusIn < minRadius || (radiusOut - radiusIn) < minRadius) {
        return false;
    }

    // 计算角度
    const double angleA = QLineF(center, A).angle();
    const double angleB = QLineF(center, B).angle();
    const double angleC = QLineF(center, C).angle();

    // 确定圆弧方向
    const bool isCounterClockwise = isPointOnCounterClockwiseSide(A, B, C, center);

    // 计算起始角度和扫过角度
    double startAngle, sweepAngle;
    calculateArcAngles(angleA, angleB, angleC, isCounterClockwise, startAngle, sweepAngle);

    // 创建内外圆弧的矩形边界
    const QRectF innerRect(center.x() - radiusIn, center.y() - radiusIn, radiusIn * 2, radiusIn * 2);
    const QRectF outerRect(center.x() - radiusOut,
                           center.y() - radiusOut,
                           radiusOut * 2,
                           radiusOut * 2);

    // 构建环状弧路径
    path.clear();

    // 外圆弧
    path.moveTo(pointFromCenter(center, radiusOut, startAngle));
    path.arcTo(outerRect, startAngle, sweepAngle);

    // 内圆弧（反向）
    path.arcTo(innerRect, startAngle + sweepAngle, -sweepAngle);

    // 闭合路径
    path.closeSubpath();

    return true;
}

auto pointFromCenter(const QPointF &center, double radius, double angle) -> QPointF
{
    double radians = qDegreesToRadians(angle);
    return center + QPointF(radius * qCos(radians), -radius * qSin(radians));
}

auto isPointNearEdge(const QPointF &point, const QLineF &line, double margin) -> bool
{
    // 使用点到线段的距离公式
    const QPointF lineVec = line.p2() - line.p1();
    const QPointF pointVec = point - line.p1();

    const double lineLengthSquared = QPointF::dotProduct(lineVec, lineVec);

    // 如果线段长度为0，检查点到端点的距离
    if (qFuzzyIsNull(lineLengthSquared)) {
        return distance(point, line.p1()) <= margin;
    }

    // 计算投影比例 t
    const double t = std::clamp(QPointF::dotProduct(pointVec, lineVec) / lineLengthSquared,
                                0.0,
                                1.0);

    // 计算投影点
    const QPointF projection = line.p1() + t * lineVec;

    // 返回点到线段的距离是否在容差范围内
    return distance(point, projection) <= margin;
}

auto pointAtDistance(const QPointF &from, const QPointF &to, double distance) -> QPointF
{
    QLineF line(from, to);
    line.setLength(distance);
    return line.p2();
}

} // namespace Graphics::Utils
