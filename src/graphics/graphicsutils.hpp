#pragma once

#include <QCursor>
#include <QLineF>
#include <QPolygonF>

namespace Graphics {

namespace Utils {

auto createBoundingRect(const QPolygonF &ply, double margin) -> QRectF;
auto distance(QPointF pos, QPointF center) -> double;
auto normalizeAngle(double angle) -> double;
auto cursorForDirection(double angle) -> QCursor;

auto calculateCircle(const QPolygonF &pts, QPointF &center, double &radius) -> bool;
auto boundingFromLine(const QLineF &line, double margin) -> QPolygonF;

} // namespace Utils

} // namespace Graphics
