#pragma once

#include <QCursor>
#include <QLineF>
#include <QPolygonF>

namespace Graphics {

namespace Utils {

auto createBoundingRect(const QPolygonF &ply, double margin) -> QRectF;
auto expandAndUnitePath(const QPainterPath &path, qreal strokeWidth) -> QPainterPath;
auto distance(QPointF pos, QPointF center) -> double;
auto pointAtDistance(const QPointF &from, const QPointF &to, double distance) -> QPointF;
auto normalizeAngle(double angle) -> double;
auto cursorForDirection(double angle) -> QCursor;

auto pointFromCenter(const QPointF &center, double radius, double angle) -> QPointF;
auto isPointOnCounterClockwiseSide(const QPointF &A,
                                   const QPointF &B,
                                   const QPointF &C,
                                   const QPointF &center) -> bool;
void calculateArcAngles(double angleA,
                        double angleB,
                        double angleC,
                        bool isCounterClockwise,
                        double &startAngle,
                        double &sweepAngle);
auto calculateCircle(const QPolygonF &pts, QPointF &center, double &radius) -> bool;
auto calculateHalfArc(const QPolygonF &ply, QPainterPath &path) -> bool;
auto calculateAllArc(const QPolygonF &ply, QPainterPath &path, const double margin) -> bool;

auto isPointNearEdge(const QPointF &point, const QLineF &line, double margin) -> bool;

} // namespace Utils

} // namespace Graphics
